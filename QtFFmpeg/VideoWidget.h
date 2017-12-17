#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QMainWindow>
#include<QtWidgets/qwidget.h>
#include <QOpenGLWidget>
#include <QPushButton>
#include <QLabel>
#include <QObject>
#include <QCloseEvent>
#include <QTimerEvent>
#include <QResizeEvent>
#include <QSlider>
class VideoWidget : public QOpenGLWidget {
Q_OBJECT

private:
    QPushButton *openButoon;
    QPushButton *playButoon;
    QLabel *timeLabel;
    QLabel *curTimeLabel;
//    QSlider *progressSlider;
    int threadId = 0;
public:
    VideoWidget(QWidget *pWidget = NULL);

    void paintEvent(QPaintEvent *e);

    void timerEvent(QTimerEvent *e);

    void resizeEvent(QResizeEvent *e);
//    virtual ~VideoWidget();

public slots:

    void handleButton();

protected:
    void closeEvent(QCloseEvent *event);

};

#endif // VIDEOWIDGET_H
