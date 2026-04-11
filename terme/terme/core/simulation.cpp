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

#include <cassert>
#include <stdexcept>
#include <array>

using std::shared_ptr;
using std::weak_ptr;
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

#if DEBUG_MODE
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

	void Simulation::MarkAreaToReprint(std::shared_ptr<GameObject> obj_area)
	{
		std::unordered_set<shared_ptr<GameObject>> to_be_reprinted_objects = world_space_.GetAreaTopLayerObjects(obj_area);
		for (shared_ptr<GameObject> obj : to_be_reprinted_objects)
			obj->must_be_reprinted_ = true;
	}

	void Simulation::MarkAreaToReprintAfterMovement(std::shared_ptr<GameObject> obj, int old_pos_x, int old_pos_y)
	{
		// finding area = combination of old position area + new position area
		int min_x = obj->GetPosX() < old_pos_x ? obj->GetPosX() : old_pos_x;
		int min_y = obj->GetPosY() < old_pos_y ? obj->GetPosY() : old_pos_y;
		bool is_movement_horizontal = old_pos_x != obj->GetPosX();
		size_t width = is_movement_horizontal ? obj->GetModelWidth() + 1 : obj->GetModelWidth();
		size_t height = !is_movement_horizontal ? obj->GetModelHeight() + 1 : obj->GetModelHeight();

		std::unordered_set<shared_ptr<GameObject>> to_be_reprinted_objects = world_space_.GetAreaTopLayerObjects(min_x, min_y, width, height);

		for (shared_ptr<GameObject> obj : to_be_reprinted_objects)
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

					shared_ptr<Particle> particle = std::make_shared<Particle>(x, y, model_char, color, speed, movement_life_time, main_direction);
					TryAddEntity(particle);
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
		if(has_new_frame_been_generated);
		{
			on_frame_generated.Notify();
			#if DEBUG_MODE && SHOW_FPS
						DebugManager::Instance().ShowAverageFPS();
			#endif
		}

		on_simulation_stepped.Notify();
	}

	void Simulation::RequestMovement(shared_ptr<GameObject> applicant_obj, Direction move_dir, double move_speed)
	{
		if (IsEntityInSimulation(applicant_obj) == false)
		{
			std::cerr << "object requesting movement but it's not in simulation";
			return;
		}

		MoveRequest request(applicant_obj, move_dir, move_speed);
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
		for (weak_ptr<ISimulationEntity>& entity : to_remove_entities_)
		{
			shared_ptr<ISimulationEntity> entity_sp = entity.lock();
			if (entity_sp != nullptr)
			{
				shared_ptr<GameObject> object_entity = std::dynamic_pointer_cast<GameObject>(entity_sp);
				if (object_entity != nullptr)
				{
					object_entity->OnDestroy();
					simulation_printer_->ClearObject(object_entity);
					world_space_.RemoveObject(object_entity);
					MarkAreaToReprint(object_entity);
				}
				entities_.erase(entity_sp);
			}
		}
		to_remove_entities_.clear();
	}

	void Simulation::RemoveEntity(shared_ptr<ISimulationEntity> entity)
	{
		if (entity == nullptr)
			return;

		if (!IsEntityInSimulation(entity))
			return;

		//entity will be removed at proper stage of step
		to_remove_entities_.push_back(entity);
	}

	void Simulation::UpdateAllEntities()
	{
		level_->Update();
		for (shared_ptr<ISimulationEntity> entity : entities_)
			entity->Update();
	}

	void Simulation::ExecuteMoveRequests()
	{
		
		for (auto it = move_requests_.begin(); it != move_requests_.end(); ++it)
		{

			shared_ptr<GameObject> obj = it->object.lock();
			if (obj == nullptr)
				continue;

			int old_pos_x = obj->GetPosX();
			int old_pos_y = obj->GetPosY();

			if (TryMoveObjectAtDirection(obj, it->move_dir))
			{
				simulation_printer_->ClearArea(old_pos_x, old_pos_y, obj->GetModelWidth(), obj->GetModelHeight());
				MarkAreaToReprintAfterMovement(obj, old_pos_x, old_pos_y);
			}
		}
	}

	bool Simulation::PrintObjects()
	{
		list<shared_ptr<GameObject>> to_be_printed_objects;

		//create sorted list
		for (shared_ptr<ISimulationEntity> entity : entities_)
		{
			shared_ptr<GameObject> obj_entity = std::dynamic_pointer_cast<GameObject>(entity);
			if (obj_entity && obj_entity->must_be_reprinted_)
				GameObject::InsertInListUsingRule
				(
					obj_entity, to_be_printed_objects,
					[](shared_ptr<GameObject> to_insert_obj, shared_ptr<GameObject> list_object) { return  to_insert_obj->GetSortingLayer() <= list_object->GetSortingLayer(); }
				);
		}

		//print objects
		for (auto obj : to_be_printed_objects)
		{
			simulation_printer_->PrintObject(obj);
			UnmarkObjectToReprint(obj);
		}

		// force all objects to be printed before proceding if needed
		if(to_be_printed_objects.size() > 0)
			Terminal::Instance().Flush(); 

		return to_be_printed_objects.size() > 0;
	}


	void Simulation::UpdateAllObjectsEndedCollisions()
	{
		for (auto& entity : entities_)
		{
			shared_ptr<Collider> collider = std::dynamic_pointer_cast<Collider>(entity);
			if (collider != nullptr)
				UpdateObjectEndedCollisions(collider);
		}
	}

	void Simulation::UpdateObjectEndedCollisions(shared_ptr<Collider> collider)
	{
		int x_pos = collider->GetPosX();
		int y_pos = collider->GetPosY();
		int x_max = collider->GetMaxPosX();
		int y_max = collider->GetMaxPosY();
		size_t width = collider->GetModelWidth();
		size_t height = collider->GetModelHeight();
		bool can_exit_screen = collider->CanExitScreenSpace();

		std::array<unordered_set<shared_ptr<Collider>>, 4> collisions;

		//screen collisions
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_pos, y_max + 1))
			collisions[Direction::kUp].insert(WorldSpace::kScreenMargin);
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_pos, y_pos - 1))
			collisions[Direction::kDown].insert(WorldSpace::kScreenMargin);
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_max + 1, y_pos))
			collisions[Direction::kRight].insert(WorldSpace::kScreenMargin);
		if (!can_exit_screen && !IsCoordinateInsideScreenSpace(x_pos - 1, y_pos))
			collisions[Direction::kLeft].insert(WorldSpace::kScreenMargin);

		// object-object collisions
		world_space_.IsCollidersAreaEmpty(x_pos, y_max + 1, width, 1, collisions[Direction::kUp]);
		world_space_.IsCollidersAreaEmpty(x_pos, y_pos - 1, width, 1, collisions[Direction::kDown]);
		world_space_.IsCollidersAreaEmpty(x_pos - 1, y_pos, 1, height, collisions[Direction::kLeft]);
		world_space_.IsCollidersAreaEmpty(x_max + 1, y_pos, 1, height, collisions[Direction::kRight]);

		collider->CalledBySimUpdateEndedCollisions(collisions);
	}

	bool Simulation::TryAddEntity(shared_ptr<ISimulationEntity> entity)
	{
		shared_ptr<GameObject> object_entity = std::dynamic_pointer_cast<GameObject>(entity);

		if (object_entity != nullptr)
			object_entity->Init();

		if (!CanEntityBeAdded(entity))
			return false;
		
		if(object_entity != nullptr)
			world_space_.InsertObject(object_entity);

		entities_.insert(entity);
		return true;
	}

	bool Simulation::CanEntityBeAdded(shared_ptr<ISimulationEntity> entity) const
	{
		if (IsEntityInSimulation(entity))
			return false;

		shared_ptr<const Collider> collider = std::dynamic_pointer_cast<const Collider>(entity);
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

	bool Simulation::IsEntityInSimulation(shared_ptr<ISimulationEntity> new_entity) const
	{
		auto it = entities_.find(new_entity);
		return it != entities_.end();
	}

	bool Simulation::TryMoveObjectAtDirection(shared_ptr<GameObject> obj, Direction direction)
	{
		unordered_set<shared_ptr<Collider>> out_colliding_objects;

		shared_ptr<Collider> collider_obj = std::dynamic_pointer_cast<Collider>(obj);

		if (world_space_.CanObjectMoveAtDirection(obj, direction, out_colliding_objects) == false)
		{
			//remove entity if trying to move out of world space
			if (out_colliding_objects.find(WorldSpace::kWorldMargin) != out_colliding_objects.end())
			{
				RemoveEntity(obj);
			}
			else
			{
				if (collider_obj != nullptr)
				{
					collider_obj->CalledBySimNotifyCollisionEnter(out_colliding_objects, direction);

					for (auto colliding_obj : out_colliding_objects)
						if (colliding_obj != WorldSpace::kScreenMargin)
							colliding_obj->CalledBySimNotifyCollisionEnter(collider_obj, direction_utils::GetInverseDirection(direction));
				}
			}
			return false;
		}

		world_space_.MoveObject(obj, direction);

		obj->CalledBySimMove(direction);

		return true;
	}
}