#ifndef THEMESETTINGDIALOG_H
#define THEMESETTINGDIALOG_H

#include <QDialog>
#include <QMap>
#include <QString>
#include <QRadioButton>

namespace Ui {
class ThemeSettingDialog;
}

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ThemeSettingDialog : public QDialog
{
    Q_OBJECT

public:
    struct Entity
    {
        QString          themeName;
        QString          themeStyleSheet;
        QString          themeSnapshot;
        QLabel           *showLabel;
        QRadioButton     *radioButton;
    };

    explicit ThemeSettingDialog(QWidget *parent = 0);
    ~ThemeSettingDialog();

    QString getTheme() const;
    void setTheme(QString themeName);
    QString getThemeStyleSheet(QString themeName);
    QString getCurrThemeStyleSheet();

private slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();
    void on_defaultRadioButton_clicked(bool checked);
    void on_blackRadioButton_clicked(bool checked);

private:
    void initTheme(QLabel *label, QRadioButton *radio,
                   QString themeName, QString themeStylesheet,
                   QString themeSnapshot);
    void updateSnapshot(QLabel *showLabel, QString themeSnapshot);

private:
    Ui::ThemeSettingDialog *ui;

    QMap<QString, Entity> themeMap;
    QString currTheme;
};

#endif // THEMESETTINGDIALOG_H
