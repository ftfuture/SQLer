#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include <QObject>
#include "dbmanager.h"

class SqliteManager : public DBAdapter
{
    Q_OBJECT
public:
    SqliteManager();
};

#endif // SQLITEMANAGER_H
