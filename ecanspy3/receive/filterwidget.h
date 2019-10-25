#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include "base/titletableviewwidget.h"
#include "receive/filtertablemodel.h"

#include <dbc4cpp/parser.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>

#define SYNC_DOCUMENT_MESSAGE_ID_FILTER_ENABLED        (0)

class FilterWidget : public TitleTableViewWidget
{
    Q_OBJECT
public:
    enum {
        DEFUALT_FILTER_COUNT = 18
    };

    FilterWidget(QWidget *parent);

    void initButtons(QVBoxLayout *layout);
    void initModel();
    void initFilters();

    void clear();

    CanObjFilter* getCanObjFilter()
    {
        return model->getCanObjFilter();
    }

    QList<FilterTableModel::Entity>* getFilterList();

    FilterTableModel *getFilterTableModel()
    {
        return this->model;
    }

    void addFilter(UINT idStart, UINT idEnd, bool checked);

public slots:
    void addFilter();
    void removeFilter();
    void onCheckboxClicked(bool enabled);
    void onToggleAllCheckbox(bool enabled);

    void setFilterList(Document *doc);

    void loadFilterEntList(QList<FilterTableModel::Entity> entList);

protected:
    void initTitle();

protected:
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *toggleAllCheckButton;

    FilterTableModel *model;
    QList<QCheckBox*> checkBoxList;
    Document *oldDoc = nullptr;
};

#endif // FILTERWIDGET_H
