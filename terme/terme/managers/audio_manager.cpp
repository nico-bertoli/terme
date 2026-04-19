#include "terme/settings.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "terme/managers/audio_manager.h"
#include "terme/managers/debug_manager.h"

#include <array>
#include <sstream>

#include <nbkit/random_utils.h>

namespace terme
{
	namespace
	{
		struct FxSlot
		{
			ma_sound sound{};
			bool initialized = false;
		};

		struct AudioState
		{
			ma_engine engine{};
			bool engine_ok = false;
			std::array<FxSlot, 6> fx{};
			FxSlot music{};
		};

		AudioState& State()
		{
			static AudioState audio_state;
			return audio_state;
		}

		bool TryGetFxSlot(unsigned& out_index)
		{
			AudioState& st = State();
			for (unsigned i = 0; i < st.fx.size(); ++i)
			{
				FxSlot& slot = st.fx[i];
				if (!slot.initialized || !ma_sound_is_playing(&slot.sound))
				{
					out_index = i;
					return true;
				}
			}
			return false;
		}
	} // namespace

	AudioManager::AudioManager()
	{
		AudioState& st = State();
		if (ma_engine_init(nullptr, &st.engine) == MA_SUCCESS)
		{
			st.engine_ok = true;

			static constexpr float kVolume = settings::kAllowSounds ? 1.0f : 0.0f;
			ma_engine_set_volume(&st.engine, kVolume);
		}
	}

	void AudioManager::PlayFx(const char* file_name, const double random_pitch)
	{
		AudioState& st = State();
		if (!st.engine_ok)
		{
			std::stringstream s;
			s << "[audio_manager] PlayFx: engine not initialized (file: " << (file_name ? file_name : "(null)") << ')';
			DebugManager::Instance().Log(s.str());
			return;
		}

		unsigned index = 0;
		if (!TryGetFxSlot(index))
		{
			std::stringstream s;
			s << "[audio_manager] PlayFx: all FX slots busy (file: " << (file_name ? file_name : "(null)") << ')';
			DebugManager::Instance().Log(s.str());
			return;
		}

		FxSlot& slot = st.fx[index];
		if (slot.initialized)
		{
			ma_sound_uninit(&slot.sound);
			slot.initialized = false;
		}

		if (ma_sound_init_from_file(&st.engine, file_name, 0, nullptr, nullptr, &slot.sound) != MA_SUCCESS)
		{
			std::stringstream s;
			s << "[audio_manager] PlayFx: failed to load sound (file: " << (file_name ? file_name : "(null)") << ')';
			DebugManager::Instance().Log(s.str());
			return;
		}

		slot.initialized = true;

		float pitch = 1.f;
		if (random_pitch != 0.0)
			pitch += static_cast<float>(nbkit::random_utils::GetRandomDouble(1.0 - random_pitch, 1.0 + random_pitch));
		ma_sound_set_pitch(&slot.sound, pitch);

		if (ma_sound_start(&slot.sound) != MA_SUCCESS)
		{
			std::stringstream s;
			s << "[audio_manager] PlayFx: failed to start playback (file: " << (file_name ? file_name : "(null)") << ')';
			DebugManager::Instance().Log(s.str());
			ma_sound_uninit(&slot.sound);
			slot.initialized = false;
		}
	}

	void AudioManager::PlayMusic(const char* file_name)
	{
		AudioState& st = State();
		if (!st.engine_ok)
			return;

		StopMusic();

		if (ma_sound_init_from_file(&st.engine, file_name, MA_SOUND_FLAG_STREAM, nullptr, nullptr, &st.music.sound) != MA_SUCCESS)
			return;

		st.music.initialized = true;

		if (ma_sound_start(&st.music.sound) != MA_SUCCESS)
		{
			ma_sound_uninit(&st.music.sound);
			st.music.initialized = false;
		}
	}

	void AudioManager::StopMusic()
	{
		AudioState& st = State();
		if (!st.music.initialized)
			return;

		if (ma_sound_is_playing(&st.music.sound))
			ma_sound_stop(&st.music.sound);

		ma_sound_uninit(&st.music.sound);
		st.music.initialized = false;
	}

} // namespace terme
