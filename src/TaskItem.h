#ifndef TASKITEM_H
#define TASKITEM_H

#include <QLabel>
#include <QRect>
#include <QDateTime>
#include <QString>
#include <QPushButton>
#include <QMouseEvent>
#include <QLineEdit>

class MainWindow;
class Page;

class TaskItem : public QWidget
{
public:
    TaskItem(MainWindow *Parent, Page *Belong, QDateTime DateTime, QString Text, unsigned int Level, bool Done, int MoocId = 0);
    static bool cmp(TaskItem *a, TaskItem *b);
    void deleteTaskEvent();

    QLineEdit       *lineEdit;
    QLabel          *timeLabel;
    QLabel          *dotLabel;
    QPushButton     *finishButton;
    QPushButton     *returnButton;
    QPushButton     *deleteButton;
    QDateTime       m_dateTime;
    unsigned int    m_level;
    unsigned int    m_number;
    bool            m_done;
    int    m_moocId;

protected:
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void finishTaskEvent();
    void returnTaskEvent();

private:
    Page *belong;
    QRect getTimeLabelRect();
    QRect getLineEditRect();
    QRect getDotLabelRect();
};

#endif // TASKITEM_H
