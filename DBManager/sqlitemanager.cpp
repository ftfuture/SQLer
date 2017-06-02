#include "sqlitemanager.h"

SqliteManager::SqliteManager()
{

}

void SqliteManager::loadSchema(const QString &t)
{
    schemaModel->setQuery(QString("PRAGMA table_info(").append(t).append(")"), database);
}

void SqliteManager::loadIndex(const QString &t)
{
    indexModel->setQuery(QString("PRAGMA index_list(").append(t).append(")"), database);
}
