#include "dirlist.h"

void DirList::add(QString path) {
    auto it = std::find_if(m_paths.begin(), m_paths.end(), [path](const QString& str) {
        return str.startsWith(path);
    });

    m_paths.erase(it, m_paths.end());

    m_paths.insert(path);
	m_isModified = true;
}

void DirList::set(const QString &rootPath, const QStringList &list) {
    m_rootPath = rootPath;
    m_paths.clear();
	m_isModified = false;
    for (const auto& it : list)
        m_paths.insert(it);
}

void DirList::remove(const QString &path) {
	if (m_paths.erase(path) > 0)
        m_isModified = true;
}

void DirList::setModified(bool modified)
{
    m_isModified = modified;
}

Qt::CheckState DirList::getCheckState(const QString &path) const {
    auto it = std::find_if(m_paths.begin(), m_paths.end(), [path](const QString& str) {
        return str.startsWith(path);
    });

    if (it == m_paths.end())
        return Qt::Unchecked;

    if (*it == path)
        return Qt::Checked;

    return Qt::PartiallyChecked;
}
