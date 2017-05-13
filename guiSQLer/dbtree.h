#ifndef DBTREE_H
#define DBTREE_H

#include <QWidget>
#include <QTreeWidget>
#include <QSqlDatabase>
#include "dbmanager.h"

class DbTree : public QWidget
{
    Q_OBJECT
public:
    int selectedIndex=0;
    QList<DBManager *> *dbManagerList;
    explicit DbTree(QWidget *parent = 0);
    //QSqlDatabase currentDatabase() const;

signals:
    void dataTableActivated(const QString &table);
    void metaDataRequested(const QString &tableName);

public slots:
    void refresh();
    void onMetaAction();
    void on_tree_itemActivated(QTreeWidgetItem *item, int column);
    void on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    void setActive(QTreeWidgetItem *item);

    QTreeWidget *tree;
    QAction *metaAction;
    QString currentDB;
};

#endif // DBTREE_H
