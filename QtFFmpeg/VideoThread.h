#include <QThread>

class VideoThread : public QThread {

public:
    bool isStop = false;

    static VideoThread *Get() {
        static VideoThread vt;
        return &vt;
    }

    void run();

    void stop();

    VideoThread();
};