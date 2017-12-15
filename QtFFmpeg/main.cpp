//#include <iostream>

#include "VideoWidget.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qapplication.h>
#include "FFmpegUtils.h"


int decodeVideo();

static double r2d(AVRational avRational) {
    return avRational.num == 0 ? 0 : (double) avRational.num / (double) avRational.den;

}

int main(int argc, char *argv[]) {
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
    VideoWidget w;
    w.show();
    return app->exec();

}
