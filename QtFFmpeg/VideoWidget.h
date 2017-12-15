#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QMainWindow>
#include<QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget {
public:
    VideoWidget(QWidget *pWidget = NULL);

    void paintEvent(QPaintEvent *e);

    void timerEvent(QTimerEvent *e);
//    virtual ~VideoWidget();
};

#endif // VIDEOWIDGET_H
