#include "dbmanager.h"
#include "sqlitemanager.h"
#include "mysqlmanager.h"
#include "postgresqlmanager.h"

DBManager::DBManager()
{
    qDebug() << "DB Manager Activated.";
}

void DBManager::setDBType(const QString &driver)
{
    qDebug() << "DB Type Set - " << driver;

    if(!driver.compare("QSQLITE"))
        adapter = dynamic_cast<DBAdapter *>(new SqliteManager);
    else if(!driver.compare("QMYSQL"))
        adapter = dynamic_cast<DBAdapter *>(new MySQLManager);
    else if(!driver.compare("QPSQL"))
        adapter = dynamic_cast<DBAdapter *>(new PostgresqlManager);
//    switch (type) {
//    case SQLITE:
//        adapter = dynamic_cast<DBAdapter *>(new SqliteManager);
//        break;
//    case MYSQL:
//        adapter = dynamic_cast<DBAdapter *>(new MySQLManager);
//        break;
//    case MSSQL:
//        break;
//    case POSTGRESQL:
//        break;
//    case ORACLE:
//        break;
//    case DB2:
//        break;
//    }
}

