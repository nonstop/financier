#ifndef EDITGROUPDLG_H_
#define EDITGROUPDLG_H_

#include "Operation.h"

#include <QDialog>
#include <QList>
class OperationWidget;
class QTreeWidget;
class QTreeWidgetItem;

class EditGroupDlg
    : public QDialog
{
    Q_OBJECT
public:
    EditGroupDlg(QWidget *parent, int id_);
    virtual ~EditGroupDlg()
    {}

    Operation operation() const;
    void setOperation(const Operation& opr);
    Operations operations() const;
    QList<int> getIdsForDelete();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private slots:
    void onOk();
    void itemDoubleClicked(QTreeWidgetItem*, int);
    void newRecord();
    void editRecord();
    void deleteRecord();

private:
    OperationWidget* m_owOpr;
    QTreeWidget *tree;

    QAction *onNewRecord;
    QAction *onEditRecord;
    QAction *onDeleteRecord;
    int id_group;
    QList<int> removedIds;

    void setValue(double value);
    double countSum();
};

#endif /* EDITGROUPDLG_H_ */

