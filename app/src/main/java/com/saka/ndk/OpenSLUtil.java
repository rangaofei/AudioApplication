package com.saka.ndk;

/**
 * Created by rangaofei on 2018/4/17.
 */

public class OpenSLUtil {
    static {
        System.loadLibrary("OpenSLUtil");
    }

    /**
     * 创建OpenSLES引擎
     *
     * @param rate
     * @param framesPerBuf
     * @param channels
     */
    public static native void createSLEngine(int rate, int framesPerBuf, int channels);

    /**
     * 开始收集声音
     *
     * @return
     */
    public static native boolean createAudioRecorder();

    /**
     * 停止收集声音
     */
    public static native void deleteAudioRecorder();

    /**
     * 开始播放声音
     */
    public static native void startPlay();

    /**
     * 停止播放声音
     */
    public static native void stopPlay();

}
