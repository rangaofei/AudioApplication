//
// Created by saka on 2018/9/26.
//
#include <jni.h>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include "saka_log.h"
#include "OpenSLPlayer.h"
#include "WavTools.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define JNI_REG_CLASS "com/saka/ndk/OpenSLPlayer"

PlayerContext *context;
AAsset *asset;
char *buffer;


/**
 * 打开资源文件
 * @param env
 * @param assetManager
 */
static void openAssetFile(JNIEnv *env, jobject assetManager) {
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) { SAKA_LOG_ERROR("AAssetManager is NULL"); }
    asset = AAssetManager_open(mgr, "test.wav", AASSET_MODE_UNKNOWN);
    //根据文件名打开资源文件
    if (NULL == asset) {
        SAKA_LOG_ERROR("未找到文件");
        return;
    }

}

static void getFileFormat(SLDataFormat_PCM *outputFormat) {
    off_t p = AAsset_seek(asset, 0, 0);
    uint32_t chunk_size;
    FmtChunk fmtChunk;
    if (p != 0) {
        SAKA_LOG_ERROR("can not set the position to beginning");
        return;
    }
    long size = AAsset_getLength(asset);
    if (size <= 44) {
        SAKA_LOG_ERROR("the file is not a wav file");
        return;
    }
    char tmp_header[5];
    memset(tmp_header, 0, 5);
    p = p + AAsset_read(asset, tmp_header, 4);
    if (strcmp(tmp_header, RIFF) != 0) {
        SAKA_LOG_ERROR("not wav file");
        return;
    }
    p = AAsset_seek(asset, 8 + p, 0);
    while (true) {
        p += AAsset_read(asset, tmp_header, 4);
        if (strcmp(tmp_header, DATA) == 0) {
            AAsset_read(asset, &chunk_size, 4);
            break;
        }
        if (strcmp(tmp_header, FMT) != 0) {
            p += AAsset_read(asset, &chunk_size, sizeof(uint32_t));
            p = AAsset_seek(asset, chunk_size + p, 0);
            continue;
        } else {
            p = AAsset_seek(asset, p - 4, 0);
            AAsset_read(asset, &fmtChunk, sizeof(FmtChunk));
            p = p + fmtChunk.chunk_size + 8;
            p = AAsset_seek(asset, p, 0);
        }

    }
    outputFormat->formatType = SL_DATAFORMAT_PCM;
    outputFormat->numChannels = fmtChunk.num_channels;
    outputFormat->samplesPerSec = fmtChunk.sample_rate * 1000;
    outputFormat->bitsPerSample = fmtChunk.bit_per_sample;
    outputFormat->containerSize = fmtChunk.bit_per_sample;
    outputFormat->channelMask =
            fmtChunk.num_channels == 1 ? SL_SPEAKER_FRONT_CENTER : SL_SPEAKER_FRONT_LEFT |
                                                                   SL_SPEAKER_FRONT_RIGHT;
    outputFormat->endianness = SL_BYTEORDER_LITTLEENDIAN;
}

/**
 * 开始播放
 */
static void jniStartPlay() {
    SLuint32 playState = 0;
    res = (*context->playItf)->GetPlayState(context->playItf, &playState);
    checkErr(res, "getPlayState");
    if (playState == SL_PLAYSTATE_PLAYING) {
        SAKA_LOG_ERROR("already playing");
        return;
    }
    res = (*context->playItf)->SetPlayState(context->playItf, SL_PLAYSTATE_PLAYING);
    checkErr(res, "SetPlayState");
    AAsset_read(asset, buffer, 1024 * 10);
    res = (*context->outputBufferQueueItf)->Enqueue(context->outputBufferQueueItf, buffer, 1);
    checkErr(res, "enqueue");
    res = (*context->playItf)->GetPlayState(context->playItf, &playState);
    checkErr(res, "getPlayingState");
    if (playState != SL_PLAYSTATE_PLAYING) {
        SAKA_LOG_ERROR("play failed");
        return;
    }
}

/**
 * 停止播放
 */
static void jniStopPlay() {
    SLuint32 playState = 0;
    res = (*context->playItf)->GetPlayState(context->playItf, &playState);
    checkErr(res, "getPlayState");
    if (playState != SL_PLAYSTATE_STOPPED) {
        res = (*context->playItf)->SetPlayState(context->playItf, SL_PLAYSTATE_STOPPED);
        checkErr(res, "setPlayStopped");
    }
}

