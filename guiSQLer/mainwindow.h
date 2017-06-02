#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "dbmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString m_sSettingsFile;
    QSettings settings;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadDBManagerInfo();
    void saveDBManagerInfo();
    QSqlError addConnection(const QString &driver, const QString &connectionName, const QString &dbName, const QString &address, bool isHistory,
                                const QString &user, const QString &passwd,const QString &path, int port);
    QSqlError resetConnection(int index, const QString &driver, const QString &connectionName, const QString &dbName, const QString &address, bool isHistory,
                                  const QString &user, const QString &passwd,const QString &path, int port);
    void insertRow();
    void deleteRow();
    void updateActions();

private slots:
    void addConnection();
    void resetConnection(int index);
    void exec();
    void setupTable(const QString &table);
    void currentChanged() { updateActions(); }

    void on_insertRowAction_triggered();
    void on_deleteRowAction_triggered();
    void on_fieldStrategyAction_triggered();
    void on_rowStrategyAction_triggered();
    void on_manualStrategyAction_triggered();
    void on_submitAction_triggered();
    void on_revertAction_triggered();
    void on_selectAction_triggered();
    void on_clearButton_clicked();
    void on_submitButton_clicked();
    void on_tree_dataTableActivated(const QString &table)
    { setupTable(table); }
    void on_tree_resetConnectionRequested(int index)
    { resetConnection(index); }
    void on_tree_activeDBRequested(int index);
    void on_reconnectConnectionAction_triggered();
    void on_deleteConnectionAction_triggered();

private:
    Ui::MainWindow *ui;

signals:
    void statusMessage(const QString &message);
};

#endif // MAINWINDOW_H
