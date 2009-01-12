#ifndef EDITDLG_H_
#define EDITDLG_H_

#include "Operation.h"
#include <QDialog>
class OperationWidget;

class EditDlg
    : public QDialog
{
    Q_OBJECT
public:
    EditDlg(QWidget *parent);
    virtual ~EditDlg()
    {}
    Operation operation();
    void setOperation(const Operation& opr);

    void setDate(const QDate& date);
/*    void setValue(double value);
    void setNotes(QString notes);
    void setAmount(int amount);
    void setDiscount(int discount);

    double getValue();
    QString getNotes();
    int getAmount();
    int getDiscount();*/
private slots:
    void onOk();

private:
    OperationWidget* m_owOpr;
};

#endif /* EDITDLG_H_ */

