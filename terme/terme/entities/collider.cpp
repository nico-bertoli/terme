#include "terme/entities/collider.h"
#include "terme/core/simulation.h"

using std::list;
using std::unordered_set;

namespace terme
{
	namespace
	{
		bool PointerListContains(const std::list<Collider*>& list, Collider* p)
		{
			for (Collider* item : list)
				if (item == p)
					return true;
			return false;
		}

		void RemovePointerFromList(std::list<Collider*>& list, Collider* target)
		{
			if (target == nullptr)
				return;
			for (auto it = list.begin(); it != list.end(); ++it)
			{
				if (*it == target)
				{
					list.erase(it);
					return;
				}
			}
		}
	}

	void Collider::CalledBySim_NotifyCollisionEnter(unordered_set<Collider*> colliding_objects, Direction collision_dir)
	{
		list<Collider*>& local_direction_coll = collisions_[collision_dir];
		for (Collider* obj : colliding_objects)
		{
			if (PointerListContains(local_direction_coll, obj) == false)
			{
				local_direction_coll.push_back(obj);
				OnCollisionEnter(obj, collision_dir);
			}
		}
	}

	void Collider::CalledBySim_NotifyCollisionEnter(Collider* colliding_object, Direction collision_dir)
	{
		CalledBySim_NotifyCollisionEnter(unordered_set<Collider*>{colliding_object}, collision_dir);
	}

	void Collider::CalledBySim_UpdateEndedCollisions(const std::array<unordered_set<Collider*>, 4>& new_collisions)
	{
		for (int i = 0; i < new_collisions.size(); ++i)
		{
			list<Collider*>& local_direction_coll = collisions_[i];
			const unordered_set<Collider*>& new_direction_coll = new_collisions[i];

			list<Collider*> to_remove;

			for (Collider* collider : local_direction_coll)
			{
				if (collider == nullptr || new_direction_coll.find(collider) == new_direction_coll.end())
					to_remove.push_back(collider);
			}

			for (Collider* to_remove_obj : to_remove)
				RemovePointerFromList(local_direction_coll, to_remove_obj);

			// call on collision exit
			if (to_remove.size() > 0)
				OnCollisionExit(static_cast<Direction>(i));
		}
	}
}