#pragma once

#include "../Resource/Audio.h"
#include <xaudio2.h>
#include <memory>
#include <vector>

#include <wrl.h>

namespace Regal::Game
{
    //�Q�[���I�u�W�F�N�g�Ɏ�������
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

        void FadeIn(float volume);//���ʂ������̒l�܂ŏ��X�ɏグ��
        void FadeOut(float volume);//���ʂ������̒l�܂ŏ��X�ɉ�����

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