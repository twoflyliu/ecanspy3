#include "aboutecanspy3.h"
#include "ui_aboutecanspy3.h"

AboutEcanSpy3Diaglog::AboutEcanSpy3Diaglog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutEcanSpy3)
{
    ui->setupUi(this);
}

AboutEcanSpy3Diaglog::~AboutEcanSpy3Diaglog()
{
    delete ui;
}

void AboutEcanSpy3Diaglog::on_okPushButton_clicked()
{
    close();
}
