#pragma once

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
