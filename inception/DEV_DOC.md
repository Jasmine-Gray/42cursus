# Inception — Developer Guide

> **During the evaluation:** Jump to [Appendix A: EVALHUB Defense Guide](#appendix-a-evalhub-defense-guide) for model answers, checks in the evaluation-sheet order, copyable commands, and the full HTTPS port-change / rollback exercise. The main chapters below remain the setup and maintenance documentation.

## 1. Architecture and prerequisites

This project builds three separate Docker images from **Debian 12** and runs them with Docker Compose inside a Debian virtual machine.

```text
Browser ── HTTPS :443 ──> NGINX
                              │ FastCGI :9000
                              v
                        WordPress + PHP-FPM
                              │ MariaDB :3306
                              v
                            MariaDB
```

NGINX is the only published entry point. All three services use the Compose bridge network `docker-network`; `network: host` and legacy Docker `links` are not used. NGINX accepts TLS 1.2 and 1.3 only.

The tested environment uses:

- VirtualBox with a Debian 12 (bookworm), amd64 guest; approximately 2 vCPUs, 4 GiB RAM, and a 30 GB virtual disk.
- Docker Engine, Docker Compose plugin, Docker Buildx, GNU Make, Git, curl, and OpenSSL.
- A NAT adapter for internet access and SSH forwarding; an optional host-only adapter for direct browser access.

**All Docker installation, project configuration, and `make` commands below run inside the Debian VM.** The school PC is only the VirtualBox host and SSH client.

## 2. Project layout

```text
inception/
├── Makefile
├── README.md
├── USER_DOC.md
├── DEV_DOC.md
├── .gitignore
├── secrets/                         # Local only; ignored by Git
│   ├── credentials.txt
│   ├── db_password.txt
│   ├── db_root_password.txt
│   └── wp_user_password.txt
└── srcs/
    ├── .env.example                 # Tracked, non-secret template
    ├── .env                         # Local only; ignored by Git
    ├── docker-compose.yml
    └── requirements/
        ├── mariadb/
        │   ├── Dockerfile
        │   ├── conf/50-server.cnf
        │   └── tools/docker-entrypoint.sh
        ├── nginx/
        │   ├── Dockerfile
        │   └── conf/nginx.conf
        └── wordpress/
            ├── Dockerfile
            ├── conf/www.conf
            └── tools/docker-entrypoint.sh
```

The exact directory layout may also include `.dockerignore` files. The Compose file is `srcs/docker-compose.yml`, and the entry point for routine operations is the root `Makefile`.

## 3. Prepare a Debian VM

Create a Debian 12 VM and install the base system with **SSH server** and **standard system utilities**. A desktop environment is optional and is not needed for this project.

For the tested VirtualBox network setup:

| VirtualBox adapter | Mode | Purpose |
| --- | --- | --- |
| Adapter 1 | NAT | Package downloads and internet connectivity; optionally forward host `127.0.0.1:2222` to guest port `22` for SSH. |
| Adapter 2 | Host-only (`vboxnet0`) | Browser access from the school PC to the guest on port `443`. |

On the current VM, the host-only interface is `enp0s8` with static address `192.168.56.10/24`; the host adapter is `192.168.56.1/24`. The guest configuration is:

```ini
# /etc/network/interfaces.d/hostonly
# This assumes the main /etc/network/interfaces sources interfaces.d/*.
auto enp0s8
iface enp0s8 inet static
    address 192.168.56.10
    netmask 255.255.255.0
```

Do not configure a second default gateway for this interface. Verify its name before applying these values on a different VM.

The VM files may be stored on the school's shared `sgoinfre` storage, but the VM must be shut down before opening the same disk on another PC. A new PC may need its own host-only adapter and VirtualBox registration.

## 4. Install Docker Engine and project tools

On a clean Debian 12 VM, install prerequisites and add Docker's official Debian APT repository:

```bash
sudo apt-get update
sudo apt-get install -y ca-certificates curl git make openssl
sudo install -d -m 0755 /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/debian/gpg \
  -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc

sudo tee /etc/apt/sources.list.d/docker.sources >/dev/null <<'SOURCES'
Types: deb
URIs: https://download.docker.com/linux/debian
Suites: bookworm
Components: stable
Architectures: amd64
Signed-By: /etc/apt/keyrings/docker.asc
SOURCES

sudo apt-get update
sudo apt-get install -y \
  docker-ce docker-ce-cli containerd.io \
  docker-buildx-plugin docker-compose-plugin
sudo systemctl enable --now docker
```

Confirm the installation:

```bash
sudo docker version
sudo docker compose version
sudo systemctl is-active docker
```

If your VM uses a different Debian release or CPU architecture, adjust the repository selection accordingly and follow the current official Docker installation instructions.

## 5. Configure Docker's persistent storage location

The subject requires the **named-volume data** to be stored under `/home/mishimod/data` **inside the VM**. In the tested implementation, this is achieved by moving Docker's entire `data-root` to that directory; **the Compose volumes themselves remain normal named volumes, not bind mounts**.

On a newly installed VM with no valuable existing Docker data:

```bash
sudo systemctl stop docker docker.socket
sudo mkdir -p /home/mishimod/data
printf '%s\n' '{"data-root":"/home/mishimod/data"}' | \
  sudo tee /etc/docker/daemon.json >/dev/null
sudo systemctl start docker

sudo docker info --format '{{.DockerRootDir}}'
```

The expected output is `/home/mishimod/data`.

**Do not blindly overwrite an existing `/etc/docker/daemon.json` on a VM that already has other Docker settings or data.** Merge the `data-root` setting and plan any required data migration first.

The resulting volumes normally have these mountpoints:

```text
/home/mishimod/data/volumes/mariadb_volume/_data
/home/mishimod/data/volumes/wordpress_volume/_data
```

The root directory also contains Docker images, container metadata, and other Docker-managed files; it is **not** exclusively a volume directory.

## 6. Prepare the project files and configuration

Copy or clone the project's source files into `/home/mishimod/inception`, preserving the `Makefile` and `srcs/` layout:

```bash
cd /home/mishimod/inception
```

Create the non-secret settings by copying the tracked template:

```bash
cp srcs/.env.example srcs/.env
```

The template contains these example values (edit them for your environment):

```dotenv
DOMAIN_NAME=mishimod.42.fr
MYSQL_DATABASE=wordpress
MYSQL_USER=wpuser
WORDPRESS_TITLE=Inception
WORDPRESS_ADMIN_USER=siteowner
WORDPRESS_ADMIN_EMAIL=siteowner@example.com
WORDPRESS_USER=writer
WORDPRESS_EMAIL=writer@example.com
```

Replace the example email addresses if real notification addresses are desired. The administrator username must not contain `admin` or `administrator`, including case variants.

Generate the **four** local secret files **only during the first installation**. Never regenerate them after MariaDB or WordPress has stored account passwords. For safety, the command below aborts without replacing any files if **even one** secret already exists:

```bash
cd /home/mishimod/inception
(
  set -eu
  umask 077
  mkdir -p secrets
  chmod 700 secrets
  for name in db_password.txt db_root_password.txt credentials.txt wp_user_password.txt; do
    if [ -e "secrets/$name" ]; then
      echo "Refusing to overwrite secrets/$name; preserve existing credentials." >&2
      exit 1
    fi
  done
  openssl rand -hex 24 > secrets/db_password.txt
  openssl rand -hex 24 > secrets/db_root_password.txt
  openssl rand -hex 24 > secrets/credentials.txt
  openssl rand -hex 24 > secrets/wp_user_password.txt
  chmod 600 secrets/*.txt
)
```

**Do not rerun the generation block on a moved, existing, or restarted VM.** If you need to rotate a password, change the corresponding account password in MariaDB or WordPress as well, verify the application, and only then recreate the service as needed. The procedure above does not perform password rotation.

WordPress initialization runs as root, reads `/run/secrets/db_password`, and creates `/run/wp-db-password` with `root:www-data` ownership and mode `0640`. On every container startup, `wp-config.php` is configured to read the copied file using PHP `file_get_contents`. The original host secret remains restricted to its owner (`0600`); **do not change its host group to `www-data`**.

After starting the containers, verify PHP-FPM can read the copy and WordPress uses it:

```bash
sudo docker exec -u www-data wordpress test -r /run/wp-db-password
echo $?
sudo docker exec wordpress sh -c \
  'grep -n "file_get_contents" /var/www/wordpress/wp-config.php'
```

The exit status should be `0` and `DB_PASSWORD` should reference `/run/wp-db-password`. Neither command prints the password itself.

Keep these paths excluded from Git:

```gitignore
secrets/
srcs/.env
```

Never commit passwords, tokens, private keys, or unredacted logs. **Changing a secret file does not rotate an account already stored in WordPress or MariaDB**; credential rotation requires updating the corresponding account and the mounted secret consistently.

## 7. Build and launch

Validate the Compose configuration:

```bash
cd /home/mishimod/inception
sudo docker compose -p inception -f srcs/docker-compose.yml config --quiet
```

Use the Makefile:

```bash
make               # Build and start all three containers
```

The current Makefile runs `sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build`. Images are built from the service Dockerfiles and tagged `nginx:inception`, `wordpress:inception`, and `mariadb:inception` (not `latest`).

Inspect running services and their logs:

```bash
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
sudo docker compose -p inception -f srcs/docker-compose.yml \
  logs --tail=30 mariadb wordpress nginx
```

**Review logs before sharing them.** Earlier versions of the initialization workflow could expose credential values in WP-CLI output; do not post unredacted logs in issues, chats, or Git.

## 8. Service configuration summary

| Service | Key details |
| --- | --- |
| NGINX | Built from Debian 12, listens on `443`, uses a locally generated self-signed TLS certificate, allows only TLS 1.2 and 1.3, and forwards PHP requests to `wordpress:9000`. |
| WordPress | Built from Debian 12 with PHP 8.2/FPM and WP-CLI. Connects to `mariadb:3306` using the application DB user; initializes the site and two WordPress users when required. |
| MariaDB | Built from Debian 12; uses an initialization script, persists `/var/lib/mysql`, and is reachable only on the internal Docker network. |

Secrets are mounted under `/run/secrets/` in the containers. NGINX is the **only** service publishing a port on the VM; the database and PHP-FPM ports are internal.

## 9. Routine management

```bash
cd /home/mishimod/inception

make              # Build/start
make down         # Stop and remove project containers/network; retain volumes
make re           # Recreate/rebuild while retaining named volumes

sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
sudo docker compose -p inception -f srcs/docker-compose.yml logs --tail=30
sudo docker image ls
sudo docker volume ls
sudo docker network ls
```

Inspect volume mountpoints:

```bash
sudo docker volume inspect mariadb_volume wordpress_volume \
  --format '{{.Name}} -> {{.Mountpoint}}'
```

**Destructive operations:** `docker compose down -v`, `docker volume rm`, and volume pruning can delete persisted content. Do not use them for normal restart or rebuild workflows.

## 10. Verification and troubleshooting

### HTTPS and TLS

From inside the VM:

```bash
curl -kI \
  --resolve mishimod.42.fr:443:127.0.0.1 \
  https://mishimod.42.fr/

openssl s_client -connect 127.0.0.1:443 \
  -servername mishimod.42.fr -tls1_2 -brief </dev/null
openssl s_client -connect 127.0.0.1:443 \
  -servername mishimod.42.fr -tls1_3 -brief </dev/null
openssl s_client -connect 127.0.0.1:443 \
  -servername mishimod.42.fr -tls1_1 -brief </dev/null
```

TLS 1.2/1.3 should negotiate successfully. TLS 1.0/1.1 should be rejected; alert number `70` means `protocol_version`. A self-signed certificate warning is expected for this local setup. The `-k` flag bypasses certificate verification for testing only.

### WordPress and database

```bash
sudo docker exec wordpress wp core is-installed --allow-root
echo $?
```

An exit status of `0` confirms that WordPress sees an installed site. If HTTPS returns HTTP 500, check application logs, the database connection, and `www-data` access to `/run/wp-db-password`.

### Persistence

1. Create a draft post and upload an image in WordPress administration.
2. Run `make down` and confirm `mariadb_volume` and `wordpress_volume` still exist.
3. Run `make` and verify that the draft and image remain intact.
4. Reboot the VM and verify that Docker and all three containers restart automatically, with the content preserved.

This setup has passed these persistence and restart checks, as well as an NGINX abnormal-termination restart test. Re-run the checks after significant changes.

## 11. Submission and portability notes

- The VM working copy at `/home/mishimod/inception` was initialized as its **own local Git repository**. It is separate from the original school-PC `42cursus` repository. Copy or commit the relevant changes into the actual **42 submission repository** before evaluation.
- Only source/configuration/documentation files belong in Git; keep `srcs/.env` and `secrets/` local. Recreate them during setup.
- Replace the login name, domain, paths, and network addresses when using another account or environment.
- The Docker persistent data directory is **inside the Debian VM**, even when the VM's virtual disk is stored on the school's shared `sgoinfre` storage.
- VirtualBox host-only adapters and hostname resolution are **per school PC**; they do not automatically transfer with the shared VM files.

See `USER_DOC.md` for day-to-day use and `README.md` for project goals and design decisions.

---

## Appendix A: EVALHUB Defense Guide

*42 Inception — EVALHUB Defense Guide: Model Answers and Verification Commands*


**Basis:** User-provided *42 EvalHub.pdf* (captured October 30, 2026), in the **same order as the evaluation sheet**.

**Project snapshot:** `mishimod`'s Inception project (Debian 12 VM, Docker Compose, NGINX, WordPress with PHP-FPM, MariaDB).

**Purpose:** A rehearsal/reference guide. The actual evaluator decides which checks and configuration changes to request. This document does not guarantee a passing grade.

> **Important source-code warning:** The reviewed MariaDB entrypoint contains `mariadbd --user=mysql --skip-networking --console &` during initialization. EVALHUB's General Instructions prohibit background-running programs inside entrypoint scripts. Even though the server is temporary, this may fail the evaluation. **Fix and clean-test this before submitting.** Also verify fresh WordPress account creation and its passwords in a clean environment. The earlier mock defense used an already-initialized VM and included manual password recovery; this does **not** establish that a fresh clone boots correctly.

### How to use this guide
Unless marked **school host**, commands run in the **Debian VM**, from the Inception project root:

```bash
cd /home/mishimod/inception
```

The equivalent of the project's Compose command is:

```bash
sudo docker compose -p inception -f srcs/docker-compose.yml
```

Project-specific details:

| Item | Value |
| --- | --- |
| 42 login | `mishimod` |
| Website | `https://mishimod.42.fr` |
| Services | `nginx`, `wordpress`, `mariadb` |
| Host-exposed port in the required final configuration | `443/tcp` only |
| NGINX-to-PHP-FPM | `wordpress:9000` |
| WordPress-to-MariaDB | `mariadb:3306` |
| Docker network | `docker-network` |
| Named volumes | `wordpress_volume`, `mariadb_volume` |
| Data root | `/home/mishimod/data` |
| WordPress administrator | `siteowner` |
| Additional WordPress user | `writer` |
| Database / application DB user | `wordpress` / `wpuser` |

**Submission rule:** Perform the official evaluation against a **fresh clone of the submitted Git repository**. Do not confuse it with the existing `inception-verify` practice VM. `.env` and real secrets are intentionally not stored in Git; set them up according to `DEV_DOC.md` during initial installation.

---

### 1. Introduction / Guidelines (evaluation sheet, pp. 1–2)
**What the evaluator checks:** The submitted Git repository is the only graded work. It must be the correct repository, cloned into an empty directory. Suspicious aliases, unauthorized shortcuts, or evaluation scripts may be inspected.

**Model answer:**

> "My submitted Git repository contains the project definition. The root Makefile invokes Docker Compose, and the service-specific configurations are under `srcs/`. I keep local credentials and private environment files out of Git."

**Commands:**

```bash
pwd
ls -la
find . -maxdepth 3 -type f | sort
# Inside the actual submitted clone:
git status --short
git ls-files
```

### 2. Preliminaries / Preliminary Tests (pp. 2–3)
**What the evaluator checks:** The evaluated student is present, the repository is not empty, and committed credentials, API keys, and passwords are not exposed. A local `.env` and evaluation-created Docker secret files are permitted when handled appropriately.

**Model answer:**

> "I use `.env` for non-secret configuration, such as the domain, database name, and usernames. I store passwords in files mounted through Docker Compose secrets. I exclude `secrets/` and `srcs/.env` from version control with `.gitignore`."

**Commands:**

```bash
cat .gitignore
# These should show no tracked secret files:
git ls-files secrets srcs/.env
# These should identify ignored local files when present:
git check-ignore secrets/db_password.txt srcs/.env
```

**Never print secret values** during the defense. Keep the actual passwords available to the student without exposing them on screen.

### 3. General Instructions (pp. 3–4)
**What the evaluator checks:** `Makefile` at repository root; configuration files under `srcs/`; no host networking, `links:`, or `--link`; no keep-alive hacks, infinite loops, or background-running entrypoint programs; valid Debian/Alpine base; all services start with the Makefile.

**Model answer:**

> "The root Makefile calls Docker Compose. `srcs/docker-compose.yml` defines three services, a private bridge network, Docker secrets, and persistent volumes. I do not use host networking or legacy Docker links. Each container runs its real service in the foreground rather than relying on a `tail -f` loop."

**Commands:**

```bash
test -f Makefile && test -f srcs/docker-compose.yml && echo OK
cat Makefile
cat srcs/docker-compose.yml
grep -H '^FROM ' srcs/requirements/*/Dockerfile
# Inspect matches manually; grep alone cannot prove compliance:
grep -RInE 'network_mode:|network: host|links:|--link|tail -f|sleep infinity|while true|mariadbd.*&' Makefile srcs || true
sudo docker compose -p inception -f srcs/docker-compose.yml config --quiet
make
```

**Known blocker:** The current MariaDB entrypoint temporarily runs `mariadbd ... &` in the background. Fix this before the official evaluation and demonstrate a clean first-time initialization; do not describe the project as fully compliant until verified.

**Evaluator's global Docker cleanup step (destructive):**

```bash
# EVALHUB's cleanup sequence, reproduced for reference only.
# DO NOT run this on a machine containing data you want to keep.
docker stop $(docker ps -qa)
docker rm $(docker ps -qa)
docker rmi -f $(docker images -qa)
docker volume rm $(docker volume ls -q)
docker network rm $(docker network ls -q) 2>/dev/null
```

This can delete databases, WordPress uploads, and resources from unrelated Docker projects. Use an intentionally disposable VM for a full clean evaluation. `make down` is a normal non-volume-deleting shutdown; avoid `docker compose down -v` unless deleting all application data is intended.

### 4. Mandatory Part — Activity Overview (p. 4)
#### Q: How do Docker and Docker Compose work?
**Model answer:**

> "Docker builds an image from a Dockerfile and runs containers from that image. Docker Compose defines multiple containers, networks, volumes, and their configuration in YAML. It lets me build and operate the stack with one command."

#### Q: How is an image different with and without Docker Compose?
**Model answer:**

> "The image is not fundamentally different. Without Compose, I run commands like `docker build` and `docker run` manually. With Compose, I declare the same build and runtime settings in `docker-compose.yml`."

```bash
sudo docker image ls
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
```

#### Q: What are Docker's benefits compared with virtual machines?
**Model answer:**

> "A VM runs a guest operating system and its own kernel. Linux containers share the host Linux kernel while isolating processes and filesystems. Containers are generally quicker to start and require fewer resources. In this project, Docker itself runs inside a Debian VM."

#### Q: Why use this directory structure?
**Model answer:**

> "The Makefile is the single entry point. Each service has its own Dockerfile and configuration under `srcs/requirements/`. The `conf/` directories hold service settings; `tools/` holds initialization scripts. This makes the project easier to maintain and reproduce."

```text
inception/
├── Makefile
├── README.md
├── USER_DOC.md
├── DEV_DOC.md
├── .gitignore
├── secrets/                  # local only; excluded from Git
└── srcs/
    ├── .env.example
    ├── .env                  # local only; excluded from Git
    ├── docker-compose.yml
    └── requirements/
        ├── nginx/{Dockerfile,conf/}
        ├── wordpress/{Dockerfile,conf/,tools/}
        └── mariadb/{Dockerfile,conf/,tools/}
```

### 5. README Check (pp. 4–5)
**What the evaluator checks:** `README.md` exists in the root; its **first line is italicized** and uses the required wording; Description, Instructions, and Resources are present, including an explanation of AI use.

**Model answer:**

> "My README explains the goal and architecture, the steps to build and use the stack, the resources consulted, and what AI assisted with and how I validated the results."

```bash
head -n 1 README.md
grep -nE '^## (Description|Instructions|Resources)|^### AI Usage' README.md
```

Example first line:

```markdown
*This project has been created as part of the 42 curriculum by mishimod.*
```

### 6. Documentation Check (p. 5)
**What the evaluator checks:** Nonempty `USER_DOC.md` and `DEV_DOC.md` at repository root.

**Model answer:**

> "`USER_DOC.md` explains how a user or administrator starts and stops the stack, accesses the website and dashboard, manages credentials, and checks service status. `DEV_DOC.md` covers prerequisites, VM setup, Docker configuration, secrets, Makefile and Compose commands, and persistent storage."

```bash
ls -lh README.md USER_DOC.md DEV_DOC.md
head -n 8 USER_DOC.md
head -n 8 DEV_DOC.md
```

### 7. Simple Setup (p. 5)
**What the evaluator checks:** Only NGINX is reachable through host port 443, an SSL/TLS certificate is used, the site loads over HTTPS, HTTP on port 80 does not work, and the WordPress installation wizard is not shown.

**Model answer:**

> "NGINX is the only public entry point, on HTTPS port 443. It terminates TLS and forwards PHP requests to `wordpress:9000` over the Docker network. WordPress connects to `mariadb:3306`. Port 80 is not published."

```bash
sudo docker ps --format 'table {{.Names}}\t{{.Ports}}\t{{.Status}}'
# HTTPS should work:
curl -kI --resolve mishimod.42.fr:443:127.0.0.1 https://mishimod.42.fr/
# HTTP should fail if no unrelated service is bound to port 80:
curl -I --connect-timeout 3 --resolve mishimod.42.fr:80:127.0.0.1 http://mishimod.42.fr/
# Certificate metadata; self-signed is acceptable:
sudo docker exec nginx openssl x509 -in /etc/nginx/ssl/inception.crt -noout -subject -dates
```

Expected: HTTPS responds with the installed WordPress site, and HTTP is not available. `curl -k` bypasses certificate-chain verification for the self-signed certificate; it does **not** prove certificate trust.

### 8. Docker Basics (pp. 5–6)
**What the evaluator checks:** One nonempty, self-written Dockerfile per service. No prebuilt application images. The permitted Debian/Alpine base is used. Built image names match service names; the Makefile uses Compose and services do not crash.

**Model answer:**

> "Each of the three services has its own Dockerfile, starting from `debian:12`. I install the packages needed for that service rather than pulling ready-made NGINX, WordPress, or MariaDB application images. Compose builds local images named `nginx:inception`, `wordpress:inception`, and `mariadb:inception`."

```bash
find srcs/requirements -name Dockerfile -type f -print
grep -H '^FROM ' srcs/requirements/*/Dockerfile
sudo docker image ls
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
cat Makefile
```

### 9. Docker Network (p. 6)
**What the evaluator checks:** A Compose-defined Docker network exists, and the student can explain it.

**Model answer:**

> "The Docker bridge network lets containers communicate privately by service name. Docker provides internal DNS: NGINX reaches `wordpress:9000`, and WordPress reaches `mariadb:3306`. Neither WordPress's nor MariaDB's port needs to be published to the host."

```bash
sudo docker network ls
sudo docker network inspect docker-network
sudo docker exec nginx getent hosts wordpress
sudo docker exec wordpress getent hosts mariadb
```

### 10. NGINX with SSL/TLS (pp. 6–7)
**What the evaluator checks:** NGINX Dockerfile and running container; HTTP unavailable; HTTPS shows WordPress; TLS 1.2 or 1.3 can be demonstrated. Self-signed certificates are permitted.

**Model answer:**

> "NGINX listens on port 443 using HTTPS. The configuration enables TLS 1.2 and TLS 1.3 and uses a self-signed certificate. PHP execution happens in the separate WordPress PHP-FPM container, reached through FastCGI."

```bash
cat srcs/requirements/nginx/Dockerfile
grep -nE 'listen|ssl_protocols|ssl_certificate|fastcgi_pass' srcs/requirements/nginx/conf/nginx.conf
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
sudo docker exec nginx nginx -t
# Check a TLS 1.2 handshake:
openssl s_client -connect 127.0.0.1:443 -servername mishimod.42.fr -tls1_2 -brief </dev/null
# Check a TLS 1.3 handshake:
openssl s_client -connect 127.0.0.1:443 -servername mishimod.42.fr -tls1_3 -brief </dev/null
```

A self-signed certificate can trigger a trust warning without preventing TLS negotiation. Old protocol tests may also fail due to the local OpenSSL client's policy; interpret the output carefully.

### 11. WordPress with PHP-FPM and Its Volume (p. 7)
**What the evaluator checks:** A self-written WordPress Dockerfile without NGINX; running container; persistent named volume mounted under `/home/mishimod/data/`; ability to add a comment as an available WordPress user; administrator username does not contain `admin` or `Admin`; administrator can edit a page and show the result on the website.

**Model answer:**

> "The WordPress container runs PHP-FPM 8.2; it does not contain NGINX. WordPress files and uploads are persisted in `wordpress_volume`. The administrator is `siteowner` and the other user is `writer`. I can create a comment, sign in as the administrator, edit a page, and verify the change on the public website."

```bash
cat srcs/requirements/wordpress/Dockerfile
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
sudo docker volume ls
sudo docker volume inspect wordpress_volume --format '{{.Name}} -> {{.Mountpoint}}'
sudo docker exec wordpress php-fpm8.2 -t
sudo docker exec wordpress wp core is-installed --allow-root
sudo docker exec wordpress wp user list --fields=user_login,roles --allow-root
```

**Browser demonstration:** Open `https://mishimod.42.fr`, submit a comment as `writer` where permitted, approve it in the dashboard if it is **Pending**, and verify it appears on its original post. Log in as `siteowner` at `https://mishimod.42.fr/wp-admin/`, edit an existing page, and verify the published change.

### 12. MariaDB and Its Volume (pp. 7–8)
**What the evaluator checks:** Self-written MariaDB Dockerfile without NGINX, running container, named volume under `/home/mishimod/data/`, a demonstrable DB login, and nonempty tables.

**Model answer:**

> "MariaDB stores WordPress posts, comments, accounts, and other structured data. Its data directory `/var/lib/mysql` is backed by `mariadb_volume`, so the data survives container recreation and VM reboots. The application database is `wordpress`, and the application user is `wpuser`. The password is read from Docker secrets rather than embedded in the source code."

```bash
cat srcs/requirements/mariadb/Dockerfile
sudo docker volume inspect mariadb_volume --format '{{.Name}} -> {{.Mountpoint}}'
# Connect using the real WordPress-to-DB path, without printing the password:
sudo docker exec wordpress sh -ec '
  MYSQL_PWD="$(cat /run/secrets/db_password)"
  export MYSQL_PWD
  exec mariadb --protocol=tcp -h mariadb -P 3306 \
    -u "$MYSQL_USER" "$MYSQL_DATABASE" \
    -e "SHOW TABLES; SELECT COUNT(*) AS users FROM wp_users; SELECT COUNT(*) AS posts FROM wp_posts;"
'
# Optional: test the MariaDB root credentials without printing the password:
sudo docker exec mariadb sh -ec '
  MYSQL_PWD="$(cat /run/secrets/db_root_password)"
  export MYSQL_PWD
  exec mariadb --protocol=socket -u root -e "SELECT CURRENT_USER();"
'
```

During the mock defense, there were 12 tables, 2 users, and 14 posts. **Use the real counts observed at evaluation time**, not hard-coded historic values.

### 13. Persistence! (p. 8)
**What the evaluator checks:** Reboot the VM, start/check Compose again, verify services are running, and demonstrate that earlier WordPress changes remain.

**Model answer:**

> "The Debian `systemd` configuration starts Docker Engine during boot, and `restart: always` lets Docker restart the containers. WordPress and MariaDB use named volumes, so data is not dependent on the disposable container write layer. Posts and comments are primarily in MariaDB; uploads and WordPress files are in the WordPress volume."

```bash
# Inside the VM (SSH will disconnect):
sudo reboot
# On the school host after the VM has booted:
ssh -p 2223 mishimod@127.0.0.1
# Back in the VM:
cd ~/inception
sudo systemctl is-active docker
sudo docker ps
# The evaluation sheet also asks you to launch Compose again:
sudo docker compose -p inception -f srcs/docker-compose.yml up -d
curl -kI --resolve mishimod.42.fr:443:127.0.0.1 https://mishimod.42.fr/
sudo docker volume inspect wordpress_volume mariadb_volume --format '{{.Name}} -> {{.Mountpoint}}'
```

Verify previously edited pages, posts, comments, and uploads in the browser. `docker compose down -v` deletes named volumes and must **not** be used for a persistence demonstration.

**If the SSH SOCKS proxy disappeared after reboot (school host):**

```bash
ssh -N -D 127.0.0.1:1080 -p 2223 mishimod@127.0.0.1
```

Keep that terminal open and use the browser configured with the SOCKS5 proxy. `ERR_PROXY_CONNECTION_FAILED` after reboot does not, by itself, mean WordPress data was lost.

### 14. Configuration Modification (p. 8)
**What the evaluator checks:** The evaluator chooses one service and requests a configuration change, **for example a port change**. The new port must be available. The evaluated student must rebuild and restart the project, and the service must still be accessible and functional.

**Model answer:**

> "A Dockerfile `COPY` places the configuration into an image at build time. `docker compose restart` only restarts an existing container and does not copy edited files into its image. After editing the relevant configuration, I rebuild the image and recreate/restart the service with Docker Compose, then verify the new behavior. I do not delete the named volumes."

#### Example A — Add a response header (the successful mock-defense exercise)
Add this directive **inside** the `server { ... }` block in `srcs/requirements/nginx/conf/nginx.conf`:

```nginx
add_header X-Inception-Eval "passed" always;
```

```bash
cd ~/inception
sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build --no-deps nginx
sudo docker exec nginx nginx -t
curl -kI --resolve mishimod.42.fr:443:127.0.0.1 https://mishimod.42.fr/
# Verify the exact header name: X-Inception-Eval: passed
```

HTTP header names are case-insensitive, but **`_` and `-` are different characters**. This exercise is optional and not a substitute for the actual evaluator's requested change.

#### Example B — Change the NGINX **listening port** from 443 to 8443 (full service-port change)
**Use this when the evaluator asks to change the port NGINX actually listens on.** Unlike a host-only port mapping, this modifies the service's own `listen` configuration. `8443` is an **example**, not a mandatory value: use the available port specified by the evaluator.

> **Mandatory-state warning:** The final Inception setup must expose NGINX on **443 only**. Port `8443` is a temporary defense exercise. **Restore 443 when the exercise is over and before final submission.** Do not change or delete either named volume. Do not regenerate any secrets.

##### B1. Confirm the existing state and check that the requested port is free
```bash
cd ~/inception
sudo docker compose -p inception -f srcs/docker-compose.yml ps
# A LISTEN row means another process is using port 8443:
sudo ss -ltnp '( sport = :8443 )'
# Inspect the current settings:
grep -nE 'listen|EXPOSE' srcs/requirements/nginx/conf/nginx.conf srcs/requirements/nginx/Dockerfile
grep -n -A 12 '^  nginx:' srcs/docker-compose.yml
```

Do not proceed with an occupied port. Choose a different port with the evaluator's approval.

##### B2. Back up the current files and WordPress URLs outside the repository
```bash
cd ~/inception
BACKUP_DIR="$HOME/inception-port-eval-backup"
mkdir -p "$BACKUP_DIR"
cp -p srcs/docker-compose.yml "$BACKUP_DIR/docker-compose.yml"
cp -p srcs/requirements/nginx/conf/nginx.conf "$BACKUP_DIR/nginx.conf"
cp -p srcs/requirements/nginx/Dockerfile "$BACKUP_DIR/nginx.Dockerfile"
# Preserve exact original WordPress URLs in case the browser needs a temporary port-specific URL:
sudo docker exec wordpress wp option get home --allow-root > "$BACKUP_DIR/wp_home"
sudo docker exec wordpress wp option get siteurl --allow-root > "$BACKUP_DIR/wp_siteurl"
```

These backup files are outside `~/inception`, so they are not accidentally included in the submitted Git repository. If the directory already exists from an earlier exercise, use a different backup directory rather than overwriting a needed backup.

##### B3. Edit **three** source files
**File 1: `srcs/requirements/nginx/conf/nginx.conf`**

Change **both** IPv4 and IPv6 listeners:

```nginx
# BEFORE:
listen 443 ssl;
listen [::]:443 ssl;

# AFTER:
listen 8443 ssl;
listen [::]:8443 ssl;
```

Keep the TLS certificate paths, `ssl_protocols`, server name, WordPress root, and `fastcgi_pass wordpress:9000` unchanged.

**File 2: `srcs/docker-compose.yml`**

Under the `nginx` service, change its `ports` entry:

```yaml
# BEFORE:
ports:
  - "443:443"

# AFTER:
ports:
  - "8443:8443"
```

The format is **`HOST_PORT:CONTAINER_PORT`**. Both sides are 8443 because NGINX itself now listens on 8443. Do **not** publish ports for WordPress or MariaDB.

**File 3: `srcs/requirements/nginx/Dockerfile`**

Change:

```dockerfile
# BEFORE:
EXPOSE 443

# AFTER:
EXPOSE 8443
```

`EXPOSE` is image metadata/documentation. It **does not publish a port by itself**; the actual listener is configured in `nginx.conf` and the host mapping is defined in Compose.

You can edit the files interactively:

```bash
nano srcs/requirements/nginx/conf/nginx.conf
nano srcs/docker-compose.yml
nano srcs/requirements/nginx/Dockerfile
```

##### B4. Validate, rebuild, and restart
```bash
# Check Compose syntax before rebuilding:
sudo docker compose -p inception -f srcs/docker-compose.yml config --quiet
# Rebuild and recreate the NGINX service, preserving WordPress and MariaDB:
sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build --no-deps nginx
# Check the actual runtime configuration:
sudo docker exec nginx nginx -t
sudo docker compose -p inception -f srcs/docker-compose.yml ps
sudo docker port nginx
sudo docker exec nginx grep -n 'listen 8443' /etc/nginx/conf.d/default.conf
```

Expected published-port mapping: `0.0.0.0:8443->8443/tcp` (and possibly an IPv6 mapping). The container image must contain the newly copied NGINX configuration.

**If the evaluator explicitly requests rebuilding and restarting the entire project**, run the following instead of only recreating NGINX:

```bash
sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build --force-recreate
```

The project's `make re` also rebuilds and restarts the stack, but first brings it down. Both approaches preserve named volumes **as long as `-v` is not used**. The stack can be temporarily unavailable while containers are recreated. Ensure secrets are still consistent before restarting.

##### B5. Verify HTTPS on the **new** port
```bash
# Test TLS on the new published port:
openssl s_client -connect 127.0.0.1:8443 -servername mishimod.42.fr -tls1_2 -brief </dev/null
# Test the actual new URL, without automatically following redirects:
curl -kI --resolve mishimod.42.fr:8443:127.0.0.1 https://mishimod.42.fr:8443/
```

A successful TLS handshake proves that NGINX is reachable on port 8443. The HTTP response might be `200 OK`, **or WordPress may redirect to its configured canonical URL** (`https://mishimod.42.fr` on default port 443). A redirect alone is not proof that the new browser URL is fully functional.

You can also test the **actual port 8443** while sending the original canonical URL to WordPress:

```bash
# Connect to 127.0.0.1:8443, but send the original HTTPS Host/SNI and URL:
curl -kI \
  --connect-to mishimod.42.fr:443:127.0.0.1:8443 \
  https://mishimod.42.fr/
```

For a browser test at `https://mishimod.42.fr:8443/`, if WordPress redirects back to port 443, **temporarily** update the WordPress `home` and `siteurl` options to match the new port:

```bash
sudo docker exec wordpress wp option update home 'https://mishimod.42.fr:8443' --allow-root
sudo docker exec wordpress wp option update siteurl 'https://mishimod.42.fr:8443' --allow-root
# Verify the values:
sudo docker exec wordpress wp option get home --allow-root
sudo docker exec wordpress wp option get siteurl --allow-root
# Retest:
curl -kI --resolve mishimod.42.fr:8443:127.0.0.1 https://mishimod.42.fr:8443/
```

**Browser access from the school host:** The verification VM uses an SSH SOCKS tunnel. If needed, run the following **on the school host** and keep the session open:

```bash
ssh -N -D 127.0.0.1:1080 -p 2223 mishimod@127.0.0.1
```

Use the configured SOCKS5 browser to open `https://mishimod.42.fr:8443/`; the self-signed certificate warning is expected. Demonstrate that WordPress loads and that existing content is still present.

##### B6. Restore the mandatory **443-only** configuration
If you temporarily changed WordPress URLs, **restore them first** while the database is reachable. The saved originals are used rather than assuming they were specific values:

```bash
cd ~/inception
BACKUP_DIR="$HOME/inception-port-eval-backup"
sudo docker exec wordpress wp option update home "$(cat "$BACKUP_DIR/wp_home")" --allow-root
sudo docker exec wordpress wp option update siteurl "$(cat "$BACKUP_DIR/wp_siteurl")" --allow-root
```

Restore and rebuild the NGINX configuration:

```bash
cp -p "$BACKUP_DIR/nginx.conf" srcs/requirements/nginx/conf/nginx.conf
cp -p "$BACKUP_DIR/docker-compose.yml" srcs/docker-compose.yml
cp -p "$BACKUP_DIR/nginx.Dockerfile" srcs/requirements/nginx/Dockerfile
sudo docker compose -p inception -f srcs/docker-compose.yml config --quiet
sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build --no-deps nginx
sudo docker exec nginx nginx -t
sudo docker port nginx
curl -kI --resolve mishimod.42.fr:443:127.0.0.1 https://mishimod.42.fr/
# The old mandatory port should now work; the temporary 8443 binding should be gone:
sudo ss -ltnp '( sport = :8443 )'
# Review uncommitted changes before any submission:
git status --short
git diff -- srcs/docker-compose.yml srcs/requirements/nginx/conf/nginx.conf srcs/requirements/nginx/Dockerfile
```

**Success criteria:** Only host port 443 is published, HTTPS serves WordPress, prior posts/comments/pages still exist, and the files have returned to their intended submission state. **Never use `down -v` or regenerate secrets to reset this exercise.**

#### Example C — Change the **host published port only** (8443:443)
Use this simpler variant **only if the evaluator asks to change the port exposed on the host**, not NGINX's own listening port.

In `srcs/docker-compose.yml`, change the `nginx` service from:

```yaml
ports:
  - "443:443"
```

to:

```yaml
ports:
  - "8443:443"
```

NGINX still listens **inside the container** on 443; only the host port is different. `nginx.conf` and the Dockerfile's `EXPOSE 443` stay unchanged.

```bash
sudo ss -ltnp '( sport = :8443 )'
sudo docker compose -p inception -f srcs/docker-compose.yml config --quiet
sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build --no-deps nginx
sudo docker port nginx
curl -kI --resolve mishimod.42.fr:8443:127.0.0.1 https://mishimod.42.fr:8443/
# Or validate the actual port while keeping the WordPress canonical Host/URL:
curl -kI --connect-to mishimod.42.fr:443:127.0.0.1:8443 https://mishimod.42.fr/
```

**Important:** This is **not** an internal NGINX port change. WordPress canonical redirects can still require the temporary URL adjustment described in B5. Restore `"443:443"` and recreate NGINX after the demonstration.

#### If the evaluator selects a different service
| Requested change | What must be updated | Important checks |
| --- | --- | --- |
| **PHP-FPM 9000 → 9001** | `srcs/requirements/wordpress/conf/www.conf`: `listen = 9001`; `srcs/requirements/nginx/conf/nginx.conf`: `fastcgi_pass wordpress:9001;`; WordPress Dockerfile: `EXPOSE 9001` | Rebuild/recreate **both** WordPress and NGINX; test `php-fpm8.2 -t`, `nginx -t`, and the HTTPS website. |
| **MariaDB 3306 → 3307** | `srcs/requirements/mariadb/conf/50-server.cnf`: `port = 3307`; MariaDB Dockerfile: `EXPOSE 3307`; WordPress entrypoint's MariaDB client readiness check: add the new `--port`; WP-CLI's `--dbhost` for fresh installations; **persisted** WordPress `wp-config.php` `DB_HOST` for existing installations | More invasive: both startup logic and saved WordPress configuration must agree, or WordPress may fail to start. Back up state and have the evaluator confirm this choice. Do not blindly change just `EXPOSE`. |

For PHP-FPM, a safe explicit rebuild is:

```bash
sudo docker compose -p inception -f srcs/docker-compose.yml up -d --build wordpress nginx
sudo docker exec wordpress php-fpm8.2 -t
sudo docker exec nginx nginx -t
curl -kI --resolve mishimod.42.fr:443:127.0.0.1 https://mishimod.42.fr/
```

For MariaDB, the source files use port `3306` in **several places**, including WordPress's first-run `wp config create` and the already-persisted `wp-config.php`. A single-file edit is **insufficient**. Avoid changing it unless that is the actual evaluation instruction and the complete migration steps have been planned.

#### `COPY` versus bind mounts: short oral follow-up
**Model answer:**

> "`COPY` captures a file in the image during `docker build`, so editing the host source afterward requires rebuilding the image and recreating the container. A bind mount makes the host file visible inside the running container, so rebuilding is not necessary, although a service such as NGINX may still need a configuration reload."

---

### 15. Bonus (p. 9)
**Evaluation rule:** Bonus is graded only if every mandatory requirement has been fully and correctly completed. Additional services require their own Dockerfiles, and any required volumes.

The sheet lists: (1) WordPress Redis cache, (2) an FTP server connected to the WordPress volume, (3) a static site built in a language other than PHP, (4) Adminer, and (5) another useful service with an explanation of its purpose.

**Project snapshot:** Only the three mandatory services were present; the bonus tasks should not be claimed as implemented.

---

### Final Pre-Submission Checklist
- [ ] **Resolve the MariaDB entrypoint background-process concern** (`mariadbd ... &`) and retest a truly clean initialization.
- [ ] Confirm a fresh clone can build from `Makefile` with `srcs/.env` and freshly created secrets, without manual account-password fixes.
- [ ] **Do not regenerate existing passwords.** `openssl rand ... > secrets/...` overwrites an existing secret but does not change the corresponding saved MariaDB or WordPress account password.
- [ ] Verify that credentials are untracked: `git ls-files secrets srcs/.env` prints nothing.
- [ ] Verify `README.md`, `USER_DOC.md`, and `DEV_DOC.md` meet the evaluation requirements.
- [ ] Ensure `nginx.conf` and Compose publish **port 443 only** again after any port-change exercise. Remove temporary headers if they were added only for testing.
- [ ] Verify TLS 1.2/1.3, login, comments, page edits, database access, and persistence on the actual submitted version.
- [ ] Use `-p inception -f srcs/docker-compose.yml` when calling Compose from the project root. Bare `docker compose ps` there does not find `srcs/docker-compose.yml`.
- [ ] Preserve named volumes when testing persistence; never confuse the evaluator's initial clean-state reset with a persistence test.
- [ ] Run `git status --short` and inspect the final submitted diff. The official evaluation grades **submitted repository content**, not local fixes in a different VM.

### 30-Second Architecture Explanation
> "Inception deploys three custom Docker services inside a Debian virtual machine using Docker Compose. NGINX exposes HTTPS on port 443 and forwards PHP requests to WordPress PHP-FPM on port 9000 inside a Docker bridge network. WordPress connects to MariaDB on port 3306. All three images are built from custom Dockerfiles based on Debian 12. Named volumes under `/home/mishimod/data` preserve WordPress files and database content, while passwords are provided through local Docker secrets that are excluded from Git."

---

**References:** User-provided `42 EvalHub.pdf`, `inception.pdf`, and `inception-review.tar.gz` project snapshot. Source paths, names, and default ports above were checked against that archive. Modify the example port to match the evaluator's instructions, and verify it against the submitted repository and live VM.
