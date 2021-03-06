#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include <QObject>
#include "dbmanager.h"

class SqliteManager : public DBAdapter
{
    Q_OBJECT
public:
    void loadSchema(const QString &t) override;
    void loadIndex(const QString &t) override;
    SqliteManager();
};

#endif // SQLITEMANAGER_H
