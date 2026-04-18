#pragma once

#include <nbkit/singleton.h>

namespace terme
{
	class AudioManager : public nbkit::Singleton<AudioManager>
	{
		friend class nbkit::Singleton<AudioManager>;

		//---------------------------------------------------------- methods
	public:
		void PlayFx(const char* file_name, double random_pitch = 0.0);
		void PlayMusic(const char* file_name);
		void StopMusic();

	private:
		AudioManager();
	};
}