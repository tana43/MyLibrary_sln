#include "AudioSource.h"

namespace Regal::Game
{
    AudioSource::AudioSource(const wchar_t* filename, UINT type) : type(type)
    {
        audio = std::make_unique<Regal::Resource::Audio>(AudioManager::Instance().GetXAudio2(),
            filename);
    }

    void AudioManager::Initialize()
    {
        HRESULT hr{ S_OK };

        // XAUDIO2
        hr = XAudio2Create(xaudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = xaudio2->CreateMasteringVoice(&masterVoice);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}