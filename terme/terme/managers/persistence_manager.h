#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <terme/managers/debug_manager.h>

namespace terme
{
	namespace PersistenceManager
	{
		inline void SaveBestScore(const char* persistence_file, int best_score)
		{
			std::ofstream out_stream(persistence_file);

			if(!out_stream)
			{
#if DEBUG
				std::stringstream s;
				s << "[persistence manager] failed to save best score at: " << persistence_file;
				DebugManager::Instance().PrintGenericLog(s.str(), 0);
#endif
			}
			else
			{
				out_stream << best_score;
			}
		}

		inline int LoadBestScore(const char* persistence_file)
		{
			std::ifstream input_stream(persistence_file);

			// best score not found, first time playing, initialize it at -1
			if (!input_stream)
			{
				SaveBestScore(persistence_file, -1);

#if DEBUG	// check file was created 
				std::ifstream verify(persistence_file);
				if (!verify)
				{
					std::stringstream s;
					s << "[persistence manager] failed to load or create persistence file at: " << persistence_file;
					DebugManager::Instance().PrintGenericLog(s.str(), 1);
				}
#endif
				return -1;
			}

			int best_score;
			input_stream >> best_score;
			return best_score;
		}
	}
}