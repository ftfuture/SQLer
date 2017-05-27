#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbdialog.h"
#include "sqlersetting.h"
#include <QtSql>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow..";
    ui->setupUi(this);
    ui->tree->refresh();
    ui->dataToolBar->layout()->setMargin(0);

    ui->dataTable->addAction(ui->insertRowAction);
    ui->dataTable->addAction(ui->deleteRowAction);
    ui->dataTable->addAction(ui->fieldStrategyAction);
    ui->dataTable->addAction(ui->rowStrategyAction);
    ui->dataTable->addAction(ui->manualStrategyAction);
    ui->dataTable->addAction(ui->submitAction);
    ui->dataTable->addAction(ui->revertAction);
    ui->dataTable->addAction(ui->selectAction);

    ui->vertSplitter->setStretchFactor(0,2);
    ui->vertSplitter->setStretchFactor(1,1);
    ui->horzSplitter->setStretchFactor(0,1);
    ui->horzSplitter->setStretchFactor(1,3);

    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(this, "addConnection", Qt::QueuedConnection);

    // set toolbar
    ui->submitAction->setShortcut(QKeySequence::Save);
    ui->submitAction->setStatusTip(tr("Commit Change."));
    ui->submitAction->setToolTip(tr("Commit Change."));
    const QIcon submitIcon = QIcon::fromTheme("Commit Change.", QIcon(":/icons/img/commit.png"));
    ui->submitAction->setIcon(submitIcon);
    ui->submitAction->setEnabled(false);

    ui->revertAction->setShortcut(QKeySequence::Redo);
    ui->revertAction->setStatusTip(tr("Rollback Change."));
    ui->revertAction->setToolTip(tr("Rollback Change."));
    const QIcon revertIcon = QIcon::fromTheme("Rollback Change.", QIcon(":/icons/img/rollback.png"));
    ui->revertAction->setIcon(revertIcon);
    ui->revertAction->setEnabled(false);

    ui->insertRowAction->setShortcut(QKeySequence::New);
    ui->insertRowAction->setStatusTip(tr("Insert Row."));
    ui->insertRowAction->setToolTip(tr("Insert Row"));
    const QIcon insertIcon = QIcon::fromTheme("Insert Row.", QIcon(":/icons/img/insert_row.png"));
    ui->insertRowAction->setIcon(insertIcon);
    ui->insertRowAction->setEnabled(false);

    ui->deleteRowAction->setShortcut(QKeySequence::Delete);
    ui->deleteRowAction->setStatusTip(tr("Delete Row."));
    ui->deleteRowAction->setToolTip(tr("Delete Row."));
    const QIcon deleteIcon = QIcon::fromTheme("Delete Row.", QIcon(":/icons/img/delete_row.png"));
    ui->deleteRowAction->setIcon(deleteIcon);
    ui->deleteRowAction->setEnabled(false);

    ui->addConnectionAction->setShortcut(QKeySequence::Open);
    ui->addConnectionAction->setStatusTip(tr("Add Connection"));
    ui->addConnectionAction->setToolTip(tr("Add Connection"));
    const QIcon addConnectionIcon = QIcon::fromTheme("Add Connection", QIcon(":/icons/img/database_connect.png"));
    ui->addConnectionAction->setIcon(addConnectionIcon);
    connect(ui->addConnectionAction, SIGNAL(triggered()), this, SLOT(addConnection()));

    ui->reconnectConnectionAction->setShortcut(QKeySequence::Refresh);
    ui->reconnectConnectionAction->setStatusTip(tr("Reconnect Connection"));
    ui->reconnectConnectionAction->setToolTip(tr("Reconnect Connection"));
    const QIcon reconnectConnectionIcon = QIcon::fromTheme("Reconnect Connection", QIcon(":/icons/img/database_reload.png"));
    ui->reconnectConnectionAction->setIcon(reconnectConnectionIcon);

    ui->mainToolBar->addAction(ui->addConnectionAction);
    ui->mainToolBar->addAction(ui->reconnectConnectionAction);
    ui->dataToolBar->addAction(ui->submitAction);
    ui->dataToolBar->addAction(ui->revertAction);
    ui->dataToolBar->addAction(ui->insertRowAction);
    ui->dataToolBar->addAction(ui->deleteRowAction);

    QObject::connect(this, SIGNAL(statusMessage(QString)),
                     ui->statusBar, SLOT(showMessage(QString)));
    emit statusMessage(tr("Ready."));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::reconnectConnection(int index)
{
    qDebug() << "reconnectConnection";
    DbDialog dialog(this);
    dialog.setAdapterInfo(SqlerSetting::getInstance().SqlerSetting::getInstance().dbManagerList.at(index)->adapter->adapterInfo);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QSqlError err = reconnectConnection(index, dialog.driverName(), dialog.connectionName(), dialog.databaseName(), dialog.hostName(), dialog.isSave(), \
                                      dialog.userName(), dialog.password(), dialog.pathName(), dialog.port());
    if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
    ui->tree->refresh();
}
QSqlError MainWindow::reconnectConnection(int index, const QString &driver, const QString &connectionName, const QString &dbName, const QString &address, bool isHistory,
                            const QString &user, const QString &passwd, const QString &path, int port)
{
    qDebug() << index  << " reconnectConnection " << driver << " , " << dbName << " , " << address << " , " << user << " , " << passwd << " , " << port;



    DBManager *dbManager = SqlerSetting::getInstance().dbManagerList.at(index);
    dbManager->adapter->adapterInfo = SqlerSetting::getInstance().makeAdapterInfo(driver, connectionName, address, \
                                                        isHistory, dbName, user, passwd, path, port);

    if(dbManager->adapter->database.isValid())
        QSqlDatabase::removeDatabase(dbManager->adapter->database.connectionName());
    dbManager->adapter->database = QSqlDatabase::addDatabase(driver,connectionName);
    if(!path.isEmpty()) dbManager->adapter->database.setDatabaseName(path);
    else dbManager->adapter->database.setDatabaseName(dbName);
    dbManager->adapter->database.setHostName(address);
    dbManager->adapter->database.setPort(port);
    dbManager->adapter->database.setUserName(user);
    dbManager->adapter->database.setPassword(passwd);

    QSqlError err;
    if(!dbManager->adapter->database.open()){
        err = dbManager->adapter->database.lastError();
        //delete dbManager;
        qDebug() << "reconnectConnection failed " << err;
        return err;
    }

    SqlerSetting::getInstance().dbList.replace(index, dbManager->adapter->adapterInfo);
    SqlerSetting::getInstance().saveDBListInfo();

    ui->tree->refresh();

    return err;
}

