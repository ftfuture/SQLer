#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql>

enum DBManagerType {
    UNKNOWN,
    SQLITE,
    MYSQL,
    MSSQL,
    POSTGRESQL,
    ORACLE,
    DB2
};
class DBAdapter;
class DBManager
{

public:
    DBAdapter *adapter; // Facade
    DBManager();
    void setDBType(DBManagerType type);
};

class DBAdapter : public QObject
{
    Q_OBJECT
public:
    DBAdapter(QObject *parent = 0) : QObject(parent){}
    void refresh();

public:
    QSqlDatabase *database;
    QSqlTableModel *schemaTableModel;
    QSqlTableModel *dataTableModel;
};
#endif // DBMANAGER_H
