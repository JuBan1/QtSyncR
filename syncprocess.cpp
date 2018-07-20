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

void SyncProcess::onReadStandardOoutput()
{
    QStringList l = QString(m_process.readAllStandardOutput()).split('\r');

    if(l.isEmpty())
        return;

    //ui->listWidget->item(0)->setText(l.last());
}

void SyncProcess::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    size_t taskNo = static_cast<size_t>(std::distance(m_paths.cbegin(), m_iterator))+1;
    emit taskFinished(taskNo, m_paths.size());

    m_iterator++;
    startProcess();
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

    m_process.start("sleep", QStringList() << "1.5");
}
