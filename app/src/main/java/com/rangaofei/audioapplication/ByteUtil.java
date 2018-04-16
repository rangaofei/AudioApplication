package com.rangaofei.audioapplication;

/**
 * very simple util for byte to int or short or long.
 * such as <code>int b=6;</code> in the pc memory it stores like this:
 * <p>
 * 1. big-endian:
 * +---------------+---------------+---------------+---------------+
 * |0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 1 1 0|0 0 0 0 0 1 1 0|
 * +---------------+---------------+---------------+---------------+
 * 2. little-endian:
 * +---------------+---------------+---------------+---------------+
 * |0 0 0 0 0 1 1 0|0 0 0 0 0 0 0 0|0 0 0 0 0 1 1 0|0 0 0 0 0 0 0 0|
 * +---------------+---------------+---------------+---------------+
 * <p>
 * so use this util can easily solve this problem when cross platform.
 */
public final class ByteUtil {

    /**
     * big-endian byte[] convert to java short
     */
    public static short bigByteToShort(byte[] b) {
        if (b.length != 2) {
            throw new UnsupportedOperationException("the byte length is not 2");
        }
        int one = ((int) b[0] & 0xff) << 8;
        int two = ((int) b[1] & 0xff) << 0;
        return (short) (one + two);
    }

    /**
     * java short to big-endian bytes[]
     */
    public static byte[] shortToBigByte(short s) {
        byte[] result = new byte[2];
        result[0] = (byte) (s >> 8 & 0xff);
        result[1] = (byte) (s >> 0 & 0xff);
        return result;
    }

    /**
     * little-endian short byte[] convert to java short
     */

    public static short littleByteToShort(byte[] b) {
        if (b.length != 2) {
            throw new UnsupportedOperationException("the byte length is not 2");
        }
        int one = ((int) b[1] & 0xff) << 8;
        int two = ((int) b[0] & 0xff) << 0;
        return (short) (one + two);
    }

    /**
     * java short convert to little-endian byte[]
     */

    public static byte[] shortToLittleByte(short i) {
        byte[] result = new byte[2];
        result[1] = (byte) (i >> 8 & 0xff);
        result[0] = (byte) (i >> 0 & 0xff);
        return result;
    }

    /**
     * big-endian byte[] convert to java int
     */

    public static int bigByteToInt(byte[] b) {
        if (b.length != 4) {
            throw new UnsupportedOperationException("the byte length is not 4");
        }
        int one = ((int) b[0] & 0xff) << 24;
        int two = ((int) b[1] & 0xff) << 16;
        int three = ((int) b[2] & 0xff) << 8;
        int four = ((int) b[3] & 0xff) << 0;
        return one + two + three + four;
    }

    /**
     * java int to big-endian byte[]
     */
    public static byte[] intToBigByte(int i) {
        byte[] result = new byte[4];
        result[0] = (byte) (i >> 24 & 0xff);
        result[1] = (byte) (i >> 16 & 0xff);
        result[2] = (byte) (i >> 8 & 0xff);
        result[3] = (byte) (i >> 0 & 0xff);
        return result;
    }

    /**
     * little-endian byte[] to java int
     */

    public static int littleByteToInt(byte[] b) {
        if (b.length != 4) {
            throw new UnsupportedOperationException("the byte length is not 4");
        }
        int one = ((int) b[3] & 0xff) << 24;
        int two = ((int) b[2] & 0xff) << 16;
        int three = ((int) b[1] & 0xff) << 8;
        int four = ((int) b[0] & 0xff) << 0;
        return one + two + three + four;
    }

    /**
     * java int convert to little-endian byte[]
     */

    public static byte[] intToLittleByte(int i) {
        byte[] result = new byte[4];
        result[3] = (byte) (i >> 24 & 0xff);
        result[2] = (byte) (i >> 16 & 0xff);
        result[1] = (byte) (i >> 8 & 0xff);
        result[0] = (byte) (i >> 0 & 0xff);
        return result;
    }

    /**
     * big-endian byte[] convert to java long
     */
    public static long bigByteToLong(byte[] b) {
        if (b.length != 8) {
            throw new UnsupportedOperationException("the byte length is not 8");
        }
        long one = ((long) b[0] & 0xff) << 56;
        long two = ((long) b[1] & 0xff) << 48;
        long three = ((long) b[2] & 0xff) << 40;
        long four = ((long) b[3] & 0xff) << 32;
        long five = ((long) b[4] & 0xff) << 24;
        long six = ((long) b[5] & 0xff) << 16;
        long seven = ((long) b[6] & 0xff) << 8;
        long eight = ((long) b[7] & 0xff) << 0;
        return one + two + three + four + five + six + seven + eight;
    }

    /**
     * java long convert to big-endian byte[]
     */

    public static byte[] longToBigByte(long i) {
        byte[] result = new byte[8];
        result[0] = (byte) (i >> 56 & 0xff);
        result[1] = (byte) (i >> 48 & 0xff);
        result[2] = (byte) (i >> 40 & 0xff);
        result[3] = (byte) (i >> 32 & 0xff);
        result[3] = (byte) (i >> 24 & 0xff);
        result[3] = (byte) (i >> 16 & 0xff);
        result[3] = (byte) (i >> 8 & 0xff);
        result[3] = (byte) (i >> 0 & 0xff);
        return result;
    }

    /**
     * little-endian convert to java long
     */
    public static long littleByteToLong(byte[] b) {
        if (b.length != 8) {
            throw new UnsupportedOperationException("the byte length is not 8");
        }
        long one = ((long) b[7] & 0xff) << 56;
        long two = ((long) b[6] & 0xff) << 48;
        long three = ((long) b[5] & 0xff) << 40;
        long four = ((long) b[4] & 0xff) << 32;
        long five = ((long) b[3] & 0xff) << 24;
        long six = ((long) b[2] & 0xff) << 16;
        long seven = ((long) b[1] & 0xff) << 8;
        long eight = ((long) b[0] & 0xff) << 0;
        return one + two + three + four + five + six + seven + eight;
    }

    /**
     * java long convert to little-endian byte[]
     */
    public static byte[] longToLittleByte(long i) {
        byte[] result = new byte[8];
        result[7] = (byte) (i >> 56 & 0xff);
        result[6] = (byte) (i >> 48 & 0xff);
        result[5] = (byte) (i >> 40 & 0xff);
        result[4] = (byte) (i >> 32 & 0xff);
        result[3] = (byte) (i >> 24 & 0xff);
        result[2] = (byte) (i >> 16 & 0xff);
        result[1] = (byte) (i >> 8 & 0xff);
        result[0] = (byte) (i >> 0 & 0xff);
        return result;
    }
}

