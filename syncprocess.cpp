#include "syncprocess.h"

#include <QDebug>

SyncProcess::SyncProcess(const Profile &profile)
    : m_profile(profile)
{
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &SyncProcess::onReadStandardOoutput);
    auto ptr = QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished);
    connect(&m_process, ptr, this, &SyncProcess::onProcessFinished);

    const auto& paths = m_profile.getDirList().getAllPaths();
    m_paths.reserve(paths.size());
    for (const auto& it : paths)
        m_paths.push_back(it);
}

void SyncProcess::start()
{
    m_iterator = m_paths.begin();
    startProcess();
}

bool SyncProcess::isRunning() const
{
    return m_process.state() != QProcess::NotRunning;
}

void SyncProcess::stopAfterCurrent()
{
    m_wantToStop = true;
}

void SyncProcess::forceCancel()
{
    m_wantToStop = true;
    m_process.kill();
    m_process.waitForFinished(100);
}

void readOutputString(const QString line, long& mib, QString& perc, QString& speed, QString& time) {
    auto parts = line.split(' ', QString::SkipEmptyParts);
    auto byteString = parts[0].remove(',');
    mib = byteString.toLong() / 1024 / 1024;
    perc = parts[1];
    speed = parts[2];
    time = parts[3];
}

void SyncProcess::onReadStandardOoutput()
{
    QStringList l = QString(m_process.readAllStandardOutput()).split('\r', QString::SkipEmptyParts);
    l.removeAll("\n");

    if(l.isEmpty())
        return;

    const auto& str = l.last();

    m_lastOutput = str;

    if (!m_timer.hasExpired())
        return;


    long mib;
    QString perc, speed, time;
    readOutputString(str, mib, perc, speed, time);
    emit progress(mib, perc, speed, time);

    m_timer.setRemainingTime(1000);
}

void SyncProcess::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_timer.setRemainingTime(0);

    long mib;
    QString perc, speed, time;
    readOutputString(m_lastOutput, mib, perc, speed, time);
    emit progress(mib, perc, speed, time);

    emit taskFinished(exitCode == 0 && exitStatus == QProcess::NormalExit);

    m_iterator++;
    startProcess();
}

QStringList createRSyncSwitches(const Profile& profile) {
    QStringList switches;
    switches << "--delete" << "-r" << "--info=progress2";

    if (profile.getFlag(Profile::FlagsDryRun)) switches << "-n";
    if (profile.getFlag(Profile::FlagArchive)) switches << "-a";
    if (profile.getFlag(Profile::FlagCompress)) switches << "-z";
    if (profile.getFlag(Profile::FlagSizeOnly)) switches << "--size-only";

    return switches;
}

void SyncProcess::startProcess()
{
    if (m_iterator == m_paths.end()) {
        emit allFinished();
        return;
    }

    if (m_wantToStop) {
        emit allFinished();
        return;
    }

    QString absSrcPath = m_profile.getSrcPath();
    QString absDestPath = m_profile.getDestPath();
    QString absDirPath = *m_iterator;
    QString relDirPath = absDirPath;
    relDirPath.remove(0, absSrcPath.length());
    QString absDirDestPath = absDestPath + relDirPath;

    // If rsyncing the root directory we need to append a '/' to prevent an additional directory layer
    // to be created. This is in line with how non-root dirs and files are synced.
    if (absSrcPath == absDirPath)
        absDirPath += '/';

    m_process.start("rsync", createRSyncSwitches(m_profile) << absDirPath << absDirDestPath);
}
