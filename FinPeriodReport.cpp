#include <QtGui>
#include <QDebug>

#include "FinPeriodReport.h"
#include "FinDatabase.h"
#include "FinMonthReport.h"

FinPeriodReportPreferencesDlg::FinPeriodReportPreferencesDlg(QWidget* parent)
    : QDialog(parent)
{
    createWidgets();
}

FinPeriodReportPreferencesDlg::~FinPeriodReportPreferencesDlg()
{
}

void FinPeriodReportPreferencesDlg::createWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_leStartMonth = new QLineEdit;
    m_leStartMonth->setValidator(new QIntValidator(1, 12, m_leStartMonth));

    m_leStartYear = new QLineEdit;
    m_leStartYear->setValidator(new QIntValidator(1000, 9999, m_leStartYear));

    m_leFinishMonth = new QLineEdit;
    m_leFinishMonth->setValidator(new QIntValidator(1, 12, m_leFinishMonth));

    m_leFinishYear = new QLineEdit;
    m_leFinishYear->setValidator(new QIntValidator(1000, 9999, m_leFinishYear));

    mainLayout->addWidget(new QLabel(tr("from")), 0);
    QHBoxLayout *hbox;
    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Month:")), 0);
    hbox->addWidget(m_leStartMonth, 1);
    mainLayout->addLayout(hbox, 0);

    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Year:")), 0);
    hbox->addWidget(m_leStartYear, 1);
    mainLayout->addLayout(hbox, 0);

    mainLayout->addWidget(new QLabel(tr("to")), 0);
    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Month:")), 0);
    hbox->addWidget(m_leFinishMonth, 1);
    mainLayout->addLayout(hbox, 0);

    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Year:")), 0);
    hbox->addWidget(m_leFinishYear, 1);
    mainLayout->addLayout(hbox, 0);

    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    hbox = new QHBoxLayout;
    hbox->addStretch(1);
    hbox->addWidget(okButton);
    hbox->addWidget(cancelButton);

    mainLayout->addStretch(1);
    mainLayout->addLayout(hbox, 0);

    setWindowTitle(tr("Month report preferences"));
    m_leStartMonth->setText(QVariant(QDate::currentDate().month()).toString());
    m_leStartYear->setText(QVariant(QDate::currentDate().year()).toString());    
    m_leFinishMonth->setText(QVariant(QDate::currentDate().month()).toString());
    m_leFinishYear->setText(QVariant(QDate::currentDate().year()).toString());    
}

void FinPeriodReportPreferencesDlg::onOk()
{
    if (m_leStartMonth->text().isEmpty()
        || m_leStartYear->text().isEmpty()
        || m_leFinishMonth->text().isEmpty()
        || m_leFinishYear->text().isEmpty())
    {
        QMessageBox::information(this, tr("Error"), tr("Nonvalid data"), QMessageBox::Ok);
        return;
    }
    accept();
}


int FinPeriodReportPreferencesDlg::getStartMonth() const
{
    return QVariant(m_leStartMonth->text()).toInt();
}

int FinPeriodReportPreferencesDlg::getStartYear() const
{
    return QVariant(m_leStartYear->text()).toInt();
}

int FinPeriodReportPreferencesDlg::getFinishMonth() const
{
    return QVariant(m_leFinishMonth->text()).toInt();
}

int FinPeriodReportPreferencesDlg::getFinishYear() const
{
    return QVariant(m_leFinishYear->text()).toInt();
}

FinPeriodReport::FinPeriodReport(int startMonth, int startYear, int finishMonth, int finishYear)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    m_twReport = new QTreeWidget;
    m_twReport->setColumnCount(3);
    QStringList header;
    header << tr("Month") << tr("Income") << tr("Charge");
    m_twReport->setHeaderLabels(header);

    PeriodInfos recs;
    FinDatabase::Instance()->loadPeriod(startMonth, startYear, finishMonth, finishYear, recs);
    
    qDebug() << "loaded: " << recs.size();

    QList<QTreeWidgetItem*> items;
    for (int i=0; i<recs.size(); ++i)
    {
        QStringList vals;
        vals << recs[i].date << QVariant(recs[i].income).toString() << QVariant(recs[i].charge).toString();
        QTreeWidgetItem* item = new QTreeWidgetItem(vals);
        items.push_back(item);
    }
    m_twReport->addTopLevelItems(items);

    vbox->addWidget(m_twReport);
    connect(m_twReport, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), 
        this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
    setLayout(vbox);
}

FinPeriodReport::~FinPeriodReport()
{
}

void FinPeriodReport::itemDoubleClicked(QTreeWidgetItem* item, int)
{
    if (!item)
        return;
    qDebug() << item->text(0).left(4) << " " << item->text(0).right(item->text(0).length() - 5);
    FinMonthReport* rep = new FinMonthReport(
        QVariant(item->text(0).right(item->text(0).length() - 5)).toInt(), 
        QVariant(item->text(0).left(4)).toInt());
    rep->show();
}