static void jniStopRecord(){
    SLuint32 playState = 0;
    res = (*context->recordItf)->GetRecordState(context->recordItf, &playState);
    checkErr(res, "getRecordState");
    if (playState != SL_RECORDSTATE_STOPPED) {
        res = (*context->recordItf)->SetRecordState(context->recordItf, SL_PLAYSTATE_STOPPED);
        checkErr(res, "setRecordStopped");
    }
}

/**
 * 销毁资源
 */
static void jniDestroyPlay() {

    if (asset != nullptr) {
        AAsset_close(asset);
        asset = nullptr;
    }
    if (context == nullptr) {
        return;
    }

    (*context->outputBufferQueueItf)->Clear(context->outputBufferQueueItf);

    (*context->audioPlayerObj)->Destroy(context->audioPlayerObj);
    context->audioPlayerObj = NULL;
    context->outputBufferQueueItf = NULL;
    context->playItf = NULL;
    (*context->outputMixItf)->Destroy(context->outputMixItf);
    (*context->slEngineObj)->Destroy(context->slEngineObj);
    SAKA_LOG_ERROR("销毁资源");
    delete context;
    context = nullptr;
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }
}

/**
 * 播放时调用的函数
 * @param bf
 * @param context
 */
static void outputQueueCallback(SLAndroidSimpleBufferQueueItf bf, void *c) {
    SAKA_LOG_ERROR("开始播放要获取数据了");
    int length = AAsset_read(asset, buffer, 1024 * 10);
    if (length > 0) {
        (*bf)->Enqueue(bf, buffer, length);
    } else {
        jniStopPlay();
    }
}

static void inputQueueCallback(SLAndroidSimpleBufferQueueItf bf, void *c) {

}

static void jniSetPlayer(SLDataFormat_PCM *outputFormat) {
    //创建音频输出设备
    res = (*context->slEngineItf)->CreateOutputMix(context->slEngineItf, &context->outputMixItf, 0,
                                                   ids, req);
    checkErr(res, "createOutputMix");
    //初始化音频输出设备
    res = (*context->outputMixItf)->Realize(context->outputMixItf, SL_BOOLEAN_FALSE);
    checkErr(res, "outputMix realize");

    //设置输出队列的类型和队列中元素的个数
    SLDataLocator_AndroidSimpleBufferQueue outputLocator = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            4};

    //设置声音源
    SLDataSource outputSource = {&outputLocator, outputFormat};
    SLDataLocator_OutputMix outputMixLocator = {SL_DATALOCATOR_OUTPUTMIX, context->outputMixItf};
    SLDataSink outputSink = {&outputMixLocator, nullptr};

    //创建播放器
    res = (*context->slEngineItf)->CreateAudioPlayer(context->slEngineItf, &context->audioPlayerObj,
                                                     &outputSource, &outputSink, 1,
                                                     outputInterfaces,
                                                     requireds);
    checkErr(res, "CreateAudioPlayer");
    //初始化播放器
    res = (*context->audioPlayerObj)->Realize(context->audioPlayerObj, SL_BOOLEAN_FALSE);
    checkErr(res, "audioPlayer realize");
    //获取播放队列的控制接口
    res = (*context->audioPlayerObj)->GetInterface(context->audioPlayerObj,
                                                   SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                   &context->outputBufferQueueItf);
    checkErr(res, "getInterface outputBufferQueue");

    //获取播放器接口
    res = (*context->audioPlayerObj)->GetInterface(context->audioPlayerObj, SL_IID_PLAY,
                                                   &context->playItf);
    checkErr(res, "getInterface playItf");

    //播放队列注册回调函数，在播放完成当前队列后会自动播放调用该回调函数
    res = (*context->outputBufferQueueItf)->RegisterCallback(context->outputBufferQueueItf,
                                                             outputQueueCallback, (void *) 1);
    checkErr(res, "RegisterCallback");

}


static void JNICALL createEngine(JNIEnv *env, jobject jobj) {
    buffer = new char[1024 * 1024];
    //初始化上下文
    context = static_cast<PlayerContext *>(calloc(1, sizeof(PlayerContext)));
    //创建引擎
    res = slCreateEngine(&context->slEngineObj, 0, nullptr, 0, nullptr, nullptr);
    checkErr(res, "slCreateEngine");
    //初始化引擎
    res = (*context->slEngineObj)->Realize(context->slEngineObj, SL_BOOLEAN_FALSE);
    checkErr(res, "slEngineObj realize");
    //获取引擎借口
    res = (*context->slEngineObj)->GetInterface(context->slEngineObj, SL_IID_ENGINE,
                                                &context->slEngineItf);
    checkErr(res, "getInterface");
}

