#ifndef OPRTYPEDLG_H
#define OPRTYPEDLG_H

#include <QDialog>
class QLineEdit;
class QPushButton;
class QListWidget;
class QListWidgetItem;

class OprTypeDlg
    : public QDialog
{
    Q_OBJECT
public:
    OprTypeDlg();
    virtual ~OprTypeDlg();
private slots:
    void addType();
    void delType();
    void selectType(QListWidgetItem* item);
    void selectTypeIcon();
private:
    QListWidget* m_lwTypes;
    QPushButton* m_bTypeIcon;
    QLineEdit* m_leType;
    QString m_typeIconPath;

    void loadTypes();
};


#endif /* OPRTYPEDLG_H */

