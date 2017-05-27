#ifndef POSTGRESQLMANAGER_H
#define POSTGRESQLMANAGER_H

#include <QObject>
#include "dbmanager.h"

class PostgresqlManager : public DBAdapter
{
    Q_OBJECT
public:
    explicit PostgresqlManager();
    void loadSchema(const QString &t) override;
    void loadIndex(const QString &t) override;

signals:

public slots:
};

#endif // POSTGRESQLMANAGER_H
