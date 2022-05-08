#ifndef TASKLINEEDIT_H
#define TASKLINEEDIT_H

#include <QTextEdit>
#include <QMouseEvent>
#include <QLineEdit>

class TaskLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    TaskLineEdit(QWidget *parent = nullptr);
    //void focusOutEvent(QFocusEvent *) {moveCursor(QTextCursor::Start);setTextInteractionFlags(Qt::TextSelectableByMouse);}
    //void focusInEvent(QFocusEvent *) {setTextInteractionFlags(Qt::TextEditorInteraction);}
};

#endif // TASKLINEEDIT_H
