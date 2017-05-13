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
    ui->setupUi(this);

    QStringList drivers = QSqlDatabase::drivers();
    ui->typeCombo->addItems(drivers);

    for(int i=0; i<SqlerSetting::getInstance().historyList.count(); i++){
        QString caption = SqlerSetting::getInstance().historyList.at(i).typeStr+':';
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

void DbDialog::on_testButton_clicked()
{

}

void DbDialog::on_cancelButton_clicked()
{
    reject();
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
    qDebug() << "get path : " <<  dir;

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

int DbDialog::port() const
{
    return ui->portSpinBox->value();
}

void DbDialog::on_okButton_clicked()
{
    if (ui->typeCombo->currentText().isEmpty()) {
        QMessageBox::information(this, tr("No database driver selected"),
                                 tr("Please select a database driver"));
        ui->typeCombo->setFocus();
    } else {
        if(ui->saveCheckBox->isCheckable() == true) addHistory();
        qDebug() << "history count " << SqlerSetting::getInstance().historyList.count();
        accept();
    }
}

void DbDialog::addHistory()
{
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
    //DBAdapterInfo &makeAdapterInfo(QString &address, bool isHistory, QString &name, \
                                   QString &passwd, QString &path, int port, QString &type);
    adapterInfo = SqlerSetting::getInstance().makeAdapterInfo(ui->addressEdit->text(), ui->saveCheckBox->isChecked(), ui->dbEdit->text(),
                                                              ui->passwordEdit->text(), ui->pathEdit->text(), ui->portSpinBox->value(),
                                                              ui->typeCombo->currentText());
    SqlerSetting::getInstance().historyList.append(adapterInfo);
    SqlerSetting::getInstance().saveHistoryInfo();
    qDebug() << SqlerSetting::getInstance().historyList.at(0).typeStr;
}

void DbDialog::on_newButton_clicked()
{
    ui->pathEdit->setText(getDbPath(true, NULL));
}

void DbDialog::on_openButton_clicked()
{
    ui->pathEdit->setText(getDbPath(false, NULL));
}

void DbDialog::on_historyCombo_currentIndexChanged(int index)
{
    if(index == 0) return;
    DBAdapterInfo adapterInfo = SqlerSetting::getInstance().historyList.at(index-1);
    ui->addressEdit->setText(adapterInfo.address);
    ui->dbEdit->setText(adapterInfo.name);
    ui->useridEdit->setText(adapterInfo.user);
    ui->pathEdit->setText(adapterInfo.path);
    ui->portSpinBox->setValue(adapterInfo.port);
    ui->saveCheckBox->setChecked(Qt::Checked);

}

void DbDialog::on_typeCombo_currentIndexChanged(int index)
{
    switch(index){
        case 1:
            ui->portSpinBox->setValue(3306);
        break;
    }
}
