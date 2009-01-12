
#include <QtGui>
#include "EditGroupDlg.h"

#include "common.h"
#include "OperationWidget.h"
#include "EditDlg.h"
#include "FinDatabase.h"
#include "FinCompletion.h"

EditGroupDlg::EditGroupDlg(QWidget *parent, int id_)
    : QDialog(parent), id_group(id_)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    
    m_owOpr = new OperationWidget;
    mainLayout->addWidget(m_owOpr);
    
    tree = new QTreeWidget;
    tree->setColumnCount(5);
    QStringList labels;
    labels << tr("Date") << tr("Sum") << tr("Amount") << tr("Discount") << tr("Describtion");
    tree->setHeaderLabels(labels);
    connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
    
    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addWidget(tree, 1);
    mainLayout->addLayout(buttonLayout, 0);

    setWindowTitle(tr("Operation"));

    onNewRecord = new QAction(QIcon(":add"), tr("Add operation"), this);
    connect(onNewRecord, SIGNAL(triggered()), this, SLOT(newRecord()));

    onEditRecord = new QAction(QIcon(":edit"), tr("Edit operation"), this);
    connect(onEditRecord, SIGNAL(triggered()), this, SLOT(editRecord()));

    onDeleteRecord = new QAction(QIcon(":delete"), tr("Delete operation"), this);
    connect(onDeleteRecord, SIGNAL(triggered()), this, SLOT(deleteRecord()));
    
    QList<QTreeWidgetItem*> items;
    FinDatabase::Instance()->loadGroupContent(id_group, items);
    for (QList<QTreeWidgetItem*>::iterator iter = items.begin(); iter != items.end(); iter++)
        tree->addTopLevelItem(*iter);
    for (int i=0; i<tree->columnCount(); i++)
        tree->resizeColumnToContents(i);
    resize(400, 300);
}

Operation EditGroupDlg::operation() const
{
    return m_owOpr->operation();
}

void EditGroupDlg::setOperation(const Operation& opr)
{
    m_owOpr->setOperation(opr);
}

void EditGroupDlg::onOk()
{
    if (!m_owOpr->checkValues())
    {
        QMessageBox::information(this, tr("Error"), tr("Nonvalid data"), QMessageBox::Ok);
        return;
    }
    accept();
}

void EditGroupDlg::itemDoubleClicked(QTreeWidgetItem*, int)
{
    editRecord();
}

void EditGroupDlg::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(onNewRecord);
    menu.addAction(onEditRecord);
    menu.addAction(onDeleteRecord);
    menu.exec(event->globalPos());
}

void EditGroupDlg::newRecord()
{
    EditDlg dlg(this);
    Operation opr;
    opr.rdate = m_owOpr->operation().rdate;
    dlg.setOperation(opr);
    
    if (dlg.exec() != QDialog::Accepted)
        return;

    opr = dlg.operation();
    qDebug() << opr;

    QTreeWidgetItem *item = new QTreeWidgetItem(opr.list());
    item->setData(0, Qt::UserRole, 0);
    tree->addTopLevelItem(item);

    setValue(countSum());

    for (int i=0; i<tree->columnCount(); i++)
        tree->resizeColumnToContents(i);
}

void EditGroupDlg::editRecord()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item)
        return;

    EditDlg dlg(this);
    Operation opr;
    opr.value = item->data(1, Qt::DisplayRole).toDouble();
    opr.amount = item->data(2, Qt::DisplayRole).toDouble();
    opr.rdate = item->data(0, Qt::DisplayRole).toDate();
    opr.discount = item->data(3, Qt::DisplayRole).toDouble();
    opr.notes = item->data(4, Qt::DisplayRole).toString();
    dlg.setOperation(opr);
    if (dlg.exec() != QDialog::Accepted)
        return;
    
    opr = dlg.operation();

    item->setData(0, Qt::DisplayRole, opr.rdate.toString(Qt::ISODate));
    item->setData(1, Qt::DisplayRole, QVariant(opr.value).toString());
    item->setData(2, Qt::DisplayRole, QVariant(opr.amount).toString());
    item->setData(3, Qt::DisplayRole, QVariant(opr.discount).toString());
    item->setData(4, Qt::DisplayRole, opr.notes);

    setValue(countSum());
}

void EditGroupDlg::deleteRecord()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item)
        return;

    qDebug() << "deleting row: " << tree->currentIndex().row()
        << " record id: " << item->data(0, Qt::UserRole).toInt();

    QMessageBox mb(tr("Delete record"), tr("Are you sure?"), QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, 0, this);
    if (mb.exec() != QMessageBox::Yes)
        return;
    int id = item->data(0, Qt::UserRole).toInt();
    if (id)
        removedIds.append(id);
    delete item;

    setValue(countSum());
}

Operations EditGroupDlg::operations() const
{
    Operations oprs;
    QList<QTreeWidgetItem*> items = tree->findItems(".*", Qt::MatchRegExp);
    for (QList<QTreeWidgetItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
        Operation opr;
        opr.id = (*it)->data(0, Qt::UserRole).toInt();
        opr.rdate = (*it)->data(0, Qt::DisplayRole).toDate();
        opr.value = (*it)->data(1, Qt::DisplayRole).toDouble();
        opr.amount = (*it)->data(2, Qt::DisplayRole).toDouble();
        opr.discount = (*it)->data(3, Qt::DisplayRole).toDouble();
        opr.notes = (*it)->data(4, Qt::DisplayRole).toString();
        oprs.push_back(opr);
    }
    return oprs;
}

QList<int> EditGroupDlg::getIdsForDelete()
{
    return removedIds;
}

void EditGroupDlg::setValue(double value)
{
    Operation opr = m_owOpr->operation();
    opr.value = value;
    m_owOpr->setOperation(opr);
}

double EditGroupDlg::countSum()
{
    QList<QTreeWidgetItem*> items = tree->findItems(".*", Qt::MatchRegExp);
    double res = 0;
    for (QList<QTreeWidgetItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
        double value = (*it)->data(1, Qt::DisplayRole).toDouble();
        double amount = (*it)->data(2, Qt::DisplayRole).toDouble();
        double discount = (*it)->data(3, Qt::DisplayRole).toDouble();
        double tmp = value * amount;
        if (discount)
            tmp *= (100. - discount) / 100.;
        tmp = qRound(tmp * 100.) / 100.;
        res += tmp;
    }
    return res;
}

