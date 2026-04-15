#pragma once
#include <terme/settings.h>
#include <nbkit/singleton.h>

namespace terme
{
	class AudioManager : public nbkit::Singleton<AudioManager>
	{
		friend class nbkit::Singleton<AudioManager>;

		//---------------------------------------------------------- methods
	public:
		void PlayFx(const char* fileName, double randomPitch = 0.0);
		void PlayMusic(const char* fileName);
		void StopMusic();

	private:
		AudioManager();
	};
}