#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QLabel>

#include "profile.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::ProgressDialog),
    m_process(Profile::getCurrent())
{
    ui->setupUi(this);

    connect(ui->btnClose, &QPushButton::clicked, this, &ProgressDialog::close);

    connect(&m_process, &SyncProcess::progress, this, &ProgressDialog::onTaskProgress);
    connect(&m_process, &SyncProcess::taskFinished, this, &ProgressDialog::onTaskFinished);
    connect(&m_process, &SyncProcess::allFinished, this, &ProgressDialog::onAllFinished);

    m_currentTaskNo = 0;
    m_totalTasksCount = Profile::getCurrentDirList().getAllPaths().size();

    m_process.start();

    updateStats();
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::updateStats()
{
    /*
    QString state;
    QStyle::StandardPixmap pixmap;
    QColor color;
    switch(m_currentState) {
    case Running:
        color = Qt::green;
        state = "Running";
        pixmap = QStyle::SP_MessageBoxInformation;
        break;
    case Stopping:
        color = Qt::yellow;
        state = "Stopping";
        pixmap = QStyle::SP_MessageBoxWarning;
        break;
    case Stopped:
        color = Qt::yellow;
        state = "Stopped";
        pixmap = QStyle::SP_MessageBoxWarning;
        break;
    case Failed:
        color = Qt::red;
        state = "Failed";
        pixmap = QStyle::SP_MessageBoxCritical;
        break;
    case Finished:
        color = Qt::green;
        state = "Finished";
        pixmap = QStyle::SP_MessageBoxInformation;
        break;
    }

    auto size = ui->lbIcon->size();

    ui->lbIcon->setPixmap( style()->standardIcon(pixmap).pixmap(size) );

    ui->lbStatus->setStyleSheet(QString("QLabel { color: %1 }").arg(color.name()));
    ui->lbStatus->setText("<b>"+state+"</b>");
    //ui->lbTasks->setText(QString("%1 / %2").arg(m_currentIndex).arg(m_paths.size()));*/

    ui->lbTasks->setText(QString("%1 / %2").arg(m_currentTaskNo).arg(m_totalTasksCount));
}

void ProgressDialog::onTaskProgress(long megabytes, QString perc, QString speed, QString time)
{
    if (!m_currentItem)
        m_currentItem = new QListWidgetItem(ui->listWidget);

    m_currentItem->setText( QString("%1MiB %2 %3 %4").arg(megabytes).arg(perc).arg(speed).arg(time) );
}

void ProgressDialog::onTaskFinished(bool /*success*/, QString exitString)
{
    m_currentItem->setText( m_currentItem->text() + " - " + exitString );

    m_currentItem = nullptr;
    updateStats();
}

void ProgressDialog::onAllFinished()
{
    ui->lbStatus->setText("<b>Status: Finished</b>");
    ui->btnClose->setText("Close"); // Before that: Abort
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    if (!m_process.isRunning()) {
        return QDialog::closeEvent(event);
    }

    QMessageBox box;
    box.setWindowTitle("");
    box.setText("Sync is still in progress. What do you want to do?");
    box.setIcon(QMessageBox::Warning);
    auto a = box.addButton( "Stop Immediately", QMessageBox::DestructiveRole );
    auto b = box.addButton( "Stop After Current", QMessageBox::DestructiveRole );
    auto c = box.addButton( "Don't Stop", QMessageBox::RejectRole );
    box.setDefaultButton(c);

    box.exec();

    auto result = box.clickedButton();

    if (result == a) {
        m_process.forceCancel();
        return QDialog::closeEvent(event);
    } else if (result == b) {
        m_process.stopAfterCurrent();
    } else if (result == c) {
        // Do nothing
    } else assert(false);

    event->ignore();
}
