FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN set -eux; \
	for attempt in 1 2 3; do \
		apt-get -o Acquire::Retries=3 update \
		&& apt-get -o Acquire::Retries=3 install -y --no-install-recommends \
			bash \
			g++-13 \
			make \
			netcat-openbsd \
			procps \
			python3 \
			valgrind \
		&& break; \
		if [ "$attempt" = "3" ]; then exit 1; fi; \
		apt-get clean; \
		rm -rf /var/lib/apt/lists/*; \
		sleep 5; \
	done; \
	ln -sf /usr/bin/g++-13 /usr/local/bin/c++; \
	ln -sf /usr/bin/g++-13 /usr/local/bin/g++; \
	rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["bash"]
