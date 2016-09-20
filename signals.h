#include <signal.h>
typedef void handler_t(int);

extern int strikes;

void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);
handler_t *Signal(int signum, handler_t *handler);
