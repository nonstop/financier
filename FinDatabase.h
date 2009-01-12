#ifndef FINDATABASE_H_
#define FINDATABASE_H_

#include <QObject>
#include <QSqlDatabase>
#include <QList>

class QTreeWidgetItem;
class QDate;
class QString;
class QStringList;
//class QSqlDatabase;

struct Record
{
    double num;
    QString name;
};
typedef QVector<Record> Records;

struct PeriodInfo
{
    QString date;
    double income;
    double charge;
};
typedef QVector<PeriodInfo> PeriodInfos;

#include "Operation.h"

class FinDatabase
    : public QObject
{
//    Q_OBJECT
public:
    virtual ~FinDatabase();
    static FinDatabase *Instance();
/*    QSqlDatabase& getDatabase();
    QSqlDatabase& getDatabase(const QString& path);*/
    void openDatabase(const QString& path);
    void createDatabase(const QString& path);
    void closeDatabase();
    int addRecord(const Operation& opr);
    void addRecordToGroup(int id_group, int id_rec);
    void editRecord(const Operation& opr);
    void delRecord(int id);
    void delRecordFromGroup(int id_group, int id);
    void loadRange(const QDate& from, const QDate& to, QList<QTreeWidgetItem*>& items);
    void loadRangeOperations(const QDate& from, const QDate& to, bool oprType, Records& recs);
    void loadGroupContent(int id_group, QList<QTreeWidgetItem*>& items);
    void loadCompletion(QStringList &completionList);
    bool isGroupRecord(int id);
    void beginTransaction();
    void rollbackTransaction();
    void commitTransaction();

    double calcMonthCharge(int month, int year);
    double calcMonthIncome(int month, int year);
    void loadMaxCharges(int month, int year, int num, Records& recs);
    void loadMaxIncomes(int month, int year, int num, Records& recs);

    void loadPeriod(int startMonth, int startYear, int finishMonth, int finishYear, PeriodInfos& recs);
private:
    FinDatabase();
    int getNextId(QString tableName);

    QSqlDatabase db;
};

#endif /* FINDATABASE_H_ */

