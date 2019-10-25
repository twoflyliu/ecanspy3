#ifndef TITLETABLEVIEWWIDGET_H
#define TITLETABLEVIEWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTableView>
#include <QVBoxLayout>

class TitleTableViewWidget : public QWidget
{
    Q_OBJECT
public:
    TitleTableViewWidget(QWidget *parent = nullptr);

    void initTitle(QVBoxLayout *);
    void initTableView(QVBoxLayout *);

signals:

public slots:

protected:
    QLabel *title;
    QTableView *tableView;
};

#endif // TITLETABLEVIEWWIDGET_H
