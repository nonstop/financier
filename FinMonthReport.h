#ifndef FINMONTHREPORT_H
#define FINMONTHREPORT_H

#include <QDate>
#include <QDialog>

class QLineEdit;
class QTextEdit;

class FinMonthReportPreferencesDlg
    : public QDialog
{
    Q_OBJECT
public:
    FinMonthReportPreferencesDlg(QWidget* parent);
    virtual ~FinMonthReportPreferencesDlg();

    int getMonth() const;
    int getYear() const;
private slots:
    void onOk();
private:
    void createWidgets();

    QLineEdit *m_leMonth;
    QLineEdit *m_leYear;
};

class QSvgWidget;

class FinMonthGraph
    : public QWidget
{
    Q_OBJECT
public:
    FinMonthGraph(double income, double charge);
    virtual ~FinMonthGraph();

    void selectIncome(double val);
    void selectCharge(double val);
private:
    double m_income, m_charge;
    double m_selInc, m_selCh;
    QSvgWidget* m_swHist;
};

class QTreeWidget;

class FinMonthReport
    : public QDialog
{
    Q_OBJECT
public:
    FinMonthReport(int month, int year);
    virtual ~FinMonthReport();
private slots:
    void incomeSelectionChanged();
    void chargeSelectionChanged();
private:
    QDate m_date;
    FinMonthGraph* m_graph;
    QTextEdit *m_teReport;
    QTreeWidget* m_twIncome;
    QTreeWidget* m_twCharge;

    QTreeWidget* createOperationList(bool oprType);
};

#endif /* FINMONTHREPORT_H */

