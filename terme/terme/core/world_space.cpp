#include <terme/core/world_space.h>
#include <terme/entities/collider.h>
#include <terme/entities/game_object.h>

#include <algorithm>

using std::unordered_set;

namespace terme
{
	void WorldSpace::Init(int x_size, int y_size, size_t screen_padding)
	{
		space_.Clear();
		space_.Resize(x_size, y_size);

		for (Cell& cell : space_)
			cell.objects.clear();

		this->screen_padding_ = screen_padding;
	}

	void WorldSpace::InsertObject(GameObject* obj)
	{
		WriteSpace(obj->GetPosX(), obj->GetPosY(), obj->GetModelWidth(), obj->GetModelHeight(), obj);
	}

	void WorldSpace::RemoveObject(GameObject* obj)
	{
		EraseSpace(obj->GetPosX(), obj->GetPosY(), obj->GetModelWidth(), obj->GetModelHeight(), obj);
	}

	void WorldSpace::MoveObject(GameObject* obj, Direction direction)
	{
		switch (direction)
		{
		case Direction::kUp:
		{
			int y_write = obj->GetMaxPosY() + 1;
			int y_clear = obj->GetPosY();
			WriteSpace(obj->GetPosX(), y_write, obj->GetModelWidth(), 1, obj);
			EraseSpace(obj->GetPosX(), y_clear, obj->GetModelWidth(), 1, obj);
			break;
		}
		case Direction::kDown:
		{
			int y_write = obj->GetPosY() - 1;
			int y_clear = obj->GetMaxPosY();
			WriteSpace(obj->GetPosX(), y_write, obj->GetModelWidth(), 1, obj);
			EraseSpace(obj->GetPosX(), y_clear, obj->GetModelWidth(), 1, obj);
			break;
		}
		case Direction::kRight:
		{
			int x_write = obj->GetMaxPosX() + 1;
			int x_clear = obj->GetPosX();
			WriteSpace(x_write, obj->GetPosY(), 1, obj->GetModelHeight(), obj);
			EraseSpace(x_clear, obj->GetPosY(), 1, obj->GetModelHeight(), obj);
			break;
		}

		case Direction::kLeft:
		{
			int x_write = obj->GetPosX() - 1;
			int x_clear = obj->GetMaxPosX();
			WriteSpace(x_write, obj->GetPosY(), 1, obj->GetModelHeight(), obj);
			EraseSpace(x_clear, obj->GetPosY(), 1, obj->GetModelHeight(), obj);
			break;
		}
		}
	}

	void WorldSpace::WriteSpace(int x_start, int y_start, size_t width, size_t height, GameObject* obj)
	{
		assert(obj != nullptr);

		for (int y = y_start; y < y_start + height; ++y)
			for (int x = x_start; x < x_start + width; ++x)
			{
				Cell& cell = space_.Get(x, y);

				{
					// add high sorting layer objects at top of list
					auto it = std::find_if
					(
						cell.objects.begin(),
						cell.objects.end(),
						[obj](GameObject* list_item) { return obj->GetSortingLayer() >= list_item->GetSortingLayer(); }
					);
					cell.objects.insert(it, obj);
				}

				Collider* obj_collider = dynamic_cast<Collider*>(obj);
				if (obj_collider)
				{
					assert(cell.collider == nullptr || cell.collider == obj_collider);
					cell.collider = obj_collider;
				}
			}
	}

	void WorldSpace::EraseSpace(int x_start, int y_start, size_t width, size_t height, GameObject* obj)
	{
		assert(obj != nullptr);

		for (int y = y_start; y < y_start + height; ++y)
			for (int x = x_start; x < x_start + width; ++x)
			{
				Cell& cell = space_.Get(x, y);

				//------------------ erase from cell.objects
				for (auto it = cell.objects.begin(); it != cell.objects.end(); ++it)
				{
					if (*it == obj)
					{
						cell.objects.erase(it);
						break;
					}
				}

				//------------------ erase collider
				Collider* obj_collider = dynamic_cast<Collider*>(obj);
				if (obj_collider)
				{
					assert(cell.collider == nullptr || cell.collider == obj_collider);
					cell.collider = nullptr;
				}
			}
	}

	bool WorldSpace::IsCollidersAreaEmpty(int starting_x, int starting_y, size_t width, size_t height, unordered_set<Collider*>& out_area_objects) const
	{
		for (int y = starting_y; y < starting_y + int(height); ++y)
		{
			for (int x = starting_x; x < starting_x + int(width); ++x)
			{
				Collider* cell_collider = space_.Get(x, y).collider;
				if (IsCoordinateInsideSpace(x, y) && cell_collider != nullptr)
					out_area_objects.insert(cell_collider);
			}
		}

		return out_area_objects.size() == 0;
	}

