#include <terme/core/simulation.h>
#include <terme/entities/collider.h>
#include <terme/entities/i_simulation_entity.h>
#include <terme/entities/game_object.h>
#include <terme/entities/level.h>
#include <terme/managers/time_manager.h>
#include <terme/core/world_space.h>
#include <terme/managers/debug_manager.h>
#include <terme/entities/particle.h>
#include <terme/terminal/terminal.h>

#include <nbkit/random_utils.h>

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <array>
#include <utility>

using std::unordered_set;
using std::list;

namespace terme
{
	size_t Simulation::GetWorldSizeX() const { return level_->GetWorldSizeX(); }
	size_t Simulation::GetWorldSizeY() const { return level_->GetWorldSizeY(); }
	size_t Simulation::GetScreenPadding() const { return level_->GetScreenPadding(); }
	size_t Simulation::GetScreenSizeX() const { return level_->GetWorldSizeX() - 2 * level_->GetScreenPadding(); }
	size_t Simulation::GetScreenSizeY() const { return level_->GetWorldSizeY() - 2 * level_->GetScreenPadding(); }

	Level& Simulation::GetActiveLevel()
	{
		assert(level_ != nullptr);
		return *level_;
	}

	UIPrinter& Simulation::GetUIPrinter() { return *ui_printer_; }

	void Simulation::LoadLevel(Level& level)
	{
		this->level_ = &level;

		world_space_.Init(level.GetWorldSizeX(), level.GetWorldSizeY(), level.GetScreenPadding());

#if DEBUG
		DebugManager::Instance().Reset(GetScreenSizeX(), GetScreenSizeY(), GetScreenPadding());
#endif
		ResetPrinters(level);
		level.LoadInSimulation();
	}

	void Simulation::ResetPrinters(Level& level)
	{
		Terminal::Instance().SetDefaultColors(level.GetDefaultFrontColor(), level.GetDefaultBackColor());
		Terminal::Instance().Clear();

		simulation_printer_ = std::make_unique<SimulationPrinter>
		(
			GetScreenSizeX(),
			GetScreenSizeY(),
			GetScreenPadding(),
			level.GetBackgroundCharsColor(),
			level.GetBackgroundFileName()
		);

		ui_printer_.reset();
		ui_printer_ = std::make_unique<UIPrinter>(GetScreenSizeX(), GetScreenSizeY(), GetScreenPadding(), level.GetMarginsColor());
	}

	bool Simulation::IsInsideScreenY(int y_pos) const
	{
		return
			y_pos >= GetScreenPadding() &&
			y_pos < GetWorldSizeY() - GetScreenPadding();
	}
	bool Simulation::IsInsideScreenX(int x_pos) const
	{
		return
			x_pos >= GetScreenPadding() &&
			x_pos < GetWorldSizeX() - GetScreenPadding();
	}

	void Simulation::MarkAreaToReprint(GameObject* obj_area)
	{
		std::unordered_set<GameObject*> to_be_reprinted_objects = world_space_.GetAreaTopLayerObjects(obj_area);
		for (GameObject* obj : to_be_reprinted_objects)
			obj->must_be_reprinted_ = true;
	}

	void Simulation::MarkAreaToReprintAfterMovement(GameObject* obj, int old_pos_x, int old_pos_y)
	{
		int min_x = obj->GetPosX() < old_pos_x ? obj->GetPosX() : old_pos_x;
		int min_y = obj->GetPosY() < old_pos_y ? obj->GetPosY() : old_pos_y;
		bool is_movement_horizontal = old_pos_x != obj->GetPosX();
		size_t width = is_movement_horizontal ? obj->GetModelWidth() + 1 : obj->GetModelWidth();
		size_t height = !is_movement_horizontal ? obj->GetModelHeight() + 1 : obj->GetModelHeight();

		std::unordered_set<GameObject*> to_be_reprinted_objects = world_space_.GetAreaTopLayerObjects(min_x, min_y, width, height);

		for (GameObject* obj : to_be_reprinted_objects)
			obj->must_be_reprinted_ = true;
	}

	void Simulation::UnloadLevel()
	{
		level_ = nullptr;
		entities_.clear();
		to_remove_entities_.clear();
		move_requests_.clear();
		world_space_.Init(1, 1, 0);
		simulation_printer_.reset();
		ui_printer_.reset();
	}

