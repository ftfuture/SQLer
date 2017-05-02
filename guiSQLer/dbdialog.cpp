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
        accept();
    }
}

void DbDialog::on_newButton_clicked()
{

}

void DbDialog::on_openButton_clicked()
{
    ui->pathEdit->setText(getDbPath(true, NULL));
}