	bool WorldSpace::IsCollidersAreaEmpty(int starting_x, int starting_y, size_t width, size_t height) const
	{
		for (int y = starting_y; y < starting_y + height; ++y)
			for (int x = starting_x; x < starting_x + width; ++x)
				if (IsCoordinateInsideSpace(x, y) && space_.Get(x, y).collider != nullptr)
					return false;

		return true;
	}


	bool WorldSpace::IsCoordinateInsideSpace(int x_pos, int y_pos) const
	{
		return IsInsideSpaceX(x_pos) && IsInsideSpaceY(y_pos);
	}


	bool WorldSpace::IsInsideSpaceX(int x_pos) const
	{
		return x_pos >= 0 && x_pos < space_.GetSizeX();
	}

	bool WorldSpace::IsInsideSpaceY(int y_pos) const
	{
		return y_pos >= 0 && y_pos < space_.GetSizeY();
	}

	bool WorldSpace::CanObjectMoveAtDirection
	(
		const GameObject* object,
		Direction direction,
		unordered_set<Collider*>& colliding_objects
	) const
	{
		const Collider* collider_obj = dynamic_cast<const Collider*>(object);

		switch (direction)
		{
		case Direction::kUp:
		{
			int moving_to_y = object->GetMaxPosY() + 1;

			//exiting world
			if (moving_to_y == space_.GetSizeY())
			{
				colliding_objects.insert(kWorldMargin);
				return false;
			}

			//exiting screen space
			if ((object->CanExitScreenSpace() == false) && (moving_to_y == space_.GetSizeY() - screen_padding_))
			{
				colliding_objects.insert(kScreenMargin);
				return false;
			}

//obj collision
			if (collider_obj != nullptr && IsCollidersAreaEmpty(collider_obj->GetPosX(), moving_to_y, collider_obj->GetModelWidth(), 1, colliding_objects) == false)
				return false;

			return true;
		}
		case Direction::kDown:
		{
			int moving_to_y = object->GetPosY() - 1;

//exiting world
			if (moving_to_y == -1)
			{
				colliding_objects.insert(kWorldMargin);
				return false;
			}

//exiting screen space
			if ((object->CanExitScreenSpace() == false) && (moving_to_y == static_cast<int>(screen_padding_) - 1))
			{
				colliding_objects.insert(kScreenMargin);
				return false;
			}

//obj collision
			if (collider_obj != nullptr && IsCollidersAreaEmpty(collider_obj->GetPosX(), moving_to_y, collider_obj->GetModelWidth(), 1, colliding_objects) == false)
				return false;

			return true;
		}
		case Direction::kRight:
		{
			int moving_to_x = object->GetMaxPosX() + 1;

			//exiting world
			if (moving_to_x == space_.GetSizeX())
			{
				colliding_objects.insert(kWorldMargin);
				return false;
			}

//exiting screen space
			if ((object->CanExitScreenSpace() == false) && (moving_to_x == space_.GetSizeX() - screen_padding_))
			{
				colliding_objects.insert(kScreenMargin);
				return false;
			}

//obj collision
			if (collider_obj != nullptr && IsCollidersAreaEmpty(moving_to_x, collider_obj->GetPosY(), 1, collider_obj->GetModelHeight(), colliding_objects) == false)
				return false;

			return true;
		}
		case Direction::kLeft:
		{
			int moving_to_x = object->GetPosX() - 1;

//exiting world
			if (moving_to_x == -1)
			{
				colliding_objects.insert(kWorldMargin);
				return false;
			}

//exiting screen space
			if ((object->CanExitScreenSpace() == false) && (moving_to_x == screen_padding_ - 1))
			{
				colliding_objects.insert(kScreenMargin);
				return false;
			}

//obj collision
			if (collider_obj != nullptr && IsCollidersAreaEmpty(moving_to_x, collider_obj->GetPosY(), 1, collider_obj->GetModelHeight(), colliding_objects) == false)
				return false;

			return true;
		}
		default:
			throw std::invalid_argument("Invalid direction");
		}
	}

	unordered_set<GameObject*> WorldSpace::GetAreaTopLayerObjects(GameObject* obj)
	{
		return GetAreaTopLayerObjects(obj->GetPosX(), obj->GetPosY(), obj->GetModelWidth(), obj->GetModelHeight());
	}

	unordered_set<GameObject*> WorldSpace::GetAreaTopLayerObjects(int starting_x, int starting_y, size_t width, size_t height)
	{
		unordered_set<GameObject*> objects;
		for (int y = starting_y; y < starting_y + height; ++y)
		{
			for (int x = starting_x; x < starting_x + width; ++x)
			{
				Cell& cell = space_.Get(x, y);
				if (!cell.objects.empty())
					objects.insert(cell.objects.front());
			}
		}
		return objects;
	}
}