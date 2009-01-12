#ifndef OPERATION_H
#define OPERATION_H

#include <QDebug>

#include <QVariant>
#include <QDate>
#include <QString>
#include <QStringList>

struct Operation
{
    Operation()
        : id(0), value(0), discount(0), amount(1), oprtype(0), rdate(QDate::currentDate())
    {}
    int id;
    double value;
    QString notes;
    double discount;
    double amount;
    int oprtype;
    QDate rdate;
    operator QString() const
    {
        qDebug() << "Operation: #" << id
            << "\ndate: " << rdate
            << "\nvalue: " << value 
            << " amount: " << amount
            << " discount: " << discount
            << "\nnotes: " << notes
            << "\ntype: " << oprtype;
        return "\n";
    }
    QStringList list() const
    {
        QStringList tmp;
        tmp << rdate.toString(Qt::ISODate) 
            << QVariant(value).toString() 
            << QVariant(amount).toString() 
            << QVariant(discount).toString() 
            << notes;
        return tmp;
    }
};

#include <QVector>
typedef QVector<Operation> Operations;

#endif /* OPERATION_H */

