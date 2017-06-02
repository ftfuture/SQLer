#include "dbmanager.h"
#include "sqlitemanager.h"
#include "mysqlmanager.h"
#include "postgresqlmanager.h"

DBManager::DBManager()
{
}

void DBManager::setDBType(const QString &driver)
{
    if(!driver.compare("QSQLITE"))
        adapter = dynamic_cast<DBAdapter *>(new SqliteManager);
    else if(!driver.compare("QMYSQL"))
        adapter = dynamic_cast<DBAdapter *>(new MySQLManager);
    else if(!driver.compare("QPSQL"))
        adapter = dynamic_cast<DBAdapter *>(new PostgresqlManager);
}

