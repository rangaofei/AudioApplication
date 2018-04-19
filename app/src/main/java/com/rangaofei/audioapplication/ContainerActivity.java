package com.rangaofei.audioapplication;

import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.rangaofei.audioapplication.fragment.OpenSLRecordFragment;
import com.rangaofei.audioapplication.fragment.PCMFragment;

public class ContainerActivity extends AppCompatActivity {
    public static final int RECORD_PCM = 1;
    public static final int RECORD_OPENSL = 2;
    private int type;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_container);
        initData();
    }

    private void getIntentData() {
        if (getIntent() != null) {
            type = getIntent().getIntExtra("type", 0);
        }
    }

    private void initData() {
        getIntentData();
        switch (type) {
            case RECORD_PCM:
                addTargetActivity(PCMFragment.newInstance());
                break;
            case RECORD_OPENSL:
                addTargetActivity(OpenSLRecordFragment.newInstance());
                break;
        }
    }

    private void addTargetActivity(Fragment fragment) {
        getSupportFragmentManager()
                .beginTransaction()
                .replace(R.id.container, fragment)
                .commitAllowingStateLoss();
    }
}
