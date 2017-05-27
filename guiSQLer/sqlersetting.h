#ifndef SQLERSETTING_H
#define SQLERSETTING_H

#include <QObject>
#include "sqlertype.h"
#include "dbmanager.h"

class SqlerSetting : public QObject
{
    Q_OBJECT
public:
    QList<DBManager *> dbManagerList;
    QList<DBAdapterInfo> historyList;
    QList<DBAdapterInfo> dbList;
    explicit SqlerSetting(QObject *parent = 0);
    static SqlerSetting &getInstance();
    void loadDBListInfo();
    void saveDBListInfo();
    void loadHistoryInfo();
    void saveHistoryInfo();
    void loadDBManagerInfo();
    DBAdapterInfo &makeAdapterInfo(const QString &driver, const QString &connection, const QString &address, bool isHistory, const QString &name, \
                                   const QString &user, const QString &passwd, const QString &path, int port);


signals:

public slots:
};

#endif // SQLERSETTING_H
