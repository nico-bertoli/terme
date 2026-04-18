#pragma once

#include "terme/core/direction.h"
#include "terme/entities/fake_collider.h"

#include <cassert>
#include <list>
#include <unordered_set>

#include <nbkit/matrix.h>

namespace terme
{
	class WorldSpace
	{
		//---------------------------------------------------------- structs
	private:
		struct Cell
		{
			// cell collider, max 1 for cell
			Collider* collider = nullptr;
			
			// all objects in cell, including collider if present
			// (using list cause there are a lot of small sets (2/3 elements max per list usually)
			std::list<GameObject*> objects;
		};

		//---------------------------------------------------------- margins
	
	private:
		static inline FakeCollider world_margin_instance_;
		static inline FakeCollider screen_margin_instance_;

	public:
		static inline Collider* kWorldMargin = &world_margin_instance_;
		static inline Collider* kScreenMargin = &screen_margin_instance_;

		//---------------------------------------------------------- fields
	private:
		nbkit::Matrix<Cell> space_;
		size_t screen_padding_;
		//---------------------------------------------------------- methods
	public:
		void Init(int x_size, int y_size, size_t screen_padding);

		bool CanObjectMoveAtDirection(const GameObject* obj, Direction direction, std::unordered_set<Collider*>& colliding_objects) const;
		void InsertObject(GameObject* obj);
		void RemoveObject(GameObject* obj);
		void MoveObject(GameObject* obj, Direction direction);
		bool IsCoordinateInsideSpace(int x_pos, int y_pos) const;
		bool IsInsideSpaceX(int x_pos) const;
		bool IsInsideSpaceY(int y_pos) const;

		bool IsCollidersAreaEmpty(int starting_x, int starting_y, size_t width, size_t height, std::unordered_set<Collider*>& out_area_objects) const;
		bool IsCollidersAreaEmpty(int starting_x, int starting_y, size_t width, size_t height) const;

		std::unordered_set<GameObject*> GetAreaTopLayerObjects(GameObject* obj);
		std::unordered_set<GameObject*> GetAreaTopLayerObjects(int starting_x, int starting_y, size_t width, size_t height);

	private:
		void WriteSpace(int x_start, int y_start, size_t width, size_t height, GameObject* obj);
		void EraseSpace(int x_start, int y_start, size_t width, size_t height, GameObject* obj);
	};
}
