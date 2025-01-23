// #include <unistd.h>
// #include <stdio.h>
// 
// int main(void)
// {
//     printf("Server PID: %d\n", getpid());
//     // サーバーの残りの処理
// }

// #include <signal.h>
// #include <unistd.h>
// #include <stdio.h>
// 
// void signal_handler(int signum) {
//     // Process the bit based on the signal received
//     static char current_char = 0;
//     static int bit_count = 0;
// 
//     if (signum == SIGUSR1) {
//         // Bit is 0
//         bit_count++;
//     } else if (signum == SIGUSR2) {
//         // Bit is 1
//         current_char |= (1 << (bit_count));
//         bit_count++;
//     }
// 
//     if (bit_count == 8) {
//         write(1, &current_char, 1);
//         current_char = 0;
//         bit_count = 0;
//     }
// }
// 
// int main(void) {
//     printf("Server PID: %d\n", getpid());
// 
//     signal(SIGUSR1, signal_handler);
//     signal(SIGUSR2, signal_handler);
// 
//     while (1) {
//         pause(); // Wait for signals
//     }
// 
//     return 0;
// }

// #include <stdio.h>
// #include <signal.h>
// #include <unistd.h>
// #include <stdlib.h>
// 
// void handle_signal(int signum) {
//     static char current_char = 0;
//     static int bit_count = 0;
// 
//     if (signum == SIGUSR1 || signum == SIGUSR2) {
//         if (signum == SIGUSR2) {
//             current_char |= (1 << bit_count);
//         }
//         bit_count++;
// 
//         if (bit_count == 8) {
//             if (current_char != '\0') {
//                 write(STDOUT_FILENO, &current_char, 1);
//             }
//             current_char = 0;
//             bit_count = 0;
//         }
//     }
// }
// 
// int main(void) {
//     struct sigaction sa;
//     sa.sa_handler = handle_signal;
//     sa.sa_flags = 0;
//     sigemptyset(&sa.sa_mask);
// 
//     if (sigaction(SIGUSR1, &sa, NULL) == -1 ||
//         sigaction(SIGUSR2, &sa, NULL) == -1) {
//         perror("sigaction");
//         exit(1);
//     }
// 
//     printf("Server PID: %d\n", getpid());
//     fflush(stdout);
// 
//     while (1) {
//         pause();
//     }
// 
//     return 0;
// }


#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handle_signal(int signum) {
    static int bit_count = 0;
    static unsigned char current_char = 0;

    if (signum == SIGUSR1 || signum == SIGUSR2) {
        current_char |= ((signum == SIGUSR2) << bit_count);
        bit_count++;

        if (bit_count == 8) {
            write(STDOUT_FILENO, &current_char, 1);
            current_char = 0;
            bit_count = 0;
        }
    }
}

int main(void) {
    pid_t pid = getpid();
    printf("Server PID: %d\n", pid);
    fflush(stdout);

    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);

    while (1) {
        pause();
    }

    return 0;
}
