#include "mysqlmanager.h"

MySQLManager::MySQLManager()
{

}

void MySQLManager::loadSchema(const QString &t)
{
    qDebug() << "mysql loadSchema " << t;
    schemaModel->setQuery(QString("DESCRIBE ").append(t), database);
}

void MySQLManager::loadIndex(const QString &t)
{
    qDebug() << "mysql loadIndex " << t;
    indexModel->setQuery(QString("SHOW INDEXES FROM ").append(t), database);
}
