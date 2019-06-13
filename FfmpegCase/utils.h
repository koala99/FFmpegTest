//
//

#ifndef VIDEODEMO_UTILS_H
#define VIDEODEMO_UTILS_H

void decodeVideo2YuvPcm(char *videoPath);

void encodeYuvPcm2Video(char *yuvPath, char *pcmPath);

void docedeVideo2H264Aac(char *filepath);

void encodeH264AAac2Video(char *h264Path, char *aacPath);


void encodePcm2Mp3(char *pcmPath);


#endif //VIDEODEMO_UTILS_H