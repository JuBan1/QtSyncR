#include "profile.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QJsonArray>

Profile Profile::s_currentProfile;

Profile::Result Profile::loadFromFile(const QString& path, Profile& profile)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return Result::FileReadWriteError;

    auto doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) return Result::FileContentsError;
    auto obj = doc.object();
    if (obj.isEmpty()) return Result::FileContentsError;

    auto list = obj["list"].toArray();
    auto src = obj["src"].toString();
    auto dest = obj["dest"].toString();

    auto flagArchive = obj["flagArchive"].toBool(false);
    auto flagCompress = obj["flagCompress"].toBool(false);
    auto flagSizeOnly = obj["flagSizeOnly"].toBool(false);

    if (src.isEmpty()) return Result::FileContentsError;

    QStringList l;
    for (const QJsonValueRef v : list)
        l << v.toString();

    Profile p;
    p.m_profileFilePath = path;
    p.m_dirList.set(src, l);
    p.m_srcPath = src;
    p.m_destPath = dest;
    p.setRSyncFlag(FlagArchive, flagArchive);
    p.setRSyncFlag(FlagCompress, flagCompress);
    p.setRSyncFlag(FlagSizeOnly, flagSizeOnly);
    p.setModified(false);

    profile = p;

    return Result::Success;
}

Profile::Result Profile::saveToFile(const QString &path) const
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return Result::FileReadWriteError;

    QJsonDocument doc;
    QJsonObject obj;
    QJsonArray arr;

    for (const auto& path : m_dirList.getAllPaths()) {
        arr << path;
    }

    obj["list"] = arr;
    obj["src"] = m_srcPath;
    obj["dest"] = m_destPath;

    obj["flagArchive"] = getRSyncFlag(FlagArchive);
    obj["flagCompress"] = getRSyncFlag(FlagCompress);
    obj["flagSizeOnly"] = getRSyncFlag(FlagSizeOnly);

    doc.setObject(obj);

    if (file.write(doc.toJson()) <= 0)
        return Result::FileReadWriteError;

    return Result::Success;
}

bool Profile::isModified() const {
	return m_modified || m_dirList.isModified();
}

void Profile::setRSyncFlag(Profile::RSyncFlag flag, bool enabled) {
    if (m_rsyncFlags[flag] == enabled)
        return;

    m_rsyncFlags[flag] = enabled;
    m_modified = true;
}

void Profile::setModified(bool modified) {
    m_modified = modified;
    m_dirList.setModified(modified);
}

void Profile::setSrcPath(const QString& newPath) {
	if (m_srcPath == newPath)
		return;

	m_srcPath = newPath;
	m_dirList.set(newPath, QStringList());
	m_modified = true;
}

void Profile::setDestPath(const QString& newPath) {
	if (m_destPath == newPath)
		return;

	m_destPath = newPath;
	m_modified = true;
}

void Profile::setProfileFilePath(const QString& newPath) {
	m_profileFilePath = newPath;
}

Profile::Profile()
{
    m_rsyncFlags[FlagArchive] = true;
}
