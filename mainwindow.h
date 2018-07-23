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
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_btnSelectSourceDir_clicked();

	void on_btnSelectDestDir_clicked();


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

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
