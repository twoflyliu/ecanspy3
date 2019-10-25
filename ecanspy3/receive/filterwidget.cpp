#include "filterwidget.h"
#include "mainwindow.h"
#include <QCheckBox>

FilterWidget::FilterWidget(QWidget *parent)
    : TitleTableViewWidget(parent)
{
    initTitle();
    setMinimumWidth(300);

    initButtons(static_cast<QVBoxLayout*>(this->layout()));
    initModel();

    initFilters();
}

void FilterWidget::initButtons(QVBoxLayout *layout)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout;

    hboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

    // 添加按钮
    addButton = new QPushButton(tr("Add"), this);
    addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addFilter()));
    hboxLayout->addWidget(addButton);

    // 删除按钮
    removeButton = new QPushButton(tr("Remove"), this);
    removeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeFilter()));
    hboxLayout->addWidget(removeButton);

    // 选中所有选项按钮
    toggleAllCheckButton = new QPushButton(tr("Toggle"), this);
    toggleAllCheckButton->setCheckable(true);
    toggleAllCheckButton->setChecked(false);
    connect(toggleAllCheckButton, SIGNAL(clicked(bool)), this, SLOT(onToggleAllCheckbox(bool)));
    hboxLayout->addWidget(toggleAllCheckButton);

    QSpacerItem *item = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    hboxLayout->addSpacerItem(item);

    layout->insertLayout(1, hboxLayout);
}

void FilterWidget::initModel()
{
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    model = new FilterTableModel(this);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
}

void FilterWidget::initFilters()
{
    for (int i = 0; i < DEFUALT_FILTER_COUNT; ++i) {
        addFilter();
    }
}

void FilterWidget::clear()
{
    model->clear();
    checkBoxList.clear();
}

QList<FilterTableModel::Entity> *FilterWidget::getFilterList()
{
    return model->getFilterList();
}

void FilterWidget::addFilter(UINT idStart, UINT idEnd, bool checked)
{
    addFilter();
    int rowIndex = model->rowCount() - 1;
    model->setData(model->index(rowIndex, 0), QString("%1").arg(idStart, 0, 16), Qt::EditRole);
    model->setData(model->index(rowIndex, 1), QString("%1").arg(idEnd, 0, 16), Qt::EditRole);
    model->setChecked(rowIndex, checked);
    checkBoxList[rowIndex]->setChecked(checked);
}

void FilterWidget::addFilter()
{
    if (model->insertRow(model->rowCount())) {
        QCheckBox *checkbox = new QCheckBox(this);
        checkBoxList.append(checkbox);
        tableView->setIndexWidget(model->index(model->rowCount()-1, 2), checkbox);
        checkbox->setProperty("row", model->rowCount()-1);
        connect(checkbox, SIGNAL(clicked(bool)), this, SLOT(onCheckboxClicked(bool)));
        UPDATE_DATAChANGED_FLAG();
    }
}

void FilterWidget::removeFilter()
{
    if (tableView->currentIndex().isValid()) {
        checkBoxList.removeAt(tableView->currentIndex().row());
        model->removeRow(tableView->currentIndex().row());
        UPDATE_DATAChANGED_FLAG();
    }
}

void FilterWidget::onCheckboxClicked(bool enabled)
{
    bool ok = false;
    int row = sender()->property("row").toInt(&ok);
    if (ok) {
        model->setChecked(row, enabled);
        UPDATE_DATAChANGED_FLAG();
    }
}

void FilterWidget::onToggleAllCheckbox(bool enabled)
{
    for (int row = 0; row < checkBoxList.size(); row++) {
        model->setChecked(row, enabled);
        checkBoxList[row]->setChecked(enabled);
    }
    UPDATE_DATAChANGED_FLAG();
}

void FilterWidget::setFilterList(Document *doc)
{
#if SYNC_DOCUMENT_MESSAGE_ID_FILTER_ENABLED
    if (oldDoc == doc) return;

    clear();
    oldDoc = doc;

    if (doc) {
        foreach (const Message *msg, doc->GetMsgList()) {
            if (msg) {
                addFilter(msg->getId(), msg->getId(), false);
            }
        }
    }
#else
    Q_UNUSED(doc);
#endif
}

void FilterWidget::loadFilterEntList(QList<FilterTableModel::Entity> entList)
{
    clear();
    foreach (const FilterTableModel::Entity &entity, entList) {
        addFilter(entity.idStart, entity.idEnd, entity.enabled);
    }
}

void FilterWidget::initTitle()
{
    title->setText(tr("过滤器"));
}
