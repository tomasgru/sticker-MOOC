#define PADDING 3
#define MOVEHEIGHT 29
#include "TranslucentWidget.h"
#include <QCursor>

QString BUTTONStyle("QPushButton {"
                    "image: url(:/new/button/icon/%1.png);"
                    "border-radius:1px;"
                    "background-color: rgba(0,0,0,0);"
                    "color: rgb(255,255,255);\n"
                    "font: 10pt \"Microsoft YaHei UI\";\n"
                    "}"
                    "QPushButton:hover {"
                    "image: url(:/new/button/icon/%1.png);"
                    "border:1px solid rgba(255,255,255,30);"
                    "border-radius:1px;"
                    "border-width: 1px;"
                    "background-color: rgba(0,0,0,30);"
                    "color: rgb(255,255,255);\n"
                    "font: 10pt \"Microsoft YaHei UI\";\n"
                    "}"
                    "QPushButton:pressed {"
                    "image: url(:/new/button/icon/%1.png);"
                    "border:1px solid rgba(255,255,255,30);"
                    "border-radius:1px;"
                    "border-width: 1px;"
                    "background-color: rgba(0,0,0,50);"
                    "color: rgb(255,255,255);\n"
                    "font: 10pt \"Microsoft YaHei UI\";\n"
                    "}");

TranslucentWidget::TranslucentWidget(QWidget *parent, int minWidth, int minHeight, int alpha)
    : QWidget(parent), alpha(alpha)
{
    ui_initialize(minWidth, minHeight);
    inHead = false;
    inResize = false;
    inBottom = false;
    resizeLocked = false;
    moveLocked = false;
    focus = false;
}

TranslucentWidget::~TranslucentWidget() {}

QString TranslucentWidget::buttonStyle(QString name) { return BUTTONStyle.arg(name); }

void TranslucentWidget::ui_initialize(int minWidth, int minHeight)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    this->setMinimumWidth(minWidth);
    this->setMinimumHeight(minHeight);
    isLeftPressing = false;
}

void TranslucentWidget::setSmallGeometry(QRect Rect)
{
    setGeometry(Rect.left()-OVER,Rect.top()-OVER,Rect.width()+2*OVER,Rect.height()+2*OVER);
}

QRect TranslucentWidget::smallGeometry()
{
    return QRect(geometry().left()+OVER,geometry().top()+OVER,geometry().width()-2*OVER,geometry().height()-2*OVER);
}

QRect TranslucentWidget::smallRect()
{
    return QRect(OVER,OVER,smallGeometry().width(),smallGeometry().height());
}

void TranslucentWidget::focusInMyEvent()
{
    alpha = 90;
    focus = true;
    repaint();
}

void TranslucentWidget::focusOutMyEvent()
{
    alpha = 50;
    focus = false;
    repaint();
}

void TranslucentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    emit paintAdd();

    QPen pen;
    pen.setColor(QColor(255,255,255,30));

    QBrush brush;
    brush.setColor(QColor(0,0,0,alpha));
    brush.setStyle(Qt::SolidPattern);

    QPainter painter(this);
    painter.setPen(QPen(QColor(0,0,0,0)));
    painter.setBrush(QBrush(QColor(0,0,0,1)));
    painter.drawRect(this->rect());
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(this->smallRect());

    if (!inResize || resizeLocked) return;
    int H = smallGeometry().height(), W = smallGeometry().width();
    painter.setBrush(QColor(255,255,255,255));
    painter.setPen(QColor(0,0,0,0));
    painter.drawRect(4+OVER,H-10+OVER,2,2);
    painter.drawRect(4+OVER,H-5+OVER,2,2);
    painter.drawRect(10+OVER,H-5+OVER,2,2);
    painter.drawRect(W-6+OVER,H-10+OVER,2,2);
    painter.drawRect(W-6+OVER,H-5+OVER,2,2);
    painter.drawRect(W-11+OVER,H-5+OVER,2,2);
    int x = 1.0*W/2;
    painter.drawRect(x+OVER,H-4+OVER,2,2);
    painter.drawRect(x+6+OVER,H-4+OVER,2,2);
    painter.drawRect(x-6+OVER,H-4+OVER,2,2);
}

