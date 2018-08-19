#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QListWidgetItem>
#include <QDialog>
#include "syncprocess.h"

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
	Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog() override;

private:
    void updateStats();

    void onTaskProgress(long megabytes, QString perc, QString speed, QString time);
    void onTaskFinished(bool success, QString exitString);
    void onAllFinished();

    Ui::ProgressDialog *ui;

    QListWidgetItem* m_currentItem = nullptr;

    SyncProcess m_process;
    size_t m_currentTaskNo;
    size_t m_totalTasksCount;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // PROGRESSDIALOG_H
