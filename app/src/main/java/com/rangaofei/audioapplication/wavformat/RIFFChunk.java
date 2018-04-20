package com.rangaofei.audioapplication.wavformat;

import com.rangaofei.audioapplication.utils.ByteBufferUtil;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by rangaofei on 2018/4/20.
 */

public class RIFFChunk {
    private static final String RIFF = "RIFF";
    private int chunkSize;
    private static final String WAVE = "WAVE";

    public static void writeRIFFCHunk(OutputStream outputStream, int chunkSize) throws IOException {
        if (outputStream == null) {
            throw new RuntimeException("outputStream is null");
        }
        outputStream.write(RIFF.getBytes());
        outputStream.write(ByteBufferUtil.intToLittleByte(chunkSize));
        outputStream.write(WAVE.getBytes());
    }
}
