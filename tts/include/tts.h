#ifndef _TTS_H_
#define _TTS_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#define FILEPATH_MAX 1024
#define Sample_Size 16 //there're two kinds of bits,8 bits and 16 bits
#define Sample_Rate 16000 //sampling rate

class TTS
{
public:
    enum voice_type {
        MALE,
        FEMALE,
    };
    enum voice_speed {
        slow = 30,
        middle = 50,
        fast = 90
    };
    /* wav音频头部格式 */
    struct wave_pcm_hdr {
        char            riff[4];                // = "RIFF"
        int     size_8;                 // = FileSize - 8
        char            wave[4];                // = "WAVE"
        char            fmt[4];                 // = "fmt "
        int     fmt_size;       // = 下一个结构体的大小 : 16

        short int       format_tag;             // = PCM : 1
        short int       channels;               // = 通道数 : 1
        int     samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
        int     avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
        short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
        short int       bits_per_sample;        // = 量化比特数: 8 | 16

        char            data[4];                // = "data";
        int     data_size;              // = 纯数据长度 : FileSize - 44
    };
    TTS(std::string path, voice_speed speed = middle,
        int volume = 50,
        voice_type voice = FEMALE);
    ~TTS() {};
    int init();//the voice init. Every thread should be a new init(maybe)
    int speak(const char* text);//the main function.
    int synthesis(const char* text);
    void SetSpeed(voice_speed speed);
    void SetVolume(int volume);
    void SetVoiceType(voice_type voice);
    void setDesPath(std::string const& path);

private:
    //const char* appid = "5833fcf1";//登录参数,appid与msc库绑定,请勿随意改动
    //离线
    //const char* appid = "58b50f9e";
    std::string appid;
    voice_type  voice; //the voice name
    voice_speed speed; //the voice speed
    std::string path;
    int  volume; //the voice volume
    int pitch; //the voice pitch
    std::string des_path; //合成的语音文件名称
    char res_path[FILEPATH_MAX]; //合成资源路径
    std::string params;
    int playSound();
    void update_params();
};

#endif
