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

    if (parts.isEmpty())
        return;

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

// Taken from man rsync
const char* rsyncExitCodeToString(int code) {
    switch(code) {
    case 0: return "Success";
    case 1: return "Syntax or usage error";
    case 2: return "Protocol incompatibility";
    case 3: return "Errors selecting input/output files, dirs";
    case 4: return "Requested action not supported";
    case 5: return "Error starting client-server protocol";
    case 6: return "Daemon unable to append to log-file";
    case 10: return "Error in socket I/O";
    case 11: return "Error in file I/O";
    case 12: return "Error in rsync protocol data stream";
    case 13: return "Errors with program diagnostics";
    case 14: return "Error in IPC code";
    case 20: return "Received SIGUSR1 or SIGINT";
    case 21: return "Some error returned by waitpid()";
    case 22: return "Error allocating core memory buffers";
    case 23: return "Partial transfer due to error";
    case 24: return "Partial transfer due to vanished source files";
    case 25: return "The --max-delete limit stopped deletions";
    case 30: return "Timeout in data send/receive";
    case 35: return "Timeout waiting for daemon connection";
    default: return "Unknown exit code";
    }
}

void SyncProcess::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_timer.setRemainingTime(0);

    const bool success = exitCode == 0 && exitStatus == QProcess::NormalExit;

    // There might be no output if the process failed.
    if (success) {
        long mib=0;
        QString perc, speed, time;
        readOutputString(m_lastOutput, mib, perc, speed, time);
        emit progress(mib, perc, speed, time);
    }

    const QString exitString = exitStatus == QProcess::NormalExit ? rsyncExitCodeToString(exitCode) : "Process execution failed";
    emit taskFinished(success, exitString);

    m_iterator++;
    startProcess();
}

QStringList createRSyncSwitches(const Profile& profile) {
    QStringList switches;
    switches << "--delete" << "-rR" << "--info=progress2";

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

    // If rsyncing the root directory we need to append a '/' to prevent an additional directory layer
    // to be created. This is in line with how non-root dirs and files are synced.
    if (absSrcPath == absDirPath)
        absDirPath += '/';

    // Insert a marker so Rsync knows where the relative part of the file path starts.
    // See: https://stackoverflow.com/questions/1636889/rsync-how-can-i-configure-it-to-create-target-directory-on-server
    absDirPath.insert(absSrcPath.length(), "/.");

    m_process.start("rsync", createRSyncSwitches(m_profile) << absDirPath << absDestPath);
}
