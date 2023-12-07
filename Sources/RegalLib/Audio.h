#pragma once
// XAUDIO2

#include <xaudio2.h>
#include <mmreg.h>

#include "../Other/Misc.h"

namespace Regal::Resource
{
	class Audio
	{
		WAVEFORMATEXTENSIBLE wfx = { 0 };
		XAUDIO2_BUFFER buffer = { 0 };

		IXAudio2SourceVoice* source_voice;

	public:
		Audio(IXAudio2* xaudio2, const wchar_t* filename);
		virtual ~Audio();
		void Play(int loop_count = 0/*255 : XAUDIO2_LOOP_INFINITE*/);
		void Stop(bool play_tails = true, bool wait_for_buffer_to_unqueue = true);
		void Volume(float volume);
		bool Queuing();//èáî‘ë“ÇøÅH	
	};
}
