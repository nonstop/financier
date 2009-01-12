#include <QtGui>

#include "FinCompletion.h"
#include "OperationWidget.h"


OperationWidget::OperationWidget()
{
    QVBoxLayout* vbox = new QVBoxLayout;

    m_leValue = new QLineEdit();
    m_leValue->setValidator(new QDoubleValidator(m_leValue));

    m_leAmount = new QLineEdit();
    m_leAmount->setValidator(new QDoubleValidator(m_leAmount));
    m_leAmount->setText("1");

    m_leDiscount = new QLineEdit();
    m_leDiscount->setValidator(new QDoubleValidator(m_leDiscount));
    m_leDiscount->setText("0");

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Value")), 0);
    hbox->addWidget(m_leValue, 1);
    vbox->addLayout(hbox);
    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Amount")), 0);
    hbox->addWidget(m_leAmount, 1);
    vbox->addLayout(hbox);
    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Discount")), 0);
    hbox->addWidget(m_leDiscount, 1);
    vbox->addLayout(hbox);

    m_cbNotes = new QComboBox();
    m_cbNotes->setEditable(true);
    m_cbNotes->addItems(FinCompletion::Instance()->getCompletionList());
    m_cbNotes->addItem("");
    m_cbNotes->setCurrentIndex(m_cbNotes->count() - 1);

    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Notes")));
    hbox->addWidget(m_cbNotes, 1);
    vbox->addLayout(hbox);

    m_deDate = new QDateEdit(QDate::currentDate());
    vbox->addWidget(m_deDate);

    setLayout(vbox);
}

OperationWidget::~OperationWidget()
{
}

Operation OperationWidget::operation() const
{
    Operation opr;
    opr.value = QVariant(m_leValue->text()).toDouble();
    opr.amount = QVariant(m_leAmount->text()).toDouble();
    opr.discount = QVariant(m_leDiscount->text()).toDouble();
    opr.notes = m_cbNotes->currentText();
    opr.rdate = m_deDate->date();

    return opr;
}

void OperationWidget::setOperation(const Operation& opr)
{
    m_leValue->setText(QVariant(opr.value).toString());
    m_leAmount->setText(QVariant(opr.amount).toString());
    m_leDiscount->setText(QVariant(opr.discount).toString());
    m_cbNotes->addItem(opr.notes);
    m_cbNotes->setCurrentIndex(m_cbNotes->count() - 1);
    m_deDate->setDate(opr.rdate);
}

bool OperationWidget::checkValues()
{
    return (m_leValue->text().length() && m_cbNotes->currentText().length());
}

void OperationWidget::setDate(const QDate& date)
{
    m_deDate->setDate(date);
}

