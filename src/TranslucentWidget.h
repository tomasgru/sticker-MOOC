#ifndef TRANSLUCENTWIDGET_H
#define TRANSLUCENTWIDGET_H
#define OVER 5

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include <QPen>
#include <QDebug>

enum edgeDirtion {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, TOP, BOTTOM, LEFT, RIGHT, MOVE, DEFAULT};

class TranslucentWidget : public QWidget
{
    Q_OBJECT

public:
    TranslucentWidget(QWidget *parent = nullptr, int minWidth = 300+2*OVER, int minHeight = 300+2*OVER, int alpha = 50);
    ~TranslucentWidget();
    static QString buttonStyle(QString name);
    void setSmallGeometry(QRect Rect);
    QRect smallGeometry();
    QRect smallRect();

signals:
    void paintAdd();
    void enterBottom();
    void leaveBottom();
    void enterHead();
    void leaveHead();
    void enterResize();
    void leaveResize();
    void tryLockedMove();

protected:
    void ui_initialize(int minWidth, int minHeight);
    void region(const QPoint &currentGlobalPoint);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void focusInMyEvent();
    void focusOutMyEvent();
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    edgeDirtion dir;
    bool isLeftPressing, resizeLocked, moveLocked, inBottom, inHead, inResize, focus;
    QPoint m_movePoint;
    int alpha;
};

#endif // TRANSLUCENTWIDGET_H
