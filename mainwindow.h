#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dirtree.h"
#include "profile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onBtnSelectSourceDirClicked();
    void onBtnSelectDestDirClicked();

	void onActionNewProfileClicked();
    void onActionLoadProfileClicked();
    void onActionSaveProfileClicked();
    void onActionSaveAsClicked();
    void onActionClearRecentClicked();

    void onActionAboutQtSyncRClicked();
    void onActionReportABugClicked();

    void onBtnRunSync();
    void onActionDryRun();

    void onCbUseArchiveClicked();
    void onCbUseCompressionClicked();
    void onCbCheckSizeOnlyClicked();

	void openRecentFileEntry(QString path);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void updateUI();
    void updateRecentList();
    void addToRecentList(const QString& path);
	void removeFromRecentList(const QString& path);

	bool loadProfile(const QString& path);
	void loadProfile(const Profile& profile);

    QString getLastSelectedDir();
    void setLastSelectedDir(const QString &path);

	Ui::MainWindow *ui;
    DirTree* m_dirTree;
};

#endif // MAINWINDOW_H
