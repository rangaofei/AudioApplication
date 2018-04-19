package com.rangaofei.audioapplication;

import com.rangaofei.audioapplication.utils.ByteBufferUtil;

import org.junit.Test;

import static org.junit.Assert.*;

public class ByteBufferUtilTest {
    private byte[] littleShort = new byte[]{0x0a, 0x00};
    private byte[] bigShort = new byte[]{0x00, 0x0a};
    private byte[] littleInt = new byte[]{0x0b, 0x00, 0x00, 0x00};
    private byte[] bigInt = new byte[]{0x00, 0x00, 0x00, 0x0b};
    private byte[] littleLong = new byte[]{0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    private byte[] bigLong = new byte[]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c};

    @Test
    public void littleByteToShort() {
        short result = ByteBufferUtil.littleByteToShort(littleShort);
        assertEquals(result, 10);
    }

    @Test
    public void bigByteToShort() {
        short result = ByteBufferUtil.bigByteToShort(bigShort);
        assertEquals(result, 10);
    }

    @Test
    public void shortToLittleByte() {
        byte[] result = ByteBufferUtil.shortToLittleByte((short) 10);
        assertArrayEquals(result, littleShort);
    }

    @Test
    public void shortToBigByte() {
        byte[] result = ByteBufferUtil.shortToBigByte((short) 10);
        assertArrayEquals(result, bigShort);
    }

    @Test
    public void littleByteToInt() {
        int result = ByteBufferUtil.littleByteToInt(littleInt);
        assertEquals(result, 11);
    }

    @Test
    public void bigByteToInt() {
        int result = ByteBufferUtil.bigByteToInt(bigInt);
        assertEquals(result, 11);
    }

    @Test
    public void intToLittleByte() {
        byte[] result = ByteBufferUtil.intToLittleByte(11);
        assertArrayEquals(result, littleInt);
    }

    @Test
    public void intToBigByte() {
        byte[] result = ByteBufferUtil.intToBigByte(11);
        assertArrayEquals(result, bigInt);
    }

    @Test
    public void littleByteToLong() {
        long result = ByteBufferUtil.littleByteToLong(littleLong);
        assertEquals(result, 12);
    }

    @Test
    public void bigByteToLong() {
        long result = ByteBufferUtil.bigByteToLong(bigLong);
        assertEquals(result, 12);
    }

    @Test
    public void longToLittleByte() {
        byte[] result = ByteBufferUtil.longToLittleByte(12);
        assertArrayEquals(result,littleLong);
    }

    @Test
    public void longToBigByte() {
        byte[] result = ByteBufferUtil.longToBigByte(12);
        assertArrayEquals(result,bigLong);
    }
}