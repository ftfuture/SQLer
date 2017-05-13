#include "sqlersetting.h"

SqlerSetting::SqlerSetting(QObject *parent) : QObject(parent)
{
    loadHistoryInfo();
}

SqlerSetting &SqlerSetting::getInstance()
{
    static SqlerSetting *instance = 0;
    if( !instance ) {
        instance =  new SqlerSetting;
    }
    return *instance;
}

DBAdapterInfo &SqlerSetting::makeAdapterInfo(const QString &address, bool isHistory, const QString &name, \
                                             const QString &passwd, const QString &path, int port,const  QString &type)
{
    DBAdapterInfo *adapterInfo = new DBAdapterInfo;
    adapterInfo->address = address;
    adapterInfo->addtime = QDateTime::currentDateTime();
    adapterInfo->isHistory = isHistory;
    adapterInfo->isLoad = true;
    adapterInfo->name = name;
    adapterInfo->passwd = passwd;
    adapterInfo->path = path;
    adapterInfo->port = port;
    if(type.contains("sqlite", Qt::CaseInsensitive)) {
        adapterInfo->type = DBAdapterType::SQLITE;
        adapterInfo->typeStr = "SQLITE";
    } else if(type.contains("mysql", Qt::CaseInsensitive)) {
        adapterInfo->type = DBAdapterType::MYSQL;
        adapterInfo->typeStr = "MYSQL";
    } else if(type.contains("postgresql", Qt::CaseInsensitive)) {
        adapterInfo->type = DBAdapterType::POSTGRESQL;
        adapterInfo->typeStr = "POSTGRESQL";
    } else {
        adapterInfo->type = DBAdapterType::UNKNOWN;
        adapterInfo->typeStr = "UNKNOWN";
    }

    return *adapterInfo;
}

void SqlerSetting::loadHistoryInfo()
{
    QSettings settings;
    historyList.clear();

    int size = settings.beginReadArray("Adapter History");
    for(int i=0; i<size; i++){
        settings.setArrayIndex(i);
        DBAdapterInfo adapterInfo;
        adapterInfo.address = settings.value("address").toString();
        adapterInfo.addtime = settings.value("addtime").toDateTime();
        adapterInfo.isHistory = settings.value("isHistory").Bool;
        adapterInfo.isLoad = settings.value("isLoad").Bool;
        adapterInfo.name = settings.value("name").toString();
        adapterInfo.passwd = settings.value("passwd").toString();
        adapterInfo.path = settings.value("path").toString();
        adapterInfo.port = settings.value("port").toInt();
        adapterInfo.type = static_cast<DBAdapterType>(settings.value("type").toInt());
        adapterInfo.typeStr = settings.value("typeStr").toString();
        adapterInfo.user = settings.value("user").toString();
        historyList.append(adapterInfo);
    }
    settings.endArray();
}

void SqlerSetting::saveHistoryInfo()
{
    QSettings settings;
    settings.remove("Adapter History");
    settings.beginWriteArray("Adapter History");
    for(int i=0; i<historyList.count(); i++){
        settings.setArrayIndex(i);
        DBAdapterInfo adapterInfo = historyList.at(i);
        settings.setValue("address",adapterInfo.address);
        settings.setValue("addtime",adapterInfo.addtime);
        settings.setValue("isHistory",adapterInfo.isHistory);
        settings.setValue("isLoad",adapterInfo.isLoad);
        settings.setValue("name",adapterInfo.name);
        settings.setValue("passwd",adapterInfo.passwd);
        settings.setValue("path",adapterInfo.path);
        settings.setValue("port",adapterInfo.port);
        settings.setValue("type",adapterInfo.type);
        settings.setValue("typeStr",adapterInfo.typeStr);
        settings.setValue("user",adapterInfo.user);
    }
    settings.endArray();
    settings.sync();
}
