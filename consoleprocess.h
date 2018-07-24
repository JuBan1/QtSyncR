#ifndef CONSOLEPROCESS_H
#define CONSOLEPROCESS_H

#include <QObject>

#include "syncprocess.h"
#include "profile.h"

class ConsoleProcess : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleProcess(const Profile& profile);

signals:
    void finished();

public slots:
    void onTaskProgress(long megabytes, QString perc, QString speed, QString time);
    void onTaskFinished(bool success);
    void onAllFinished();

private:
    SyncProcess m_process;
    const Profile& m_profile;
};

#endif // CONSOLEPROCESS_H
