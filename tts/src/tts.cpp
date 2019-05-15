/*
* 语音合成（Text To Speech，TTS）技术能够自动将任意文字实时转换为连续的
* 自然语音，是一种能够在任何时间、任何地点，向任何人提供语音信息服务的
* 高效便捷手段，非常符合信息时代海量数据、动态更新和个性化查询的需求。
*/


#include <fcntl.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>  //包含了Linux C 中的函数getcwd()

#include "tts.h"


TTS::TTS(std::string path,
         voice_speed speed,
         int volume,
         voice_type voice): appid("58b50f9e"), pitch(50), des_path("/tmp/tts.wav")
{
    this->path = path;
    this->speed = speed;
    this->volume = volume;
    this->voice = voice;
    update_params();
}

//engine_type = local, text_encoding = UTF8, tts_res_path = fo|res/tts/xiaoyan.jet;fo|res/tts/common.jet, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2
void TTS::update_params()
{
    //online
    /*
    strcpy(params,"voice_name = ");
    strcat(params,voice_name);
    strcat(params,", text_encoding = utf8, sample_rate = 16000, speed =");
    strcat(params,speed);
    strcat(params,", volume = ");
    strcat(params,volume);
    strcat(params,", pitch = ");
    strcat(params,pitch);
    strcat(params,", rdn = 2");
    */
    //offline
    if (this->voice == MALE) {
        snprintf(res_path, sizeof(res_path), "fo|%s/res/xiaofeng.jet;fo|%s/res/common.jet", this->path.c_str(),
                 this->path.c_str());
    } else {
        snprintf(res_path, sizeof(res_path), "fo|%s/res/xiaoyan.jet;fo|%s/res/common.jet", this->path.c_str(),
                 this->path.c_str());
    }
    this->params = "engine_type = local, tts_res_path = ";
    this->params.append(res_path);
    if (this->voice == MALE) {
        this->params.append(", voice_name = ").append("xiaofeng");
    } else {
        this->params.append(", voice_name = ").append("xiaoyan");
    }
    this->params.append(", text_encoding = utf8, sample_rate = 16000, speed = ").append(std::to_string(speed)).
    append(", volume = ").append(std::to_string(volume)).
    append(", pitch = ").append(std::to_string(pitch)).
    append(", rdn = 2");
#ifndef NDEBUG
    //std::cout << "TTS Params: " << params << std::endl;
#endif
}
int TTS::init()
{
    int ret = MSP_SUCCESS;
    char login_params[100];
    //the login_params init;
    strcpy(login_params, "appid = ");
    strcat(login_params, appid.c_str());
    strcat(login_params, ", work_dir = .");
    /* 用户登录 */
    ret = MSPLogin(NULL, NULL,
                   login_params);//第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://open.voicecloud.cn注册获取
    return ret;
}

void TTS::SetSpeed(voice_speed speed)
{
    this->speed = speed;
    update_params();
}

void TTS::SetVolume(int volume)
{
    this->volume = volume;
    update_params();
}

void TTS::SetVoiceType(voice_type voice)
{
    this->voice = voice;
    update_params();
}

int TTS::synthesis(const char* text)
{
    int ret = -1;
    FILE* fp = NULL;
    const char* sessionID = NULL;
    unsigned int audio_len = 0;
    wave_pcm_hdr wav_hdr = {
        { 'R', 'I', 'F', 'F' },
        0,
        {'W', 'A', 'V', 'E'},
        {'f', 'm', 't', ' '},
        16,
        1,
        1,
        16000,
        32000,
        2,
        16,
        {'d', 'a', 't', 'a'},
        0
    };
    int synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
    if (NULL == text || des_path.empty()) {
        std::cout << "params is error!" << std::endl;
        return ret;
    }
    fp = fopen(des_path.c_str(), "wb");
    if (NULL == fp) {
        std::cout << "open +des_path error.\n" << std::endl;
        return ret;
    }
    /* 开始合成 */
    sessionID = QTTSSessionBegin(params.c_str(), &ret);
    if (MSP_SUCCESS != ret) {
        std::cout << "QTTSAudioGe6" << std::endl;
        std::cout << "QTTSSessionBegin failed, error code:" + ret << std::endl;
        fclose(fp);
        return ret;
    }
    ret = QTTSTextPut(sessionID, text, (unsigned int)strlen(text), NULL);
    if (MSP_SUCCESS != ret) {
        std::cout << "QTTSAudioGet5" << std::endl;

        std::cout << "QTTSTextPut failed, error code: " << ret << std::endl;
        QTTSSessionEnd(sessionID, "TextPutError");
        fclose(fp);
        return ret;
    }
    fwrite(&wav_hdr, sizeof(wav_hdr) , 1, fp); //添加wav音频头，使用采样率为16000

    while (1) {

        /* 获取合成音频 */
        const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);

        if (MSP_SUCCESS != ret)
            break;
        if (NULL != data) {
            fwrite(data, audio_len, 1, fp);
            wav_hdr.data_size += audio_len; //计算data_size大小
        }
        if (MSP_TTS_FLAG_DATA_END == synth_status)
            break;

        //usleep(150*1000); //防止频繁占用CPU
        usleep(10 * 1000); //防止频繁占用CPU
    }//合成状态synth_status取值请参阅《讯飞语音云API文档》

    if (MSP_SUCCESS != ret) {
        std::cout << "QTTSAudioGet failed, error code: " << ret << std::endl;
        QTTSSessionEnd(sessionID, "AudioGetError");
        fclose(fp);
        return ret;
    }
    /* 修正wav文件头数据的大小 */
    wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

    /* 将修正过的数据写回文件头部,音频文件为wav格式 */
    fseek(fp, 4, 0);
    fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
    fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
    fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
    fclose(fp);
    fp = NULL;
    /* 合成完毕 */
    ret = QTTSSessionEnd(sessionID, "Normal");
    return ret;
}

int TTS::speak(const char* text)
{
    int ret = synthesis(text);
    if (MSP_SUCCESS != ret) {
        std::cout << "QTTSSessionEnd failed, error code: " << ret << std::endl;
        return ret;
    } else {
        playSound();
    }
    return ret;
}

int TTS::playSound()
{
    std::string cmd = std::string("aplay ") + std::string(des_path);
    return system(cmd.c_str());
}

void TTS::setDesPath(std::string const& path)
{
    this->des_path = path;
}
