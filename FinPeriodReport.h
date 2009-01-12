#ifndef FINPERIODREPORT_H
#define FINPERIODREPORT_H

#include <QDialog>

class QLineEdit;

class FinPeriodReportPreferencesDlg
    : public QDialog
{
    Q_OBJECT
public:
    FinPeriodReportPreferencesDlg(QWidget* parent);
    virtual ~FinPeriodReportPreferencesDlg();

    int getStartMonth() const;
    int getStartYear() const;
    int getFinishMonth() const;
    int getFinishYear() const;
private slots:
    void onOk();
private:
    QLineEdit* m_leStartMonth;
    QLineEdit* m_leStartYear;
    QLineEdit* m_leFinishMonth;
    QLineEdit* m_leFinishYear;

    void createWidgets();
};

class QTreeWidget;
class QTreeWidgetItem;

class FinPeriodReport
    : public QDialog
{
    Q_OBJECT
public:
    FinPeriodReport(int startMonth, int startYear, int finishMonth, int finishYear);
    virtual ~FinPeriodReport();
private slots:
    void itemDoubleClicked(QTreeWidgetItem*, int);
private:
    QTreeWidget* m_twReport;
};

#endif /* FINPERIODREPORT_H */

