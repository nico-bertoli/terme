#pragma once

#include <nbkit/singleton.h>
#include <terme/managers/debug_manager.h>
#include <terme/printers/debug_printer.h>
#include <list>
#include <string>
#include <memory>

namespace terme
{
	class DebugManager : public nbkit::Singleton<DebugManager>
	{
		friend class nbkit::Singleton<DebugManager>;
		//---------------------------------------------------------- settings
	private:
		static constexpr double kRefreshFpsEverySeconds = 0.5;
		//---------------------------------------------------------- fields
		std::unique_ptr<DebugPrinter> debugPrinter;

		//fps
		std::list<double> fpsRecord;
		double lastTimePrintedFps = 0;
		double averageFPS = 0;

		//cout calls
		int coutCallsCount = 0;
		//---------------------------------------------------------- methods
	public:
		void Reset(size_t screenSizeX, size_t screenSizeY, size_t screenPadding);
		void ShowAverageFPS();
		void IncrementCoutCalls();
		void PrintGenericLog(const std::string& str, int lineIndex);

	private:
		size_t GetAverageFps();
	};
}