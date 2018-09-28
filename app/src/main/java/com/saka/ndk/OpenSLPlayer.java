package com.saka.ndk;

import android.content.res.AssetManager;

public class OpenSLPlayer {

    static {
        System.loadLibrary("OpenSLPlayer");
    }

    public static native void createEngine();

    public static native void setPlayer(AssetManager assetManager);

    public static native void startPlay();

    public static native void stopPlay();
}