void MainWindow::addConnection()
{
    qDebug() << "MainWindow::addConnection";
    DbDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QSqlError err = addConnection(dialog.driverName(), dialog.connectionName(), dialog.databaseName(), dialog.hostName(), dialog.isSave(), \
                                      dialog.userName(), dialog.password(), dialog.pathName(), dialog.port());
    if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
}

QSqlError MainWindow::addConnection(const QString &driver, const QString &connectionName, const QString &dbName, const QString &address, bool isHistory,
                            const QString &user, const QString &passwd, const QString &path, int port)
{
    qDebug() << "addConnection " << driver << " , " << dbName << " , " << address << " , " << user << " , " << passwd << " , " << port;

    QSqlError err;
    if(QSqlDatabase::contains(connectionName)){
        QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                                                     "opening the connection: ") + "already have same connection name");
        return err;
    }

    DBManager *dbManager = new DBManager;
    dbManager->setDBType(driver);
    dbManager->adapter->adapterInfo = SqlerSetting::getInstance().makeAdapterInfo(driver, connectionName, address, \
                                                        isHistory, dbName, user, passwd, path, port);
    dbManager->adapter->database = QSqlDatabase::addDatabase(driver,connectionName);
    if(!path.isEmpty()) dbManager->adapter->database.setDatabaseName(path);
    else dbManager->adapter->database.setDatabaseName(dbName);
    dbManager->adapter->database.setHostName(address);
    dbManager->adapter->database.setPort(port);
    dbManager->adapter->database.setUserName(user);
    dbManager->adapter->database.setPassword(passwd);

    if(!dbManager->adapter->database.open()){
        err = dbManager->adapter->database.lastError();
        QSqlDatabase::removeDatabase(dbManager->adapter->database.connectionName());
        delete dbManager;
        qDebug() << "addConnection failed " << err;
        return err;
    }

    SqlerSetting::getInstance().dbManagerList.append(dbManager);
    if(dbManager->adapter->adapterInfo.isLoad) {
        qDebug() << "add dbtree list " << dbManager->adapter->adapterInfo.connection;
        SqlerSetting::getInstance().dbList.append(dbManager->adapter->adapterInfo);
        SqlerSetting::getInstance().saveDBListInfo();
    }

    ui->tree->refresh();
    return err;
}