/**
 * 设置播放器参数
 * @param env
 * @param jobj
 */
static void JNICALL setPlayer(JNIEnv *env, jobject jobj, jobject assetManager) {
    openAssetFile(env, assetManager);
    getFileFormat(&context->outputFormat);
    jniSetPlayer(&context->outputFormat);
}

static void JNICALL setRecorder(JNIEnv *env, jobject jobj) {
    SLDataLocator_IODevice deviceInputLocator = {SL_DATALOCATOR_ADDRESS, SL_IODEVICE_AUDIOINPUT,
                                                 SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr};
    SLDataSource inputSource = {&deviceInputLocator, nullptr};
    SLDataLocator_AndroidSimpleBufferQueue inputLocator = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                           1};
    SLDataFormat_PCM inputFormat = {SL_DATAFORMAT_PCM,
                                    1,
                                    44100 * 1000,
                                    SL_PCMSAMPLEFORMAT_FIXED_16,
                                    SL_SPEAKER_FRONT_CENTER,
                                    SL_BYTEORDER_LITTLEENDIAN};
    SLDataSink inputSink = {&inputLocator, &inputFormat};
    const SLInterfaceID inputInterfaces[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                              SL_IID_ANDROIDCONFIGURATION};
    const SLboolean requireds[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};
    res = (*context->slEngineItf)->CreateAudioRecorder(context->slEngineItf,
                                                       &context->audioRecorderObj, &inputSource,
                                                       &inputSink, 2, inputInterfaces, requireds);
    checkErr(res, "CreateAudioRecorder");

    res = (*context->audioRecorderObj)->Realize(context->audioRecorderObj, SL_BOOLEAN_FALSE);
    checkErr(res, "audioRecorder realize");

    res = (*context->audioRecorderObj)->GetInterface(context->audioRecorderObj,
                                                     SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                     &context->inputBufferQueueItf);
    checkErr(res, "get input buffer queue");

    res = (*context->audioRecorderObj)->GetInterface(context->audioRecorderObj, SL_IID_RECORD,
                                                     &context->recordItf);
    checkErr(res, "get record interface");

    res = (*context->inputBufferQueueItf)->RegisterCallback(context->inputBufferQueueItf,
                                                            inputQueueCallback, (void *) 1);
    checkErr(res, "input buffer queue register callback");
}

/**
 * 开始播放
 * @param env
 * @param jobj
 */
static void JNICALL startPlay(JNIEnv *env, jobject jobj) {
    jniStartPlay();
    while (true) {
        SLuint32 playState = 0;
        res = (*context->playItf)->GetPlayState(context->playItf, &playState);
        checkErr(res, "getPlayState");
        if (playState == SL_PLAYSTATE_STOPPED) {
            jniDestroyPlay();
            break;
        }
    }

}

static void JNICALL stopPlay(JNIEnv *env, jobject jobj) {
    jniStopPlay();
    jniDestroyPlay();
}


static JNINativeMethod nativeMethod[] = {
        {"createEngine", "()V",                                   (void *) createEngine},
        {"setPlayer",    "(Landroid/content/res/AssetManager;)V", (void *) setPlayer},
        {"startPlay",    "()V",                                   (void *) startPlay},
        {"stopPlay",     "()V",                                   (void *) stopPlay},
};


/** 为某一个类注册本地方法*/
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods,
                                 int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) { return JNI_FALSE; }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) { return JNI_FALSE; }
    return JNI_TRUE;
}

/** 为所有类注册本地方法*/
static int registerNatives(JNIEnv *env) {
    return registerNativeMethods(env, JNI_REG_CLASS, nativeMethod,
                                 sizeof(nativeMethod) / sizeof(nativeMethod[0]));

}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) { return -1; }
    assert(env != NULL);
    if (!registerNatives(env)) {//注册
        SAKA_LOG_DEBUG("JNI_OnLoad failed");
        return -1;
    }
    //成功
    SAKA_LOG_DEBUG("JNI_OnLoad success");
    return JNI_VERSION_1_4;
}


