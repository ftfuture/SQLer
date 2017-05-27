#ifndef DBDIALOG_H
#define DBDIALOG_H

#include <QDialog>
#include "sqlersetting.h"

namespace Ui {
class DbDialog;
}

class DbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbDialog(QWidget *parent = 0);
    ~DbDialog();
    QString driverName() const;
    QString databaseName() const;
    QString userName() const;
    QString password() const;
    QString hostName() const;
    QString pathName() const;
    QString connectionName() const;
    bool isSave() const;
    int port() const;
    QString getDbPath(bool newFileMode, const QString &startWith);
    void addHistory();
    void setAdapterInfo(const DBAdapterInfo &adapterInfo);

private slots:
    void on_cancelButton_clicked();
    void on_okButton_clicked();
    void on_newButton_clicked();
    void on_openButton_clicked();

    void on_historyCombo_currentIndexChanged(int index);

    void on_typeCombo_currentIndexChanged(int index);

private:
    Ui::DbDialog *ui;
};

#endif // DBDIALOG_H
