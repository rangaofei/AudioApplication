//
// Created by 冉高飞 on 2018/4/17.
//

#ifndef AUDIOAPPLICATION_OPENSL_UTIL_H_H
#define AUDIOAPPLICATION_OPENSL_UTIL_H_H

#include <SLES/OpenSLES.h>
#include <sys/types.h>
#include "buf_manager.h"
#include "audio_recorder.h"

struct AudioEngine {
    SLmilliHertz fastPathSampleRate;
    uint32_t fastPathFramesPerBuf;
    uint16_t sampleChannels;
    uint16_t bitsPerSample;

    SLObjectItf slEngineObj;
    SLEngineItf slEngineItf;

    AudioRecorder *recorder;
    AudioQueue *freeBufQueue;
    AudioQueue *recBufQueue;

    sample_buf *bufs;
    uint32_t bufCount;
    uint32_t frameCount;

};

bool EngineService(void *ctx, uint32_t msg, void *data);
#endif //AUDIOAPPLICATION_OPENSL_UTIL_H_H
