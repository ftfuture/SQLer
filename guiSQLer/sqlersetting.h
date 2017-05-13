#ifndef SQLERSETTING_H
#define SQLERSETTING_H

#include <QObject>
#include "sqlertype.h"

class SqlerSetting : public QObject
{
    Q_OBJECT
public:
    QList<DBAdapterInfo> historyList;
    explicit SqlerSetting(QObject *parent = 0);
    static SqlerSetting &getInstance();
    void loadHistoryInfo();
    void saveHistoryInfo();
    DBAdapterInfo &makeAdapterInfo(const QString &address, bool isHistory, const QString &name, \
                                   const QString &passwd, const QString &path, int port, const QString &type);

signals:

public slots:
};

#endif // SQLERSETTING_H
