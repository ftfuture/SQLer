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
    QList<DBManager *> dbManagerList;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
                                const QString &user, const QString &passwd, int port);
    void insertRow();
    void deleteRow();
    void updateActions();

private slots:
    void addConnection();
    void on_action_Add_Database_triggered();
    void exec();
    void about();
    void showTable(const QString &table);
    void showMetaData(const QString &table);
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
    { showTable(table); }
    void on_tree_metaDataRequested(const QString &table)
    { showMetaData(table); }
private:
    Ui::MainWindow *ui;

signals:
    void statusMessage(const QString &message);
};

class CustomModel: public QSqlTableModel
{
    Q_OBJECT
public:
    explicit CustomModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase()):QSqlTableModel(parent, db) {}
    QVariant data(const QModelIndex &idx, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole && isDirty(idx))
            return QBrush(QColor(Qt::yellow));
        return QSqlTableModel::data(idx, role);
    }
};

class TableInfoModel: public QSqlTableModel
{
    Q_OBJECT
public:
    explicit TableInfoModel(QObject *parent=0, QSqlDatabase db = QSqlDatabase()):QSqlTableModel(parent,db){}
    QVariant data(const QModelIndex &idx, int role) const;
};

#endif // MAINWINDOW_H
