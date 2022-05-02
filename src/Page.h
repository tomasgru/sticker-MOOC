#ifndef PAGE_H
#define PAGE_H

#include "TaskItem.h"
#include "ClicableLabel.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

class MainWindow;

class Page : public QWidget
{
    Q_OBJECT
public:
    Page(MainWindow *Parent, bool TODO);
    void addTask(QDateTime DateTime, QString Text, int MoocId, QString type = nullptr);
    void updateGeometry();
    void delteItem(int number);
    void delteSpareDate();
    void setMain();
    void setVice();
    static bool cmp(TaskItem *a, TaskItem *b);
    void show();
    void hide();

    Page* operator=(Page *b) {return b;}

    MainWindow              *parent;
    ClicableLabel           *lab;
    QPushButton             *prevPage, *nextPage;
    bool                    m_done;
    std::vector<TaskItem*>  items;

signals:
    void taskNew();
    void taskClear();

public slots:
    void turnPrevPage();
    void turnNextPage();

private:
    std::vector<int> startItem;
    int currentPageNumber;
    bool search(QDateTime val);
    bool searchMooc(int val);
    QRect getPrevButtonRect();
    QRect getNextButtonRect();
};

#endif // PAGE_H
