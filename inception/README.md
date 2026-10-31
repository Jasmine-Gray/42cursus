*This project has been created as part of the 42 curriculum by mishimod.*

# Inception

## Description

Inception is a system administration project focused on building a containerized web infrastructure using Docker inside a virtual machine.

The project consists of three services:

- **NGINX**: Handles HTTPS requests using TLS 1.2 and TLS 1.3.
- **WordPress + PHP-FPM**: Runs the WordPress application with PHP 8.2.
- **MariaDB**: Stores WordPress data.

Each service runs in a dedicated Docker container built from Debian 12. Docker Compose manages the services, network, and persistent volumes.

### Architecture

```text
Browser
   |
   | HTTPS :443
   v
NGINX
   |
   | FastCGI :9000
   v
WordPress + PHP-FPM
   |
   | MariaDB :3306
   v
MariaDB
```

Only NGINX publishes a port on the Docker host. The containers communicate through a dedicated Docker bridge network.

## Design Choices

### Virtual Machines vs Docker

A virtual machine runs its own operating system and kernel. Docker containers share the host's Linux kernel while providing isolated application environments.

This project uses a Debian virtual machine to host the Docker infrastructure.

### Docker Secrets vs Environment Variables

Environment variables are used for non-sensitive application configuration, including database names and domain names.

Docker Compose secrets are used for confidential values such as database and WordPress passwords. These secrets are provided through files mounted under `/run/secrets/` inside the containers.

### Docker Network vs Host Network

A dedicated Docker bridge network allows the three services to communicate using their service names.

Host networking is not used. MariaDB and WordPress do not publish ports to the Docker host.

### Docker Volumes vs Bind Mounts

Docker named volumes preserve data independently of container lifecycles.

This project uses two named volumes:

- `mariadb_volume`: stores MariaDB data.
- `wordpress_volume`: stores WordPress files and uploads.

The Docker data root is configured so the volume data is stored under `/home/mishimod/data` inside the virtual machine.

## Instructions

### Prerequisites

- A Linux virtual machine
- Docker Engine
- Docker Compose
- GNU Make
- Project environment variables and secret files

See `DEV_DOC.md` for environment setup instructions.

### Build and Run

```bash
make
```

This command builds the Docker images and starts all three containers.

### Stop

```bash
make down
```

### Rebuild and Restart

```bash
make re
```

The regular restart and rebuild commands preserve the named volumes.

### Access

WordPress website:

`https://mishimod.42.fr`

WordPress administration:

`https://mishimod.42.fr/wp-admin/`

The domain must resolve to the virtual machine's local IP address. A self-signed TLS certificate is used for local development.

For additional usage instructions, refer to `USER_DOC.md`.

## Verification

The infrastructure has been tested for:

- HTTPS connectivity
- TLS 1.2 and TLS 1.3 support
- Rejection of TLS 1.0 and TLS 1.1
- Data persistence after container recreation
- Automatic recovery after virtual machine restart
- Automatic NGINX container restart after abnormal termination

## Resources

- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Documentation](https://docs.docker.com/compose/)
- [Debian Documentation](https://www.debian.org/doc/)
- [NGINX Documentation](https://nginx.org/en/docs/)
- [WP-CLI Documentation](https://wp-cli.org/)

### AI Usage

AI tools were used to explain Docker and virtualization concepts, suggest configuration and initialization script changes, troubleshoot networking and permission problems, and assist with documentation.

The generated suggestions were reviewed and tested through Docker builds, service checks, HTTPS requests, persistence tests, and restart tests.
