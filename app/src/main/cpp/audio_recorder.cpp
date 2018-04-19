//
// Created by 冉高飞 on 2018/4/19.
//

#include "audio_recorder.h"
#include "stdlib.h"
#include "stdio.h"

const char *RIFF = "RIFF";
const char *WAVE = "WAVE";
const char *fmt = "fmt ";
const char *data = "data";
char *file_name = "sdcard/txt.wav";

#include <sys/stat.h>

int file_size2(char *filename) {
    struct stat statbuf;
    stat(filename, &statbuf);
    int size = statbuf.st_size;

    return size;
}

void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *rec) {
    (static_cast<AudioRecorder *>(rec))->ProcessSLCallback(bq);
}

void AudioRecorder::ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq) {
    SAKA_LOG_DEBUG("ProcessSLCallback");
    assert(bq == recBufQueueItf);
    sample_buf *dataBuf = NULL;
    devShadowQueue->front(&dataBuf);
    devShadowQueue->pop();
    dataBuf->size = dataBuf->cap;
//    recQueue->push(dataBuf);
    fwrite(dataBuf->buf, sizeof(char), dataBuf->size, file);
    freeQueue->push(dataBuf);

    sample_buf *freeBuf;
    while (freeQueue->front(&freeBuf) && devShadowQueue->push(freeBuf)) {
        freeQueue->pop();
        SLresult result = (*bq)->Enqueue(bq, freeBuf->buf, freeBuf->cap);
        SLASSERT(result);
    }
    ++audioBufCount;
    if (devShadowQueue->size() == 0) {
        SAKA_LOG_DEBUG("devShadowQueue.size=%d", 0);
        (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);
    }
}

AudioRecorder::AudioRecorder(SampleFormat *sampleFormat, SLEngineItf slEngine) :
        freeQueue(nullptr), recQueue(nullptr), devShadowQueue(nullptr), callback(nullptr) {
    file = fopen(file_name, "w+");
    writeHeader();
    SLresult result;
    sampleInfo = *sampleFormat;
    SLAndroidDataFormat_PCM_EX format_pcm;
    ConvertToSampleFormat(&format_pcm, &sampleInfo);

    SLDataLocator_IODevice loc_dev = {
            SL_DATALOCATOR_IODEVICE,
            SL_IODEVICE_AUDIOINPUT,
            SL_DEFAULTDEVICEID_AUDIOINPUT,
            NULL
    };
    SLDataSource audioSrc = {&loc_dev, NULL};

    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            DEVICE_SHADOW_BUFFER_QUEUE_LEN
    };

    SLDataSink audioSnk = {&loc_bq, &format_pcm};

    const SLInterfaceID id[2] = {
            SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            SL_IID_ANDROIDCONFIGURATION
    };

    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*slEngine)->CreateAudioRecorder(slEngine, &recObjectItf, &audioSrc, &audioSnk,
                                              sizeof(id) / sizeof(id[0]), id, req);
    SLASSERT(result);
    SAKA_LOG_DEBUG("CreateAudioRecorder result=%d", result);
    SLAndroidConfigurationItf inputConfig;
    result = (*recObjectItf)->GetInterface(recObjectItf, SL_IID_ANDROIDCONFIGURATION, &inputConfig);
    SAKA_LOG_DEBUG("GetInterface result=%d", result);
    if (SL_RESULT_SUCCESS == result) {
        SLuint32 presetValue = SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION;
        (*inputConfig)->SetConfiguration(inputConfig, SL_ANDROID_KEY_RECORDING_PRESET, &presetValue,
                                         sizeof(SLuint32));
    }

    result = (*recObjectItf)->Realize(recObjectItf, SL_BOOLEAN_FALSE);
    SLASSERT(result);
    SAKA_LOG_DEBUG("Realize result=%d", result);
    result = (*recObjectItf)->GetInterface(recObjectItf, SL_IID_RECORD, &recordItf);
    SLASSERT(result);

    result = (*recObjectItf)->GetInterface(recObjectItf, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                           &recBufQueueItf);
    SLASSERT(result);

    result = (*recBufQueueItf)->RegisterCallback(recBufQueueItf, bqRecorderCallback, this);
    SLASSERT(result);

    devShadowQueue = new AudioQueue(DEVICE_SHADOW_BUFFER_QUEUE_LEN);
    assert(devShadowQueue);
}

