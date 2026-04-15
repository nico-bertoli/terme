#include <terme/settings.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <terme/managers/audio_manager.h>
#include <nbkit/random_utils.h>

#include <array>

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
			bool engineOk = false;
			std::array<FxSlot, 6> fx{};
			FxSlot music{};
		};

		AudioState& State()
		{
			static AudioState s;
			return s;
		}

		bool TryGetFxSlot(unsigned& outIndex)
		{
			AudioState& st = State();
			for (unsigned i = 0; i < st.fx.size(); ++i)
			{
				FxSlot& slot = st.fx[i];
				if (!slot.initialized || !ma_sound_is_playing(&slot.sound))
				{
					outIndex = i;
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
			st.engineOk = true;
			
			static constexpr float volume = settings::kAllowSounds ? 1.0f : 0.0f;
			ma_engine_set_volume(&st.engine, volume);
		}
	}

	void AudioManager::PlayFx(const char* fileName, const double randomPitch)
	{
		AudioState& st = State();
		if (!st.engineOk)
			return;

		unsigned index = 0;
		if (!TryGetFxSlot(index))
			return;

		FxSlot& slot = st.fx[index];
		if (slot.initialized)
		{
			ma_sound_uninit(&slot.sound);
			slot.initialized = false;
		}

		if (ma_sound_init_from_file(&st.engine, fileName, 0, nullptr, nullptr, &slot.sound) != MA_SUCCESS)
			return;

		slot.initialized = true;

		float pitch = 1.f;
		if (randomPitch != 0.0)
			pitch += static_cast<float>(nbkit::random_utils::GetRandomDouble(1.0 - randomPitch, 1.0 + randomPitch));
		ma_sound_set_pitch(&slot.sound, pitch);

		if (ma_sound_start(&slot.sound) != MA_SUCCESS)
		{
			ma_sound_uninit(&slot.sound);
			slot.initialized = false;
		}
	}

	void AudioManager::PlayMusic(const char* fileName)
	{
		AudioState& st = State();
		if (!st.engineOk)
			return;

		StopMusic();

		if (ma_sound_init_from_file(&st.engine, fileName, MA_SOUND_FLAG_STREAM, nullptr, nullptr, &st.music.sound) != MA_SUCCESS)
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
