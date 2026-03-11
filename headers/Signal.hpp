#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <signal.h>
#include <cstring>

extern volatile sig_atomic_t stop_sig;

void signal_handler(int sig);
void init_signal_handler();

#endif