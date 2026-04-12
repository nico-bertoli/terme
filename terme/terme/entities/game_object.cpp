#include <terme/entities/game_object.h>
#include <terme/managers/time_manager.h>
#include <terme/core/simulation.h>

namespace terme
{
	GameObject::GameObject(int x_pos, int y_pos) :
		x_pos_(x_pos),
		y_pos_(y_pos),
		x_pos_continuous_(x_pos),
		y_pos_continuous_(y_pos)
	{
		ResetPartialMovement();
	}

	void GameObject::Update()
	{
		ApplyGravity();
	}

	void GameObject::ApplyGravity()
	{
		double gravity_scale = GetGravityScale();
		if (gravity_scale == 0)
			return;
		if (gravity_scale > 0)
			TryMove(Direction::kDown, gravity_scale);
		else
			TryMove(Direction::kUp, gravity_scale);
	}

	void GameObject::TryMove(Direction direction, double move_speed)
	{
		if (can_move_ == false)
			return;

		move_speed = abs(move_speed);
		double delta_time = TimeManager::Instance().GetFixedDeltaTime();

		switch (direction)
		{
		case Direction::kUp:
			y_pos_continuous_ += move_speed * delta_time;
			break;
		case Direction::kDown:
			y_pos_continuous_ -= move_speed * delta_time;
			break;
		case Direction::kRight:
			x_pos_continuous_ += move_speed * delta_time;
			break;
		case Direction::kLeft:
			x_pos_continuous_ -= move_speed * delta_time;
			break;
		}

		if (direction == Direction::kLeft || direction == Direction::kRight)
		{
			if (round(x_pos_continuous_) != x_pos_)
				Simulation::Instance().RequestMovement(this, direction, move_speed);
		}
		else if (round(y_pos_continuous_) != y_pos_)
			Simulation::Instance().RequestMovement(this, direction, move_speed);
	}

	Model GameObject::CreteModelUsingChar(char model_char, size_t size_x, size_t size_y) const
	{
		Model result;
		result.Resize(size_x, size_y);

		for (auto& elem : result)
			elem = model_char;

		return result;
	}

	void GameObject::SetModel(const Model& new_model)
	{
		if (model_ == &new_model)
			return;

		model_ = &new_model;

		Simulation::Instance().MarkAreaToReprint(this);
	}

	void GameObject::CalledBySimMove(Direction direction)
	{
		switch (direction)
		{
		case Direction::kUp:
			++y_pos_;
			y_pos_continuous_ = y_pos_;
			break;
		case Direction::kDown:
			--y_pos_;
			y_pos_continuous_ = y_pos_;
			break;
		case Direction::kRight:
			++x_pos_;
			x_pos_continuous_ = x_pos_;
			break;
		case Direction::kLeft:
			--x_pos_;
			x_pos_continuous_ = x_pos_;
			break;
		}
		on_move.Notify(this, direction);
	}
}
