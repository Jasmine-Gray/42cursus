#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void fatal(const char *msg) {
    perror(msg);
    exit(1);
}

void send_char(pid_t pid, char c) {
    int bit;
    int i;
    unsigned char uc;
    uc = (unsigned char)c;
    i = 0;
    while (i < 8) {
        usleep(50);
        bit = (uc >> i) & 0x01;
        if (kill(pid, SIGUSR1 + bit) == -1)
            fatal("kill error");
        i++;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pid> <message>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    char *message = argv[2];

    while (*message) {
        send_char(pid, *message);
        message++;
    }

    return 0;
}
