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
	explicit ProgressDialog(QWidget *parent = 0);
	~ProgressDialog();

private:
    void updateStats();

    void onTaskFinished(size_t current, size_t all);
    void onAllFinished();

    Ui::ProgressDialog *ui;

    SyncProcess m_process;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // PROGRESSDIALOG_H
