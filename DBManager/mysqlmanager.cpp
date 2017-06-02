#include "mysqlmanager.h"

MySQLManager::MySQLManager()
{

}

void MySQLManager::loadSchema(const QString &t)
{
    schemaModel->setQuery(QString("DESCRIBE ").append(t), database);
}

void MySQLManager::loadIndex(const QString &t)
{
    indexModel->setQuery(QString("SHOW INDEXES FROM ").append(t), database);
}
