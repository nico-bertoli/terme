#pragma once

#include "terme/printers/debug_printer.h"

#include <list>
#include <memory>
#include <string>

#include <nbkit/singleton.h>

namespace terme
{
	class DebugManager : public nbkit::Singleton<DebugManager>
	{
		friend class nbkit::Singleton<DebugManager>;
		//---------------------------------------------------------- settings
	private:
		static constexpr double kRefreshFpsEverySeconds = 0.5;
		//---------------------------------------------------------- fields
		std::unique_ptr<DebugPrinter> debug_printer_;

//fps
		std::list<double> fps_record_;
		double last_time_printed_fps_ = 0;
		double average_fps_ = 0;

		//cout calls
		int cout_calls_count_ = 0;
		//---------------------------------------------------------- methods
	public:
		void Reset(size_t screen_size_x, size_t screen_size_y, size_t screen_padding);
		void ShowAverageFPS();
		void IncrementCoutCalls();
		void PrintGenericLog(const std::string& str, int line_index);

	private:
		size_t GetAverageFps();
	};
}