	void Simulation::SpawnParticles
	(
		int pos_x,
		int pos_y,
		size_t width,
		size_t height,
		char model_char,
		TerminalColor color,
		double speed,
		size_t movement_life_time,
		double density,
		std::optional<Direction> main_direction
	)
	{
		int density_int_part = static_cast<int>(density);
		double density_decimal_part = density - density_int_part;

		for (int y = pos_y; y < pos_y + height; ++y)
		{
			for (int x = pos_x; x < pos_x + width; ++x)
			{
				//add particles depending on density int part
				for (int density = 0; density <= density_int_part; ++density)
				{
					//add particles depending on density fractional part
					if (density == density_int_part && nbkit::random_utils::GetRandomDouble(0, 1) > density_decimal_part)
						continue;

					std::unique_ptr<ISimulationEntity> particle = std::make_unique<Particle>(x, y, model_char, color, speed, movement_life_time, main_direction);
					TryAddEntity(std::move(particle));
				}
			}
		}
	}

	void Simulation::Step()
	{
		if (level_->IsTerminated())
			return;

		if (level_->IsPostGameOverPauseEnded())
		{
			level_->Update();
			return;
		}

		move_requests_.clear();

		UpdateAllEntities();

		ExecuteMoveRequests();

		RemoveMarkedEntities();

		UpdateAllObjectsEndedCollisions();

		bool has_new_frame_been_generated = PrintObjects();
		if (has_new_frame_been_generated)
		{
			on_frame_generated.Notify();
#if DEBUG && SHOW_FPS
			DebugManager::Instance().ShowAverageFPS();
#endif
		}

		on_simulation_stepped.Notify();
	}

	void Simulation::RequestMovement(GameObject* applicant_obj, Direction move_direction, double move_speed)
	{
		if (IsEntityInSimulation(applicant_obj) == false)
		{
			std::cerr << "object requesting movement but it's not in simulation";
			return;
		}

		MoveRequest request(applicant_obj, move_direction, move_speed);
		EnqueueMoveRequestSortingBySpeed(request);
	}

	void Simulation::EnqueueMoveRequestSortingBySpeed(MoveRequest request)
	{
		auto it = move_requests_.begin();
		while (it != move_requests_.end())
		{
			if (it->move_speed > request.move_speed)
				break;
			++it;
		}
		move_requests_.insert(it, request);
	}

	void Simulation::RemoveMarkedEntities()
	{
		for (ISimulationEntity* entity : to_remove_entities_)
		{
			auto it = entities_.find(entity);
			if (it == entities_.end())
				continue;
			GameObject* object_entity = dynamic_cast<GameObject*>(entity);
			if (object_entity != nullptr)
			{
				object_entity->OnDestroy();
				simulation_printer_->ClearObject(object_entity);
				world_space_.RemoveObject(object_entity);
				MarkAreaToReprint(object_entity);
			}
			entities_.erase(it);
		}
		to_remove_entities_.clear();
	}

	void Simulation::RemoveEntity(ISimulationEntity* entity)
	{
		if (entity == nullptr)
			return;

		if (!IsEntityInSimulation(entity))
			return;

		to_remove_entities_.push_back(entity);
	}

	void Simulation::UpdateAllEntities()
	{
		level_->Update();
		for (auto& kv : entities_)
			kv.second->Update();
	}

	void Simulation::ExecuteMoveRequests()
	{
		for (auto it = move_requests_.begin(); it != move_requests_.end(); ++it)
		{
			GameObject* obj = it->object;
			if (obj == nullptr)
				continue;

			int old_pos_x = obj->GetPosX();
			int old_pos_y = obj->GetPosY();

			if (TryMoveObjectAtDirection(obj, it->move_direction))
			{
				simulation_printer_->ClearArea(old_pos_x, old_pos_y, obj->GetModelWidth(), obj->GetModelHeight());
				MarkAreaToReprintAfterMovement(obj, old_pos_x, old_pos_y);
			}
		}
	}

	bool Simulation::PrintObjects()
	{
		list<GameObject*> to_be_printed_objects;

		for (auto& kv : entities_)
		{
			GameObject* obj_entity = dynamic_cast<GameObject*>(kv.second.get());
			if (obj_entity && obj_entity->must_be_reprinted_)
			{
				auto it = std::find_if
				(
					to_be_printed_objects.begin(),
					to_be_printed_objects.end(),
					[obj_entity](GameObject* list_object) { return obj_entity->GetSortingLayer() <= list_object->GetSortingLayer(); }
				);
				to_be_printed_objects.insert(it, obj_entity);
			}
		}

		for (GameObject* obj : to_be_printed_objects)
		{
			simulation_printer_->PrintObject(obj);
			UnmarkObjectToReprint(obj);
		}

		if (to_be_printed_objects.size() > 0)
			Terminal::Instance().Flush();

		return to_be_printed_objects.size() > 0;
	}


