#pragma once

#include "../Resource/Audio.h"
#include <xaudio2.h>
#include <memory>
#include <vector>

#include <wrl.h>

namespace Regal::Game
{
    //ゲームオブジェクトに持たせる
    class AudioSource
    {
    public:
        enum class TYPE
        {
            BGM,
            SE
        };

    public:
        AudioSource(const wchar_t* filename,UINT type);
        ~AudioSource() {}

        void Play(const bool loop);
        void Play(const bool loop,const float volume);

        void Stop();

        void FadeIn(float volume);//音量を引数の値まで徐々に上げる
        void FadeOut(float volume);//音量を引数の値まで徐々に下げる

    private:
        std::unique_ptr<Regal::Resource::Audio> audio;
        UINT type;

        float volume;

        UINT myNamber;
    };


    class AudioManager
    {
    private: 
        AudioManager() {}
        ~AudioManager() {}
        
    public:
        static AudioManager& Instance()
        {
            static AudioManager instance;
            return instance;
        }

        void Initialize();

        IXAudio2* GetXAudio2() { return xaudio2.Get(); }

        void StopAllBgm();

    private:
        // XAUDIO2
        Microsoft::WRL::ComPtr<IXAudio2> xaudio2;
        IXAudio2MasteringVoice* masterVoice = nullptr;

        std::vector<Regal::Resource::Audio*> backGroundMusics;//bgm
        std::vector<Regal::Resource::Audio*> soundEffects;//se
    };
}