void MainWindow::exec()
{
    qDebug() << "exec";
    QSqlQueryModel *model = new QSqlQueryModel(ui->dataTable);
    model->setQuery(QSqlQuery(ui->queryEdit->toPlainText(), SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->database));
    ui->dataTable->setModel(model);

    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));

    updateActions();
    ui->tree->refresh();
}

void MainWindow::showTable(const QString &t)
{
    qDebug() << "showTable";
    // data table
    if(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel == NULL)
        SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel = new QSqlTableModel(ui->tree, SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->database);

    SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel->setEditStrategy(QSqlTableModel::OnRowChange);
    SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel->setTable(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->database.driver()->escapeIdentifier(t, QSqlDriver::TableName));
    SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel->select();

    if (SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel->lastError().type() != QSqlError::NoError)
        emit statusMessage(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel->lastError().text());
    ui->dataTable->setModel(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->dataModel);
    ui->dataTable->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(ui->dataTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));

    // schema table
    if(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->schemaModel == NULL)
        SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->schemaModel = new QSqlQueryModel(ui->tree);
    SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->loadSchema(t);
    ui->schemaTable->setModel(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->schemaModel);
    if(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->schemaModel->lastError().type() != QSqlError::NoError)
        emit statusMessage(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->schemaModel->lastError().text());

    // index table
    if(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->indexModel == NULL)
        SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->indexModel = new QSqlQueryModel(ui->indexTable);
    SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->loadIndex(t);
    ui->indexTable->setModel(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->indexModel);
    if(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->indexModel->lastError().type() != QSqlError::NoError)
        emit statusMessage(SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->indexModel->lastError().text());

    updateActions();
}

void MainWindow::showMetaData(const QString &t)
{
    qDebug() << "show Metadata";
    QSqlRecord rec = SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex)->adapter->database.record(t);
    QStandardItemModel *model = new QStandardItemModel(ui->dataTable);

    model->insertRows(0, rec.count());
    model->insertColumns(0, 7);

    model->setHeaderData(0, Qt::Horizontal, "Fieldname");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Length");
    model->setHeaderData(3, Qt::Horizontal, "Precision");
    model->setHeaderData(4, Qt::Horizontal, "Required");
    model->setHeaderData(5, Qt::Horizontal, "AutoValue");
    model->setHeaderData(6, Qt::Horizontal, "DefaultValue");


    for (int i = 0; i < rec.count(); ++i) {
        QSqlField fld = rec.field(i);
        model->setData(model->index(i, 0), fld.name());
        model->setData(model->index(i, 1), fld.typeID() == -1
                ? QString(QMetaType::typeName(fld.type()))
                : QString("%1 (%2)").arg(QMetaType::typeName(fld.type())).arg(fld.typeID()));
        model->setData(model->index(i, 2), fld.length());
        model->setData(model->index(i, 3), fld.precision());
        model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
                : QVariant(bool(fld.requiredStatus())));
        model->setData(model->index(i, 5), fld.isAutoValue());
        model->setData(model->index(i, 6), fld.defaultValue());
    }

    ui->dataTable->setModel(model);
    ui->dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    updateActions();
}

void MainWindow::insertRow()
{
    qDebug() << "insertRow";
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (!model)
        return;

    QModelIndex insertIndex = ui->dataTable->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    ui->dataTable->setCurrentIndex(insertIndex);
    ui->dataTable->edit(insertIndex);
}

