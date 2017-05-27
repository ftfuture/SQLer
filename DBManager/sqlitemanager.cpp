#include "sqlitemanager.h"

SqliteManager::SqliteManager()
{

}

void SqliteManager::loadSchema(const QString &t)
{
    qDebug() << "sqlite loadSchema " << t;
    schemaModel->setQuery(QString("PRAGMA table_info(").append(t).append(")"), database);
}

void SqliteManager::loadIndex(const QString &t)
{
    qDebug() << "sqlite loadIndex " << t;
    indexModel->setQuery(QString("PRAGMA index_list(").append(t).append(")"), database);
}
