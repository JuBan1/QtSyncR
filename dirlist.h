#ifndef DIRLIST_H
#define DIRLIST_H

#include <set>
#include <algorithm>

#include <QString>
#include <QStringList>

class DirList {

public:
    const std::set<QString>& getAllPaths() const { return m_paths; }
    const QString& getRootPath() const { return m_rootPath; }

    void add(QString path);
    void set(const QString& rootPath, const QStringList& list);
    void remove(const QString& path);

    void setModified(bool modified);
	bool isModified() const { return m_isModified; }

    Qt::CheckState getCheckState(const QString& path) const;

private:
	bool m_isModified = false;
    QString m_rootPath;
    std::set<QString> m_paths;
};


#endif // DIRLIST_H
