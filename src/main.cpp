#include "MainWindow.h"
#include <windows.h>
#include <QApplication>
#include <QDateTime>
#include <QTextCodec>
#include <QProcess>

bool desktop = false;

bool enumUserWindowsCB(HWND hwnd, LPARAM lParam)
{
    long wflags = GetWindowLong(hwnd, GWL_STYLE);
    if (!(wflags & WS_VISIBLE)) {
        return TRUE;
    };
    HWND targetWnd;
    if (desktop) {
        HWND sndWnd;
        if (!(sndWnd=FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL))) {
            return TRUE;
        }
        if (!(targetWnd=FindWindowEx(sndWnd, NULL, L"SysListView32", L"FolderView"))) {
            return TRUE;
        }
    } else {
        if (!(targetWnd=FindWindowEx(hwnd, NULL, L"TXMiniSkin", L"桌面整理"))) {
            return TRUE;
        }
    }
    HWND* resultHwnd = (HWND*)lParam;
    *resultHwnd = targetWnd;
    return FALSE;
}

void setParentDesktop(QWidget *pWidget)
{
    HWND resultHwnd = NULL;
    EnumWindows((WNDENUMPROC)enumUserWindowsCB, (LPARAM)&resultHwnd);
    if (!resultHwnd) {
        desktop = true;
        EnumWindows((WNDENUMPROC)enumUserWindowsCB, (LPARAM)&resultHwnd);
    }

    if (resultHwnd) {
        SetParent((HWND)pWidget->winId(), resultHwnd);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QSettings *config = new QSettings("./config.ini",QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF8"));

    MainWindow *w = new MainWindow(nullptr,config);

    if (!config->contains("desktop")) {
        config->setValue("desktop","true");
    }
    if (config->value("desktop").toBool()) {
        setParentDesktop(w);
    }

    w->page_ToDo->show();
    w->page_Done->hide();
    w->show();

    return a.exec();
}
