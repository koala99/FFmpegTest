#include "AudioPlay.h"
#include <QAudioOutput>
#include <QThread>
#include <QMutex>

bool AudioPlay::start() {
    stop();
    mutex.lock();
    QAudioFormat fmt;
    fmt.setSampleRate(this->sampleRate);
    fmt.setSampleSize(this->sampleSize);
    fmt.setChannelCount(this->channel); //左右声道
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(fmt)) {
        printf("default format not supported try to use nearest");
    }
    out = new QAudioOutput(fmt);
    ad = out->start();
    mutex.unlock();
    return true;
}

bool AudioPlay::play(bool isPlay) {
    mutex.lock();
    if (!out) {
        mutex.unlock();
        return false;

    }
    if (isPlay) {
        out->resume();
    } else {
        out->suspend();
    }
    mutex.unlock();
    return true;
}

void AudioPlay::stop() {
    mutex.lock();
    if (out) {
        out->stop();
        delete out;
        ad = NULL;
    }
    mutex.unlock();
}

bool AudioPlay::writeData(const char *data, int datasize) {
    if (!data || datasize <= 0)
        return false;
    mutex.lock();
    if (ad) {
        mutex.unlock();
        return false;
    }
    ad->write(data, datasize);
    mutex.unlock();
    return true;
}

int AudioPlay::getFree() {
    mutex.lock();
    if (!out) {
        mutex.unlock();
        return 0;
    }
    mutex.unlock();
    return out->bytesFree();
}


AudioPlay::AudioPlay() {

}

AudioPlay *AudioPlay::Get() {
    static AudioPlay ap;
    return &ap;
}


