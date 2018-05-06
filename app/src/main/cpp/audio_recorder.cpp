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
char *file_name = "sdcard/e.mp3";

#include <sys/stat.h>
#include "Lame.h"
#include "lameutils/lame_utils.h"

lame_utils *lameUtils;
int bufferSize;
unsigned char *mp3_buffer;
FILE *file;


void encode(char *pcm_data, int data_size) {
    int wroteSize = lameUtils->lame_encode((char *) pcm_data, data_size, mp3_buffer,
                                           bufferSize);
    fwrite(mp3_buffer, 1, (size_t) wroteSize, file);

}

void stop() {
//    lame_encode_flush(lameClient, mp3_buffer, );
//    lame_close(lameClient);
    lameUtils->lame_destroy();
    delete[] mp3_buffer;
}

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
    SAKA_LOG_DEBUG("dataBuf.size=%d", dataBuf->size);
//    recQueue->push(dataBuf);
    encode((char *) dataBuf->buf, dataBuf->size);
//    fwrite(dataBuf->buf, sizeof(char), dataBuf->size, file);
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

AudioRecorder::AudioRecorder(SampleFormat *sampleFormat, SLEngineItf slEngine, int buffer_size) :
        freeQueue(nullptr), recQueue(nullptr), devShadowQueue(nullptr), callback(nullptr) {
    file = fopen(file_name, "w+");
    assert(file);
    SLresult result;
    sampleInfo = *sampleFormat;
//    writeHeader();
    lameUtils = new lame_utils();
    LamePara *para = (LamePara *) malloc(sizeof(LamePara));
    para->quality = 2;
    para->mode = MONO;
    para->brate = sampleInfo.channels * sampleInfo.sampleRate * 16 / 8 / 1000;
    para->sample_rate = 44100;
    para->num_channels = 1;
    lameUtils->init_param(para);

    lameUtils->get_mp3_buffer_size(&bufferSize, buffer_size);
    SAKA_LOG_DEBUG("buffer_size=%d", buffer_size);
    mp3_buffer = new unsigned char[buffer_size];

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

    stop();
    SLresult result = (*recordItf)->GetRecordState(recordItf, &curState);
    SLASSERT(result);
    if (curState == SL_RECORDSTATE_STOPPED) {
        return SL_BOOLEAN_TRUE;
    }
    int32_t size = file_size2(file_name);
    SAKA_LOG_DEBUG("the file size is %d", size);
//    file = fopen(file_name, "r+");
//    fseek(file, 4, SEEK_SET);
//    int32_t chunkSize = size - 8;
//    fwrite(&chunkSize, sizeof(int32_t), 1, file);
//    fseek(file, 40, SEEK_SET);
//    int32_t dataSize = size - 44;
//    fwrite(&dataSize, sizeof(uint32_t), 1, file);
//    fclose(file);
    result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);
    result = (*recBufQueueItf)->Clear(recBufQueueItf);
    SLASSERT(result);
    fclose(file);
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
    fwrite(RIFF, sizeof(char), 4, file);      //写入"RIFF"
    uint32_t s = 36;
    fwrite(&s, sizeof(s), 1, file);             //写入文件长度
    fwrite(WAVE, sizeof(char), 4, file);         //写入"WAVE"
    fwrite(fmt, sizeof(char), 4, file);         //写入"fmt "
    s = 16;            //写入"chunksize"
    fwrite(&s, sizeof(s), 1, file);
    int16_t pcm = 1;
    fwrite(&pcm, sizeof(int16_t), 1, file);
    fwrite(&(sampleInfo.channels), sizeof(int16_t), 1, file);
    s = 44100;
    fwrite(&s, sizeof(s), 1, file);
    s = 1 * 441000 * 16 / 8;
    fwrite(&s, sizeof(s), 1, file);
    pcm = 2;
    fwrite(&pcm, sizeof(int16_t), 1, file);
    pcm = 16;
    fwrite(&pcm, sizeof(int16_t), 1, file);
    fwrite(data, sizeof(char), 4, file);
    s = 0;
    fwrite(&s, sizeof(s), 1, file);
    return 0;
}

