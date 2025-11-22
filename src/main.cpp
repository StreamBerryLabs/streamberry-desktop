#include "ui/mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("StreamBerry Desktop");
    a.setOrganizationName("StreamBerry Labs");
    
    // Set application icon
    a.setWindowIcon(QIcon(":/icon.png")); // We'll need to add a resource file or load from file

    QCommandLineParser parser;
    parser.setApplicationDescription("StreamBerry Desktop Client");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption minimizedOption("minimized", "Start minimized to system tray");
    parser.addOption(minimizedOption);
    
    parser.process(a);
    
    bool startMinimized = parser.isSet(minimizedOption);

    MainWindow w(nullptr, startMinimized);
    
    if (!startMinimized) {
        w.show();
    }

    return a.exec();
}
