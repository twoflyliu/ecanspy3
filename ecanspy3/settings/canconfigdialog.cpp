#include "canconfigdialog.h"
#include "ui_canconfigdialog.h"

#include "UsbCanUtil.h"

CanConfigDialog::CanConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CanConfigDialog)
{
    ui->setupUi(this);
    initBaurateCombo();
}

CanConfigDialog::~CanConfigDialog()
{
    delete ui;
}

QString CanConfigDialog::getBaurate()
{
    return ui->baurateComboBox->currentText();
}

void CanConfigDialog::on_okPushButton_clicked()
{
    accept();
}

void CanConfigDialog::initBaurateCombo()
{
    QList<QString> supportedBaurateList = UsbCanUtil::GetInstance().GetSupportedBaurateList();
    foreach (QString baurate, supportedBaurateList) {
        ui->baurateComboBox->addItem(baurate);
    }

    int index = supportedBaurateList.indexOf(UsbCanUtil::GetInstance().GetBaurate());
    ui->baurateComboBox->setCurrentIndex(index);
}
