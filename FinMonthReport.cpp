#include <QtGui>
#include <QtSvg>

#include "FinMonthReport.h"
#include "FinDatabase.h"


FinMonthReportPreferencesDlg::FinMonthReportPreferencesDlg(QWidget* parent)
    : QDialog(parent)
{
    createWidgets();
}

FinMonthReportPreferencesDlg::~FinMonthReportPreferencesDlg()
{
}

int FinMonthReportPreferencesDlg::getMonth() const
{
    return QVariant(m_leMonth->text()).toInt();
}

int FinMonthReportPreferencesDlg::getYear() const
{
    return QVariant(m_leYear->text()).toInt();
}

void FinMonthReportPreferencesDlg::createWidgets()
{
    QVBoxLayout* vbox = new QVBoxLayout;

    m_leMonth = new QLineEdit;
    m_leMonth->setValidator(new QIntValidator(1, 12, m_leMonth));

    m_leYear = new QLineEdit;
    m_leYear->setValidator(new QIntValidator(1000, 9999, m_leYear));

    QHBoxLayout *hbox= new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Month:")), 0);
    hbox->addWidget(m_leMonth, 1);
    vbox->addLayout(hbox, 0);

    hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Year:")), 0);
    hbox->addWidget(m_leYear, 1);
    vbox->addLayout(hbox, 0);

    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    hbox = new QHBoxLayout;
    hbox->addStretch(1);
    hbox->addWidget(okButton);
    hbox->addWidget(cancelButton);

    vbox->addStretch(1);
    vbox->addLayout(hbox, 0);
    
    setLayout(vbox);

    setWindowTitle(tr("Month report preferences"));
    m_leMonth->setText(QVariant(QDate::currentDate().month()).toString());
    m_leYear->setText(QVariant(QDate::currentDate().year()).toString());    
}

void FinMonthReportPreferencesDlg::onOk()
{
    if (m_leMonth->text().isEmpty()
        || m_leYear->text().isEmpty())
    {
        QMessageBox::information(this, tr("Error"), tr("Nonvalid data"), QMessageBox::Ok);
        return;
    }
    accept();
}

namespace {

QByteArray createSvg(double income, double charge, double selInc = 0, double selCh = 0)
{
    qDebug() << "Income: " << income << "(" << selInc << ")\nCharge: " << charge << "(" << selCh << ")";
    QString svg(
"<?xml version='1.0' standalone='no'?>"
"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' " 
"'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>"
"<svg width='200' height='200' version='1.1' xmlns='http://www.w3.org/2000/svg'>");
    int incH, chH;
    if (income >= charge)
    {
        incH = 200;
        chH = int(200 * (charge / income));
        qDebug() << (charge / income);
    }
    else
    {
        incH = int(200 * (income / charge));
        chH = 200;
        qDebug() << (income / charge);
    }
    int selIncH = int(incH * (selInc / income));
    int selChH = int(chH * (selCh / charge));

    svg += 
"<rect x='0' y='" + QVariant(200 - incH).toString()
+ "' width='100' height='" + QVariant(incH).toString() + "' "
"style='fill:green;stroke:green;stroke-width:5;fill-opacity:0.1;stroke-opacity:0.9'/>"
"<rect x='100' y='" + QVariant(200 - chH).toString() 
+ "' width='100' height='" + QVariant(chH).toString() + "' "
"style='fill:red;stroke:green;stroke-width:5;fill-opacity:0.1;stroke-opacity:0.9'/>"
"<rect x='0' y='" + QVariant(200 - selIncH).toString()
+ "' width='100' height='" + QVariant(selIncH).toString() + "' "
"style='fill:green;stroke:green;stroke-width:5;fill-opacity:0.3;stroke-opacity:0.9'/>"
"<rect x='100' y='" + QVariant(200 - selChH).toString() 
+ "' width='100' height='" + QVariant(selChH).toString() + "' "
"style='fill:red;stroke:green;stroke-width:5;fill-opacity:0.3;stroke-opacity:0.9'/>"
"<g transform='translate(10,190)'><text id='TextElement' x='0' y='0' style='font-size:14;'>"
+ QVariant(income).toString() + "</text></g>"
"<g transform='translate(110,190)'><text id='TextElement' x='0' y='0' style='font-size:14;'>"
+ QVariant(-charge).toString() + "</text></g>"
"</svg>";
    qDebug() << svg;
    return svg.toAscii();
}


}

FinMonthGraph::FinMonthGraph(double income, double charge)
    : m_income(income), m_charge(charge), m_selInc(0), m_selCh(0)
{
    m_swHist = new QSvgWidget;

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(m_swHist, 1);
    
    m_swHist->load(createSvg(income, -charge));
    setLayout(vbox);
}

