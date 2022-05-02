#ifndef MOOCSETTING_H
#define MOOCSETTING_H

#include "TranslucentWidget.h"
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QProcess>

class MainWindow;

struct elem {
    int cid, tid;
    QCheckBox *checkBox;
    elem(int Cid, int Tid, QString Name, QWidget *parent = nullptr) : cid(Cid), tid(Tid) {
        checkBox = new QCheckBox(parent);
        checkBox->setText(Name);
        checkBox->setStyleSheet("color: rgb(255,255,255);\n"
                                "font: 10pt \"Microsoft YaHei UI\";\n");
    }
};

class MoocSetting : public TranslucentWidget
{
    Q_OBJECT
public:
    MoocSetting(QWidget *parent = nullptr, MainWindow *Sibling = nullptr);
    void getAllCourse();
    void confirmEvent();
    void updateGeometry();
    void turnPrevPage();
    void turnNextPage();
    ~MoocSetting();

private:
    MainWindow *sibling;
    QTextEdit *tokenEdit;
    QList<elem*> items;
    QPushButton *prevPage, *nextPage;
    QPushButton *getCourse, *confirm;
    int currentPageNumber;
};

#endif // MOOCSETTING_H