SLboolean AudioRecorder::Start(void) {
    if (!freeQueue || !recQueue || !devShadowQueue) {
        SAKA_LOG_ERROR("====NULL pointer to start(%p,%p,%p", freeQueue, recQueue, devShadowQueue);
        return SL_BOOLEAN_FALSE;
    }
    audioBufCount = 0;

    SLresult result;
    result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);

    result = (*recBufQueueItf)->Clear(recBufQueueItf);
    SLASSERT(result);

    for (int i = 0; i < RECORD_DEVICE_KICKSTART_BUF_COUNT; i++) {
        sample_buf *buf = NULL;
        if (!freeQueue->front(&buf)) {
            SAKA_LOG_ERROR("====OutOfFreeBuffers @ startingRecording @ (%d)", i);
            break;
        }
        freeQueue->pop();
        assert(buf->buf && buf->cap && !buf->size);

        result = (*recBufQueueItf)->Enqueue(recBufQueueItf, buf->buf, buf->cap);
        SLASSERT(result);
        devShadowQueue->push(buf);
    }
    SAKA_LOG_DEBUG("prepare to start recording!!!");
    result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_RECORDING);
    SLASSERT(result);
    return (result == SL_RESULT_SUCCESS ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE);
}

SLboolean AudioRecorder::Stop() {
    SLuint32 curState;
    fclose(file);
    SLresult result = (*recordItf)->GetRecordState(recordItf, &curState);
    SLASSERT(result);
    if (curState == SL_RECORDSTATE_STOPPED) {
        return SL_BOOLEAN_TRUE;
    }
    int size = file_size2(file_name);
    file = fopen(file_name, "w+");
    fseek(file, 8, SEEK_SET);
    fprintf(file, "%d", size - 8);
    fseek(file, 36, SEEK_SET);
    fprintf(file, "%d", size - 44);
    fclose(file);
    result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);
    result = (*recBufQueueItf)->Clear(recBufQueueItf);
    SLASSERT(result);

    return SL_BOOLEAN_TRUE;
}

AudioRecorder::~AudioRecorder() {
    if (recObjectItf != NULL) {
        (*recObjectItf)->Destroy(recObjectItf);
    }

    if (devShadowQueue) {
        sample_buf *buf = NULL;
        while (devShadowQueue->front(&buf)) {
            devShadowQueue->pop();
            freeQueue->push(buf);
        }
        delete (devShadowQueue);
    }
}

void AudioRecorder::SetBufQueues(AudioQueue *freeQ, AudioQueue *recQ) {
    assert(freeQ && recQ);
    freeQueue = freeQ;
    recQueue = recQ;
}

void AudioRecorder::RegisterCallback(ENGINE_CALLBACK cb, void *ctx) {
    callback = cb;
    ctx_ = ctx;
}

uint32_t AudioRecorder::dbgGetDevBufCount(void) {
    return (devShadowQueue->size());
}

int AudioRecorder::writeHeader() {
    fprintf(file, "%s", "RIFF");        //写入"RIFF"
    fprintf(file, "%d", 36);             //写入文件长度
    fprintf(file, "%s", "WAVE");        //写入"WAVE"
    fprintf(file, "%s", "fmt ");        //写入"fmt "
    fprintf(file, "%d", 16);            //写入"chunksize"
    fprintf(file, "%d", 0x00010001);     //写入编码格式(01)和音轨(01)
    fprintf(file, "%d", 441000);
    fprintf(file, "%d", 1 * 441000 * 16 / 8);
    fprintf(file, "%d", 0x0002000f);
    fprintf(file, "%s", data);
    fprintf(file, "%d", 0);
    return 0;
}

