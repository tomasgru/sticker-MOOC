#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPushButton>
#include <QTimer>
#include "TranslucentWidget.h"
#include "AddWindow.h"
#include "Page.h"
#include <MoocSetting.h>
#include <QSettings>
#include <QTextCodec>

enum pageType {ToDo, Done};

class MainWindow : public TranslucentWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addTask(QDateTime DateTime, QString Text, bool Done, int Mooc, QString Type = nullptr);
    void save();
    void parse();
    void moocParse();
    void moocStart();
    Page *page_ToDo, *page_Done;
    QPushButton *addButton, *delButton;
    QPushButton *settingButton;
    QPushButton *foldButton, *openButton;
    QPushButton *lockButton;
    ClicableLabel *titleLabel;

    AddWindow *addWindow;
    MoocSetting *moocSetting;
    pageType currentPage;

signals:
    void doubleClicked();

protected:
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);
    void foldEvent();
    void openEvent();
    void lockEvent();
    void showHeadPart();
    void hideHeadPart();
    void showResizePart();
    void hideResizePart();
    void transPage();
    void newAddWindow();
    void newMoocSettingWindow();
    void delDoneTask();

private:
    QRect getAddButtonRect();
    QRect getdelButtonRect();
    QRect getSettingButtonRect();
    QRect getFoldButtonRect();
    QRect getLockButtonRect();
    QRect getAddWindowRect();
    QRect getTitleRect();
    void moocAutoEvent();
    void doubleClickedTimerStart();
    void paintBackground();
    void updateLabelColor();
    void startColorTimer();
    void savePosEvent();
    QTimer *doubleClickedTimer, *colorTimer, *moocAuto, *saveTimer;
    QSettings *config;
    int oldHeight;
    bool folded;
};

#endif // MAINWINDOW_H
