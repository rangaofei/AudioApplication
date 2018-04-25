package com.rangaofei.audioapplication.fragment;


import android.databinding.DataBindingUtil;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.media.PlaybackParams;
import android.net.Uri;
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
import com.rangaofei.audioapplication.wavformat.DataChunk;
import com.rangaofei.audioapplication.wavformat.FMTChunk;
import com.rangaofei.audioapplication.wavformat.RIFFChunk;
import com.saka.ndk.Mp3Encoder;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
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
        binding.btnConvert.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        convertToMp3(new File(Environment.getExternalStorageDirectory(), fileName).getAbsolutePath(),
                                2, 88200, 22050,
                                new File(Environment.getExternalStorageDirectory(), "target.mp3").getAbsolutePath());
                    }
                }).start();
            }
        });

        binding.btnPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                initPlayer();
            }
        });

    }

    private void startRecord() throws IOException {
        int bufferSize = AudioRecord.getMinBufferSize(22050,
                AudioFormat.CHANNEL_IN_STEREO, AudioFormat.ENCODING_PCM_16BIT);
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, 22050,
                AudioFormat.CHANNEL_IN_STEREO, AudioFormat.ENCODING_PCM_16BIT, bufferSize);
        byte[] audio = new byte[bufferSize];
        audioRecord.startRecording();

        isRecording = true;
        fileName = System.currentTimeMillis() + ".wav";
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
        fixWAVSize(file);

    }

    private void fixWAVSize(File file) throws IOException {
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
            RIFFChunk.writeRIFFCHunk(fileOutputStream, 44);
            FMTChunk.writeFMTChunkWithDefaultSize(fileOutputStream, (short) 2, audioRecord.getSampleRate());
            DataChunk.writeDATAChunk(fileOutputStream, 0);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void initPlayer() {
        Uri uri = Uri.fromFile(new File(Environment.getExternalStorageDirectory(), fileName));
        MediaPlayer mediaPlayer = MediaPlayer.create(getActivity(), uri);
        mediaPlayer.start();
    }

    private void convertToMp3(String pcmFilePath, int channels, int bitRate, int sampleRate, String mp3FilePath) {
        Mp3Encoder.init(pcmFilePath, channels, bitRate, sampleRate, mp3FilePath);
        Log.d("---", "pcmpath=" + pcmFilePath + ",,,mp3path=" + mp3FilePath);
        Mp3Encoder.encode();
        Mp3Encoder.destroy();
    }
}
