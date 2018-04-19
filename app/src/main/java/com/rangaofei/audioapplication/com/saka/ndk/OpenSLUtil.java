package com.rangaofei.audioapplication.com.saka.ndk;

/**
 * Created by rangaofei on 2018/4/17.
 */

public class OpenSLUtil {
    static {
        System.loadLibrary("OpenSLUtil");
    }

    public static native void createSLEngine(int rate, int framesPerBuf, int channels);

    public static native boolean createAudioRecorder();

    public static native void deleteAudioRecorder();

    public static native void startPlay();

    public static native void stopPlay();
}
