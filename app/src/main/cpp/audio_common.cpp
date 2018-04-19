//
// Created by 冉高飞 on 2018/4/17.
//

#include <cassert>
#include <cstring>
#include "audio_common.h"

void ConvertToSampleFormat(SLAndroidDataFormat_PCM_EX *pFormat, SampleFormat *pSampleInfo) {
    assert(pFormat);
    memset(pFormat, 0, sizeof(*pFormat));
    pFormat->formatType = SL_DATAFORMAT_PCM;
    if (pSampleInfo->channels <= 1) {
        pFormat->numChannels = 1;
        pFormat->channelMask = SL_SPEAKER_FRONT_CENTER;
    } else {
        pFormat->numChannels = 2;
        pFormat->channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_BACK_RIGHT;
    }

    pFormat->sampleRate = pSampleInfo->sampleRate;
    pFormat->endianness = SL_BYTEORDER_LITTLEENDIAN;
    pFormat->bitsPerSample = pSampleInfo->pcmFormat;
    pFormat->containerSize = pSampleInfo->pcmFormat;

    pFormat->representation = pSampleInfo->representation;
    switch (pFormat->representation) {
        case SL_ANDROID_PCM_REPRESENTATION_UNSIGNED_INT:
            pFormat->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_8;
            pFormat->containerSize = SL_PCMSAMPLEFORMAT_FIXED_8;
            pFormat->formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
            break;
        case SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT:
            pFormat->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
            pFormat->containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
            pFormat->formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
            break;
        case SL_ANDROID_PCM_REPRESENTATION_FLOAT:
            pFormat->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_32;
            pFormat->containerSize = SL_PCMSAMPLEFORMAT_FIXED_32;
            pFormat->formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
            break;
        case 0:
            break;
        default:
            assert(0);
    }
}