#ifndef DIRTREE_H
#define DIRTREE_H

#include <QObject>
#include <QTreeWidget>

class DirTree : public QObject
{
    Q_OBJECT

public:
    DirTree(QTreeWidget* w);

    void setRootPath(QString path);

signals:
	void itemCheckChanged(QTreeWidgetItem* item, Qt::CheckState newState);

private:
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemChanged(QTreeWidgetItem* item, int column);

    bool m_disableSignals = false;
    QTreeWidget* m_tree;
};

#endif // DIRTREE_H
