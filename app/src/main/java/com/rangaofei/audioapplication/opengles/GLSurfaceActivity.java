package com.rangaofei.audioapplication.opengles;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.pm.ConfigurationInfo;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

public class GLSurfaceActivity extends AppCompatActivity {
    private GLSurfaceView glSurfaceView;
    private boolean renderSet = false;
    private AirHockeyRender airHockeyRender;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glSurfaceView = new GLSurfaceView(this);
        final ActivityManager activityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        ConfigurationInfo info = activityManager.getDeviceConfigurationInfo();
        boolean support = info.reqGlEsVersion >= 0x20000;
        Log.e("---", "是否支持OpenGLES2.0：" + support);
        if (support) {
            airHockeyRender = new AirHockeyRender(this);
            glSurfaceView.setEGLContextClientVersion(2);
            glSurfaceView.setRenderer(airHockeyRender);
            renderSet = true;
        } else {
            Toast.makeText(this, "this device does not support OpenGLES", Toast.LENGTH_SHORT).show();
            return;
        }
        glSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event != null) {
                    final float normalizedX = event.getX() / (float) v.getWidth() * 2 - 1;
                    final float normalizedY = -(event.getY() / (float) v.getHeight() * 2 - 1);
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        glSurfaceView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                airHockeyRender.handleTouchPress(normalizedX, normalizedY);
                            }
                        });
                    } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                        glSurfaceView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                airHockeyRender.handleTouchDrag(normalizedX, normalizedY);
                            }
                        });
                    }
                    return true;
                } else {
                    return false;
                }
            }
        });
        setContentView(glSurfaceView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (renderSet) {
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (renderSet) {
            glSurfaceView.onResume();
        }
    }
}
