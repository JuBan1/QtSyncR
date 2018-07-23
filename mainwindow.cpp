#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <set>

#include <QLineEdit>
#include <QToolButton>

#include <QDebug>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QSettings>
#include <QMessageBox>
#include <QProcess>
#include <QCloseEvent>
#include <QDesktopServices>

#include "globals.h"
#include "aboutdialog.h"
#include "progressdialog.h"
#include "profile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_dirTree = new DirTree(ui->fsTree);

    auto ac = new QAction("&Dry-Run");
    ac->setIcon( QIcon::fromTheme("system-run"));
    ui->btnRunSync->addAction(ac);

    connect(ui->btnRunSync, &QToolButton::clicked, this, &MainWindow::onBtnRunSync);
    connect(ac, &QAction::triggered, this, &MainWindow::onActionDryRun);

    connect(m_dirTree, &DirTree::itemCheckChanged, this, &MainWindow::updateUI);

    connect(ui->actionNew_Profile, &QAction::triggered, this, &MainWindow::onActionNewProfileClicked);
    connect(ui->actionLoad_Profile, &QAction::triggered, this, &MainWindow::onActionLoadProfileClicked);
    connect(ui->actionSave_Profile, &QAction::triggered, this, &MainWindow::onActionSaveProfileClicked);
    connect(ui->actionSave_Profile_As, &QAction::triggered, this, &MainWindow::onActionSaveAsClicked);
    connect(ui->actionClear_Recent_List, &QAction::triggered, this, &MainWindow::onActionClearRecentClicked);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionAbout_QtSyncR, &QAction::triggered, this, &MainWindow::onActionAboutQtSyncRClicked);
    connect(ui->actionAbout_Qt, &QAction::triggered, QApplication::aboutQt);
    connect(ui->actionReport_A_Bug, &QAction::triggered, this, &MainWindow::onActionReportABugClicked);


    updateRecentList();

    QSettings s;
    auto lastOpened = s.value("lastOpened").toString();

    Profile profile;
    Profile::loadFromFile(lastOpened, profile);

    loadProfile(profile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onActionNewProfileClicked()
{
    loadProfile(Profile());
}

void MainWindow::onActionLoadProfileClicked() {
    QFileDialog d;
    d.setFileMode(QFileDialog::ExistingFile);
    d.setDirectory(getLastSelectedDir());
    d.setNameFilter("SyncR Files (*.syncr);;All Files (*.*)");
    d.exec();

    const auto& files = d.selectedFiles();

    if (files.isEmpty())
        return;

    auto filePath = files.first();
    setLastSelectedDir(QFileInfo(filePath).path());

    loadProfile(filePath);
}

void MainWindow::onActionSaveProfileClicked() {
    auto& p = Profile::getCurrent();

    if (p.getProfileFilePath().isEmpty()) {
        onActionSaveAsClicked();
        return;
    }

    auto result = p.saveToFile(p.getProfileFilePath());
    if (result != Profile::Success) {
        QMessageBox::critical(this, "Error", "There was an error writing the file. Could not save.");
        return;
    }

    p.setModified(false);
    updateUI();
}

void MainWindow::onActionSaveAsClicked() {
    QFileDialog d;
    d.setFileMode(QFileDialog::AnyFile);
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setDirectory(getLastSelectedDir());
    d.setNameFilter("SyncR Files (*.syncr);;All Files (*.*)");
    d.exec();

    const auto& files = d.selectedFiles();

    if (files.isEmpty())
        return;

    auto filePath = files.first();
    setLastSelectedDir(QFileInfo(filePath).path());

    auto& p = Profile::getCurrent();

    p.saveToFile(filePath);
    auto result = p.saveToFile(p.getProfileFilePath());
    if (result != Profile::Success) {
        QMessageBox::critical(this, "Error", "There was an error writing the file. Could not save.");
        return;
    }

    p.setProfileFilePath(filePath);
    p.setModified(false);

    addToRecentList(filePath);
    updateUI();
}

void MainWindow::onActionClearRecentClicked() {
    QSettings s;
    s.setValue("recent", QStringList());
}

void MainWindow::onActionAboutQtSyncRClicked()
{
    AboutDialog d;
    d.exec();
}

void MainWindow::onActionReportABugClicked()
{
    QDesktopServices::openUrl(QUrl(GITHUB_ISSUES_ADDRESS));
}

void MainWindow::openRecentFileEntry(QString path)
{
    loadProfile(path);
}

void MainWindow::onBtnRunSync()
{
    ProgressDialog d;
    Profile::getCurrent().setRSyncFlag(Profile::FlagsDryRun, false);
    d.exec();
}

void MainWindow::onActionDryRun()
{
    ProgressDialog d;
    Profile::getCurrent().setRSyncFlag(Profile::FlagsDryRun, true);
    d.exec();
}

void MainWindow::onCbUseArchiveClicked()
{
    Profile::getCurrent().setRSyncFlag(Profile::FlagArchive, ui->cbUseArchive->isChecked());
}

void MainWindow::onCbUseCompressionClicked()
{
    Profile::getCurrent().setRSyncFlag(Profile::FlagCompress, ui->cbUseCompression->isChecked());
}

void MainWindow::onCbCheckSizeOnlyClicked()
{
    Profile::getCurrent().setRSyncFlag(Profile::FlagSizeOnly, ui->cbCheckFileSize->isChecked());
}

void MainWindow::updateUI()
{
    const Profile& p = Profile::getCurrent();

    const auto& path = p.getProfileFilePath();
    QString suffix = p.isModified() ? "*" : "";
    setWindowTitle( "QtSyncR - " + (path.isEmpty() ? "Unsaved Profile*" : (path + suffix)) );

    //ui->btnRunSync->setEnabled( !p.getSrcPath().isEmpty() );
}

void MainWindow::updateRecentList()
{
    QSettings s;
    auto recent = s.value("recent").toStringList();
    auto empty = recent.isEmpty();

    ui->menuLoad_Recent->setEnabled(!empty);

    if (empty)
        return;

    ui->menuLoad_Recent->clear();
    for (const auto& path : recent) {
        auto ac = new QAction(path);
        connect(ac, &QAction::triggered, this, [this, path]() { openRecentFileEntry(path); });
        ui->menuLoad_Recent->addAction(ac);
    }
    ui->menuLoad_Recent->addSeparator();
    ui->menuLoad_Recent->addAction(ui->actionClear_Recent_List);
}

void MainWindow::addToRecentList(const QString& path)
{
    QSettings s;
    auto list = s.value("recent").toStringList();

    list.removeAll(path);
    list.prepend(path);

    if(list.size() > 6)
        list.pop_back();

    s.setValue("recent", list);

    updateRecentList();
}

void MainWindow::removeFromRecentList(const QString& path)
{
    QSettings s;
    auto list = s.value("recent").toStringList();

    list.removeAll(path);
    s.setValue("recent", list);
    updateRecentList();
}

bool MainWindow::loadProfile(const QString& path)
{
    Profile p;
    auto result = Profile::loadFromFile(path, p);


    if (result != Profile::Success) {
        QString errorString;
        if (result == Profile::FileContentsError) errorString = "Malformed file contents.";
        else if (result == Profile::FileReadWriteError) errorString = "Could not read/write file.";
        else assert(false); // Need to add new error cases here

        QMessageBox::critical(this, "Error", QString("An error occured during execution:\n  %1").arg(errorString));
        return false;
    }

    loadProfile(p);
    addToRecentList(path);

    ui->leSourceDir->setText(p.getSrcPath());
    ui->leDestDir->setText(p.getDestPath());
    m_dirTree->setRootPath(p.getSrcPath());

    return true;
}

void MainWindow::loadProfile(const Profile& profile)
{
    Profile::setCurrent(profile);
    updateUI();

    ui->leSourceDir->setText(profile.getSrcPath());
    ui->leDestDir->setText(profile.getDestPath());
    m_dirTree->setRootPath(profile.getSrcPath());
}

QString MainWindow::getLastSelectedDir()
{
    return QSettings().value("lastSelectedDir", "~").toString();
}

void MainWindow::setLastSelectedDir(const QString& path)
{
    QSettings().setValue("lastSelectedDir", path);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (Profile::getCurrent().isModified()) {
        auto ret = QMessageBox::information(this,
                                            "Anything not saved will be lost!",
                                            "Closing without saving will discard all changes you've made. Close anyway?",
                                            QMessageBox::Yes | QMessageBox::Abort,
                                            QMessageBox::Abort);

        if (ret == QMessageBox::Abort) {
            event->ignore();
            return;
        }
    }

    QSettings s;
    s.setValue("lastOpened", Profile::getCurrent().getProfileFilePath());

    QWidget::closeEvent(event);
}


void MainWindow::on_btnSelectSourceDir_clicked()
{
    const Profile& p = Profile::getCurrent();
    const auto startPath = p.getSrcPath().isEmpty() ? "~" : p.getSrcPath();

    QFileDialog d;
    d.setFileMode(QFileDialog::DirectoryOnly);
    d.setDirectory(startPath);
    d.exec();

    const auto& files = d.selectedFiles();

    if (files.isEmpty())
        return;

    auto newSrcPath = files.first();

    if (newSrcPath == p.getSrcPath())
        return;

    if (!Profile::getCurrentDirList().getAllPaths().empty()) {
        auto result = QMessageBox::information(this, "Changing Source Directory",
                                 "Changing the source directory will clear all selections. Do you want to continue?",
                                 QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (result == QMessageBox::Cancel)
            return;
    }

    Profile::getCurrent().setSrcPath(newSrcPath);

    ui->leSourceDir->setText(newSrcPath);
    m_dirTree->setRootPath(newSrcPath);

    updateUI();
}

void MainWindow::on_btnSelectDestDir_clicked()
{
    QFileDialog d;
    d.setFileMode(QFileDialog::DirectoryOnly);
    d.setDirectory(Profile::getCurrent().getDestPath());
    d.exec();

    qDebug() << d.selectedFiles();

    auto dest = d.selectedFiles().first();

    Profile::getCurrent().setDestPath(dest);

    ui->leDestDir->setText(dest);

    updateUI();
}
