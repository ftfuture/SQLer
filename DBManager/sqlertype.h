#ifndef SQLERTYPE_H
#define SQLERTYPE_H
#include <QtCore>

#define MAX_COUNT_HISTORY 10

typedef struct DBAdapterInfo {
    QString driver;
    QString connection;
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
