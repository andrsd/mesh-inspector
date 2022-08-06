#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget * parent) : QLabel(parent) {}

void
ClickableLabel::enterEvent(QEvent * event)
{
    auto f = this->font();
    f.setUnderline(true);
    this->setFont(f);
}

void
ClickableLabel::leaveEvent(QEvent * event)
{
    auto f = this->font();
    f.setUnderline(false);
    this->setFont(f);
}

void
ClickableLabel::mouseReleaseEvent(QMouseEvent * ev)
{
    emit clicked();
}
