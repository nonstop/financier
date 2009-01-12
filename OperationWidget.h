#ifndef OPERATIONWIDGET_H
#define OPERATIONWIDGET_H

#include <QWidget>
#include <QDate>
#include "Operation.h"

class QLineEdit;
class QComboBox;
class QDateEdit;

class OperationWidget
    : public QWidget
{
    Q_OBJECT
public:
    OperationWidget();
    virtual ~OperationWidget();

    Operation operation() const;
    void setOperation(const Operation& opr);

    void setDate(const QDate& date);
    bool checkValues();
private:
    QLineEdit* m_leValue;
    QLineEdit* m_leAmount;
    QLineEdit* m_leDiscount;
    QComboBox* m_cbNotes;
    QDateEdit* m_deDate;
};

#endif /* OPERATIONWIDGET_H */

