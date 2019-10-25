#include "statuswidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTimer>

StatusWidget::StatusWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    initCanBusStatus(vboxLayout);
    initErrorCounter(vboxLayout);
    initControlStatus(vboxLayout);
    vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout->addLayout(vboxLayout);
    setLayout(layout);

    initUpdateTimer();
}

void StatusWidget::initErrorCounter(QLayout *layout)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    QGroupBox *counterGroupBox = new QGroupBox(tr("bus error count"), this);
    counterGroupBox->setLayout(vboxLayout);

    sendErrCnt = new QLabel(tr("receive: 0"), this);
    sendErrCnt->setEnabled(false);
    vboxLayout->addWidget(sendErrCnt);

    receiveErrCnt = new QLabel(tr("send: 0"), this);
    receiveErrCnt->setEnabled(false);
    vboxLayout->addWidget(receiveErrCnt);

    //vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout->addWidget(counterGroupBox);
}

void StatusWidget::initCanBusStatus(QLayout *layout)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    QGroupBox *counterGroupBox = new QGroupBox(tr("bus status"), this);
    counterGroupBox->setLayout(vboxLayout);

    busNormal = new QCheckBox(tr("bus normal"), this);
    busNormal->setEnabled(false);
    vboxLayout->addWidget(busNormal);

    passiveErr = new QCheckBox(tr("passive error"), this);
    passiveErr->setEnabled(false);
    vboxLayout->addWidget(passiveErr);

    activeErr = new QCheckBox(tr("active error"), this);
    activeErr->setEnabled(false);
    vboxLayout->addWidget(activeErr);

    busOff = new QCheckBox(tr("bus off"), this);
    busOff->setEnabled(false);
    vboxLayout->addWidget(busOff);

    //vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout->addWidget(counterGroupBox);
}

void StatusWidget::initControlStatus(QLayout *layout)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    QGroupBox *counterGroupBox = new QGroupBox(tr("control status"), this);
    counterGroupBox->setLayout(vboxLayout);

    receiveRegFull = new QCheckBox(tr("receive reg full"), this);
    receiveRegFull->setEnabled(false);
    vboxLayout->addWidget(receiveRegFull);

    receiveRegOv = new QCheckBox(tr("receive reg overflow"), this);
    receiveRegOv->setEnabled(false);
    vboxLayout->addWidget(receiveRegOv);

    controllerErrWarn = new QCheckBox(tr("controller error warning"), this);
    controllerErrWarn->setEnabled(false);
    vboxLayout->addWidget(controllerErrWarn);

    controllerFIFOOv = new QCheckBox(tr("controller fifo overflow"), this);
    controllerFIFOOv->setEnabled(false);
    vboxLayout->addWidget(controllerFIFOOv);

    //vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout->addWidget(counterGroupBox);
}

void StatusWidget::initUpdateTimer()
{
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    updateTimer->start(UPDATE_PERIOD_SECOND*1000);
}

void StatusWidget::updateStatus()
{
    updateErrCnt();
    updateBusStatus();
}

void StatusWidget::updateErrCnt()
{
    CAN_STATUS status = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (!UsbCanUtil::GetInstance().ReadCANStatus(&status)) return;

    sendErrCnt->setText(QString("send: %1").arg(status.regTECounter));
    receiveErrCnt->setText(QString("receive: %1").arg(status.regRECounter));
}

void StatusWidget::updateBusStatus()
{
    ERR_INFO errInfo;
    if (!UsbCanUtil::GetInstance().ReadErrorInfo(errInfo)) return;
    activeErr->setChecked(ERR_CAN_ZHUDONG & errInfo.ErrCode);
    passiveErr->setChecked(ERR_CAN_PASSIVE & errInfo.ErrCode);
    busOff->setChecked(ERR_CAN_BUSERR & errInfo.ErrCode);
    busNormal->setChecked(!(activeErr->isChecked() && passiveErr->isChecked() && busOff->isChecked()));

    updateControllerStatus(&errInfo);
}

void StatusWidget::updateControllerStatus(ERR_INFO *err)
{
    receiveRegFull->setChecked(err->ErrCode&ERR_CAN_REG_FULL);
    receiveRegOv->setChecked(err->ErrCode&ERR_CAN_REG_OVER);
    controllerErrWarn->setChecked(err->ErrCode&ERR_CAN_ERRALARM);
    controllerFIFOOv->setChecked(err->ErrCode&ERR_CAN_OVERFLOW);
}
