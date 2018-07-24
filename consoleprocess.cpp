#include "consoleprocess.h"

#include <QDebug>

ConsoleProcess::ConsoleProcess(const Profile &profile)
    : QObject(nullptr),
      m_process(profile),
      m_profile(profile)
{
    connect(&m_process, &SyncProcess::progress, this, &ConsoleProcess::onTaskProgress);
    connect(&m_process, &SyncProcess::taskFinished, this, &ConsoleProcess::onTaskFinished);
    connect(&m_process, &SyncProcess::allFinished, this, &ConsoleProcess::onAllFinished);
}

void ConsoleProcess::onTaskProgress(long megabytes, QString perc, QString speed, QString time)
{

}

void ConsoleProcess::onTaskFinished(bool success)
{
    qInfo() << "Task Finished";
}

void ConsoleProcess::onAllFinished()
{
    emit finished();
}
