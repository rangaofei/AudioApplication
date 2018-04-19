#include <jni.h>
#include <cassert>
#include "audio_common.h"
#include <cstring>
#include "opensl_util.h"
#include "saka_log.h"

static AudioEngine audioEngine;
extern "C"
JNIEXPORT void JNICALL
Java_com_rangaofei_audioapplication_com_saka_ndk_OpenSLUtil_createSLEngine(JNIEnv *env, jclass type,
                                                                           jint rate,
                                                                           jint framesPerBuf,
                                                                           jint channels) {
    SAKA_LOG_DEBUG("the rate is %d,the framePerBuffer is %d", rate, framesPerBuf);
    SLresult result;
    memset(&audioEngine, 0, sizeof(audioEngine));

    audioEngine.fastPathSampleRate = static_cast<SLmilliHertz >(rate) * 1000;
    audioEngine.fastPathFramesPerBuf = static_cast<uint32_t >(framesPerBuf);
    audioEngine.sampleChannels = static_cast<uint16_t >(channels);
    audioEngine.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;

    result = slCreateEngine(&audioEngine.slEngineObj, 0, NULL, 0, NULL, NULL);
    SLASSERT(result);

    result = (*audioEngine.slEngineObj)->Realize(audioEngine.slEngineObj, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    result = (*audioEngine.slEngineObj)->GetInterface(audioEngine.slEngineObj, SL_IID_ENGINE,
                                                      &audioEngine.slEngineItf);
    SLASSERT(result);

    uint32_t bufSize = audioEngine.fastPathFramesPerBuf *
                       audioEngine.sampleChannels * audioEngine.bitsPerSample;
    bufSize = (bufSize + 7) >> 3;
    audioEngine.bufCount = 16;
    SAKA_LOG_DEBUG("bufferSize=%ld", bufSize);
    audioEngine.bufs = allocateSampleBUfs(audioEngine.bufCount, bufSize);
    assert(audioEngine.bufs);

    audioEngine.freeBufQueue = new AudioQueue(audioEngine.bufCount);
    audioEngine.recBufQueue = new AudioQueue(audioEngine.bufCount);

    for (uint32_t i = 0; i < audioEngine.bufCount; i++) {
        audioEngine.freeBufQueue->push(&audioEngine.bufs[i]);
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_rangaofei_audioapplication_com_saka_ndk_OpenSLUtil_createAudioRecorder(JNIEnv *env,
                                                                                jclass type) {

    SampleFormat sampleFormat;
    memset(&sampleFormat, 0, sizeof(sampleFormat));
    sampleFormat.pcmFormat = static_cast<uint16_t >(audioEngine.bitsPerSample);
    sampleFormat.channels = audioEngine.sampleChannels;
    sampleFormat.sampleRate = audioEngine.fastPathSampleRate;
    sampleFormat.framesPerBuf = audioEngine.fastPathFramesPerBuf;
    audioEngine.recorder = new AudioRecorder(&sampleFormat, audioEngine.slEngineItf);

    if (!audioEngine.recorder) {
        return JNI_FALSE;
    }

    audioEngine.recorder->SetBufQueues(audioEngine.freeBufQueue, audioEngine.recBufQueue);
    audioEngine.recorder->RegisterCallback(EngineService, (void *) &audioEngine);

    return JNI_TRUE;

}extern "C"
JNIEXPORT void JNICALL
Java_com_rangaofei_audioapplication_com_saka_ndk_OpenSLUtil_deleteAudioRecorder(JNIEnv *env,
                                                                                jclass type) {

    if (audioEngine.recorder) {
        delete audioEngine.recorder;
    }
    audioEngine.recorder = nullptr;

}extern "C"
JNIEXPORT void JNICALL
Java_com_rangaofei_audioapplication_com_saka_ndk_OpenSLUtil_startPlay(JNIEnv *env, jclass type) {

    audioEngine.frameCount = 0;

    audioEngine.recorder->Start();

}extern "C"
JNIEXPORT void JNICALL
Java_com_rangaofei_audioapplication_com_saka_ndk_OpenSLUtil_stopPlay(JNIEnv *env, jclass type) {

    audioEngine.recorder->Stop();
    delete audioEngine.recorder;
    audioEngine.recorder = nullptr;
}

uint32_t dbgEngineGetBufCount(void) {
    uint32_t count = audioEngine.recorder->dbgGetDevBufCount();
    count += audioEngine.freeBufQueue->size();
    count += audioEngine.recBufQueue->size();

    SAKA_LOG_ERROR("Buf Distributions: RecDev=%d,FreeQ=%d,RecQ=%d",
                   audioEngine.recorder->dbgGetDevBufCount(),
                   audioEngine.freeBufQueue->size(),
                   audioEngine.recBufQueue->size());

    if (count != audioEngine.bufCount) {
        SAKA_LOG_ERROR("====Lost Bufs among the queue (supposed = %d ,found = %d)", 16,
                       count);
    }
    return count;
}

bool EngineService(void *ctx, uint32_t msg, void *data) {
    assert(ctx = &audioEngine);
    switch (msg) {
        case ENGINE_SERVICE_MSG_RETRIVE_DUMP_BUFS:
            *(static_cast<uint32_t *>(data)) = dbgEngineGetBufCount();
            break;
        default:
            assert(false);
            return false;
    }
}