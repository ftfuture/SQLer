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
    const QIcon submitIcon = QIcon::fromTheme("Commit Change.", QIcon(":/icons/image/commit.png"));
    ui->submitAction->setIcon(submitIcon);
    ui->submitAction->setEnabled(false);

    ui->revertAction->setShortcut(QKeySequence::Redo);
    ui->revertAction->setStatusTip(tr("Rollback Change."));
    ui->revertAction->setToolTip(tr("Rollback Change."));
    const QIcon revertIcon = QIcon::fromTheme("Rollback Change.", QIcon(":/icons/image/rollback.png"));
    ui->revertAction->setIcon(revertIcon);
    ui->revertAction->setEnabled(false);

    ui->insertRowAction->setShortcut(QKeySequence::New);
    ui->insertRowAction->setStatusTip(tr("Insert Row."));
    ui->insertRowAction->setToolTip(tr("Insert Row"));
    const QIcon insertIcon = QIcon::fromTheme("Insert Row.", QIcon(":/icons/image/insert_row.png"));
    ui->insertRowAction->setIcon(insertIcon);
    ui->insertRowAction->setEnabled(false);

    ui->deleteRowAction->setShortcut(QKeySequence::Delete);
    ui->deleteRowAction->setStatusTip(tr("Delete Row."));
    ui->deleteRowAction->setToolTip(tr("Delete Row."));
    const QIcon deleteIcon = QIcon::fromTheme("Delete Row.", QIcon(":/icons/image/delete_row.png"));
    ui->deleteRowAction->setIcon(deleteIcon);
    ui->deleteRowAction->setEnabled(false);

    ui->addConnectionAction->setShortcut(QKeySequence::Open);
    ui->addConnectionAction->setStatusTip(tr("Add Connection"));
    ui->addConnectionAction->setToolTip(tr("Add Connection"));
    const QIcon addConnectionIcon = QIcon::fromTheme("Add Connection", QIcon(":/icons/image/database_connect.png"));
    ui->addConnectionAction->setIcon(addConnectionIcon);
    connect(ui->addConnectionAction, SIGNAL(triggered()), this, SLOT(addConnection()));

    ui->reconnectConnectionAction->setShortcut(QKeySequence::Refresh);
    ui->reconnectConnectionAction->setStatusTip(tr("Reconnect Connection"));
    ui->reconnectConnectionAction->setToolTip(tr("Reconnect Connection"));
    const QIcon reconnectConnectionIcon = QIcon::fromTheme("Reconnect Connection", QIcon(":/icons/image/database_reload.png"));
    ui->reconnectConnectionAction->setIcon(reconnectConnectionIcon);

    ui->deleteConnectionAction->setStatusTip(tr("Delete Connection"));
    ui->deleteConnectionAction->setToolTip(tr("Delete Connection"));
    const QIcon deleteConnectionIcon = QIcon::fromTheme("Delete Connection", QIcon(":/icons/image/database_disconnect.png"));
    ui->deleteConnectionAction->setIcon(deleteConnectionIcon);

    ui->mainToolBar->addAction(ui->addConnectionAction);
    ui->mainToolBar->addAction(ui->reconnectConnectionAction);
    ui->mainToolBar->addAction(ui->deleteConnectionAction);
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

void MainWindow::on_reconnectConnectionAction_triggered()
{
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

void MainWindow::resetConnection(int index)
{
    DbDialog dialog(this);
    dialog.setAdapterInfo(SqlerSetting::getInstance().SqlerSetting::getInstance().dbManagerList.at(index)->adapter->adapterInfo);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QSqlError err = resetConnection(index, dialog.driverName(), dialog.connectionName(), dialog.databaseName(), dialog.hostName(), dialog.isSave(), \
                                      dialog.userName(), dialog.password(), dialog.pathName(), dialog.port());
    if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
    ui->tree->refresh();
}
QSqlError MainWindow::resetConnection(int index, const QString &driver, const QString &connectionName, const QString &dbName, const QString &address, bool isHistory,
                            const QString &user, const QString &passwd, const QString &path, int port)
{
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
        qWarning() << "resetConnection failed " << err;
        return err;
    }

    SqlerSetting::getInstance().dbList.replace(index, dbManager->adapter->adapterInfo);
    SqlerSetting::getInstance().saveDBListInfo();

    ui->tree->refresh();
    return err;
}

