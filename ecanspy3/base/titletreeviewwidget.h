#ifndef TITLETREEVIEWWIDGET_H
#define TITLETREEVIEWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>

class TitleTreeViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleTreeViewWidget(QWidget *parent = nullptr);

    void initTitle(QVBoxLayout *);
    void initTreeView(QVBoxLayout *);

signals:

public slots:

protected:
    QLabel *title;
    QTreeView *treeView;
};

#endif // TITLETREEVIEWWIDGET_H
