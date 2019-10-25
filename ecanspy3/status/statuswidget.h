#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include "UsbCanUtil.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QCheckBox;
class QHBoxLayout;
class QVBoxLayout;
class QTimer;
QT_END_NAMESPACE

class StatusWidget : public QWidget
{
    Q_OBJECT
public:
    enum {
        UPDATE_PERIOD_SECOND = 1,
    };

    explicit StatusWidget(QWidget *parent = nullptr);

    void initErrorCounter(QLayout *layout);
    void initCanBusStatus(QLayout *layout);
    void initControlStatus(QLayout *layout);
    void initUpdateTimer();

signals:

public slots:
    void updateStatus();
    void updateErrCnt();
    void updateBusStatus();
    void updateControllerStatus(ERR_INFO *err);

private:
    QTimer *updateTimer;

    QLabel *sendErrCnt;
    QLabel *receiveErrCnt;

    QCheckBox *busNormal;
    QCheckBox *passiveErr;
    QCheckBox *activeErr;
    QCheckBox *busOff;

    QCheckBox *receiveRegFull;
    QCheckBox *receiveRegOv;
    QCheckBox *controllerErrWarn;
    QCheckBox *controllerFIFOOv;
};

#endif // STATUSWIDGET_H
