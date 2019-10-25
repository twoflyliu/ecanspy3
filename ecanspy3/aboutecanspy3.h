#ifndef ABOUTECANSPY3_H
#define ABOUTECANSPY3_H

#include <QDialog>

namespace Ui {
class AboutEcanSpy3;
}

class AboutEcanSpy3Diaglog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutEcanSpy3Diaglog(QWidget *parent = 0);
    ~AboutEcanSpy3Diaglog();

private slots:
    void on_okPushButton_clicked();

private:
    Ui::AboutEcanSpy3 *ui;
};

#endif // ABOUTECANSPY3_H
