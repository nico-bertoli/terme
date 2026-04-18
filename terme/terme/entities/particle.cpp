#include "terme/entities/particle.h"
#include "terme/core/simulation.h"

#include <nbkit/random_utils.h>

namespace terme
{
	Particle::Particle
	(
		int x_pos,
		int y_pos,
		char model_char,
		TerminalColor color,
		double move_speed,
		size_t moves_before_destruction,
		std::optional<Direction> main_direction
	)
		:
		GameObject(x_pos, y_pos),
		model_char_(model_char),
		color_(color),
		remaining_movements_before_destruction_(moves_before_destruction)
	{
		//--------- evaluate main direction

		if (!main_direction.has_value()) //main direction = random value
		{
			move_directions_[0] = static_cast<Direction>(nbkit::random_utils::GetRandomInt(0, Direction::kCount - 1));
		}
		else //main direction is chosen randomly between given direction and orthogonal ones
		{
			move_directions_[0] = main_direction.value();
		}

		//--------- evaluate orthogonal direction

		Direction orthogonal_direction = direction_utils::GetClockwiseDirection(move_directions_[0]);
		if (nbkit::random_utils::GetRandomBool())
			orthogonal_direction = direction_utils::GetInverseDirection(orthogonal_direction);
		move_directions_[1] = orthogonal_direction;

		//--------- evaluate directions speeds

		double orthogonal_speed = nbkit::random_utils::GetRandomDouble(0, move_speed);
		move_speed -= orthogonal_speed;

		move_speeds_[0] = move_speed;
		move_speeds_[1] = orthogonal_speed;

		//--------- boost horizzontal speed (cause cells are smaller on x side)
		for (int i = 0; i < move_directions_.size(); ++i)
			if (direction_utils::IsDirectionHorizontal(move_directions_[i]))
				move_speeds_[i] *= 2;

		on_move_.Subscribe([this](GameObject*, Direction) { OnMoveCallback(); });
	}

	void Particle::InitModel()
	{
		model_ = CreateModelUsingChar(model_char_, 1, 1);
		SetModel(model_);
	}

	void Particle::Update()
	{
		for (int i = 0; i < move_directions_.size(); ++i)
			TryMove(move_directions_[i], move_speeds_[i]);
	}

	void Particle::OnMoveCallback()
	{
		if (--remaining_movements_before_destruction_ == 0)
			Simulation::Instance().RemoveEntity(this);
	}
}