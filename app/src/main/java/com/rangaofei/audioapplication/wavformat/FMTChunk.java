package com.rangaofei.audioapplication.wavformat;

import com.rangaofei.audioapplication.utils.ByteBufferUtil;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by rangaofei on 2018/4/20.
 */

public class FMTChunk {
    private static final int DEFAULT_CHUNK_SIZE = 16;
    private static final short AUDIO_PCM_FORMAT = 1;
    private static final short BIT_WIDTH_8 = 8;
    private static final short BIT_WIDTH_16 = 16;
    private static final short BIT_WIDHT_32 = 32;
    private static final String FMT = "fmt ";

    public static void writeFMTChunk(OutputStream outputStream, int chunkSize, short audioFormat,
                                     short numChannels, int sampleRate, short bitPerSample) throws IOException {
        if (outputStream == null) {
            throw new RuntimeException("outputStream is null");
        }
        outputStream.write(FMT.getBytes());
        outputStream.write(ByteBufferUtil.intToLittleByte(chunkSize));
        outputStream.write(ByteBufferUtil.shortToLittleByte(audioFormat));
        outputStream.write(ByteBufferUtil.shortToLittleByte(numChannels));
        outputStream.write(ByteBufferUtil.intToLittleByte(sampleRate));
        outputStream.write(ByteBufferUtil.intToLittleByte(generateByteRate(
                bitPerSample,numChannels,sampleRate
        )));
        outputStream.write(ByteBufferUtil.shortToLittleByte(generateBlockAlign(
                bitPerSample,numChannels
        )));
        outputStream.write(ByteBufferUtil.shortToLittleByte(bitPerSample));
    }

    public static void writeFMTChunkWithDefaultSize(OutputStream outputStream, short numChannels,
                                                     int sampleRate) throws IOException {
        writeFMTChunk(outputStream, DEFAULT_CHUNK_SIZE, AUDIO_PCM_FORMAT, numChannels, sampleRate, BIT_WIDTH_16);
    }

    private static short generateBlockAlign(short bitPerSample,short numChannels) {
        return (short) (bitPerSample * numChannels / 8);
    }

    private static int generateByteRate(short bitPerSample,short numChannels,int sampleRate) {
        return sampleRate * bitPerSample * numChannels / 8;
    }
}
