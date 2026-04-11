#pragma once

#include <terme/config.h>
#include <terme/core/world_space.h>
#include <terme/printers/ui_printer.h>
#include <terme/printers/simulation_printer.h>
#include <list>
#include <unordered_set>
#include <optional>
#include <memory>
#include <nbkit/singleton.h>

namespace terme
{
	class Collider;
	class GameObject;
	class ISimulationEntity;
	class Frame;
	class SimulationPrinter;
	class UIPrinter;
	class Level;

	class Simulation : public nbkit::Singleton<Simulation>
	{
		//------------------------------------------------------------------------------------ friend classes
		friend class nbkit::Singleton<Simulation>;
		friend class GameObject;

		//------------------------------------------------------------------------------------ structs
		struct MoveRequest
		{
			std::weak_ptr<GameObject> object;
			Direction move_dir;
			double move_speed;

			MoveRequest(std::shared_ptr<GameObject> object, Direction direction, double speed)
				:object(object), move_dir(direction), move_speed(speed){ }
		};

		//------------------------------------------------------------------------------------ fields
	public:
		nbkit::Event<> on_frame_generated;
		nbkit::Event<> on_simulation_stepped;

	private:
		std::unique_ptr<SimulationPrinter> simulation_printer_;
		std::unique_ptr<UIPrinter> ui_printer_;
		Level* level_ = nullptr;
		WorldSpace world_space_;
		std::unordered_set<std::shared_ptr<ISimulationEntity>> entities_;
		std::list<std::weak_ptr<ISimulationEntity>> to_remove_entities_;

		// move requests are sorted from slower to faster
		// slower objects have to move before faster ones to prevent false collisions detection
		std::list<MoveRequest> move_requests_;

		//------------------------------------------------------------------------------------ methods
	public:
		void LoadLevel(Level& level);
		void UnloadLevel();
		void Step();
		bool TryAddEntity(std::shared_ptr<ISimulationEntity> entity);
		void RemoveEntity(std::shared_ptr<ISimulationEntity> entity);
		void RequestMovement(std::shared_ptr<GameObject> applicant_obj, Direction move_dir, double move_speed);
		size_t GetWorldSizeX() const;
		size_t GetWorldSizeY() const;
		size_t GetScreenPadding() const;
		size_t GetScreenSizeX() const;
		size_t GetScreenSizeY() const;
		Level& GetActiveLevel();
		UIPrinter& GetUIPrinter();

		void SpawnParticles
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
			std::optional<Direction> main_direction = std::nullopt
		);

	private:
		bool TryMoveObjectAtDirection(std::shared_ptr<GameObject> obj, Direction direction);
		bool CanEntityBeAdded(std::shared_ptr<ISimulationEntity> entity) const;
		bool IsEntityInSimulation(std::shared_ptr<ISimulationEntity> new_entity) const;
		void UpdateObjectEndedCollisions(std::shared_ptr<Collider> colliding_obj);
		void ResetPrinters(Level& level);
		void EnqueueMoveRequestSortingBySpeed(MoveRequest request);
		void UpdateAllEntities();
		void ExecuteMoveRequests();
		void UpdateAllObjectsEndedCollisions();
		bool PrintObjects();
		void RemoveMarkedEntities();

		bool IsInsideScreenX(int x_pos) const;
		bool IsInsideScreenY(int y_pos) const;
		bool IsCoordinateInsideScreenSpace(int x_pos, int y_pos) const { return IsInsideScreenX(x_pos) && IsInsideScreenY(y_pos); }

		void MarkAreaToReprint(std::shared_ptr<GameObject> obj_area);
		void MarkAreaToReprintAfterMovement(std::shared_ptr<GameObject> obj, int old_pos_x, int old_pos_y);
		void UnmarkObjectToReprint(std::shared_ptr<GameObject> obj) { obj->must_be_reprinted_ = false; }
	};
}
