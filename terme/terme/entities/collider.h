#pragma once

#include "terme/entities/game_object.h"

#include <array>
#include <list>
#include <unordered_set>

namespace terme
{
	class Collider : public GameObject
	{
		friend class Simulation;

		//---------------------------------------------------------- fields

	protected:
		std::array<std::list<Collider*>, 4> collisions_;

		//---------------------------------------------------------- methods
	public:
		using GameObject::GameObject;

	protected:

		virtual void OnCollisionEnter(Collider* other, Direction collision_dir) {}
		virtual void OnCollisionExit(Direction ending_collision_dir) {}

	private:
		void CalledBySim_NotifyCollisionEnter(std::unordered_set<Collider*> colliding_objects, Direction collision_dir);
		void CalledBySim_NotifyCollisionEnter(Collider* colliding_object, Direction collision_dir);
		void CalledBySim_UpdateEndedCollisions(const std::array<std::unordered_set<Collider*>, 4>& new_collisions);
	};
}