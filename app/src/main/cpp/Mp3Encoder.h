//
// Created by 冉高飞 on 2018/4/24.
//

#ifndef AUDIOAPPLICATION_MP3ENCODER_H
#define AUDIOAPPLICATION_MP3ENCODER_H


#include <stdio.h>
#include "include/lame.h"

class Mp3Encoder {
private:
    FILE * pcmFile;
    FILE * mp3File;
    lame_t lameClient;

public:
    Mp3Encoder();
    ~Mp3Encoder();
    int Init(const char * pcmFilePath, const char *mp3FilePath,int sampleRate,int channels,int bitrate);
    void Encode();
    void Destroy();
};


#endif //AUDIOAPPLICATION_MP3ENCODER_H
