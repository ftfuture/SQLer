#ifndef DBDIALOG_H
#define DBDIALOG_H

#include <QDialog>

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
    int port() const;
    QString getDbPath(bool newFileMode, const QString &startWith);

private slots:
    void on_testButton_clicked();
    void on_cancelButton_clicked();
    void on_okButton_clicked();

    void on_newButton_clicked();

    void on_openButton_clicked();

private:
    Ui::DbDialog *ui;
};

#endif // DBDIALOG_H
