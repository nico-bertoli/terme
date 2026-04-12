#pragma once
#include <terme/entities/collider.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

namespace terme
{
	class MovingStraightObject : public Collider
	{
		//---------------------------------------------------------- fields
	private:
		Direction move_direction_;
		const double move_speed_;

		//---------------------------------------------------------- methods
	public:
		MovingStraightObject(int x_pos, int y_pos, Direction move_direction, double move_speed);

	protected:
		void Update() override;
	};
}