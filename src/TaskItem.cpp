#include "TaskItem.h"
#include "MainWindow.h"
#include "Page.h"
#include <QDebug>

QString lineEditStyle("background-color:transparent;\n"
                      "border-width:0;\n"
                      "border-style:outset;\n"
                      "color: rgb(255,255,255);\n"
                      "font: 11pt \"Microsoft YaHei UI\";\n"
                      "text-align: left;\n"
                      "text-align: VCenter;");

QString dateTitleStyle("background-color:transparent;\n"
                      "border-width:0;\n"
                      "border-style:outset;\n"
                      "color: rgb(255,255,255);\n"
                      "font: 12pt \"Microsoft YaHei UI\";\n"
                      "text-align: left;\n"
                      "text-align: bottom;");

QString timeLabelStyle("color: rgb(255,255,255);\n"
                       "font: 11pt \"Microsoft YaHei UI\";\n"
                       "text-align: left;\n"
                       "text-align: VCenter;");

QString dotLabelStyle("color: rgb(255,255,255);\n"
                       "font: 13pt \"Microsoft YaHei UI\";\n"
                       "text-align: VCenter;\n"
                       "text-align: HCenter;");

QString TaskButtonStyle("QPushButton {"
                    "image: url(:/new/button/icon/%1.png);"
                    "border:1px solid rgba(255,255,255,30);"
                    "border-radius:9px;"
                    "border-width: 1px;"
                    "background-color: rgba(%2,%3,%4,200);"
                    "}"
                    "QPushButton:hover {"
                    "image: url(:/new/button/icon/%1.png);"
                    "border:1px solid rgba(255,255,255,30);"
                    "border-radius:9px;"
                    "border-width: 1px;"
                    "background-color: rgba(%2,%3,%4,230);"
                    "}"
                    "QPushButton:pressed {"
                    "image: url(:/new/button/icon/%1.png);"
                    "border:1px solid rgba(255,255,255,30);"
                    "border-radius:9px;"
                    "border-width: 1px;"
                    "background-color: rgba(%2,%3,%4,255);"
                    "}");

TaskItem::TaskItem(MainWindow *Parent, Page *Belong, QDateTime DateTime, QString Text, unsigned int Level, bool Done, int MoocId)
    : QWidget(Parent), m_dateTime(DateTime), m_level(Level), m_done(Done), m_moocId(MoocId), belong(Belong)
{
    //setMouseTracking(true);
    if (Level == 0) {
        dotLabel = nullptr;
        timeLabel = nullptr;
    }
    else {
        timeLabel = new QLabel(this);
        dotLabel = new QLabel(this);
        timeLabel->setMouseTracking(true);
        dotLabel->setMouseTracking(true);
        timeLabel->setStyleSheet(timeLabelStyle);
        dotLabel->setStyleSheet(dotLabelStyle);
        timeLabel->setGeometry(getTimeLabelRect());
        dotLabel->setGeometry(getDotLabelRect());
        timeLabel->setText(DateTime.toString("hh:mm"));
        dotLabel->setText("●");

    }

    if (!Done && Level) {
        finishButton = new QPushButton(this);
        finishButton->setMouseTracking(true);
        finishButton->setGeometry(16,3,19,19);
        connect(finishButton,&QPushButton::clicked,this,&TaskItem::finishTaskEvent);
        finishButton->setStyleSheet(TaskButtonStyle.arg("finishButton").arg(67).arg(178).arg(86));
        finishButton->hide();
    }
    else if (Done && Level) {
        returnButton = new QPushButton(this);
        returnButton->setMouseTracking(true);
        returnButton->setGeometry(16,3,19,19);
        connect(returnButton,&QPushButton::clicked,this,&TaskItem::returnTaskEvent);
        returnButton->setStyleSheet(TaskButtonStyle.arg("returnButton").arg(16).arg(174).arg(194));
        returnButton->hide();
    }
    deleteButton = new QPushButton(this);
    deleteButton->setMouseTracking(true);
    deleteButton->setGeometry(41,3,19,19);
    connect(deleteButton,&QPushButton::clicked,this,&TaskItem::deleteTaskEvent);
    deleteButton->setStyleSheet(TaskButtonStyle.arg("deleteButton").arg(239).arg(71).arg(53));
    deleteButton->hide();

    lineEdit = new QLineEdit(this);
    if (m_level) lineEdit->setStyleSheet(lineEditStyle);
    else lineEdit->setStyleSheet(dateTitleStyle);
    lineEdit->setGeometry(getLineEditRect());
    lineEdit->setText(Text);
    if (Level == 0 || MoocId) lineEdit->setReadOnly(true);
}

void TaskItem::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (dotLabel != nullptr) dotLabel->setGeometry(getDotLabelRect());
    if (timeLabel != nullptr) timeLabel->setGeometry(getTimeLabelRect());
    lineEdit->setGeometry(getLineEditRect());

}

void TaskItem::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (m_level == 0) return;
    dotLabel->hide(), timeLabel->hide();
    deleteButton->show();
    if (!m_done) finishButton->show();
    else returnButton->show();
}

void TaskItem::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (m_level == 0) return;
    dotLabel->show(), timeLabel->show();
    deleteButton->hide();
    if (!m_done) finishButton->hide();
    else returnButton->hide();
}

bool TaskItem::cmp(TaskItem *a, TaskItem *b)
{
    unsigned long long aIntTime = a->m_dateTime.toString("yyyyMMddhhmmsszzz").toULongLong();
    unsigned long long bIntTime = b->m_dateTime.toString("yyyyMMddhhmmsszzz").toULongLong();
    if (aIntTime == bIntTime) return a->m_level < b->m_level;
    else return aIntTime < bIntTime;
}

void TaskItem::finishTaskEvent()
{
    belong->parent->addTask(m_dateTime, lineEdit->text(), true, m_moocId);
    belong->delteItem(m_number);
    this->deleteLater();
}

void TaskItem::returnTaskEvent()
{
    belong->parent->addTask(m_dateTime, lineEdit->text(), false, m_moocId);
    belong->delteItem(m_number);
    this->deleteLater();
}

void TaskItem::deleteTaskEvent()
{
    belong->delteItem(m_number);
    this->deleteLater();
}

QRect TaskItem::getDotLabelRect() {
    return QRect(5, 3, 15, 16);
}

QRect TaskItem::getTimeLabelRect() {
    return QRect(dotLabel->geometry().right()+8, 0, 40, geometry().height());
}

QRect TaskItem::getLineEditRect() {
    if (m_level == 0) return QRect(0,0,geometry().width(),geometry().height());
    else return QRect(timeLabel->geometry().right()+8, 0, geometry().width()-timeLabel->geometry().right()-8, geometry().height());
}

/*
QRect TaskItem::getTimeLabelRect() {
    return QRect(geometry().right()-40, geometry().bottom()-20, 40, 20);
}

QRect TaskItem::getLineEditRect() {
    if (m_level == 0) return QRect(geometry().left(), geometry().top(), geometry().width()-65, geometry().height());
    else return QRect(geometry().left()+20, geometry().top(), geometry().width()-85, geometry().height());
}
*/
