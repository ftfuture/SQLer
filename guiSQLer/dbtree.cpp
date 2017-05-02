#include "dbtree.h"
#include <QtWidgets>
#include <QtSql>

DbTree::DbTree(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    tree = new QTreeWidget(this);
    tree->setObjectName(QLatin1String("tree"));
    tree->setHeaderLabels(QStringList(tr("database")));
    tree->header()->setSectionResizeMode(QHeaderView::Stretch);

    QAction *refreshAction = new QAction(tr("Refresh"), tree);
    metaAction = new QAction(tr("Show Schema"), tree);
    connect(refreshAction, SIGNAL(triggered()), SLOT(refresh()));
    connect(metaAction, SIGNAL(triggered()), SLOT(onMetaAction()));
    tree->addAction(refreshAction);
    tree->addAction(metaAction);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);

    layout->addWidget(tree);
    QMetaObject::connectSlotsByName(this);
}

static QString qDBCaption(const QSqlDatabase &db)
{
    QString dbCaption = db.driverName().append(QLatin1Char(':'));
    if(!db.userName().isEmpty())
        dbCaption.append(db.userName()).append(QLatin1Char('@'));
    dbCaption.append(db.databaseName());
    return dbCaption;
}

void DbTree::refresh()
{
    tree->clear();
    QStringList connectionNames = QSqlDatabase::connectionNames();

    bool isActiveDB = false;
    for(int i=0; i<connectionNames.count(); ++i){
        QTreeWidgetItem *root = new QTreeWidgetItem(tree);
        QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
        root->setText(0, qDBCaption(db));
        if(connectionNames.at(i) == currentDB) {
            isActiveDB = true;
            setActive(root);
        }
        if(db.isOpen()) {
            QStringList tables = db.tables();
            qDebug() << "tables count" << db.tables().count();
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
                    QString colStr = record.field(col).name();
                    colStr.append("(").append(record.field(col).type()) .append(")");
                    QVariant variant = record.field(col).type();
                    column->setText(0,tableModel->record().fieldName(col));
                }
            }
        }
    }
    if(!isActiveDB) {
        currentDB = connectionNames.value(0);
        setActive(tree->topLevelItem(0));
    }

    tree->doItemsLayout();
}

QSqlDatabase DbTree::currentDatabase() const
{
    return QSqlDatabase::database(currentDB);
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
    currentDB = QSqlDatabase::connectionNames().value(tree->indexOfTopLevelItem(item));
}

void DbTree::on_tree_itemActivated(QTreeWidgetItem *item, int /* column */)
{
    if (!item)
        return;

    if (!item->parent()) {
        setActive(item);
    } else {
        qDebug() << "item activated";
        setActive(item->parent());
        emit dataTableActivated(item->text(0));
    }
}

void DbTree::onMetaAction()
{
    QTreeWidgetItem *cItem = tree->currentItem();
    if (!cItem || !cItem->parent())
        return;
    setActive(cItem->parent());
    emit metaDataRequested(cItem->text(0));
}

void DbTree::on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    metaAction->setEnabled(current && current->parent());
}
