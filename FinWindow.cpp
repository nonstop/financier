
#include <QtGui>
#include <QtSql>
#include <QDebug>

#include "common.h"

#include "FinWindow.h"
#include "EditDlg.h"
#include "EditGroupDlg.h"
#include "FinDatabase.h"
#include "FinMonthReport.h"
#include "FinPeriodReport.h"
#include "OprTypeDlg.h"


FinWindow::FinWindow()
    : m_settings("Nonstop", "Financier")
{
    QWidget *centralWidget = new QWidget;
    
    mainLayout = new QVBoxLayout;
    
    createActions();
    createMenus();

    hbox = new QHBoxLayout();
    mainLayout->addLayout(hbox);
    deFrom = new QDateEdit(QDate::currentDate().addDays(-5));
    deTo = new QDateEdit(QDate::currentDate());
    bRefresh = new QPushButton(tr("Refresh"));
    bRefresh->setEnabled(false);
    connect(bRefresh, SIGNAL(clicked(bool)), this, SLOT(refresh()));

    hbox->addWidget(deFrom, 1);
    hbox->addWidget(deTo, 1);
    hbox->addWidget(bRefresh, 0);

    tree = new QTreeWidget;
    tree->setColumnCount(5);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QStringList labels;
    labels << tr("Date") << tr("Sum") << tr("Amount") << tr("Discount") << tr("Describtion");
    tree->setHeaderLabels(labels);
    connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemClicked(QTreeWidgetItem*, int)));
    
    mainLayout->addWidget(tree);
    centralWidget->setLayout(mainLayout);

    statusBar = new QStatusBar;
    setStatusBar(statusBar);
    setCountSum();

    setCentralWidget(centralWidget);
    setWindowTitle(tr("Financier"));
    setMinimumSize(160, 160);
    resize(600, 400);

    enableRecordOperations(false);
}

FinWindow::~FinWindow()
{
    FinDatabase::Instance()->closeDatabase();
}

void FinWindow::createActions()
{
    onNewFile = new QAction(QIcon(":new"), tr("New file"), this);
    onNewFile->setShortcut(tr("Ctrl+N"));
    connect(onNewFile, SIGNAL(triggered()), this, SLOT(newFile()));

    onOpenFile = new QAction(QIcon(":open"), tr("Open file"), this);
    onOpenFile->setShortcut(tr("Ctrl+O"));
    connect(onOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));

    onQuit = new QAction(QIcon(":quit"), tr("Quit"), this);
    connect(onQuit, SIGNAL(triggered()), this, SLOT(quit()));

    onNewRecord = new QAction(QIcon(":add"), tr("Add operation"), this);
    connect(onNewRecord, SIGNAL(triggered()), this, SLOT(newRecord()));

    onNewGroupRecord = new QAction(QIcon(":add"), tr("Add group operation"), this);
    connect(onNewGroupRecord, SIGNAL(triggered()), this, SLOT(newGroupRecord()));

    onEditRecord = new QAction(QIcon(":edit"), tr("Edit operation"), this);
    connect(onEditRecord, SIGNAL(triggered()), this, SLOT(editRecord()));

    onDeleteRecord = new QAction(QIcon(":delete"), tr("Delete operation"), this);
    connect(onDeleteRecord, SIGNAL(triggered()), this, SLOT(deleteRecord()));

    onMonthReport = new QAction(QIcon(":monthReport"), tr("Month report"), this);
    connect(onMonthReport, SIGNAL(triggered()), this, SLOT(monthReport()));

    onPeriodReport = new QAction(QIcon(":periodReport"), tr("Period report"), this);
    connect(onPeriodReport, SIGNAL(triggered()), this, SLOT(periodReport()));

    onOprTypes = new QAction(QIcon(":oprTypes"), tr("Operation types"), this);
    connect(onOprTypes, SIGNAL(triggered()), this, SLOT(oprTypes()));
}


void FinWindow::createMenus()
{
    QMenu *menu = menuBar()->addMenu(tr("File"));
    menu->addAction(onNewFile);
    menu->addAction(onOpenFile);
    menu->addSeparator();
    menu->addAction(onQuit);

    menu = menuBar()->addMenu(tr("Reports"));
    menu->addAction(onMonthReport);
    menu->addAction(onPeriodReport);

    menu = menuBar()->addMenu(tr("Options"));
    menu->addAction(onOprTypes);
}

void FinWindow::enableRecordOperations(bool enable)
{
    onNewRecord->setEnabled(enable);
    onNewGroupRecord->setEnabled(enable);
    onEditRecord->setEnabled(enable);
    onDeleteRecord->setEnabled(enable);

    onMonthReport->setEnabled(enable);
    onPeriodReport->setEnabled(enable);
    
    onOprTypes->setEnabled(enable);

    bRefresh->setEnabled(enable);
}

void FinWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(onNewFile);
    menu.addAction(onOpenFile);
    menu.addSeparator(); 
    menu.addAction(onNewRecord);
    menu.addAction(onNewGroupRecord);
    menu.addAction(onEditRecord);
    menu.addAction(onDeleteRecord);
    menu.addSeparator(); 
    menu.addAction(onQuit);

    menu.exec(event->globalPos());
}

void FinWindow::openDatabase(QString &file)
{
    FinDatabase::Instance()->openDatabase(file);
}

void FinWindow::newFile()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter new account file name"),
                                         tr("File name:"), QLineEdit::Normal, "", &ok);
    if (!ok)
        return;
    if (ok && text.isEmpty())
        return;

    FinDatabase::Instance()->createDatabase(text);
    refresh();
}

void FinWindow::openFile()
{
    QString dir = QDir::current().absolutePath();
    if (m_settings.contains("last_dir"))
        dir = m_settings.value("last_dir").toString();
    qDebug() << "last_dir: " << dir;
    QString s = QFileDialog::getOpenFileName(this, tr("Choose a file"), dir);
    if (!s.length())
        return;
    FinDatabase::Instance()->openDatabase(s);
    refresh();
    m_settings.setValue("last_dir", QFileInfo(s).dir().absolutePath());
    qDebug() << QFileInfo(s).dir().absolutePath();
}

void FinWindow::newRecord()
{
    qDebug() << __FUNCTION__;
    EditDlg dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    Operation opr = dlg.operation();
    qDebug() << opr;
    int id = FinDatabase::Instance()->addRecord(opr);

    QTreeWidgetItem *item = new QTreeWidgetItem(opr.list());
    item->setData(0, Qt::UserRole, id);
    tree->addTopLevelItem(item);
    setCountSum();
}

void FinWindow::newGroupRecord()
{
    EditGroupDlg dlg(this, 0);
    if (dlg.exec() != QDialog::Accepted)
        return;
    
    Operation opr = dlg.operation();
    int id = FinDatabase::Instance()->addRecord(opr);
    QList<int> removeIds = dlg.getIdsForDelete();
    for (QList<int>::iterator it = removeIds.begin(); it != removeIds.end(); it++)
    {
        FinDatabase::Instance()->delRecordFromGroup(id, *it);
        FinDatabase::Instance()->delRecord(*it);
    }
    Operations oprs = dlg.operations();
    for (Operations::iterator it = oprs.begin(); it != oprs.end(); it++)
    {
        int newId = FinDatabase::Instance()->addRecord(*it);
        FinDatabase::Instance()->addRecordToGroup(id, newId);
    }
    QTreeWidgetItem *item = new QTreeWidgetItem(opr.list());
    item->setData(0, Qt::UserRole, id);
    tree->addTopLevelItem(item);
    setCountSum();
}

void FinWindow::editRecord()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item)
        return;
    
    int id = item->data(0, Qt::UserRole).toInt();
    qDebug() << "editing: " << id;
    if (FinDatabase::Instance()->isGroupRecord(id))
        editGroupRecord(*item, id);
    else
        editRecord(*item, id);    
    setCountSum();
}

void FinWindow::editGroupRecord(QTreeWidgetItem &item, int id)
{
    EditGroupDlg dlg(this, id);
    Operation opr;
    opr.id = id;
    opr.rdate = item.data(0, Qt::DisplayRole).toDate();
    opr.value = item.data(1, Qt::DisplayRole).toDouble();
    opr.amount = item.data(2, Qt::DisplayRole).toDouble();
    opr.discount = item.data(3, Qt::DisplayRole).toDouble();
    opr.notes = item.data(4, Qt::DisplayRole).toString();
    dlg.setOperation(opr);
    if (dlg.exec() != QDialog::Accepted)
        return;
    qDebug() << "editing group: " << id;
    
    opr = dlg.operation();
    FinDatabase::Instance()->editRecord(opr);
    QList<int> removeIds = dlg.getIdsForDelete();
    for (QList<int>::iterator it = removeIds.begin(); it != removeIds.end(); it++)
    {
        FinDatabase::Instance()->delRecordFromGroup(id, *it);
        FinDatabase::Instance()->delRecord(*it);
    }
    Operations oprs = dlg.operations();
    for (Operations::iterator it = oprs.begin(); it != oprs.end(); ++it)
    {
        qDebug() << "group item: " << *it;
        if (it->id)
            FinDatabase::Instance()->editRecord(*it);
        else
        {
            int newId = FinDatabase::Instance()->addRecord(*it);
            FinDatabase::Instance()->addRecordToGroup(id, newId);
        }
    }
    item.setData(0, Qt::DisplayRole, opr.rdate.toString(Qt::ISODate));
    item.setData(1, Qt::DisplayRole, QVariant(opr.value).toString());
    item.setData(2, Qt::DisplayRole, QVariant(opr.amount).toString());
    item.setData(3, Qt::DisplayRole, QVariant(opr.discount).toString());
    item.setData(4, Qt::DisplayRole, opr.notes);
}

