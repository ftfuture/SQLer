#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql>
//#include <QDebug>
#include "sqlertype.h"

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

class DBManager
{

public:
    DBAdapter *adapter; // Facade
    DBManager();
    void setDBType(const QString &driver);
};
#endif // DBMANAGER_H
