
#include <QtSql>

#include <QDebug>

#include "common.h"
#include "FinDatabase.h"
#include <QTreeWidget>

FinDatabase::FinDatabase()
{}


FinDatabase::~FinDatabase()
{
    closeDatabase();
}

FinDatabase *FinDatabase::Instance()
{
    static FinDatabase *fd = NULL;
    if (!fd)
        fd = new FinDatabase;

    return fd;
}

void FinDatabase::openDatabase(const QString& path)
{
    if (db.isOpen())
        db.close();
    if (!db.databaseName().isEmpty())
        QSqlDatabase::removeDatabase(db.databaseName());

    db = QSqlDatabase::addDatabase("QSQLITE", path);
    db.setDatabaseName(path);
    qDebug() << db.databaseName();

    if (!db.open())
        throw FinException("failed to open db");
}

void FinDatabase::closeDatabase()
{
    if (db.isOpen())
    {
        qDebug("closing db");
        db.close();
    }
    db = QSqlDatabase();
}

void FinDatabase::createDatabase(const QString& path)
{
    openDatabase(path);
    QSqlQuery q(db);
    q.exec("DROP TABLE operations");
    q.exec("CREATE TABLE operations "
        "(id INT PRIMARY KEY, "
        "value NUMBER, "
        "notes VARCHAR, "
        "discount NUMBER NOT NULL DEFAULT 0, "
        "amount NUMBER NOT NULL DEFAULT 1, "
        "oprtype INT NOT NULL DEFAULT 0, "
        "rdate DATE NOT NULL DEFAULT CURRENT_DATE)");
    q.exec("DROP TABLE groups");
    q.exec("CREATE TABLE groups (id_group INT NOT NULL, id INT NOT NULL)");
    q.exec("DROP TABLE oprtypes");
    q.exec("CREATE TABLE oprtypes (id INT NOT NULL, name VARCHAR NOT NULL, path VARCHAR NOT NULL)");
}

int FinDatabase::getNextId(QString tableName)
{
    QString sql = "SELECT MAX(id) FROM " + tableName;

    int val = 0;
    QSqlQuery q(db);
    if (!q.exec(sql))
    {
        qDebug() << q.lastError().text();
        return 0;
    }
    while (q.next())
        val = q.value(0).toInt();
    return val + 1;
}

int FinDatabase::addRecord(const Operation& opr)
{
    int id = getNextId("operations");

    QSqlQuery query(db);
    query.prepare("INSERT INTO operations (id, value, notes, amount, discount, oprtype, rdate) "
                  "VALUES (:id, :value, :notes, :amount, :discount, :oprtype, :rdate)");
    query.bindValue(":id", id);
    query.bindValue(":value", opr.value);
    query.bindValue(":notes", opr.notes);
    query.bindValue(":amount", opr.amount);
    query.bindValue(":discount", opr.discount);
    query.bindValue(":oprtype", opr.oprtype);
    query.bindValue(":rdate", opr.rdate);
    if (!query.exec())
    {
        qDebug() << query.lastError();
        return 0;
    }

    return id;
}

void FinDatabase::editRecord(const Operation& opr)
{
    QSqlQuery query(db);
    query.prepare("UPDATE operations SET "
        "value = :value, notes = :notes, amount = :amount, "
        "discount = :discount, oprtype = :oprtype, rdate = :rdate "
        "WHERE id = :id");
    query.bindValue(":id", opr.id);
    query.bindValue(":value", opr.value);
    query.bindValue(":notes", opr.notes);
    query.bindValue(":amount", opr.amount);
    query.bindValue(":discount", opr.discount);
    query.bindValue(":oprtype", opr.oprtype);
    query.bindValue(":rdate", opr.rdate);
    if (!query.exec())
    {
        qDebug() << query.lastError();
        throw FinException("updating failed");
    }
}

void FinDatabase::delRecord(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM operations WHERE id = :id1 OR id IN (select id from groups where id_group = :id2)");
    query.bindValue(":id1", id);
    query.bindValue(":id2", id);
    if (!query.exec())
        throw FinException("deleting failed");
}

