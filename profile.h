#ifndef PROFILE_H
#define PROFILE_H

#include <QFile>

#include "dirlist.h"

class Profile
{
public:
    enum Result {
        Success,
        FileReadWriteError,
        FileContentsError
    };

    enum ProfileFlag {
        FlagArchive,    // Preserve symlinks, ownership, ..
        FlagCompress,   // Compress files before transfer
        FlagSizeOnly,   // Check files only based on size
        FlagsDryRun,    // Only simulate changes. This setting won't be saved
        FlagShowFiles,
        FlagShowHidden,
        FLAGS_MAX
    };

	static Profile& getCurrent() { return s_currentProfile; }
	static DirList& getCurrentDirList() { return s_currentProfile.getDirList(); }
    static void setCurrent(const Profile& p) { s_currentProfile = p; }


    static Result loadFromFile(const QString& path, Profile &profile);
    Result saveToFile(const QString& path) const;

	bool isModified() const;
    const QString& getSrcPath() const { return m_srcPath; }
    const QString& getDestPath() const { return m_destPath; }
    const QString& getProfileFilePath() const { return m_profileFilePath; }
	DirList& getDirList() { return m_dirList; }
    const DirList& getDirList() const { return m_dirList; }

    bool getFlag(ProfileFlag flag) const { return m_rsyncFlags[flag]; }
    void setFlag(ProfileFlag flag, bool enabled);

    void setModified(bool modified);
	void setSrcPath(const QString& newPath);
	void setDestPath(const QString& newPath);
	void setProfileFilePath(const QString& newPath);

    Profile();

private:
    static Profile s_currentProfile;

    std::array<bool, FLAGS_MAX> m_rsyncFlags = {true, false, false, false, true, true};
    QString m_profileFilePath;

    QString m_srcPath;
    QString m_destPath;
    DirList m_dirList;

    bool m_modified = false;
};

#endif // PROFILE_H
