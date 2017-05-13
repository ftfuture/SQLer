#ifndef SQLERTYPE_H
#define SQLERTYPE_H
#include <QtCore>

#define MAX_COUNT_HISTORY 10

enum DBAdapterType {
    UNKNOWN,
    SQLITE,
    MYSQL,
    MSSQL,
    POSTGRESQL,
    ORACLE,
    DB2
};

typedef struct DBAdapterInfo {
    DBAdapterType type;
    QString typeStr;
    QString name;
    QString path;
    QString user;
    QString passwd;
    QString address;
    int port;
    QDateTime addtime;
    bool isLoad;
    bool isHistory;
} DBAdapterInfo;

#endif // SQLERTYPE_H
