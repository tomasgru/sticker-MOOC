#include "ClicableLabel.h"

ClicableLabel::ClicableLabel(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    doubleClickedTimer = new QTimer(this);
    doubleClickedTimer->setSingleShot(true);
}

void ClicableLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev != nullptr && ev->button() == Qt::LeftButton && !this->m_locked) {
        emit clicked();
        emit doubleClickedTimerStart();
    }
}

void ClicableLabel::doubleClickedTimerStart()
{
    if (doubleClickedTimer->isActive()) {
        emit doubleClicked();
        doubleClickedTimer->stop();
    }
    else doubleClickedTimer->start(500);
}
