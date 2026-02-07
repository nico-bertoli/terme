#pragma once
#include <terme/entities/game_object.h>
#include <optional>

namespace Platformer
{
	class SortingLayerTestObject : public terme::GameObject
	{
	private:
		size_t size_x_;
		size_t size_y_;
		char model_char_;
		terme::TerminalColor color_;
		size_t sorting_layer_;
		double move_speed_;
		std::optional<terme::Direction> main_direction_;
		terme::Model model_;

	public:
		SortingLayerTestObject
		(
			int x_pos,
			int y_pos,
			size_t size_x,
			size_t size_y,
			char model_char,
			terme::TerminalColor color,
			size_t sorting_layer,
			double move_speed = 0,
			std::optional<terme::Direction> main_direction = std::nullopt
		);

		bool CanExitScreenSpace() const override { return true; }
		double GetGravityScale() const override { return 0; }
		terme::TerminalColor GetColor() const override { return color_; }
		size_t GetSortingLayer() const override { return sorting_layer_; }

	protected:
		void InitModel() override;
		void Update()override;
	};
}