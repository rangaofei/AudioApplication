//
// Created by saka on 2018/9/26.
//

#ifndef AUDIOAPPLICATION_OPENSLPLAYER_H
#define AUDIOAPPLICATION_OPENSLPLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

SLresult res;

const SLInterfaceID ids[] = {SL_IID_VOLUME};
const SLboolean req[] = {SL_BOOLEAN_FALSE};
const SLInterfaceID outputInterfaces[1] = {SL_IID_BUFFERQUEUE};
const SLboolean requireds[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

typedef struct PlayerContext {
    SLDataFormat_PCM outputFormat;
    SLObjectItf slEngineObj;
    SLEngineItf slEngineItf;
    SLObjectItf outputMixItf;

    SLObjectItf audioPlayerObj;
    SLAndroidSimpleBufferQueueItf outputBufferQueueItf;
    SLPlayItf playItf;

    SLObjectItf audioRecorderObj;
    SLAndroidSimpleBufferQueueItf inputBufferQueueItf;
    SLRecordItf recordItf;
} PlayerContext;

void checkErr(SLresult result, const char *tag) {
    if (result != SL_RESULT_SUCCESS) {
        std::string message = nullptr;
        switch (result) {
            case SL_RESULT_PRECONDITIONS_VIOLATED:
                message = "SL_RESULT_PRECONDITIONS_VIOLATED";
                break;
            case SL_RESULT_PARAMETER_INVALID:
                message = "SL_RESULT_PARAMETER_INVALID";
                break;
            case SL_RESULT_MEMORY_FAILURE:
                message = "SL_RESULT_MEMORY_FAILURE";
                break;
            case SL_RESULT_IO_ERROR:
                message = "SL_RESULT_IO_ERROR";
                break;
            case SL_RESULT_CONTENT_CORRUPTED:
                message = "SL_RESULT_CONTENT_CORRUPTED";
                break;
            case SL_RESULT_CONTENT_UNSUPPORTED:
                message = "SL_RESULT_CONTENT_UNSUPPORTED";
                break;
            case SL_RESULT_CONTENT_NOT_FOUND:
                message = "SL_RESULT_CONTENT_NOT_FOUND";
                break;
            case SL_RESULT_PERMISSION_DENIED:
                message = "SL_RESULT_PERMISSION_DENIED";
                break;
            default:
                message = "unknown";
                break;
        }
        SAKA_LOG_ERROR("Error ===> %s: +++ %s :%d", tag, message.data(), result);
    }
}


#endif //AUDIOAPPLICATION_OPENSLPLAYER_H
