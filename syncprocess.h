#ifndef SYNCPROCESS_H
#define SYNCPROCESS_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QDeadlineTimer>

#include <vector>

#include "profile.h"

class SyncProcess : public QObject
{
    Q_OBJECT
public:
    SyncProcess(const Profile& profile);

    void start();
    bool isRunning() const;
    void stopAfterCurrent();
    void forceCancel();

    struct TaskResult {
        QString path;
        bool success;
        QString message;
    };

signals:
    void progress(long megabytes, QString percentage, QString speed, QString time);
    void taskFinished(bool success, QString exitString);
    void allFinished();

private slots:
    void onReadStandardOoutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void startProcess();

    QString m_lastOutput;
    QDeadlineTimer m_timer;
    bool m_wantToStop = false;
    const Profile& m_profile;
    std::vector<QString> m_paths;
    std::vector<QString>::const_iterator m_iterator;

    QProcess m_process;

};

#endif // SYNCPROCESS_H
