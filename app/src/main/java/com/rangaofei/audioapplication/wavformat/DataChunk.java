package com.rangaofei.audioapplication.wavformat;

import com.rangaofei.audioapplication.utils.ByteBufferUtil;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by rangaofei on 2018/4/20.
 */

public class DataChunk {
    private static final String DATA="data";
    private int chunkSize;

    public static void writeDATAChunk(OutputStream outputStream,int chunkSize) throws IOException {
        if(outputStream==null){
            throw new RuntimeException("outputStream is null");
        }
        outputStream.write(DATA.getBytes());
        outputStream.write(ByteBufferUtil.intToLittleByte(chunkSize));
    }
}
