package com.rangaofei.audioapplication.utils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ByteBufferUtil {
    public static short littleByteToShort(byte[] data) {
        if (data.length != 2) {
            throw new UnsupportedOperationException("the byte length is not 2");
        }
        return ByteBuffer.allocate(data.length).order(ByteOrder.LITTLE_ENDIAN).put(data).getShort(0);
    }

    public static short bigByteToShort(byte[] data) {
        if (data.length != 2) {
            throw new UnsupportedOperationException("the byte length is not 2");
        }
        return ByteBuffer.allocate(data.length).order(ByteOrder.BIG_ENDIAN).put(data).getShort(0);
    }

    public static byte[] shortToLittleByte(short data) {
        return ByteBuffer.allocate(2).order(ByteOrder.LITTLE_ENDIAN).putShort(data).array();
    }

    public static byte[] shortToBigByte(short data) {
        return ByteBuffer.allocate(2).order(ByteOrder.BIG_ENDIAN).putShort(data).array();
    }

    public static int littleByteToInt(byte[] data) {
        if (data.length != 4) {
            throw new UnsupportedOperationException("the byte length is not 4");
        }
        return ByteBuffer.allocate(data.length).order(ByteOrder.LITTLE_ENDIAN).put(data).getInt(0);
    }

    public static int bigByteToInt(byte[] data) {
        if (data.length != 4) {
            throw new UnsupportedOperationException("the byte length is not 4");
        }
        return ByteBuffer.allocate(data.length).order(ByteOrder.BIG_ENDIAN).put(data).getInt(0);
    }

    public static byte[] intToLittleByte(int data) {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(data).array();
    }

    public static byte[] intToBigByte(int data) {
        return ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN).putInt(data).array();
    }

    public static long littleByteToLong(byte[] data) {
        if (data.length != 8) {
            throw new UnsupportedOperationException("the byte length is not 8");
        }
        return ByteBuffer.allocate(data.length).order(ByteOrder.LITTLE_ENDIAN).put(data).getLong(0);
    }

    public static long bigByteToLong(byte[] data) {
        if (data.length != 8) {
            throw new UnsupportedOperationException("the byte length is not 8");
        }
        return ByteBuffer.allocate(data.length).order(ByteOrder.BIG_ENDIAN).put(data).getLong(0);
    }

    public static byte[] longToLittleByte(int data) {
        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putLong(data).array();
    }

    public static byte[] longToBigByte(int data) {
        return ByteBuffer.allocate(8).order(ByteOrder.BIG_ENDIAN).putLong(data).array();
    }
}
