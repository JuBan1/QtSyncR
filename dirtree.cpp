#include "dirtree.h"

#include <QDir>
#include <QDirIterator>
#include <QFileIconProvider>

#include "profile.h"


QList<QTreeWidgetItem*> getAllCheckedItems(QTreeWidgetItem* node)
{
    const auto state = node->checkState(0);
    QList<QTreeWidgetItem*> items;

    if(state == Qt::Unchecked)
        return items;

    if(state == Qt::Checked)
        items << node;

    const int count = node->childCount();
    for(int i=0; i<count; ++i)
        items += getAllCheckedItems(node->child(i));


    return items;
}

QTreeWidgetItem* getItemForPath(QString path, QTreeWidget* tree) {
    QDir d(path);

    QTreeWidgetItem* found = nullptr;
    for (int i=0; i<tree->topLevelItemCount(); ++i) {
        auto* tli = tree->topLevelItem(i);

        if (path.startsWith(tli->text(1))) {
            found = tli;
            break;
        }
    }

    while(found->text(1) != path) {

        for (int i=0; i<found->childCount(); ++i) {
            auto* it = found->child(i);

            if (path.startsWith(it->text(1))) {
                found = it;
                break;
            }
        }
    }

    if( found->text(1) == path)
        return found;

    return nullptr;
}

void populateDirectory(QTreeWidgetItem* item) {
    QFileIconProvider iconProvider;
    const QIcon& dirIcon = iconProvider.icon(QFileIconProvider::Folder);

    const bool disabled = item->checkState(0) == Qt::Checked || item->isDisabled();

    QString path = item->text(1);

    QDirIterator dirIt(path,
                       QStringList(),
                       QDir::Dirs | QDir::Readable | QDir::Hidden | QDir::NoDotAndDotDot);

    QStringList dirList;
    while(dirIt.hasNext()) {
        dirList << dirIt.next();
    }
    dirList.sort();

    for (const auto& dirPath : dirList) {
        QDir dir(dirPath);

        auto* d = new QTreeWidgetItem(item);
        d->setText(0, dir.dirName());
        d->setText(1, dir.path());
        d->setCheckState(0, Profile::getCurrentDirList().getCheckState(dirPath));
        d->setIcon(0, dirIcon);
        d->setDisabled(disabled);
        d->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }

    QDirIterator filesIt(path,
                         QStringList(),
                         QDir::Files | QDir::Readable | QDir::Hidden);

    QStringList fileList;
    while(filesIt.hasNext()) {
        fileList << filesIt.next();
    }
    fileList.sort();

    for (const auto& filePath : fileList) {
        QFileInfo fi(filePath);

        auto* d = new QTreeWidgetItem(item);
        d->setText(0, fi.fileName());
        d->setText(1, fi.filePath());
        d->setIcon(0, iconProvider.icon(fi));
        d->setDisabled(disabled);
        d->setCheckState(0, Profile::getCurrentDirList().getCheckState(filePath));
    }



    if (item->childCount() == 0)
        item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
}

void uncheckAllChildren(QTreeWidgetItem* it) {
    for (int i=0; i<it->childCount(); ++i) {
        auto* c = it->child(i);
        c->setCheckState(0, Qt::Unchecked);
        c->setFlags( c->flags() & ~Qt::ItemIsEnabled );
        uncheckAllChildren(c);
    }
}

void setChildrenEnabled(QTreeWidgetItem* it, bool enabled) {
    for (int i=0; i<it->childCount(); ++i) {
        auto* c = it->child(i);
        c->setDisabled(!enabled);
        setChildrenEnabled(c, enabled);
    }
}

void updateCheckState(QTreeWidgetItem* it) {
    if(!it) return;

    for (int i=0; i<it->childCount(); ++i) {
        auto* c = it->child(i);

        if (c->checkState(0) != Qt::Unchecked) {
            if (it->checkState(0) == Qt::PartiallyChecked)
                return;

            it->setCheckState(0, Qt::PartiallyChecked);
            updateCheckState(it->parent()); // Could loop through parents and just set to partiallychecked
            return;
        }
    }

    it->setCheckState(0, Qt::Unchecked);
    updateCheckState(it->parent());

}

void setChecked(QTreeWidgetItem* it, bool checked) {
    setChildrenEnabled(it, !checked);
    updateCheckState(it->parent());

	DirList& dirList = Profile::getCurrentDirList();

	if(checked)
		dirList.add(it->text(1));
	else
		dirList.remove(it->text(1));

    if(!checked)
        return;


    uncheckAllChildren(it);
}

DirTree::DirTree(QTreeWidget *w) : QObject(nullptr), m_tree(w) {
    connect(m_tree, &QTreeWidget::itemExpanded, this, &DirTree::onItemExpanded);
    connect(m_tree, &QTreeWidget::itemChanged, this, &DirTree::onItemChanged);
}

void DirTree::setRootPath(QString path)
{
    if (m_tree->topLevelItemCount() > 0
            && path == m_tree->topLevelItem(0)->text(1))
        return;

    m_tree->clear();

    if (path.isEmpty())
        return;

    QDir d(path);

	m_disableSignals = true;
    QTreeWidgetItem* it = new QTreeWidgetItem(m_tree);
    it->setText(0, d.dirName());
    it->setText(1, d.path());
    it->setCheckState(0, Profile::getCurrentDirList().getCheckState(path));
    it->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	m_disableSignals = false;
}

void DirTree::onItemExpanded(QTreeWidgetItem* item)
{
    if(item->childCount() > 0)
        return;

    m_disableSignals = true;
    populateDirectory(item);
    m_disableSignals = false;
}

void DirTree::onItemChanged(QTreeWidgetItem* item, int column)
{
    if(m_disableSignals) return;

    if(column != 0)
        return;

    m_disableSignals = true;
	setChecked(item, item->checkState(0) == Qt::Checked);
	emit itemCheckChanged(item, item->checkState(0));
    m_disableSignals = false;
}
