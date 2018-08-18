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
    void setShowFiles(bool show);
    void setShowHidden(bool show);

    enum NodeData {
        IsDir = Qt::UserRole + 1,
        IsHidden
    };

signals:
	void itemCheckChanged(QTreeWidgetItem* item, Qt::CheckState newState);

private:
    void updateItemVisibility(QTreeWidgetItem* item);

    void onItemExpanded(QTreeWidgetItem* item);
    void onItemChanged(QTreeWidgetItem* item, int column);

    bool m_disableSignals = false;
    bool m_showFiles = true;
    bool m_showHidden = true;
    QTreeWidget* m_tree;
};

#endif // DIRTREE_H
