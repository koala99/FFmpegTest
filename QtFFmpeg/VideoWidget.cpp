#include <QPainter>
#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "FFmpegUtils.h"
#include "VideoThread.h"
#include "AudioPlay.h"

#include <qobject.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QFont>
#include <QPalette>

VideoWidget::VideoWidget(QWidget *pWidget) : QOpenGLWidget(pWidget) {

    //设置窗口大小
    setGeometry(0, 0, 800, 600);
    timeLabel = new QLabel(QObject::tr("00:00"), this);
    timeLabel->setGeometry(350, 520, 100, 40);
    QFont ft;
    ft.setPointSize(15);
    timeLabel->setFont(ft);
    QPalette pa;
    pa.setColor(QPalette::WindowText, Qt::red);
    timeLabel->setPalette(pa);

//    progressSlider = new QSlider(Qt::Horizontal, this);
//    progressSlider->setMinimum(0);
//    //设置滑动条控件的最大值
//    progressSlider->setMaximum(999);
//    //设置滑动条控件的值
//    progressSlider->setValue(50);
//    pa.setColor(QPalette::Background, Qt::red);
//    progressSlider->setPalette(pa);
//    progressSlider->setGeometry(50, 440, 700, 5);

    curTimeLabel = new QLabel(QObject::tr("00:00"), this);
    curTimeLabel->setGeometry(240, 520, 100, 40);
    curTimeLabel->setFont(ft);
    pa.setColor(QPalette::WindowText, Qt::red);
    curTimeLabel->setPalette(pa);

    openButoon = new QPushButton(QObject::tr("打开视频"), this);
    openButoon->setGeometry(100, 520, 80, 40);
    openButoon->setObjectName("1");


    playButoon = new QPushButton(QObject::tr("播放"), this);
    playButoon->setObjectName("2");
    playButoon->setGeometry(520, 520, 80, 40);

    connect(openButoon, SIGNAL(clicked()), this, SLOT(handleButton()));
    connect(playButoon, SIGNAL(clicked()), this, SLOT(handleButton()));


    //设置背景
//    QPalette pal(pWidget->palette());
//    pal.setColor(QPalette::Background, Qt::white);
//    pWidget->setAutoFillBackground(false);
//    pWidget->setPalette(pal);
//    playButoon->show();
    threadId = startTimer(16); //fps
    VideoThread::Get()->start();
}


void VideoWidget::paintEvent(QPaintEvent *e) {
    static QImage *image = NULL;
    static int w, h;
    if (w != width() || h != height()) {
        if (image) {
            delete image->bits();
            delete image;
            image = NULL;
        }
    }

    if (image == NULL) {
        uchar *buf = new uchar[width() * height() * 4];
        image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
    }

    FFmpegUtils::Get()->toRgb((char *) image->bits(), width(), height()
    );
    QPainter painter;
    painter.begin(this);
    painter.drawImage(QPoint(0, 0), *image);
    painter.end();
}

void VideoWidget::timerEvent(QTimerEvent *e) {
    int min = FFmpegUtils::Get()->pts / 60;
    int sec = FFmpegUtils::Get()->pts % 60;
    char buf[1024] = {0};
    sprintf(buf, "%d:%d", min, sec);
    curTimeLabel->setText(buf);

//    if (FFmpegUtils::Get()->totalSec > 0) {
//        float rate = (float) FFmpegUtils::Get()->pts / (float) FFmpegUtils::Get()->totalSec;
//        progressSlider->setValue(rate * 1000);
//    }
    this->update();
}

//点击事件
void VideoWidget::handleButton() {
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    bool ok;
    int action = btn->objectName().toInt(&ok, 10);
    switch (action) {
        case 1: {
            QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
            if (fileName.isEmpty()) {
                return;
            }
            this->setWindowTitle(fileName);
            int totalTime = FFmpegUtils::Get()->openVideo(fileName.toLocal8Bit());
            if (totalTime < 1) {
                QMessageBox::information(this, "error", "file open failed");
                return;
            }
            VideoThread::Get()->isStop = false;
            FFmpegUtils::Get()->isPlaying = true;

            AudioPlay::Get()->sampleRate = FFmpegUtils::Get()->sampleRate;
//            AudioPlay::Get()->sampleSize = FFmpegUtils::Get()->sampleSize;
            AudioPlay::Get()->channel = FFmpegUtils::Get()->channel;

            AudioPlay::Get()->start();
            playButoon->setText("暂停");
            char buf[1024] = {0};
            int min = 0, sec = 0;
            sec = (totalTime) % 60;
            min = (totalTime) / 60;
            sprintf(buf, "%d:%d", min, sec);
            timeLabel->setText(buf);
            break;
        }
        case 2: {
            if (FFmpegUtils::Get()->isPlaying) {
                playButoon->setText("暂停");
            } else {
                playButoon->setText("播放");
            }
            FFmpegUtils::Get()->isPlaying = !FFmpegUtils::Get()->isPlaying;
            break;
        }

    }
}

void VideoWidget::closeEvent(QCloseEvent *event) {
    //响应主动关闭事件
//    killTimer(threadId);
    VideoThread::Get()->stop();
}


void VideoWidget::resizeEvent(QResizeEvent *e) {
    this->resize(size());
    //移动控件的位置
//    playButoon->move();
}




