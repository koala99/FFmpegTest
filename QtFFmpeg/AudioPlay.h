#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>
#include <QThread>
#include <QMutex>
#include <QAudioFormat> //QtMultimedia

#ifndef QTFFMPEG_QTAUDIOPLAY_H
#define QTFFMPEG_QTAUDIOPLAY_H
using namespace std;

class AudioPlay {

public:
    static AudioPlay *Get();

    QAudioOutput *out;
    QIODevice *ad;
    int sampleRate = 48000;
    int sampleSize = 16;
    int channel = 2;
    QMutex mutex;

    bool start();

    void stop();

    int getFree();

    bool play(bool isPlay);

    bool writeData(const char *data, int datasize);

    AudioPlay();
};

#endif //QTFFMPEG_QTAUDIOPLAY_H