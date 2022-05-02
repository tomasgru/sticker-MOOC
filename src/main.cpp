#include "MainWindow.h"
#include <windows.h>
#include <QApplication>
#include <QDateTime>
#include <QTextCodec>
#include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    MainWindow w;
    w.page_ToDo->show();
    w.page_Done->hide();
    w.show();
    return a.exec();
}
