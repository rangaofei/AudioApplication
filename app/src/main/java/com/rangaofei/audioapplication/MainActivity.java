package com.rangaofei.audioapplication;

import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.rangaofei.audioapplication.opengles.GLSurfaceActivity;
import com.saka.ndk.OpenSLPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        File file = Environment.getExternalStorageDirectory();
        if (file == null) {
            throw new NullPointerException("外部存储为空");
        }
        File wavFile = new File(file, "test.wav");
        if (!wavFile.exists()) {
            throw new NullPointerException("未找到测试文件");
        }
        final AssetManager assetManager = getResources().getAssets();
        new Thread(new Runnable() {
            @Override
            public void run() {
                OpenSLPlayer.createEngine();
                OpenSLPlayer.setPlayer(assetManager);
                OpenSLPlayer.startPlay();
            }
        }).start();
    }


    @Override
    protected void onDestroy() {
        OpenSLPlayer.stopPlay();
        super.onDestroy();
    }

    public void addPCM(View view) {
        startTargetActivity(ContainerActivity.RECORD_PCM);
    }

    public void addOpenSL(View view) {
        startTargetActivity(ContainerActivity.RECORD_OPENSL);
    }

    private void startTargetActivity(int i) {
        Intent intent = new Intent(this, ContainerActivity.class);
        intent.putExtra("type", i);
        startActivity(intent);
    }

    public void startOoenGl(View view) {
        Intent intent = new Intent(this, GLSurfaceActivity.class);
        startActivity(intent);
    }
}
