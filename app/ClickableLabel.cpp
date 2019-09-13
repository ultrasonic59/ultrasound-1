#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event) {
    if(m_image.width() > 0 && m_image.height() > 0)
        emit doubleClicked(m_image);
}