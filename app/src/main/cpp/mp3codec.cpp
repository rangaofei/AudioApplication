//
// Created by 冉高飞 on 2018/4/24.
//
#include <jni.h>
#include "Mp3Encoder.h"
#include "saka_log.h"

Mp3Encoder *encoder;
extern "C"
JNIEXPORT jint JNICALL
Java_com_saka_ndk_Mp3Encoder_init(JNIEnv *env, jclass type, jstring pcmFilePath_,
                                  jint audioChannels, jint bitRate, jint sampleRate,
                                  jstring mp3FilePath_) {
    const char *pcmFilePath = env->GetStringUTFChars(pcmFilePath_, 0);
    const char *mp3FilePath = env->GetStringUTFChars(mp3FilePath_, 0);
    SAKA_LOG_DEBUG("pcm=%s", pcmFilePath);
    SAKA_LOG_DEBUG("mp3=%s", pcmFilePath);
    encoder = new Mp3Encoder();
    encoder->Init(pcmFilePath, mp3FilePath, sampleRate, audioChannels, bitRate);
    env->ReleaseStringUTFChars(pcmFilePath_, pcmFilePath);
    env->ReleaseStringUTFChars(mp3FilePath_, mp3FilePath);
    return 1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_saka_ndk_Mp3Encoder_encode(JNIEnv *env, jclass type) {

    encoder->Encode();

}extern "C"
JNIEXPORT void JNICALL
Java_com_saka_ndk_Mp3Encoder_destroy(JNIEnv *env, jclass type) {

    encoder->Destroy();

}