#ifndef MMTIMER_H
#define MMTIMER_H

#include <windows.h>
#include <mmsystem.h>

#include <QObject>

class MMTimer : public QObject
{
    Q_OBJECT
public:
    friend void WINAPI CALLBACK mmtimer_proc(uint, uint, DWORD_PTR user, DWORD_PTR, DWORD_PTR);

    explicit MMTimer(int interval, QObject *parent=nullptr);

signals:
    void timeout();

public slots:
    void start();
    void stop();

private:
    int interval;
    int id;
};

#endif // MMTIMER_H
