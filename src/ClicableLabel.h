#ifndef ClicableLabel_H
#define ClicableLabelL_H

#include <QLabel>
#include <QMouseEvent>
#include <QTimer>

class ClicableLabel : public QLabel
{
    Q_OBJECT
public:
    ClicableLabel(QWidget *parent = nullptr);
    ClicableLabel* operator=(ClicableLabel *b) {return b;}

    bool m_locked;
    QTimer *doubleClickedTimer;

signals:
    void clicked();
    void doubleClicked();

protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void doubleClickedTimerStart();

};

#endif // ClicableLabelL_H
