#include "mainwindow.h"
#include <QApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QtDebug>

#include "consoleprocess.h"
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

    if (p.isSet(file)) {
        QString filePath = p.value(file);
        Profile profile;
        auto result = Profile::loadFromFile(filePath, profile);

        if (result == Profile::FileReadWriteError) {
            qCritical() << QString("Failed to read file '%1'").arg(filePath);
            return EXIT_FAILURE;
        } else if (result == Profile::FileReadWriteError) {
            qCritical() << QString("Bad contents in file '%1'").arg(filePath);
            return EXIT_FAILURE;
        } else {
            ConsoleProcess* cp = new ConsoleProcess(profile);
            QObject::connect(cp, &ConsoleProcess::finished, &QApplication::quit);
        }
    } else {
        MainWindow* w = new MainWindow();
        w->show();
    }

    return a.exec();
}
