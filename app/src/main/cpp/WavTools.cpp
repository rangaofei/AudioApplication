//
// Created by 冉高飞 on 2018/5/1.
//

#include "WavTools.h"


WavTools::WavTools() = default;

WavTools::~WavTools() {
//    delete wavHeader;
}


int WavTools::writeWavInfoToFile(std::ofstream *outstream, WavHeader *header) {
    if (!outstream->is_open()) {
        std::cout << "the stream is null" << std::endl;
        return -1;
    }
    header->fmtChunk.byte_rate =
            header->fmtChunk.sample_rate * header->fmtChunk.num_channels *
            header->fmtChunk.bit_per_sample / 8;
    outstream->write((char *) header, sizeof(WavHeader));
    return 0;
}

int WavTools::getWavFormat(std::ifstream *in_stream, FmtChunk *fmtChunk, bool show) {
    if (!in_stream->is_open()) {
        std::cout << "the stream is null" << std::endl;
        return -1;
    }
    in_stream->seekg(0, std::ios::beg);
    char tmp_header[5];
    memset(tmp_header, 0, 5 * sizeof(char));
    in_stream->read(tmp_header, 4);
    if (strcmp(tmp_header, RIFF) != 0) {
        std::cout << "tmp_header:" << tmp_header << std::endl;
        return STATE_ERR_NOT_WAV;
    }
    in_stream->seekg(8, std::ios::cur);
    while (true) {
        in_stream->read(tmp_header, 4);
        if (strcmp(tmp_header, FMT) != 0) {
            in_stream->read((char *) &chunk_size, sizeof(uint32_t));
            in_stream->seekg(chunk_size, std::ios::cur);
            continue;
        } else {
            in_stream->seekg(-4, std::ios::cur);
            in_stream->read((char *) fmtChunk, sizeof(FmtChunk));
            if (show)
                printWAVFmtChunk(fmtChunk);
            break;
        }

    }
    return STATE_SUCCESS;
}

int WavTools::seekToRealData(std::ifstream *in_stream) {
    if (!(in_stream->is_open())) {
        std::cout << "the stream is null" << std::endl;
        return -1;
    }
    in_stream->seekg(0, std::ios::beg);
    char tmp_header[5];
    memset(tmp_header, 0, 5 * sizeof(char));
    in_stream->read(tmp_header, 4);
    if (strcmp(tmp_header, RIFF) != 0) {
        std::cout << "tmp_header:" << tmp_header << std::endl;
        return STATE_ERR_NOT_WAV;
    }
    in_stream->seekg(8, std::ios::cur);
    while (true) {
        in_stream->read(tmp_header, 4);
        if (strcmp(tmp_header, DATA) != 0) {
            in_stream->read((char *) &chunk_size, sizeof(uint32_t));
            in_stream->seekg(chunk_size, std::ios::cur);
            continue;
        } else {
            auto *dataChunk = static_cast<DataChunk *>(malloc(sizeof(DataChunk)));
            in_stream->seekg(-4, std::ios::cur);
            in_stream->read((char *) dataChunk, sizeof(DataChunk));
            std::cout << "data chunk:\n"
                      << "name:"
                      << dataChunk->data
                      << "\nchunk_size:"
                      << dataChunk->chunk_size
                      << std::endl;
            break;
        }

    }
    return STATE_SUCCESS;
}

void WavTools::printWAVFmtChunk(FmtChunk *fmtChunk) {
    if (strcmp(fmtChunk->fmt, FMT) != 0) {
        std::cerr << "FormatChunk is not correct" << std::endl;
        return;
    }
    std::cout << "\nAudio Format    : " << ((fmtChunk->audio_format == 1) ? "PCM" : "OTHER")
              << "\nAudio Channels  : " << fmtChunk->num_channels
              << "\nBit PerSample   : " << fmtChunk->bit_per_sample
              << "\nSample Rate     : " << fmtChunk->sample_rate
              << "\nByte Rate       : " << fmtChunk->byte_rate << std::endl;

}

void WavTools::printWAVHeader(WavHeader *wavHeader) {
    if (strcmp(wavHeader->riff, RIFF) != 0) {
        std::cerr << "Is not a wav file" << std::endl;
        return;
    }
    if (&wavHeader->fmtChunk != nullptr)
        printWAVFmtChunk(&wavHeader->fmtChunk);
    if (&wavHeader->dataChunk != nullptr)
        printWAVDataChunk(&wavHeader->dataChunk);
}

void WavTools::printWAVDataChunk(DataChunk *dataChunk) {
    if (strcmp(dataChunk->data, DATA) != 0) {
        std::cerr << "Is not dataChunk!" << std::endl;
        return;
    }
    std::cout << "PCM Length        :" << dataChunk->chunk_size << std::endl;
}

int WavTools::getWavFormat(char *in_data, FmtChunk *fmtChunk) {
    return 0;
}