void FinDatabase::delRecordFromGroup(int id_group, int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM groups WHERE id_group = :id_group and id = :id");
    query.bindValue(":id_group", id_group);
    query.bindValue(":id", id);
    if (!query.exec())
        throw FinException("deleting failed");
}

void FinDatabase::loadRange(const QDate& from, const QDate& to, QList<QTreeWidgetItem*>& items)
{
    QSqlQuery q(db);
    q.prepare("select id, rdate, value, amount, discount, notes from operations where rdate >= :dtFrom and rdate <= :dtTo and id not in (select id from groups) order by rdate");
    q.bindValue(":dtFrom", from);
    q.bindValue(":dtTo", to);
    if (!q.exec())
    {
        qDebug() << q.lastError().text();
        return;
    }
    while (q.next())
    {
        QStringList list;
        list << q.value(1).toString() << q.value(2).toString() << q.value(3).toString() << q.value(4).toString() << q.value(5).toString();
        QTreeWidgetItem *item = new QTreeWidgetItem(list);
        item->setData(0, Qt::UserRole, q.value(0));
        items.append(item);
    }
}

void FinDatabase::loadRangeOperations(const QDate& from, const QDate& to, bool oprType, Records& recs)
{
    QString sql(
        "select value, notes from operations "
        "where rdate >= :dtFrom and rdate <= :dtTo "
        "and id not in (select id from groups) ");
    if (oprType)
        sql += "and value > 0 ";
    else
        sql += "and value < 0 ";
    sql += "order by rdate";

    QSqlQuery q(db);
    q.prepare(sql);
    q.bindValue(":dtFrom", from);
    q.bindValue(":dtTo", to);
    if (!q.exec())
    {
        qDebug() << q.lastError().text();
        return;
    }
    while (q.next())
    {
        Record rec;
        rec.num = q.value(0).toDouble();
        rec.name = q.value(1).toString();
        recs.push_back(rec);
    }
}

void FinDatabase::loadGroupContent(int id_group, QList<QTreeWidgetItem*>& items)
{
    QSqlQuery q(db);
    q.prepare("select o.id, o.rdate, o.value, o.amount, o.discount, o.notes from operations o, groups g where o.id = g.id and g.id_group = :id_group");
    q.bindValue(":id_group", id_group);
    if (!q.exec())
    {
        qDebug() << q.lastError().text();
        return;
    }
    while (q.next())
    {
        QStringList list;
        list << q.value(1).toString() << q.value(2).toString() << q.value(3).toString() << q.value(4).toString() << q.value(5).toString();
        QTreeWidgetItem *item = new QTreeWidgetItem(list);
        item->setData(0, Qt::UserRole, q.value(0));
        items.append(item);
    }
}

bool FinDatabase::isGroupRecord(int id)
{
    QSqlQuery q(db);
    q.prepare("select count(id) from groups where id_group = :id");
    q.bindValue(":id", id);
    if (!q.exec())
    {
        qDebug("checking groups failed");
        return false;
    }
    int res = 0;
    while (q.next())
        res = q.value(0).toInt();
    return res;
}

void FinDatabase::addRecordToGroup(int id_group, int id_rec)
{ 
    qDebug("adding rec %d to group %d", id_rec, id_group);
    QSqlQuery query(db);
    query.prepare("INSERT INTO groups VALUES (:id_group, :id_rec)");
    query.bindValue(":id_group", id_group);
    query.bindValue(":id_rec", id_rec);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        throw FinException("inserting in group failed");
    }
}

void FinDatabase::beginTransaction()
{
    QSqlQuery query(db);
    query.prepare("begin transaction");
    if (!query.exec())
        throw FinException("failed to begin transaction");
}

void FinDatabase::commitTransaction()
{
    QSqlQuery query(db);
    query.prepare("commit transaction");
    if (!query.exec())
        throw FinException("failed to commit transaction");
}

void FinDatabase::rollbackTransaction()
{
    QSqlQuery query(db);
    query.prepare("rollback transaction");
    if (!query.exec())
        throw FinException("failed to rollback transaction");
}

void FinDatabase::loadCompletion(QStringList &completionList)
{
    QSqlQuery q(db);
    q.prepare("select distinct notes from operations order by notes");
    if (!q.exec())
    {
        qDebug("loading completion failed");
        return;
    }
    while (q.next())
        completionList.append(q.value(0).toString());
}

