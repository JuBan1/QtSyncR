#include "mainwindow.h"
#include <QApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>

#include "globals.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    QApplication::setOrganizationName("QtSyncR");
    QApplication::setApplicationName("QtSyncR");
#if QT_VERSION >= QT_VERSION_CHECK(5,7,0)
    QApplication::setDesktopFileName("qtsyncr");
#endif
    QApplication::setApplicationVersion(VERSION);

    QCommandLineParser p;
    p.setApplicationDescription("File synchronization and backup using rsync.");
    p.addHelpOption();
    p.addVersionOption();

    QCommandLineOption file(QStringList() << "file" << "f",     // Switches
                            "Don't show GUI. Only run the specified Profile file.",  // Description
                            "file", // Name
                            ""); // Default value
    p.addOption(file);

    p.process(a.arguments());


	MainWindow w;
	w.show();

	return a.exec();
}
