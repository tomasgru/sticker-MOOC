#ifndef ADDWINDOW_H
#define ADDWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QTimer>
#include "TranslucentWidget.h"

class MainWindow;

class AddWindow : public TranslucentWidget
{
    Q_OBJECT

public:
    AddWindow(QWidget *parent = nullptr, MainWindow *Sibling = nullptr);
    ~AddWindow();
    void createElem();
    void setLayout();
    void setStyle();
    void setRect();
    void sendTask();
    void cancelEvent();

private:
    MainWindow *sibling;
    QLabel *m_Title, *m_Date, *m_Time, *m_Dot, *m_Colon;
    QLineEdit *m_Text, *m_Month, *m_Day, *m_Hour, *m_Minute;
    QPushButton *m_Confirm, *m_Cancel;
    QTimer *delTimer;
};

#endif // ADDWINDOW_H
