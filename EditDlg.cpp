
#include <QtGui>
#include "EditDlg.h"
#include "FinCompletion.h"
#include "common.h"

#include "OperationWidget.h"

EditDlg::EditDlg(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
   
    m_owOpr = new OperationWidget;
    mainLayout->addWidget(m_owOpr);

    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);

    setWindowTitle(tr("Operation"));
}

Operation EditDlg::operation()
{
    return m_owOpr->operation();
}

void EditDlg::setOperation(const Operation& opr)
{
    m_owOpr->setOperation(opr);
}

void EditDlg::setDate(const QDate& date)
{
    m_owOpr->setDate(date);
}

void EditDlg::onOk()
{
    if (!m_owOpr->checkValues())
    {
        QMessageBox::information(this, tr("Error"), tr("Nonvalid data"), QMessageBox::Ok);
        return;
    }
    accept();
}