FinMonthGraph::~FinMonthGraph()
{
}

void FinMonthGraph::selectIncome(double val)
{
    m_selInc = val;
    m_swHist->load(createSvg(m_income, -m_charge, m_selInc, -m_selCh));
}

void FinMonthGraph::selectCharge(double val)
{
    m_selCh = val;
    m_swHist->load(createSvg(m_income, -m_charge, m_selInc, -m_selCh));
}

void FinMonthReport::incomeSelectionChanged()
{
    QList<QTreeWidgetItem*> items = m_twIncome->selectedItems();
    double selVal = 0;
    for (int i=0; i<items.size(); ++i)
        selVal += items.at(i)->data(0, Qt::DisplayRole).toDouble();
    m_graph->selectIncome(selVal);
}

void FinMonthReport::chargeSelectionChanged()
{
    QList<QTreeWidgetItem*> items = m_twCharge->selectedItems();
    double selVal = 0;
    for (int i=0; i<items.size(); ++i)
        selVal += items.at(i)->data(0, Qt::DisplayRole).toDouble();
    m_graph->selectCharge(selVal);
}

QTreeWidget* FinMonthReport::createOperationList(bool oprType)
{
    QTreeWidget* tw = new QTreeWidget;
    tw->setColumnCount(2);
    QStringList header;
    header << tr("Sum");
    if (oprType)
        header << tr("Income");
    else
        header << tr("Charge");
    tw->setHeaderLabels(header);
    tw->setSelectionMode(QAbstractItemView::ExtendedSelection);
    if (oprType)
        connect(tw, SIGNAL(itemSelectionChanged()), this, SLOT(incomeSelectionChanged()));
    else
        connect(tw, SIGNAL(itemSelectionChanged()), this, SLOT(chargeSelectionChanged()));
    
    Records recs;
    FinDatabase::Instance()->loadRangeOperations(
        m_date, 
        QDate(m_date.year(), m_date.month(), m_date.daysInMonth()),
        oprType,
        recs);

    qDebug() << recs.size();

    QList<QTreeWidgetItem*> items;
    for (int i=0; i< recs.size(); ++i)
    {
        QStringList vals;
        vals << QVariant(recs[i].num).toString() << recs[i].name;
        QTreeWidgetItem* item = new QTreeWidgetItem(vals);
        items.append(item);
    }
    tw->addTopLevelItems(items);

    return tw;
}


FinMonthReport::FinMonthReport(int month, int year)
    : QDialog(NULL, Qt::Dialog), m_date(year, month, 1)
{
    double charge = FinDatabase::Instance()->calcMonthCharge(month, year);
    double income = FinDatabase::Instance()->calcMonthIncome(month, year);

    Records charges;
    FinDatabase::Instance()->loadMaxCharges(month, year, 3, charges);
    Records incomes;
    FinDatabase::Instance()->loadMaxIncomes(month, year, 3, incomes);
    
    QString report;
    
    QDate fromDate(year, month, 1);
    QDate toDate(year, month, fromDate.daysInMonth());
    report += tr("from ") + fromDate.toString() + tr(" to ") + toDate.toString() + "\n";
    report += tr("Income: ") + QVariant(income).toString() + "\n";

    for (int i=0; i<incomes.size(); ++i)
        report += "   " + incomes[i].name + " (" + QVariant(incomes[i].num).toString() + ")\n";
    report += "\n";

    report += tr("Charge: ") + QVariant(-charge).toString() + "\n";
    for (int i=0; i<charges.size(); ++i)
        report += "    " + charges[i].name + " (" + QVariant(charges[i].num).toString() + ")\n";
    report += "\n";

    QString title = tr("Finances: ") + QVariant(month).toString() + "." + QVariant(year).toString();
    setWindowTitle(title);

    
    m_twIncome = createOperationList(true);
    m_twCharge = createOperationList(false);

    QSplitter* spl = new QSplitter;
    spl->addWidget(m_twIncome);
    spl->addWidget(m_twCharge);
    spl->setOrientation(Qt::Vertical);

    QVBoxLayout *vbox = new QVBoxLayout;
    setLayout(vbox);

    m_teReport = new QTextEdit;
    m_teReport->setReadOnly(true);
    m_teReport->setPlainText(report);

    m_graph = new FinMonthGraph(income, charge);
    QTabWidget* tw = new QTabWidget;
    tw->addTab(m_graph, tr("Graph"));
    tw->addTab(m_teReport, tr("Text"));

    QSplitter* splAll = new QSplitter;
    splAll->addWidget(spl);
    splAll->addWidget(tw);
    splAll->setStretchFactor(0, 1);
    splAll->setStretchFactor(1, 5);
    
    vbox->addWidget(splAll, 1);
}

FinMonthReport::~FinMonthReport()
{
}

