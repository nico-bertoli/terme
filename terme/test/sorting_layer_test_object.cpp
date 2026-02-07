#include "sorting_layer_test_object.h"

using terme::Direction;

namespace Platformer
{
	SortingLayerTestObject::SortingLayerTestObject
	(
		int x_pos,
		int y_pos,
		size_t size_x,
		size_t size_y,
		char model_char,
		terme::TerminalColor color,
		size_t sorting_layer,
		double move_speed,
		std::optional<Direction> main_direction
	) 
		: 
		GameObject(x_pos, y_pos),
		size_x_(size_x),
		size_y_(size_y),
		model_char_(model_char),
		color_(color),
		sorting_layer_(sorting_layer),
		move_speed_(move_speed),
		main_direction_(main_direction){ }

	void SortingLayerTestObject::InitModel()
	{
		model_ = CreteModelUsingChar(model_char_, size_x_, size_y_);
		SetModel(model_);
	}

	void SortingLayerTestObject::Update()
	{

	}
}