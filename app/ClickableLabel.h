#pragma once

#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

    void SetImage(const QImage& image) { m_image = image; }
    void ClearImage() { m_image = QImage(0, 0, QImage::Format::Format_RGBX8888); }

signals:
    void clicked();
    void doubleClicked(QImage& image);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    QImage m_image;
};