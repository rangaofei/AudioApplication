package com.rangaofei.audioapplication;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.rangaofei.audioapplication.opengles.GLSurfaceActivity;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
