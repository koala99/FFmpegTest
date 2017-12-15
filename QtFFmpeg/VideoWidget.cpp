#include <QPainter>
#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "FFmpegUtils.h"

VideoWidget::VideoWidget(QWidget *pWidget) : QOpenGLWidget(pWidget) {
    //设置窗口大小
    setGeometry(0, 0, 800, 600);
    QPushButton *openButoon = new QPushButton(QObject::tr("打开视频"), this);
    openButoon->setGeometry(100, 520, 80, 40);

    QPushButton *playButoon = new QPushButton(QObject::tr("播放"), this);
    playButoon->setGeometry(520, 520, 80, 40);


    //设置背景
//    QPalette pal(pWidget->palette());
//    pal.setColor(QPalette::Background, Qt::white);
//    pWidget->setAutoFillBackground(false);
//    pWidget->setPalette(pal);
//    playButoon->show();
    FFmpegUtils::Get()->openVideo("test.mp4");
    startTimer(16); //fps
}

void VideoWidget::paintEvent(QPaintEvent *e) {
    static QImage *image = NULL;
    if (image == NULL) {
        uchar *buf = new uchar[width() * height() * 4];
        image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
    }
    AVPacket pkt = FFmpegUtils::Get()->readVideo();
    if (pkt.stream_index != FFmpegUtils::Get()->videoStream) {
        return;
    }
    if (pkt.size == 0) {
        return;
    }
    AVFrame *yuv = FFmpegUtils::Get()->decodeMedia(&pkt);
    av_packet_unref(&pkt);
    if (yuv == NULL) {
        return;
    }
    FFmpegUtils::Get()->toRgb(yuv, (char *) image->bits(), width(), height());
    QPainter painter;
    painter.begin(this);
    painter.drawImage(QPoint(0, 0), *image);
    painter.end();
}

void VideoWidget::timerEvent(QTimerEvent *e) {
    this->update();
}

