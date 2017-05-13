#include "dbmanager.h"
#include <QDebug>
#include "sqlitemanager.h"
#include "mysqlmanager.h"

DBManager::DBManager()
{
    qDebug() << "DB Manager Activated.";
}

void DBManager::setDBType(DBAdapterType type)
{
    qDebug() << "DB Type Set - " << type;

    switch (type) {
    case UNKNOWN:
        break;
    case SQLITE:
        adapter = dynamic_cast<DBAdapter *>(new SqliteManager);
        break;
    case MYSQL:
        adapter = dynamic_cast<DBAdapter *>(new MySQLManager);
        break;
    case MSSQL:
        break;
    case POSTGRESQL:
        break;
    case ORACLE:
        break;
    case DB2:
        break;
    }
}