namespace 
{
    void prepareDate(int month, int year, QString& date)
    {
        date = QVariant(year).toString() + "-";
        if (month >= 10)
            date += QVariant(month).toString();
        else
            date += "0" + QVariant(month).toString();
    }
    void prepareDates(int month, int year, QString& startDate, QString& finishDate)
    {
        prepareDate(month, year, startDate);

        if (month == 12)
            finishDate = QVariant(year + 1).toString() + "-01";
        else
        {
            month += 1;
            prepareDate(month, year, finishDate);
        }
    }
}

double FinDatabase::calcMonthCharge(int month, int year)
{
    QString startDate, finishDate;
    prepareDates(month, year, startDate, finishDate);

    QString sql = "select sum(value) from operations where value < 0 ";
    sql += "and rdate > '" + startDate + "' ";
    sql += "and rdate < '" + finishDate + "' ";
    sql += "and id not in (select id from groups)";

    double res = 0;

    QSqlQuery q(db);
    q.prepare(sql);    
    if (!q.exec())
    {
        qDebug("calcMonthCharge failed");
        return 0;
    }
    while (q.next())
        res = q.value(0).toDouble();

    return res;
}

double FinDatabase::calcMonthIncome(int month, int year)
{
    QString startDate, finishDate;
    prepareDates(month, year, startDate, finishDate);

    QString sql = "select sum(value) from operations where value > 0 ";
    sql += "and rdate > '" + startDate + "' ";
    sql += "and rdate < '" + finishDate + "' ";
    sql += "and id not in (select id from groups)";

    double res = 0;

    QSqlQuery q(db);
    q.prepare(sql);    
    if (!q.exec())
    {
        qDebug("calcMonthCharge failed");
        return 0;
    }
    while (q.next())
        res = q.value(0).toDouble();

    return res;
}

void FinDatabase::loadMaxCharges(int month, int year, int num, Records& recs)
{
    QString startDate, finishDate;
    prepareDates(month, year, startDate, finishDate);

    QString sql = "select value, notes from operations where value < 0 ";
    sql += "and rdate > '" + startDate + "' ";
    sql += "and rdate < '" + finishDate + "' ";
    sql += "and id not in (select id from groups) ";
    sql += "order by value";
    
    QSqlQuery q(db);
    q.prepare(sql);
    if (!q.exec())
    {
        qDebug("loadMaxCharges failed");
        return;
    }
    int i = 1;
    while (q.next())
    {
        Record rec;
        rec.num = q.value(0).toDouble();
        rec.name = q.value(1).toString();
        recs.push_back(rec);
        if (i >= num)
            break;
        i++;
    }
}

void FinDatabase::loadMaxIncomes(int month, int year, int num, Records& recs)
{
    QString startDate, finishDate;
    prepareDates(month, year, startDate, finishDate);

    QString sql = "select value, notes from operations where value > 0 ";
    sql += "and rdate > '" + startDate + "' ";
    sql += "and rdate < '" + finishDate + "' ";
    sql += "and id not in (select id from groups) ";
    sql += "order by value desc";

    QSqlQuery q(db);
    q.prepare(sql);
    if (!q.exec())
    {
        qDebug("loadMaxIncomes failed");
        return;
    }
    int i = 1;
    while (q.next())
    {
        Record rec;
        rec.num = q.value(0).toDouble();
        rec.name = q.value(1).toString();
        recs.push_back(rec);
        if (i >= num)
            break;
        i++;
    }
}

void FinDatabase::loadPeriod(int startMonth, int startYear, int finishMonth, int finishYear, PeriodInfos& recs)
{
    QDate startDate(startYear, startMonth, 1);
    QDate finishDate(finishYear, finishMonth, 1);

    QDate currDate(startDate);
    while (currDate < finishDate)
    {
        PeriodInfo pi;
        prepareDate(currDate.month(), currDate.year(), pi.date);
        pi.income = calcMonthIncome(currDate.month(), currDate.year());
        pi.charge = calcMonthCharge(currDate.month(), currDate.year());
        if (pi.income || pi.charge)
            recs.push_back(pi);
        currDate = currDate.addMonths(1);
    }
}

