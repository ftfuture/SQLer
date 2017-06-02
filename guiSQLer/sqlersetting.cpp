#include "sqlersetting.h"

SqlerSetting::SqlerSetting(QObject *parent) : QObject(parent)
{
    loadHistoryInfo();
    loadDBListInfo();
    loadDBManagerInfo();
}

void SqlerSetting::loadDBManagerInfo()
{
    qDeleteAll(dbManagerList);
    int count = dbList.count();
    for(int i=0; i<count; i++){
        DBAdapterInfo adapterInfo = dbList.at(i);
        if(adapterInfo.isLoad == false) continue;

        DBManager *dbManager = new DBManager;
        dbManager->setDBType(adapterInfo.driver);
        dbManager->adapter->adapterInfo = adapterInfo;
        dbManager->adapter->database = QSqlDatabase::addDatabase(adapterInfo.driver,adapterInfo.connection);
        if(!adapterInfo.path.isEmpty()) dbManager->adapter->database.setDatabaseName(adapterInfo.path);
        else dbManager->adapter->database.setDatabaseName(adapterInfo.name);
        dbManager->adapter->database.setHostName(adapterInfo.address);
        dbManager->adapter->database.setPort(adapterInfo.port);
        dbManager->adapter->database.setUserName(adapterInfo.user);
        dbManagerList.append(dbManager);
    }
}
SqlerSetting &SqlerSetting::getInstance()
{
    static SqlerSetting *instance = 0;
    if( !instance ) {
        instance =  new SqlerSetting;
    }
    return *instance;
}

DBAdapterInfo &SqlerSetting::makeAdapterInfo(const QString &driver, const QString &connection, const QString &address, bool isHistory, const QString &name, \
                                             const QString &user, const QString &passwd, const QString &path, int port)
{
    DBAdapterInfo *adapterInfo = new DBAdapterInfo;
    adapterInfo->driver = driver;
    adapterInfo->connection = connection;
    adapterInfo->address = address;
    adapterInfo->addtime = QDateTime::currentDateTime();
    adapterInfo->isHistory = isHistory;
    adapterInfo->isLoad = true;
    adapterInfo->name = name;
    adapterInfo->user = user;
    adapterInfo->passwd = passwd;
    adapterInfo->path = path;
    adapterInfo->port = port;

    return *adapterInfo;
}

void SqlerSetting::loadDBListInfo()
{
    QSettings settings;
    dbList.clear();

    int size = settings.beginReadArray("AdapterList");
    for(int i=0; i<size; i++){
        settings.setArrayIndex(i);
        DBAdapterInfo adapterInfo;
        adapterInfo.driver = settings.value("driver").toString();
        adapterInfo.connection = settings.value("connection").toString();
        adapterInfo.address = settings.value("address").toString();
        adapterInfo.addtime = settings.value("addtime").toDateTime();
        adapterInfo.isHistory = settings.value("isHistory").Bool;
        adapterInfo.isLoad = settings.value("isLoad").Bool;
        adapterInfo.name = settings.value("name").toString();
        adapterInfo.path = settings.value("path").toString();
        adapterInfo.port = settings.value("port").toInt();
        adapterInfo.user = settings.value("user").toString();
        dbList.append(adapterInfo);
    }
    settings.endArray();
}

void SqlerSetting::saveDBListInfo()
{
    QSettings settings;
    settings.remove("AdapterList");
    settings.beginWriteArray("AdapterList");
    for(int i=0; i<dbList.count(); i++){
        settings.setArrayIndex(i);
        DBAdapterInfo adapterInfo = dbList.at(i);
        settings.setValue("driver",adapterInfo.driver);
        settings.setValue("connection",adapterInfo.connection);
        settings.setValue("address",adapterInfo.address);
        settings.setValue("addtime",adapterInfo.addtime);
        settings.setValue("isHistory",adapterInfo.isHistory);
        settings.setValue("isLoad",adapterInfo.isLoad);
        settings.setValue("name",adapterInfo.name);
        settings.setValue("path",adapterInfo.path);
        settings.setValue("port",adapterInfo.port);
        settings.setValue("user",adapterInfo.user);
    }
    settings.endArray();
    settings.sync();
}

void SqlerSetting::loadHistoryInfo()
{
    QSettings settings;
    historyList.clear();

    int size = settings.beginReadArray("AdapterHistory");
    for(int i=0; i<size; i++){
        settings.setArrayIndex(i);
        DBAdapterInfo adapterInfo;
        adapterInfo.driver = settings.value("driver").toString();
        adapterInfo.connection = settings.value("connection").toString();
        adapterInfo.address = settings.value("address").toString();
        adapterInfo.addtime = settings.value("addtime").toDateTime();
        adapterInfo.isHistory = settings.value("isHistory").Bool;
        adapterInfo.isLoad = settings.value("isLoad").Bool;
        adapterInfo.name = settings.value("name").toString();
        adapterInfo.path = settings.value("path").toString();
        adapterInfo.port = settings.value("port").toInt();
        adapterInfo.user = settings.value("user").toString();
        historyList.append(adapterInfo);
    }
    settings.endArray();
}

void SqlerSetting::saveHistoryInfo()
{
    QSettings settings;
    settings.remove("AdapterHistory");
    settings.beginWriteArray("AdapterHistory");
    for(int i=0; i<historyList.count(); i++){
        settings.setArrayIndex(i);
        DBAdapterInfo adapterInfo = historyList.at(i);
        settings.setValue("driver",adapterInfo.driver);
        settings.setValue("connection",adapterInfo.connection);
        settings.setValue("address",adapterInfo.address);
        settings.setValue("addtime",adapterInfo.addtime);
        settings.setValue("isHistory",adapterInfo.isHistory);
        settings.setValue("isLoad",adapterInfo.isLoad);
        settings.setValue("name",adapterInfo.name);
        settings.setValue("path",adapterInfo.path);
        settings.setValue("port",adapterInfo.port);
        settings.setValue("user",adapterInfo.user);
    }
    settings.endArray();
    settings.sync();
}
