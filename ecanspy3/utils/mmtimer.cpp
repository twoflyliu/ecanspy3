#include "mmtimer.h"

#ifdef __MINGW32__ //w32api bug
#define TIME_KILL_SYNCHRONOUS 0x0100
#endif

void WINAPI CALLBACK mmtimer_proc(uint, uint, DWORD_PTR user, DWORD_PTR, DWORD_PTR)
{
    MMTimer *t = reinterpret_cast<MMTimer*>(user);
    emit t->timeout();
}

MMTimer::MMTimer(int interval, QObject *parent)
    : QObject(parent)
{
    this->interval = interval;
    this->id = 0;
}

void MMTimer::start()
{
    this->id = timeSetEvent(this->interval, 1, mmtimer_proc, (DWORD_PTR)this,
                            TIME_CALLBACK_FUNCTION | TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
}

void MMTimer::stop()
{
    if (this->id) {
        timeKillEvent(this->id);
        this->id = 0;
    }
}
