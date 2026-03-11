#include "Signal.hpp"

volatile sig_atomic_t stop_sig = 0;

void signal_handler(int sig){
    if (sig == SIGINT || sig == SIGTERM)
        stop_sig = 1;
}

void    init_signal_handler(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
}