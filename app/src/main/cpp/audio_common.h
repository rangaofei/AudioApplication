//
// Created by 冉高飞 on 2018/4/17.
//

#ifndef AUDIOAPPLICATION_AUDIO_COMMON_H
#define AUDIOAPPLICATION_AUDIO_COMMON_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#define RECORD_DEVICE_KICKSTART_BUF_COUNT 2
#define DEVICE_SHADOW_BUFFER_QUEUE_LEN 4
struct SampleFormat {
    uint32_t sampleRate;
    uint32_t framesPerBuf;
    uint16_t channels;
    uint16_t pcmFormat;
    uint32_t representation;
};


extern void ConvertToSampleFormat(SLAndroidDataFormat_PCM_EX *pFormat, SampleFormat *format);

#define SLASSERT(x)     do {\
assert(SL_RESULT_SUCCESS==(x));\
(void)(x);\
}while(0)

#define ENGINE_SERVICE_MSG_RETRIVE_DUMP_BUFS 2
typedef bool (*ENGINE_CALLBACK)(void *pCTX, uint32_t msg, void *pData);

#endif //AUDIOAPPLICATION_AUDIO_COMMON_H
