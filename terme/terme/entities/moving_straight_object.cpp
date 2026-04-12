#include <terme/entities/moving_straight_object.h>



namespace terme
{
	MovingStraightObject::MovingStraightObject(int x_pos, int y_pos, Direction move_direction, double move_speed)
		:Collider(x_pos, y_pos), move_direction_(move_direction), move_speed_(move_speed){ }

	void MovingStraightObject::Update()
	{
		Collider::Update();

                if (can_move_)
			TryMove(move_direction_, move_speed_);
	}
}