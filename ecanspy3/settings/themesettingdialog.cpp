#include "themesettingdialog.h"
#include "ui_themesettingdialog.h"

#include <QDebug>

#define THEME_FILE_DEFAULT      tr("")
#define THEME_NAME_DEFAULT      tr("default")
#define THEME_SNAPSHOT_DEFAULT  tr(":/themes/white_snapshot.PNG")

#define THEME_FILE_BLACK        tr(":/themes/dracula.qss")
#define THEME_NAME_BLACK        tr("black")
#define THEME_SNAPSHOT_BLACK    tr(":/themes/black_snapshot.png")

ThemeSettingDialog::ThemeSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ThemeSettingDialog)
{
    ui->setupUi(this);

    initTheme(ui->defaultThemeLabel, ui->defaultRadioButton,
              THEME_NAME_DEFAULT, THEME_FILE_DEFAULT, THEME_SNAPSHOT_DEFAULT);
    initTheme(ui->blackThemeLabel, ui->blackRadioButton,
              THEME_NAME_BLACK, THEME_FILE_BLACK, THEME_SNAPSHOT_BLACK);
}

ThemeSettingDialog::~ThemeSettingDialog()
{
    delete ui;
}

void ThemeSettingDialog::initTheme(QLabel *label, QRadioButton *radio,
                                   QString themeName, QString themeStylesheet,
                                   QString themeSnapshot)
{
    Entity entity;
    entity.themeName = themeName;
    entity.themeStyleSheet = themeStylesheet;
    entity.showLabel = label;
    entity.radioButton = radio;
    entity.themeSnapshot = themeSnapshot;
    themeMap.insert(themeName, entity);

    updateSnapshot(label, themeSnapshot);
}

void ThemeSettingDialog::updateSnapshot(QLabel *showLabel, QString themeSnapshot)
{
    if (!showLabel) return;

    int width = showLabel->width();
    int height = showLabel->height();

    QPixmap px;
    px.load(themeSnapshot);

    showLabel->setPixmap(px.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

QString ThemeSettingDialog::getTheme() const
{
    return this->currTheme;
}

void ThemeSettingDialog::setTheme(QString themeName)
{
    auto iter = themeMap.find(themeName);
    if (iter != themeMap.end()) {
        this->currTheme = themeName;
        auto &entity = iter.value();
        if (entity.radioButton) {
            entity.radioButton->setChecked(true);
        }
    }
}

QString ThemeSettingDialog::getThemeStyleSheet(QString themeName)
{
    auto iter = themeMap.find(themeName);
    if (iter != themeMap.end()) {
        qDebug() << tr("ThemeSettingDialog::getThemeStyleSheet -> [%1]").arg(iter.value().themeStyleSheet);
        return iter.value().themeStyleSheet;
    }

    qDebug() << tr("ThemeSettingDialog::getThemeStyleSheet -> [%1]").arg(THEME_FILE_DEFAULT);
    return THEME_FILE_DEFAULT;
}

QString ThemeSettingDialog::getCurrThemeStyleSheet()
{
    return getThemeStyleSheet(this->currTheme);
}

void ThemeSettingDialog::on_okPushButton_clicked()
{
    accept();
}

void ThemeSettingDialog::on_cancelPushButton_clicked()
{
    reject();
}

void ThemeSettingDialog::on_defaultRadioButton_clicked(bool checked)
{
    if (checked) {
        this->currTheme = THEME_NAME_DEFAULT;
    }
}

void ThemeSettingDialog::on_blackRadioButton_clicked(bool checked)
{
    if (checked) {
        this->currTheme = THEME_NAME_BLACK;
    }
}
