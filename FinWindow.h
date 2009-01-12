#ifndef FINWINDOW_H_
#define FINWINDOW_H_

#include <QMainWindow>
#include <QSettings>

class QDate;
class QDateEdit;
class QVBoxLayout;
class QHBoxLayout;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QStatusBar;

class FinWindow
    : public QMainWindow
{
    Q_OBJECT
public:
    FinWindow();
    virtual ~FinWindow();
protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void newFile();
    void openFile();
    void newRecord();
    void newGroupRecord();
    void editRecord();
    void deleteRecord();
    void refresh();
    void quit();
    void monthReport();
    void periodReport();
    void oprTypes();

    void itemDoubleClicked(QTreeWidgetItem*, int);
    void itemClicked(QTreeWidgetItem*, int);

private:
    void createActions();
    void createMenus();
    void openDatabase(QString &file);
    void setCountSum();
    void enableRecordOperations(bool enable = true);
    void editRecord(QTreeWidgetItem &item, int id);
    void editGroupRecord(QTreeWidgetItem &item, int id);

    QSettings m_settings;

    QAction *onNewFile;
    QAction *onOpenFile;
    QAction *onNewRecord;
    QAction *onNewGroupRecord;
    QAction *onEditRecord;
    QAction *onDeleteRecord;
    QAction *onQuit;

    QAction *onMonthReport;
    QAction *onPeriodReport;

    QAction* onOprTypes;

    QTreeWidget *tree;
    QVBoxLayout *mainLayout;
    QHBoxLayout *hbox;
    QDateEdit *deFrom, *deTo;
    QPushButton *bRefresh;
    QStatusBar *statusBar;
};

#endif /* FINWINDOW_H_ */

