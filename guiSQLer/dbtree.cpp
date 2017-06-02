#include "dbtree.h"
#include <QtWidgets>
#include <QtSql>
#include "sqlersetting.h"

DbTree::DbTree(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tree = new QTreeWidget(this);
    tree->setObjectName(QLatin1String("tree"));
    tree->setHeaderLabels(QStringList(tr("database")));

    QAction *refreshAction = new QAction(tr("Refresh"), tree);
    deleteAction = new QAction(tr("Delete"), tree);

    connect(refreshAction, SIGNAL(triggered()), SLOT(refresh()));
    connect(deleteAction, SIGNAL(triggered()), SLOT(deleteDB()));

    tree->addAction(refreshAction);
    tree->addAction(deleteAction);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);

    layout->setMargin(3);
    layout->addWidget(tree);
    QMetaObject::connectSlotsByName(this);
}

void DbTree::deleteDB()
{
    if(tree->currentIndex().row() < 0 || tree->currentItem()->parent()) return;

    QModelIndex index = tree->currentIndex();
    DBManager *dbManger = SqlerSetting::getInstance().dbManagerList.at(index.row());

    QString dbName;
    if(!dbManger->adapter->adapterInfo.path.isEmpty())
        dbName = dbManger->adapter->adapterInfo.path;
    else if(!dbManger->adapter->adapterInfo.address.isEmpty())
        dbName = dbManger->adapter->adapterInfo.address.append(':').append(dbManger->adapter->adapterInfo.name);

    QMessageBox msgBox;
    msgBox.setText("Confirm Delete.");
    msgBox.setInformativeText("Do you want to delete "+dbName+" ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Yes){
        if(dbManger->adapter->database.isOpen()) dbManger->adapter->database.close();
        QSqlDatabase::removeDatabase(dbManger->adapter->database.connectionName());
        SqlerSetting::getInstance().dbManagerList.removeAt(index.row());
        SqlerSetting::getInstance().dbList.removeAt(index.row());
        SqlerSetting::getInstance().saveDBListInfo();
    }

    refresh();
}

QString DbTree::qDBCaption(DBAdapterInfo adapterInfo)
{
    QString dbCaption = adapterInfo.driver;
    if(!adapterInfo.user.isEmpty()) dbCaption.append(':').append(adapterInfo.user).append(QLatin1Char('@'));
    if(!adapterInfo.name.isEmpty()) dbCaption.append(':').append(adapterInfo.name);
    if(!adapterInfo.path.isEmpty()) dbCaption.append(':').append(adapterInfo.path);
    if(!adapterInfo.address.isEmpty()) dbCaption.append(':').append(adapterInfo.address);
    return dbCaption;
}

void DbTree::refresh()
{
    tree->clear();

    bool isActiveDB = false;
    for(int i=0; i<SqlerSetting::getInstance().dbManagerList.count(); i++){
        QTreeWidgetItem *root = new QTreeWidgetItem(tree);

        QSqlDatabase db = SqlerSetting::getInstance().dbManagerList.at(i)->adapter->database;
        root->setText(0, qDBCaption(SqlerSetting::getInstance().dbManagerList.at(i)->adapter->adapterInfo));
        if(SqlerSetting::getInstance().dbManagerList.at(i)->adapter->database.connectionName() == currentDB) {
            isActiveDB = true;
        }
        if(db.isOpen()) {
            QStringList tables = db.tables();
            QSqlTableModel *tableModel = new QSqlTableModel(this, db);
            for(int t=0; t<tables.count(); ++t) {
                QTreeWidgetItem *table = new QTreeWidgetItem(root);
                table->setText(0, tables.at(t));
                tableModel->setTable(tables.at(t));

                // column..
                QSqlDriver *driver = db.driver();
                QSqlRecord record = driver->record(tables.at(t));
                for(int col=0; col<record.count(); ++col){
                    QTreeWidgetItem *column = new QTreeWidgetItem(table);
                    column->setText(0,tableModel->record().fieldName(col));
                }
            }
        }
    }

    if(!isActiveDB && SqlerSetting::getInstance().dbManagerList.count() != 0) {
        currentDB = SqlerSetting::getInstance().dbManagerList.at(SqlerSetting::getInstance().dbManagerList.count()-1)->adapter->database.connectionName();
        setActive(tree->topLevelItem(0));
    }

    tree->doItemsLayout();
}

static void qSetBold(QTreeWidgetItem *item, bool bold)
{
    QFont font = item->font(0);
    font.setBold(bold);
    item->setFont(0, font);
}

void DbTree::setActive(QTreeWidgetItem *item)
{
    for(int i=0; i<tree->topLevelItemCount(); ++i) {
        if(tree->topLevelItem(i)->font(0).bold()) qSetBold(tree->topLevelItem(i), false);
    }

    if(!item) return;
    qSetBold(item, true);

    selectedIndex = tree->indexOfTopLevelItem(item);
    currentDB = SqlerSetting::getInstance().dbManagerList.at(selectedIndex)->adapter->database.connectionName();
    if(!SqlerSetting::getInstance().dbManagerList.at(selectedIndex)->adapter->database.isOpen()){
        if(!SqlerSetting::getInstance().dbManagerList.at(selectedIndex)->adapter->database.open()){
            QSqlError err = SqlerSetting::getInstance().dbManagerList.at(selectedIndex)->adapter->database.lastError();
            if (err.type() == QSqlError::ConnectionError){
                emit resetConnectionRequested(selectedIndex);
            } else if (err.type() != QSqlError::NoError){
                QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                           "opening the connection: ") + err.text());
                return;
            }
        }
        refresh();
    }

    emit activeDBRequested(selectedIndex);

}

void DbTree::on_tree_itemActivated(QTreeWidgetItem *item, int /* column */)
{
    if (!item) return;
    if (!item->parent()) {// database
        setActive(item);
    } else { // table
        setActive(item->parent());
        emit dataTableActivated(item->text(0));
    }
}

void DbTree::on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    deleteAction->setEnabled(current && !current->parent());
}