	void Simulation::UpdateAllObjectsEndedCollisions()
	{
		for (auto& kv : entities_)
		{
			Collider* collider = dynamic_cast<Collider*>(kv.second.get());
			if (collider != nullptr)
				UpdateObjectEndedCollisions(collider);
		}
	}

	void Simulation::UpdateObjectEndedCollisions(Collider* collider)
	{
		int x_pos = collider->GetPosX();
		int y_pos = collider->GetPosY();
		int x_max = collider->GetMaxPosX();
		int y_max = collider->GetMaxPosY();
		size_t width = collider->GetModelWidth();
		size_t height = collider->GetModelHeight();
		bool can_exit_screen = collider->CanExitScreenSpace();

		std::array<unordered_set<Collider*>, 4> collisions;

		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_pos, y_max + 1))
			collisions[Direction::kUp].insert(WorldSpace::kScreenMargin);
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_pos, y_pos - 1))
			collisions[Direction::kDown].insert(WorldSpace::kScreenMargin);
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_max + 1, y_pos))
			collisions[Direction::kRight].insert(WorldSpace::kScreenMargin);
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_pos - 1, y_pos))
			collisions[Direction::kLeft].insert(WorldSpace::kScreenMargin);

		world_space_.IsCollidersAreaEmpty(x_pos, y_max + 1, width, 1, collisions[Direction::kUp]);
		world_space_.IsCollidersAreaEmpty(x_pos, y_pos - 1, width, 1, collisions[Direction::kDown]);
		world_space_.IsCollidersAreaEmpty(x_pos - 1, y_pos, 1, height, collisions[Direction::kLeft]);
		world_space_.IsCollidersAreaEmpty(x_max + 1, y_pos, 1, height, collisions[Direction::kRight]);

		collider->CalledBySim_UpdateEndedCollisions(collisions);
	}

	bool Simulation::TryAddEntity(std::unique_ptr<ISimulationEntity>&& entity_ptr)
	{
		if (!entity_ptr)
			return false;

		ISimulationEntity* entity = entity_ptr.get();
		GameObject* object_entity = dynamic_cast<GameObject*>(entity);
		if (object_entity != nullptr)
			object_entity->Init();

		if (!CanEntityBeAdded(entity))
			return false;

		if (object_entity != nullptr)
			world_space_.InsertObject(object_entity);

		entities_.emplace(entity, std::move(entity_ptr));
		return true;
	}

	bool Simulation::CanEntityBeAdded(ISimulationEntity* entity) const
	{
		if (IsEntityInSimulation(entity))
			return false;

		const Collider* collider = dynamic_cast<const Collider*>(entity);
		if (collider != nullptr)
		{
			return world_space_.IsCollidersAreaEmpty
			(
				collider->GetPosX(),
				collider->GetPosY(),
				collider->GetModelWidth(),
				collider->GetModelHeight()
			);
		}
		else
			return true;
	}

	bool Simulation::IsEntityInSimulation(const ISimulationEntity* entity) const
	{
		return entities_.find(const_cast<ISimulationEntity*>(entity)) != entities_.end();
	}

	bool Simulation::TryMoveObjectAtDirection(GameObject* obj, Direction direction)
	{
		unordered_set<Collider*> out_colliding_objects;

		Collider* collider_obj = dynamic_cast<Collider*>(obj);

		if (world_space_.CanObjectMoveAtDirection(obj, direction, out_colliding_objects) == false)
		{
			if (out_colliding_objects.find(WorldSpace::kWorldMargin) != out_colliding_objects.end())
			{
				RemoveEntity(obj);
			}
			else
			{
				if (collider_obj != nullptr)
				{
					collider_obj->CalledBySim_NotifyCollisionEnter(out_colliding_objects, direction);

					for (Collider* colliding_obj : out_colliding_objects)
						if (colliding_obj != WorldSpace::kScreenMargin)
							colliding_obj->CalledBySim_NotifyCollisionEnter(collider_obj, direction_utils::GetInverseDirection(direction));
				}
			}
			return false;
		}

		world_space_.MoveObject(obj, direction);

		obj->CalledBySimMove(direction);

		return true;
	}
}