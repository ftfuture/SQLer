#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql>
#include "sqlertype.h"

class DBAdapter;
class DBManager
{

public:
    DBAdapter *adapter; // Facade
    DBManager();
    void setDBType(DBAdapterType type);
};

class DBAdapter : public QObject
{
    Q_OBJECT
public:
    DBAdapterInfo adapterInfo;
    DBAdapter(QObject *parent = 0) : QObject(parent){}
    virtual void loadSchema(const QString&){}
    virtual void loadIndex(const QString&){}
    void refresh();

public:
    bool isSaved=false;
    QSqlDatabase database;
    QSqlTableModel *dataModel=NULL;
    QSqlQueryModel *schemaModel=NULL;
    QSqlQueryModel *indexModel=NULL;
};
#endif // DBMANAGER_H
