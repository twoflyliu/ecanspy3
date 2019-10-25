#ifndef CANCONFIGDIALOG_H
#define CANCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class CanConfigDialog;
}

class CanConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CanConfigDialog(QWidget *parent = 0);
    ~CanConfigDialog();

    QString getBaurate();

private slots:
    void on_okPushButton_clicked();

private:
    void initBaurateCombo();

private:
    Ui::CanConfigDialog *ui;
};

#endif // CANCONFIGDIALOG_H
