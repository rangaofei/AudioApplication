package com.rangaofei.audioapplication.fragment;


import android.databinding.DataBindingUtil;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.rangaofei.audioapplication.utils.ByteUtil;
import com.rangaofei.audioapplication.R;
import com.rangaofei.audioapplication.databinding.FragmentPcmBinding;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;

/**
 * A simple {@link Fragment} subclass.
 */
public class PCMFragment extends Fragment {
    private FragmentPcmBinding binding;
    private AudioRecord audioRecord;
    private boolean isRecording;

    public static final String HEADER_CHUNK = "RIFF";
    public static final String FORMAT = "WAVE";
    public static final String FMT_CHUNK = "fmt ";
    public static final String DATA_CHUNK = "data";


    private String fileName;


    public PCMFragment() {
        // Required empty public constructor
    }

    public static PCMFragment newInstance() {
        PCMFragment fragment = new PCMFragment();
        return fragment;
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        binding = DataBindingUtil.inflate(inflater,
                R.layout.fragment_pcm, container, false);
        initData();
        return binding.getRoot();
    }

    private void initData() {
        binding.btnAudio.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e("---", "isrecording=" + isRecording);
                if (isRecording) {
                    stopRecord();
                } else {
                    binding.btnAudio.setText("stop");
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                startRecord();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }).start();

                }
            }

        });

    }

    private void startRecord() throws IOException {
        Log.e("---", "start");
        int bufferSize = AudioRecord.getMinBufferSize(22050,
                AudioFormat.CHANNEL_IN_STEREO, AudioFormat.ENCODING_PCM_16BIT);
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, 22050,
                AudioFormat.CHANNEL_IN_STEREO, AudioFormat.ENCODING_PCM_16BIT, bufferSize);
        byte[] audio = new byte[bufferSize];
        audioRecord.startRecording();

        Log.e("---", "samplerate=" + audioRecord.getSampleRate() + ",channelcount" + audioRecord.getChannelCount());
        isRecording = true;
        fileName = "" + System.currentTimeMillis() + ".wav";
        File file = new File(Environment.getExternalStorageDirectory(), fileName);
        if (!file.exists()) {
            file.createNewFile();
        }
        BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(file, true));
        pcmToWav(bos);
        while (isRecording) {
            audioRecord.read(audio, 0, bufferSize);
            bos.write(audio);
        }
        audioRecord.stop();
        bos.flush();
        bos.close();
        long length = file.length() - 44;
        Log.e("-----", "" + length);
        RandomAccessFile raf = new RandomAccessFile(file, "rwd");
        raf.seek(4);
        raf.write(ByteUtil.intToLittleByte((int) length + 36));
        raf.seek(40);
        raf.write(ByteUtil.intToLittleByte((int) length));
        raf.close();

    }

    private void stopRecord() {
        Log.e("---", "stop");
        isRecording = false;
        binding.btnAudio.setText("start");
    }

    private void pcmToWav(BufferedOutputStream fileOutputStream) {
        try {

            fileOutputStream.write(HEADER_CHUNK.getBytes());
            fileOutputStream.write(ByteUtil.intToLittleByte(36));
            fileOutputStream.write(FORMAT.getBytes());

            fileOutputStream.write(FMT_CHUNK.getBytes());
            fileOutputStream.write(ByteUtil.intToLittleByte(16));
            fileOutputStream.write(ByteUtil.shortToLittleByte((short) 1));
            fileOutputStream.write(ByteUtil.shortToLittleByte((short) (audioRecord.getChannelCount())));
            fileOutputStream.write(ByteUtil.intToLittleByte(audioRecord.getSampleRate()));
            fileOutputStream.write(ByteUtil.intToLittleByte(2 * 22050 * 16 / 8));
            fileOutputStream.write(ByteUtil.shortToLittleByte((short) (2 * 16 / 8)));
            fileOutputStream.write(ByteUtil.shortToLittleByte((short) 16));

            fileOutputStream.write(DATA_CHUNK.getBytes());
            fileOutputStream.write(0);
            fileOutputStream.write(0);
            fileOutputStream.write(0);
            fileOutputStream.write(0);
            fileOutputStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
