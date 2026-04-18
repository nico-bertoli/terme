#include "terme/settings.h"

#include "terme/managers/debug_manager.h"
#include "terme/managers/time_manager.h"
#include "terme/terminal/terminal.h"

namespace terme
{
	void DebugManager::Log(const std::string& str)
	{
		if constexpr (settings::kDebug == false)
			return;

		if (!log_file_.is_open())
		{
			log_file_.clear();
			log_file_.open(settings::kDebugLogFilePath, std::ios::out | std::ios::app);
			if (!log_file_)
				return;
		}

		log_file_ << str << '\n';
		log_file_.flush();
	}

	void DebugManager::Reset
	(
		size_t screen_size_x,
		size_t screen_size_y,
		size_t screen_padding
	)
	{
		if constexpr (settings::kDebug == false)
			return;

		fps_record_.clear();
		cout_calls_count_ = 0;

		debug_printer_ = std::make_unique<DebugPrinter>(screen_size_x, screen_size_y, screen_padding);

		log_file_.close();
		log_file_.clear();
		log_file_.open(settings::kDebugLogFilePath, std::ios::out | std::ios::app);
	}

	void DebugManager::ShowAverageFPS()
	{
		if constexpr (settings::kDebug == false)
			return;

		if (debug_printer_ != nullptr)
			debug_printer_->PrintFpsString(GetAverageFps());
	}

	size_t DebugManager::GetAverageFps()
	{
		if constexpr (settings::kDebug == false)
			return -1;

		double fps_value = TimeManager::Instance().GetFPS();
		fps_record_.push_back(fps_value);

		if (TimeManager::Instance().GetTime() - last_time_printed_fps_ > kRefreshFpsEverySeconds)
		{
			average_fps_ = 0;

			for (double& recorded_fps : fps_record_)
				average_fps_ += recorded_fps;
			average_fps_ /= fps_record_.size();

			fps_record_.clear();
			last_time_printed_fps_ = TimeManager::Instance().GetTime();
		}
		return static_cast<size_t>(average_fps_);
	}

	void DebugManager::IncrementCoutCalls()
	{
		if constexpr (settings::kDebug == false)
			return;

		if (debug_printer_ == nullptr)
			return;

		if (++cout_calls_count_ == 1000)
			cout_calls_count_ = 0;
		debug_printer_->PrintCoutCallsCount(cout_calls_count_);
	}

}