# Inception — User Guide

## 1. What this project provides

Inception runs a WordPress website inside a Debian virtual machine using three Docker containers:

| Service | Purpose |
| --- | --- |
| NGINX | The HTTPS entry point. It accepts connections on port **443** using TLS 1.2 or 1.3. |
| WordPress + PHP-FPM | Serves the website and administration interface. |
| MariaDB | Stores WordPress posts, accounts, and other application data. |

The services communicate over a private Docker bridge network. Only NGINX publishes a port on the Docker host.

The MariaDB database and WordPress files are stored in **Docker named volumes**, independently of the container lifecycles.

## 2. Starting and stopping the website

Run these commands **inside the Debian virtual machine**, not in the school computer's Ubuntu terminal:

```bash
cd /home/mishimod/inception
make              # Build (if necessary) and start the services
```

Check their status:

```bash
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
```

The `mariadb`, `wordpress`, and `nginx` containers should each show `Up`.

Stop the application:

```bash
make down
```

Rebuild and restart it without deleting its named volumes:

```bash
make re
```

Docker Engine is configured to start when Debian boots, and the three services use the `always` restart policy. After a VM restart, check their status **before** manually running `make`.

**Important:** Do not use `docker compose down -v` or delete the named volumes unless you intentionally want to erase stored website data.

## 3. Accessing the website and administration panel

| Destination | Address |
| --- | --- |
| Website | https://mishimod.42.fr/ |
| WordPress administration | https://mishimod.42.fr/wp-admin/ |

The domain must resolve to the Debian VM's local IP address. In the current VirtualBox configuration, the host-only network uses:

- School PC (host): `192.168.56.1`
- Debian VM (guest): `192.168.56.10`

On a computer where you are permitted to edit its hosts file, map `mishimod.42.fr` to `192.168.56.10`.

### School PCs without permission to edit `/etc/hosts`

On the **school PC**, launch a separate Chrome profile with a browser-only hostname override:

```bash
mkdir -p /goinfre/mishimod/chrome-inception

google-chrome \
  --user-data-dir=/goinfre/mishimod/chrome-inception \
  --host-resolver-rules="MAP mishimod.42.fr 192.168.56.10" \
  'https://mishimod.42.fr/'
```

This is a workaround for the school PC, **not** a system-wide DNS configuration. A different school PC may need its own host-only adapter (`vboxnet0`) and browser configuration.

The NGINX image creates a self-signed certificate for local development. The browser may display a certificate warning; only proceed if you have verified that you are connecting to your own VM. This certificate is not intended for public production hosting.

## 4. Accounts and credentials

The initial WordPress accounts are:

| Account | Role | Password source |
| --- | --- | --- |
| `siteowner` | Administrator | `secrets/credentials.txt` |
| `writer` | Author | `secrets/wp_user_password.txt` |

Additional application secrets are stored locally in the project directory:

| File | Purpose |
| --- | --- |
| `secrets/db_password.txt` | MariaDB password used by WordPress |
| `secrets/db_root_password.txt` | MariaDB root password |

To inspect a password, connect to the VM using SSH, then read **only the required file** locally, for example:

```bash
ssh -p 2222 mishimod@127.0.0.1   # From the school PC
cd ~/inception
cat secrets/credentials.txt         # Inside the VM: administrator password
```

Do not paste passwords into Git, screenshots, chats, or shared logs. Both `secrets/` and `srcs/.env` are excluded by `.gitignore`.

**Changing a secret file alone does not change an existing WordPress or MariaDB account password.** The application credential and the corresponding secret must be updated consistently; see the developer documentation before rotating credentials.

## 5. Checking that everything works

Inside the VM, inspect service status:

```bash
cd ~/inception
sudo docker compose -p inception -f srcs/docker-compose.yml ps -a
```

Check HTTPS locally from the VM:

```bash
curl -kI \
  --resolve mishimod.42.fr:443:127.0.0.1 \
  https://mishimod.42.fr/
```

`HTTP/1.1 200 OK` is the expected response for the website. `-k` is used **only** because the local certificate is self-signed.

If a service fails, inspect its recent logs, taking care not to share any passwords that may appear in historical output:

```bash
sudo docker compose -p inception -f srcs/docker-compose.yml \
  logs --tail=30 mariadb wordpress nginx
```

To verify persisted content, open WordPress administration and check that:

- The draft post **Inception Persistence Test** is still available under **Posts**.
- The previously uploaded image is still available under **Media → Library** and opens successfully.

These checks were performed successfully after container recreation and a VM reboot.

## 6. Using the VM from another school PC

The VM files are kept on the shared school storage (`sgoinfre`), but **VirtualBox registration, host-only networking, and browser hostname overrides are specific to each school PC**.

1. Shut down Debian normally on the current PC; do not run the same VM from two PCs simultaneously.
2. On the new PC, open the shared VM's `.vbox` file in VirtualBox if it is not already registered.
3. Ensure Adapter 1 is NAT and Adapter 2 is attached to a compatible host-only network, typically `vboxnet0`.
4. Start the VM; verify that Debian has `192.168.56.10` on its host-only interface.
5. Use the dedicated Chrome command above if normal DNS resolution is unavailable.

The host-only IP values are specific to the current setup; adjust them if the network configuration changes.

## 7. Troubleshooting

| Symptom | What to check |
| --- | --- |
| Website does not open | Is the VM running? Does `docker compose ps -a` show all services as `Up`? Is the host-only network attached? |
| Domain cannot be resolved | Check the local hosts mapping or the dedicated Chrome hostname override. |
| `HTTP 500` from WordPress | Check WordPress/PHP-FPM logs and whether the `www-data` user can read `/run/wp-db-password`. |
| Administration login fails | Check the account name and current password; changing a secret file does not automatically update an existing account. |
| Data disappears after recreation | Check that the correct named volumes exist and that they were not deleted with `-v`. |

For installation, network details, Docker settings, and volume management, see `DEV_DOC.md`.
