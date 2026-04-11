#include <terme/managers/debug_manager.h>

#if DEBUG

#include <terme/terminal/terminal.h>
#include <terme/managers/time_manager.h>

namespace terme
{
	void DebugManager::PrintGenericLog(const std::string& str, int lineIndex) { debugPrinter->PrintGenericLog(str, lineIndex); }

	void DebugManager::Reset
	(
		size_t screenSizeX,
		size_t screenSizeY,
		size_t screenPadding
	)
	{
		fpsRecord.clear();
		coutCallsCount = 0;

		debugPrinter = std::make_unique<DebugPrinter>(screenSizeX, screenSizeY, screenPadding);
	}

	void DebugManager::ShowAverageFPS()
	{
		if (debugPrinter != nullptr)
			debugPrinter->PrintFpsString(GetAverageFps());
	}

	size_t DebugManager::GetAverageFps()
	{
		double fps = TimeManager::Instance().GetFPS();
		fpsRecord.push_back(fps);

		if (TimeManager::Instance().GetTime() - lastTimePrintedFps > kRefreshFpsEverySeconds)
		{
			averageFPS = 0;

			for (double& fps : fpsRecord)
				averageFPS += fps;
			averageFPS /= fpsRecord.size();

			fpsRecord.clear();
			lastTimePrintedFps = TimeManager::Instance().GetTime();
		}
		return static_cast<size_t>(averageFPS);
	}

	void DebugManager::IncrementCoutCalls()
	{
		if (debugPrinter == nullptr)
			return;

		if (++coutCallsCount == 1000)
			coutCallsCount = 0;
		debugPrinter->PrintCoutCallsCount(coutCallsCount);
	}

}
#endif