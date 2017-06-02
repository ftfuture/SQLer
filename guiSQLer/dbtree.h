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
    int selectedIndex=-1;
    explicit DbTree(QWidget *parent = 0);
    static QString qDBCaption(DBAdapterInfo adapterInfo);

signals:
    void activeDBRequested(int index);
    void resetConnectionRequested(int index);
    void dataTableActivated(const QString &table);

public slots:
    void deleteDB();
    void refresh();
    void on_tree_itemActivated(QTreeWidgetItem *item, int column);
    void on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    void setActive(QTreeWidgetItem *item);

    QTreeWidget *tree;
    QAction *deleteAction;
    QString currentDB;
};

#endif // DBTREE_H
