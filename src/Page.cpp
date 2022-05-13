#include "Page.h"
#include "MainWindow.h"
#include <QDebug>

#define INF 0x3f3f3f3f

QRect mainLableRect(25+OVER,30+OVER,90,35);
QRect viceLableRect(120+OVER,40+OVER,60,25);

QString mainLableStyle("color: rgb(255,255,255);\n"
                       "font: bold 24pt \"Microsoft YaHei UI\";\n"
                       "text-align: left;\n"
                       "text-align: bottom;");

QString viceLableStyle("color: rgb(200,200,200);\n"
                       "font: bold 16pt \"Microsoft YaHei UI\";\n"
                       "text-align: left;\n"
                       "text-align: bottom;");

Page::Page(MainWindow *Parent, bool TODO)
    : QWidget(Parent), m_done(!TODO)
{
    QWidget::hide();
    parent = Parent;
    lab = new ClicableLabel(Parent);
    prevPage = new QPushButton(Parent);
    nextPage = new QPushButton(Parent);
    prevPage->setMouseTracking(true);
    nextPage->setMouseTracking(true);
    startItem.emplace_back(0);
    currentPageNumber = 0;

    prevPage->setStyleSheet(MainWindow::buttonStyle("prevPage"));
    nextPage->setStyleSheet(MainWindow::buttonStyle("nextPage"));
    QObject::connect(prevPage,&QPushButton::clicked,this,&Page::turnPrevPage);
    QObject::connect(nextPage,&QPushButton::clicked,this,&Page::turnNextPage);
    prevPage->setToolTip("上一页");
    nextPage->setToolTip("下一页");

    if (TODO) {
        lab->setText("ToDo");
        setMain();
    }
    else {
        lab->setText("Done");
        setVice();
    }
    lab->show();
}

void Page::addTask(QDateTime DateTime, QString Text, int MoocId, QString type)
{
    if (MoocId) {
        if (searchMooc(MoocId)) return;
        if (!Text.contains("（作业）") && !Text.contains("（测验）") && !Text.contains("（互评）") && !Text.contains("（考试）")) {
            if (type == "homeworks") Text += "（作业）";
            else if (type == "quizs") Text += "（测验）";
            else if (type == "evaluate") Text += "（互评）";
            else Text += "（考试）";
        }
    }
    QDateTime today = QDateTime::fromString(DateTime.date().toString("yyyyMMdd000000"),"yyyyMMddhhmmss");
    if (!search(today)) {
        TaskItem *tmp = new TaskItem(parent, this, today, today.toString("MM.dd"), 0, m_done, false);
        items.push_back(tmp);
    }
    TaskItem *add = new TaskItem(parent, this, DateTime, Text, 1, m_done, MoocId);
    items.push_back(add);
    std::sort(items.begin(),items.end(),TaskItem::cmp);
    updateGeometry();
    if (!Done && parent->currentPage == ToDo) setMain();
    emit taskNew();
}

void Page::updateGeometry()
{
    for (unsigned int i=0; i<items.size(); i++) items[i]->m_number = i;
    startItem.clear(), startItem.emplace_back(0);
    for (int i=0; i<(int)items.size(); i++) {
        if (i == startItem.back()) {
            items[i]->setGeometry(25+OVER,74+OVER,parent->smallGeometry().width()-50,24);
        }
        else {
            if (items[i-1]->geometry().bottom()+(items[i]->m_level?0:5) + items[i-1]->geometry().height() + 10 > parent->smallGeometry().height()) {
                startItem.emplace_back(i);
                i--;
                continue;
            }
            items[i]->setGeometry(25+OVER,items[i-1]->geometry().bottom()+(items[i]->m_level?1:4),items[i-1]->geometry().width(),items[i-1]->geometry().height());
        }
    }
    if (currentPageNumber >= (int)startItem.size())
        currentPageNumber = startItem.size()-1;
    //调试->任务列表
/*
    qDebug()<<items.size()<<endl;
    for (unsigned int i=0; i<items.size(); i++) {
        if (items[i]->m_level == 0) continue;
        qDebug()<<items[i]->geometry()<<items[i]->isHidden()
                <<items[i]->dotLabel->geometry()<<items[i]->dotLabel->isHidden()
                <<items[i]->timeLabel->geometry()<<items[i]->timeLabel->isHidden()
                <<items[i]->lineEdit->geometry()<<items[i]->lineEdit->isHidden()
                <<items[i]->lineEdit->text()
                <<items[i]->m_dateTime<<"Level:"<< items[i]->m_level
                <<" Done:"<<items[i]->m_done <<items[i]->m_moocId<<items[i]->m_number<<endl;
    }
*/
}

void Page::delteItem(int number)
{
    items.erase(items.begin()+number);
    delteSpareDate();
    setMain();
    if (items.empty()) emit taskClear();
    parent->save();
}

void Page::delteSpareDate()
{
    for (int i=0; i<(int)items.size(); i++) {
        int j=i+1;
        if ((j == (int)items.size() || items[j]->m_level == 0) && items[i]->m_level == 0) {
            items[i]->deleteLater();
            items.erase(items.begin()+i);
        }
    }
}

void Page::turnPrevPage() { if (currentPageNumber) currentPageNumber--; setMain();}
void Page::turnNextPage() { if (currentPageNumber < (int)items.size()) currentPageNumber++; setMain();}

void Page::setMain()
{
    lab->setGeometry(mainLableRect);
    lab->setStyleSheet(mainLableStyle);
    prevPage->show(), nextPage->show();
    lab->m_locked = true;
    updateGeometry();
    prevPage->setGeometry(getPrevButtonRect());
    nextPage->setGeometry(getNextButtonRect());
    hide();
    show();
}

void Page::setVice()
{
    lab->setGeometry(viceLableRect);
    lab->setStyleSheet(viceLableStyle);
    prevPage->hide(), nextPage->hide();
    lab->m_locked = false;
    updateGeometry();
    prevPage->setGeometry(getPrevButtonRect());
    nextPage->setGeometry(getNextButtonRect());
    hide();
}

bool Page::search(QDateTime val)
{
    for (auto it:items) {
        if (val == it->m_dateTime) return true;
    }
    return false;
}

bool Page::searchMooc(int val)
{
    for (auto it:items) {
        if (it->m_moocId == 0) continue;
        if (val == it->m_moocId) return true;
    }
    return false;
}

void Page::show()
{
    for (int i=startItem[currentPageNumber]; i<(int)items.size(); i++) {
        if (currentPageNumber != (int)startItem.size()-1
                && i == startItem[currentPageNumber+1]) break;
        items[i]->show();
    }
}

void Page::hide()
{
    for (auto it:items) {
        it->hide();
    }
}

QRect Page::getPrevButtonRect()
{
    return QRect(parent->width()-68-OVER, 43+OVER, 20, 20);
}

QRect Page::getNextButtonRect()
{
    return QRect(parent->width()-45-OVER, 43+OVER, 20, 20);
}