void MainWindow::addConnection()
{
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
        qWarning() << "addConnection failed " << err;
        return err;
    }

    SqlerSetting::getInstance().dbManagerList.append(dbManager);
    if(dbManager->adapter->adapterInfo.isLoad) {
        SqlerSetting::getInstance().dbList.append(dbManager->adapter->adapterInfo);
        SqlerSetting::getInstance().saveDBListInfo();
    }

    ui->tree->refresh();
    return err;
}

void MainWindow::exec()
{
    if(ui->tree->selectedIndex == -1) return;
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

void MainWindow::setupTable(const QString &t)
{   
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

void MainWindow::insertRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (!model) return;

    QModelIndex insertIndex = ui->dataTable->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    ui->dataTable->setCurrentIndex(insertIndex);
    ui->dataTable->edit(insertIndex);
}

void MainWindow::deleteRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (!model) return;

    QModelIndexList currentSelection = ui->dataTable->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0) continue;
        model->removeRow(currentSelection.at(i).row());
    }

    updateActions();
}

void MainWindow::updateActions()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    bool enableIns = tableModel;
    bool enableDel = enableIns && ui->dataTable->currentIndex().isValid();

    ui->insertRowAction->setEnabled(enableIns);
    ui->deleteRowAction->setEnabled(enableDel);

    ui->fieldStrategyAction->setEnabled(tableModel);
    ui->rowStrategyAction->setEnabled(tableModel);
    ui->manualStrategyAction->setEnabled(tableModel);
    ui->submitAction->setEnabled(tableModel);
    ui->revertAction->setEnabled(tableModel);
    ui->selectAction->setEnabled(tableModel);

    if (tableModel) {
        QSqlTableModel::EditStrategy es = tableModel->editStrategy();
        ui->fieldStrategyAction->setChecked(es == QSqlTableModel::OnFieldChange);
        ui->rowStrategyAction->setChecked(es == QSqlTableModel::OnRowChange);
        ui->manualStrategyAction->setChecked(es == QSqlTableModel::OnManualSubmit);
    }
}

void MainWindow::on_fieldStrategyAction_triggered()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tableModel) tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
}

void MainWindow::on_rowStrategyAction_triggered()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tableModel) tableModel->setEditStrategy(QSqlTableModel::OnRowChange);
}

void MainWindow::on_manualStrategyAction_triggered()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tableModel) tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void MainWindow::on_submitAction_triggered()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tableModel) tableModel->submitAll();
}

void MainWindow::on_revertAction_triggered()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tableModel) tableModel->revertAll();
}

void MainWindow::on_selectAction_triggered()
{
    QSqlTableModel * tableModel = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tableModel) tableModel->select();
}

void MainWindow::on_insertRowAction_triggered(){
    insertRow();
}

void MainWindow::on_deleteRowAction_triggered(){
    deleteRow();
}

void MainWindow::on_clearButton_clicked()
{
    ui->queryEdit->clear();
    ui->queryEdit->setFocus();
}

void MainWindow::on_submitButton_clicked()
{
    exec();
    ui->queryEdit->setFocus();
}

void MainWindow::on_tree_activeDBRequested(int index)
{
    ui->queryGroupBox->setTitle(tr("SQL Query - ") + DbTree::qDBCaption(SqlerSetting::getInstance().dbManagerList.at(index)->adapter->adapterInfo));
}

void MainWindow::on_deleteConnectionAction_triggered()
{
    ui->tree->deleteDB();
}