void MainWindow::deleteRow()
{
    qDebug() << "deleteRow";
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (!model)
        return;

    QModelIndexList currentSelection = ui->dataTable->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        model->removeRow(currentSelection.at(i).row());
    }

    updateActions();
}

void MainWindow::updateActions()
{
    qDebug() << "updateActions";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    bool enableIns = tm;
    bool enableDel = enableIns && ui->dataTable->currentIndex().isValid();

    ui->insertRowAction->setEnabled(enableIns);
    ui->deleteRowAction->setEnabled(enableDel);

    ui->fieldStrategyAction->setEnabled(tm);
    ui->rowStrategyAction->setEnabled(tm);
    ui->manualStrategyAction->setEnabled(tm);
    ui->submitAction->setEnabled(tm);
    ui->revertAction->setEnabled(tm);
    ui->selectAction->setEnabled(tm);

    if (tm) {
        QSqlTableModel::EditStrategy es = tm->editStrategy();
        ui->fieldStrategyAction->setChecked(es == QSqlTableModel::OnFieldChange);
        ui->rowStrategyAction->setChecked(es == QSqlTableModel::OnRowChange);
        ui->manualStrategyAction->setChecked(es == QSqlTableModel::OnManualSubmit);
    }
}

void MainWindow::about()
{
    qDebug() << "about";
    QMessageBox::about(this, tr("About"), tr("The SQL Browser demonstration "
        "shows how a data browser can be used to visualize the results of SQL"
                                             "statements on a live database"));
}

void MainWindow::on_fieldStrategyAction_triggered()
{
    qDebug() << "on_fieldStrategyAction_triggered";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->setEditStrategy(QSqlTableModel::OnFieldChange);
}

void MainWindow::on_rowStrategyAction_triggered()
{
    qDebug() << "on_rowStrategyAction_triggered";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->setEditStrategy(QSqlTableModel::OnRowChange);
}

void MainWindow::on_manualStrategyAction_triggered()
{
    qDebug() << "on_manualStategyAction_triggered";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void MainWindow::on_submitAction_triggered()
{
    qDebug() << "on_submitAction_triggered";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->submitAll();
}

void MainWindow::on_revertAction_triggered()
{
    qDebug() << "on_revertAction_triggered";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->revertAll();
}

void MainWindow::on_selectAction_triggered()
{
    qDebug() << "on_selectAction_triggered";
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->select();
}

void MainWindow::on_insertRowAction_triggered(){
    qDebug() << "on_insertRowAction_tiggered";
    insertRow();
}

void MainWindow::on_deleteRowAction_triggered(){
    qDebug() << "on_deleteRowAction_tirggered()";
    deleteRow();
}

void MainWindow::on_clearButton_clicked()
{
    qDebug() << "on_clearButton_clicked";
    ui->queryEdit->clear();
    ui->queryEdit->setFocus();
}

void MainWindow::on_submitButton_clicked()
{
    qDebug() << "on_submitButton_clicked";
    exec();
    ui->queryEdit->setFocus();
}

 QVariant TableInfoModel::data(const QModelIndex &idx, int role) const
{
     qDebug() << "TableInfoModel::data";
    if (role == Qt::BackgroundRole && isDirty(idx))
        return QBrush(QColor(Qt::yellow));
    return QSqlTableModel::data(idx, role);
}

void MainWindow::on_reconnectConnectionAction_triggered()
{
    qDebug() << "on_reconnectConnectionAction_triggered";
    if(ui->tree->selectedIndex == -1) return;
    DBManager *dbManager = SqlerSetting::getInstance().dbManagerList.at(ui->tree->selectedIndex);
    if(dbManager->adapter->database.isOpen()) dbManager->adapter->database.close();
    if(!dbManager->adapter->database.open()){
        QSqlError err = dbManager->adapter->database.lastError();
        QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                   "opening the connection: ") + err.text());
    }
    ui->tree->refresh();
}
void MainWindow::on_tree_activeDBRequested(int index)
{
    qDebug() << "on_tree_activeDBRequested";
    ui->queryGroupBox->setTitle(tr("SQL Query : ") + DbTree::qDBCaption(SqlerSetting::getInstance().dbManagerList.at(index)->adapter));
}
