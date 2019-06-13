

#include <chrono>
#include "base_utils.h"


time_t BaseUtils::getTimeStamp() {
    chrono::time_point <chrono::system_clock, chrono::milliseconds> tp = chrono::time_point_cast<chrono::milliseconds>(
            chrono::system_clock::now());
    auto tmp = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch());
    time_t timestamp = tmp.count();
    return timestamp;
}