#pragma once
#include <terme/managers/debug_manager.h>
#include <string>

namespace terme
{
	class ISimulationEntity
	{
		friend class Simulation;
		//---------------------------------------------------------- methods
	public:
		virtual ~ISimulationEntity() = default;

	protected:
		virtual void Update() = 0;
	};
}
