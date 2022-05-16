#include <Signals.hpp>

/*
 * If a signal occurs then the signal handler sets
 * the m_is_signalled flag in the Config singleton
 * instance. Both .setSignalled and .isSignalled are
 * protected by a mutex and can be used to interact
 * with this flag. The threads use .isSignalled to
 * check the flag.
 */
void            signal_handler(int sig)
{
    ConfigUtil  *util;

    util = &ConfigUtil::getHandle();
    util->setSignalled(sig);
    fprintf(stdout, "[INFO] Server %s received\n", strsignal(sig));
}

void            register_signals(void)
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGHUP, signal_handler);
    std::signal(SIGQUIT, signal_handler);
}
