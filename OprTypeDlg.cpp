#include <QtGui>

#include <QDebug>
#include "OprTypeDlg.h"


OprTypeDlg::OprTypeDlg()
{
    QVBoxLayout *ml = new QVBoxLayout;
    setLayout(ml);

    QHBoxLayout* hbox = new QHBoxLayout;
    m_lwTypes = new QListWidget;
    connect(m_lwTypes, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(selectType(QListWidgetItem*)));
    hbox->addWidget(m_lwTypes, 1);
    
    QVBoxLayout* vbox = new QVBoxLayout;
    QPushButton* bDel = new QPushButton;
    bDel->setIcon(QIcon(":delete"));
    connect(bDel, SIGNAL(clicked()), this, SLOT(delType()));
    vbox->addWidget(bDel);
    vbox->addStretch(1);
    hbox->addLayout(vbox);

    ml->addLayout(hbox, 1);

    hbox = new QHBoxLayout;
    m_bTypeIcon = new QPushButton;
    m_bTypeIcon->setIcon(QIcon(":blank"));
    connect(m_bTypeIcon, SIGNAL(clicked()), this, SLOT(selectTypeIcon()));
    hbox->addWidget(m_bTypeIcon);
    m_leType = new QLineEdit;
    hbox->addWidget(m_leType, 1);
    QPushButton* bAdd = new QPushButton;
    bAdd->setIcon(QIcon(":add"));
    connect(bAdd, SIGNAL(clicked()), this, SLOT(addType()));
    hbox->addWidget(bAdd);

    ml->addLayout(hbox);
    
    hbox = new QHBoxLayout;
    hbox->addStretch(1);
    QPushButton *bCancel = new QPushButton(tr("Cancel"));
    connect(bCancel, SIGNAL(clicked()), this, SLOT(accept()));
    hbox->addWidget(bCancel);

    ml->addLayout(hbox);

    setWindowTitle(tr("Operation"));
}

OprTypeDlg::~OprTypeDlg()
{
}

void OprTypeDlg::loadTypes()
{
}

void OprTypeDlg::addType()
{
    QString typeName = m_leType->text();
    QList<QListWidgetItem*> items = m_lwTypes->findItems(typeName, Qt::MatchExactly);
    if (items.size())
        return;
    QListWidgetItem* item = new QListWidgetItem(QIcon(m_typeIconPath), typeName);
    m_lwTypes->addItem(item);
}

void OprTypeDlg::delType()
{
    QListWidgetItem* item = m_lwTypes->currentItem();
    if (!item)
        return;
// TODO check if type is in use
    delete item;
}

void OprTypeDlg::selectType(QListWidgetItem* item)
{
    if (!item)
        return;
    
    m_leType->setText(item->text());
    m_bTypeIcon->setIcon(item->icon());
}

void OprTypeDlg::selectTypeIcon()
{
    QFileDialog dlg(this);
    QString lastIconDir = QDir::current().absolutePath();
    QString iconPath = dlg.getOpenFileName(this, tr("Select icon"), lastIconDir, "Image files (*.png *.jpg *.svg)");
    if (iconPath == "")
        return;
    
    m_typeIconPath = iconPath;
    m_bTypeIcon->setIcon(QIcon(iconPath));
}

