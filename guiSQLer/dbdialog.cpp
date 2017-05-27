#include "dbdialog.h"
#include "ui_dbdialog.h"
#include <QSqlDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

DbDialog::DbDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbDialog)
{
    qDebug() << "DbDialog::DbDlialog";
    ui->setupUi(this);

    // SQLITE, MYSQL, MSSQL, POSTGRESQL, ORACLE, DB2
    // QSQLITE, QMYSQL, QMYSQL3, QODBC, QODBC3, QPSQL, QPSQL7
    QStringList drivers = QSqlDatabase::drivers();
    ui->typeCombo->addItems(drivers);

    //sui->historyCombo->clear();
    SqlerSetting::getInstance().loadHistoryInfo();
    for(int i=0; i<SqlerSetting::getInstance().historyList.count(); i++){
        QString caption = SqlerSetting::getInstance().historyList.at(i).driver+':';
        if(!SqlerSetting::getInstance().historyList.at(i).user.isEmpty()) caption.append(SqlerSetting::getInstance().historyList.at(i).user).append("@");
        if(!SqlerSetting::getInstance().historyList.at(i).name.isEmpty()) {
            caption.append(SqlerSetting::getInstance().historyList.at(i).name);
            if(!SqlerSetting::getInstance().historyList.at(i).address.isEmpty()) caption.append('/').append(SqlerSetting::getInstance().historyList.at(i).address);
        } else if(!SqlerSetting::getInstance().historyList.at(i).path.isEmpty()) caption.append(SqlerSetting::getInstance().historyList.at(i).path);
        ui->historyCombo->addItem(caption);
    }

}


DbDialog::~DbDialog()
{
    delete ui;
}

void DbDialog::on_cancelButton_clicked()
{
    qDebug() << "DbDialog::on_cancelButton_clicked";
    reject();
}

void DbDialog::setAdapterInfo(const DBAdapterInfo &adapterInfo)
{
    qDebug() << "DbDialog::setAdapterInfo";
    ui->addressEdit->setText(adapterInfo.address);
    ui->dbEdit->setText(adapterInfo.name);
    ui->pathEdit->setText(adapterInfo.path);
    ui->typeCombo->setCurrentText(adapterInfo.driver);
    ui->useridEdit->setText(adapterInfo.user);
    ui->portSpinBox->setValue(adapterInfo.port);
    ui->passwordEdit->setFocus();
}

QString DbDialog::getDbPath(bool newFileMode, const QString &startWith)
{
    QString dir = startWith;

    QStringList filters;
    filters += QObject::tr("All SQLite databases")+" (*.db *.sdb *.sqlite *.db3 *.s3db *.sqlite3 *.sl3 *.db2 *.s2db *.sqlite2 *.sl2)";
    filters += "SQLite3 (*.db3 *.s3db *.sqlite3 *.sl3)";
    filters += "SQLite2 (*.db2 *.s2db *.sqlite2 *.sl2)";
    filters += QObject::tr("All files")+" (*)";
    QString filter = filters.join(";;");

    if (newFileMode)
        dir =  QFileDialog::getSaveFileName(0, QObject::tr("Database file"), dir, filter, &filters[0], QFileDialog::DontConfirmOverwrite);
    else
        dir = QFileDialog::getOpenFileName(0, QObject::tr("Database file"), dir, filter, &filters[0]);

    return dir;
}

QString DbDialog::driverName() const
{
    return ui->typeCombo->currentText();
}

QString DbDialog::databaseName() const
{
    if(!ui->pathEdit->text().isEmpty()) return ui->pathEdit->text();
    return ui->dbEdit->text();
}

QString DbDialog::userName() const
{
    return ui->useridEdit->text();
}

QString DbDialog::password() const
{
    return ui->passwordEdit->text();
}

QString DbDialog::hostName() const
{
    return ui->addressEdit->text();
}

QString DbDialog::pathName() const
{
    return ui->pathEdit->text();
}

bool DbDialog::isSave() const
{
    return ui->saveCheckBox->isChecked();
}

QString DbDialog::connectionName() const
{
    if(!pathName().isEmpty()) return driverName().append(pathName()).append(hostName()).append(ui->portSpinBox->text());
    return driverName().append(ui->dbEdit->text()).append(hostName()).append(ui->portSpinBox->text());
}

int DbDialog::port() const
{
    return ui->portSpinBox->value();
}

void DbDialog::on_okButton_clicked()
{
    qDebug() << "DbDialog::on_okButton_clicked";
    if(ui->saveCheckBox->isCheckable() == true) addHistory();
    accept();

}

void DbDialog::addHistory()
{
    qDebug() << "DbDialog::addHistory";
    QDateTime addtime = QDateTime::currentDateTime();
    int removeIndex=0;
    if(SqlerSetting::getInstance().historyList.count() >= MAX_COUNT_HISTORY) {
        for(int i=0; i<SqlerSetting::getInstance().historyList.count(); i++){
            DBAdapterInfo adapterInfo = SqlerSetting::getInstance().historyList.at(i);
            if(adapterInfo.addtime.secsTo(addtime) < 0) removeIndex = i;
        }
        SqlerSetting::getInstance().historyList.removeAt(removeIndex);
    }
    DBAdapterInfo adapterInfo;
    adapterInfo = SqlerSetting::getInstance().makeAdapterInfo(ui->typeCombo->currentText(), connectionName(), ui->addressEdit->text(), ui->saveCheckBox->isChecked(), ui->dbEdit->text(),
                                                              ui->useridEdit->text(), ui->passwordEdit->text(), ui->pathEdit->text(), ui->portSpinBox->value());
    SqlerSetting::getInstance().historyList.append(adapterInfo);
    SqlerSetting::getInstance().saveHistoryInfo();
}

void DbDialog::on_newButton_clicked()
{
    qDebug() << "DbDialog::on_newButton_clicked";
    ui->pathEdit->setText(getDbPath(true, NULL));
}

void DbDialog::on_openButton_clicked()
{
    qDebug() << "DbDialog::on_openButton_clicked";
    ui->pathEdit->setText(getDbPath(false, NULL));
}

void DbDialog::on_historyCombo_currentIndexChanged(int index)
{
    if(index == 0) return;
    DBAdapterInfo adapterInfo = SqlerSetting::getInstance().historyList.at(index-1);
    ui->typeCombo->setCurrentText(adapterInfo.driver);
    ui->addressEdit->setText(adapterInfo.address);
    ui->dbEdit->setText(adapterInfo.name);
    ui->useridEdit->setText(adapterInfo.user);
    ui->pathEdit->setText(adapterInfo.path);
    ui->portSpinBox->setValue(adapterInfo.port);
    ui->saveCheckBox->setChecked(Qt::Checked);

}

void DbDialog::on_typeCombo_currentIndexChanged(int index)
{
    if(ui->typeCombo->currentText().compare(""))
    switch(index){
        case 1:
            ui->portSpinBox->setValue(3306);
        break;
        case 5:
            ui->portSpinBox->setValue(5432);
        break;
    }
}
