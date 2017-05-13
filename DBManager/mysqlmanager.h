#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H
#include <QObject>
#include "dbmanager.h"

class MySQLManager : public DBAdapter
{
    Q_OBJECT
public:
    void loadSchema(const QString &t) override;
    void loadIndex(const QString &t) override;
    MySQLManager();
};

#endif // MYSQLMANAGER_H