void TranslucentWidget::region(const QPoint &currentGlobalPoint)
{
    QRect rect = this->smallGeometry();
    int top = rect.top();
    int bottom = rect.bottom();
    int left = rect.left();
    int right = rect.right();

    int x = currentGlobalPoint.x();
    int y = currentGlobalPoint.y();
    dir = DEFAULT;
    if (y <= top + MOVEHEIGHT && y>= top && x >= left && x <= right) {
        if (!inHead) inHead = true, emit enterHead();
    }
    else if (inHead) inHead = false, emit leaveHead();
    if (!resizeLocked) {
        if (y >= bottom - PADDING && y<= bottom && x >= left && x <= right) {
            if (!inBottom) inBottom = true, emit enterBottom();
        }
        else if (inBottom) inBottom = false, emit leaveBottom();
        if (x >= left && x <= left + PADDING &&
            y >= top && y <= top + PADDING) {
            dir = TOP_LEFT;
            this->setCursor(Qt::SizeFDiagCursor);
        }
        else if (x >= right - PADDING && x <= right &&
                 y >= top && y <= top + PADDING) {
            dir = TOP_RIGHT;
            this->setCursor(Qt::SizeBDiagCursor);
        }
        else if (x >= left && x <= left + PADDING &&
                 y >= bottom - PADDING && y <= bottom) {
            dir = BOTTOM_LEFT;
            this->setCursor(Qt::SizeBDiagCursor);
        }
        else if (x >= right - PADDING && x <= right &&
                 y >= bottom - PADDING && y <= bottom) {
            dir = BOTTOM_RIGHT;
            this->setCursor(Qt::SizeFDiagCursor);
        }
        else if (x >= left&& x <= left + PADDING &&
                 y >= top&& y <= bottom) {
            dir = LEFT;
            this->setCursor(Qt::SizeHorCursor);
        }
        else if (x >= right - PADDING && x <= right&&
                 y >= top&& y <= bottom) {
            dir = RIGHT;
            this->setCursor(Qt::SizeHorCursor);
        }
        else if (y >= top && y <= top + PADDING &&
                 x >= left && x <= right) {
            dir = TOP;
            this->setCursor(Qt::SizeVerCursor);
        }
        else if (y >= bottom - PADDING && y <= bottom &&
                 x >= left && x <= right) {
            dir = BOTTOM;
            this->setCursor(Qt::SizeVerCursor);
        }
    }
    if (dir != DEFAULT && !inResize) inResize = true, emit enterResize();
    else if (dir == DEFAULT && inResize) inResize = false, emit leaveResize();
    if (dir == DEFAULT && y <= top + MOVEHEIGHT && y >= top) {
        dir = MOVE;
        this->setCursor(Qt::ArrowCursor);
    }
    else if (dir == DEFAULT) this->setCursor(Qt::ArrowCursor);
}

void TranslucentWidget::mousePressEvent(QMouseEvent *event)
{
    if (event != nullptr && event->button() == Qt::LeftButton)
        isLeftPressing = true;
    m_movePoint = event->globalPos() - this->geometry().topLeft();
    if (!focus && smallGeometry().contains(event->globalPos())) emit focusInMyEvent();
    else emit focusOutMyEvent();
}

void TranslucentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    isLeftPressing = false;
}

void TranslucentWidget::mouseMoveEvent(QMouseEvent *event)
{
    //emit mouseMoveAdd(event);
    //qDebug()<<geometry()<<smallGeometry()<<QCursor::pos();
    QPoint globalPoint = event->globalPos();
    int minH = minimumHeight()-2*OVER, minW = minimumWidth()-2*OVER;

    if (focus && !smallGeometry().contains(event->globalPos())) emit focusOutMyEvent();
    if (!isLeftPressing) {
        region(globalPoint);
        //qDebug()<<globalPoint;
        return;
    }
    QRect newRect = this->smallGeometry();
    if (dir != MOVE) {
        switch (dir) {
        case TOP_LEFT:
            if (newRect.bottom() - globalPoint.y() > minH &&
                newRect.right() - globalPoint.x() > minW)
                newRect.setTopLeft(globalPoint);
            else if (newRect.bottom() - globalPoint.y() > minH)
                newRect.setTop(globalPoint.y());
            else if (newRect.right() - globalPoint.x() > minW)
                newRect.setLeft(globalPoint.x());
            break;
        case TOP_RIGHT:
            if (newRect.bottom() - globalPoint.y() <= minH)
                newRect.setRight(globalPoint.x());
            else newRect.setTopRight(globalPoint);
            break;
        case BOTTOM_LEFT:
            if (newRect.right() - globalPoint.x() <= minW)
                newRect.setBottom(globalPoint.y());
            else newRect.setBottomLeft(globalPoint);
            break;
        case BOTTOM_RIGHT:
            newRect.setBottomRight(globalPoint);
            break;
        case TOP:
            if (newRect.bottom() - globalPoint.y() > minH)
                newRect.setTop(globalPoint.y());
            break;
        case BOTTOM:
            newRect.setBottom(globalPoint.y());
            break;
        case LEFT:
            if (newRect.right() - globalPoint.x() > minW)
            newRect.setLeft(globalPoint.x());
            break;
        case RIGHT:
            newRect.setRight(globalPoint.x());
            break;
        default:
            break;
        }
        this->setSmallGeometry(newRect);
    }
    else if (dir != DEFAULT) {
        if (!moveLocked) move(globalPoint - m_movePoint);
        else emit tryLockedMove();
    }
}
