//
// Created by 冉高飞 on 2018/4/19.
//

#ifndef AUDIOAPPLICATION_AUDIO_RECORDER_H
#define AUDIOAPPLICATION_AUDIO_RECORDER_H

#include <sys/types.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "audio_common.h"
#include "buf_manager.h"

class AudioRecorder {
    SLObjectItf recObjectItf;
    SLRecordItf recordItf;
    SLAndroidSimpleBufferQueueItf recBufQueueItf;

    SampleFormat sampleInfo;
    AudioQueue *freeQueue;
    AudioQueue *recQueue;
    AudioQueue *devShadowQueue;
    uint32_t audioBufCount;

    ENGINE_CALLBACK callback;
    void *ctx_;
    FILE *file;

public:
    explicit AudioRecorder(SampleFormat *, SLEngineItf engineItf);

    ~AudioRecorder();

    SLboolean Start(void);

    SLboolean Stop(void);

    void SetBufQueues(AudioQueue *freeQ, AudioQueue *recQ);

    void ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq);

    void RegisterCallback(ENGINE_CALLBACK cb, void *ctx);

    uint32_t dbgGetDevBufCount(void);

    int writeHeader();
};


#endif //AUDIOAPPLICATION_AUDIO_RECORDER_H
