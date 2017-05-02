#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbdialog.h"
#include <QtSql>
#include <QStandardItemModel>
#include <QMessageBox>
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tree->dbManagerList = &dbManagerList;

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
    ui->horzSplitter->setStretchFactor(1,2);

    if(QSqlDatabase::drivers().isEmpty())
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This app requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));

    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(this, "addConnection", Qt::QueuedConnection);
     QObject::connect(this, SIGNAL(statusMessage(QString)),
                     ui->statusBar, SLOT(showMessage(QString)));
    emit statusMessage(tr("Ready."));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Add_Database_triggered()
{
    addConnection();
}
QSqlError MainWindow::addConnection(const QString &driver, const QString &dbName, const QString &host,
                            const QString &user, const QString &passwd, int port)
{
    qDebug() << "addConnection " << driver << " , " << dbName << " , " << host << " , " << user << " , " << passwd << " , " << port;
    static int cCount = 0;

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase(driver);
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        qDebug() << "addConnection failed " << err;
        return err;
        //db = QSqlDatabase();
        //QSqlDatabase::removeDatabase(QString("Sqler%1").arg(cCount));
    }

    if(driver.contains("sqlite", Qt::CaseInsensitive)) {
        DBManager *dbManager = new DBManager;
        dbManager->setDBType(DBManagerType::SQLITE);
        dbManager->adapter->database = &db;
        dbManagerList.append(dbManager);
    }
    ui->tree->refresh();

    return err;
}

void MainWindow::addConnection()
{
    DbDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    //if (dialog.useInMemoryDatabase()) {
//        QSqlDatabase::database("in_mem_db", false).close();
//        QSqlDatabase::removeDatabase("in_mem_db");
//        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "in_mem_db");
//        db.setDatabaseName(":memory:");
//        if (!db.open())
//            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
//                                                                         "opening the connection: ") + db.lastError().text());
//        QSqlQuery q("", db);
//        q.exec("drop table Movies");
//        q.exec("drop table Names");
//        q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
//        q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
//        q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
//        q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
//        q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
//        q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
//        q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
//        q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
//        q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
//        q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
//        q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
//        q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
//        ui->tree->refresh();
    //} else {
        QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                           dialog.userName(), dialog.password(), dialog.port());
        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
//    }
}
void MainWindow::exec()
{
    QSqlQueryModel *model = new QSqlQueryModel(ui->dataTable);
    model->setQuery(QSqlQuery(ui->queryEdit->toPlainText(), ui->tree->currentDatabase()));
    ui->dataTable->setModel(model);

    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));

    updateActions();
}

void MainWindow::showTable(const QString &t)
{
    // data
    QSqlTableModel *model = new QSqlTableModel(ui->dataTable, ui->tree->currentDatabase());
    //model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(ui->tree->currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    ui->dataTable->setModel(model);
    ui->dataTable->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(ui->dataTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));

    // schema
    QSqlQueryModel *schemaModel = new QSqlQueryModel(ui->schemaTable);
    //QString *querystring = "PRAGMA table_info(" << t << ")";
    schemaModel->setQuery(QString("PRAGMA table_info(").append(t).append(")"), ui->tree->currentDatabase());
    ui->schemaTable->setModel(schemaModel);
    if(schemaModel->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
//    else if(!model->query().isSelect())
//        emit statusMessage("Schema Query Ok");
    updateActions();
}

void MainWindow::showMetaData(const QString &t)
{
    QSqlRecord rec = ui->tree->currentDatabase().record(t);
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
    QMessageBox::about(this, tr("About"), tr("The SQL Browser demonstration "
        "shows how a data browser can be used to visualize the results of SQL"
                                             "statements on a live database"));
}

void MainWindow::on_fieldStrategyAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->setEditStrategy(QSqlTableModel::OnFieldChange);
}

void MainWindow::on_rowStrategyAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->setEditStrategy(QSqlTableModel::OnRowChange);
}

void MainWindow::on_manualStrategyAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void MainWindow::on_submitAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->submitAll();
}

void MainWindow::on_revertAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->revertAll();
}

void MainWindow::on_selectAction_triggered()
{
    QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(ui->dataTable->model());
    if (tm)
        tm->select();
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

 QVariant TableInfoModel::data(const QModelIndex &idx, int role) const
{
    if (role == Qt::BackgroundRole && isDirty(idx))
        return QBrush(QColor(Qt::yellow));
    return QSqlTableModel::data(idx, role);
}