void FinWindow::editRecord(QTreeWidgetItem &item, int id)
{
    EditDlg dlg(this);
    Operation opr;
    opr.id = id;
    opr.value = item.data(1, Qt::DisplayRole).toDouble();
    opr.amount = item.data(2, Qt::DisplayRole).toDouble();
    opr.rdate = item.data(0, Qt::DisplayRole).toDate();
    opr.discount = item.data(3, Qt::DisplayRole).toDouble();
    opr.notes = item.data(4, Qt::DisplayRole).toString();
    dlg.setOperation(opr);
    if (dlg.exec() != QDialog::Accepted)
        return;
    try
    {
        FinDatabase::Instance()->beginTransaction();
        opr = dlg.operation();
        qDebug() << opr;
        FinDatabase::Instance()->editRecord(opr);
        FinDatabase::Instance()->commitTransaction();

        item.setData(0, Qt::DisplayRole, opr.rdate.toString(Qt::ISODate));
        item.setData(1, Qt::DisplayRole, QVariant(opr.value).toString());
        item.setData(2, Qt::DisplayRole, QVariant(opr.amount).toString());
        item.setData(3, Qt::DisplayRole, QVariant(opr.discount).toString());
        item.setData(4, Qt::DisplayRole, opr.notes);
    }
    catch (const FinException &e)
    {
        FinDatabase::Instance()->rollbackTransaction();
        QMessageBox::critical(this, tr("Financier error"), e.what());
    }
}

void FinWindow::deleteRecord()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item)
        return;

    qDebug() << "deleting row: " << tree->currentIndex().row()
        << " record id: "<< item->data(0, Qt::UserRole).toInt();

    QMessageBox mb(tr("Delete record"), tr("Are you sure?"), QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, 0, this);
    if (mb.exec() != QMessageBox::Yes)
        return;

    FinDatabase::Instance()->delRecord(item->data(0, Qt::UserRole).toInt());
    delete item;
    setCountSum();
}

void FinWindow::quit()
{
    FinDatabase::Instance()->closeDatabase();
    close();
}

void FinWindow::refresh()
{
    tree->clear();
    QList<QTreeWidgetItem*> items;
    FinDatabase::Instance()->loadRange(deFrom->date(), deTo->date(), items);
    
    for (QList<QTreeWidgetItem*>::iterator iter = items.begin(); iter != items.end(); iter++)
        tree->addTopLevelItem(*iter);
    
    for (int i=0; i<tree->columnCount(); i++)
        tree->resizeColumnToContents(i);
    setCountSum();
    enableRecordOperations(true);
}

void FinWindow::monthReport()
{
    FinMonthReportPreferencesDlg dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    qDebug() << dlg.getMonth() << "." << dlg.getYear();

    FinMonthReport* rep = new FinMonthReport(dlg.getMonth(), dlg.getYear());
    rep->show();
}

void FinWindow::periodReport()
{
    FinPeriodReportPreferencesDlg dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    qDebug() << dlg.getStartMonth() << "." << dlg.getStartYear();
    qDebug() << dlg.getFinishMonth() << "." << dlg.getFinishYear();

    FinPeriodReport* rep = new FinPeriodReport(dlg.getStartMonth(), dlg.getStartYear(), 
        dlg.getFinishMonth(), dlg.getFinishYear());
    rep->show();
}

void FinWindow::oprTypes()
{
    OprTypeDlg dlg;
    dlg.exec();
}

void FinWindow::itemDoubleClicked(QTreeWidgetItem*, int)
{
    editRecord();
}

void FinWindow::itemClicked(QTreeWidgetItem*, int)
{
    setCountSum();
}

void FinWindow::setCountSum()
{
    double res = 0, resSelected = 0;
    QList<QTreeWidgetItem*> items = tree->findItems(".*", Qt::MatchRegExp);
    for (QList<QTreeWidgetItem*>::iterator it = items.begin(); it != items.end(); it ++)
    {
        int amount = (*it)->data(2, Qt::DisplayRole).toInt();
        int discount = (*it)->data(3, Qt::DisplayRole).toInt();
        double value = (*it)->data(1, Qt::DisplayRole).toDouble();
        double tmp = value * amount;

        if (discount)
            tmp *= (100. - discount) / 100.;
        tmp = qRound(tmp * 100.) / 100.;

        if (tree->isItemSelected(*it))
            resSelected += tmp;
        res += tmp;
    }
    QString msg = tr("Sum") + ":" + QVariant(res).toString() + " ";
    msg += tr("Selected") + ":" + QVariant(resSelected).toString();
    statusBar->showMessage(msg);

}

