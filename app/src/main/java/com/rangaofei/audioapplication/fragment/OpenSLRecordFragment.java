package com.rangaofei.audioapplication.fragment;


import android.content.Context;
import android.content.pm.PackageManager;
import android.databinding.DataBindingUtil;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.rangaofei.audioapplication.R;
import com.rangaofei.audioapplication.com.saka.ndk.OpenSLUtil;
import com.rangaofei.audioapplication.databinding.FragmentOpenSlrecordBinding;

/**
 * A simple {@link Fragment} subclass.
 */
public class OpenSLRecordFragment extends Fragment {

    private FragmentOpenSlrecordBinding binding;
    private int nativeSampleRate;
    private int nativeSampleBufferSize;
    private boolean supportRecording;
    private boolean isRecordding;

    public static OpenSLRecordFragment newInstance() {
        OpenSLRecordFragment fragment = new OpenSLRecordFragment();
        return fragment;
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_open_slrecord, container, false);
        initData();
        return binding.getRoot();
    }

    private void initData() {
        queryNativeAudioParameters();
        OpenSLUtil.createSLEngine(nativeSampleRate, nativeSampleBufferSize,1);
        binding.btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startRecord();
            }
        });
    }

    private void startRecord() {
        if (!supportRecording) {
            return;
        }
        if (!isRecordding) {
            if (!OpenSLUtil.createAudioRecorder()) {
                OpenSLUtil.deleteAudioRecorder();
                Toast.makeText(getActivity(), "create recorder failed!!!", Toast.LENGTH_SHORT).show();
                return;
            }
            OpenSLUtil.startPlay();
        } else {
            OpenSLUtil.stopPlay();
            OpenSLUtil.deleteAudioRecorder();
        }
        isRecordding = !isRecordding;
    }


    private void queryNativeAudioParameters() {
        PackageManager packageManager = getActivity().getPackageManager();
        boolean hasLowLatency = packageManager.hasSystemFeature(PackageManager.FEATURE_AUDIO_LOW_LATENCY);
        Log.e("---", "this device has low latency:" + hasLowLatency);
        AudioManager audioManager = (AudioManager) getActivity().getSystemService(Context.AUDIO_SERVICE);
        nativeSampleRate = Integer.parseInt(audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE));
        nativeSampleBufferSize = Integer.parseInt(audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER));
        int bufferSize = AudioRecord.getMinBufferSize(nativeSampleRate, AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT);
        supportRecording = true;
        if (bufferSize == AudioRecord.ERROR || bufferSize == AudioRecord.ERROR_BAD_VALUE) {
            supportRecording = false;
        }
    }
}
