#pragma once

#include "terme/core/world_space.h"
#include "terme/printers/simulation_printer.h"
#include "terme/printers/ui_printer.h"

#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

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
	private:
		struct MoveRequest
		{
			GameObject* object;
			Direction move_direction;
			double move_speed;

			MoveRequest(GameObject* object, Direction direction, double speed)
				: object(object), move_direction(direction), move_speed(speed) { }
		};

		//------------------------------------------------------------------------------------ fields
	public:
		nbkit::Event<> on_frame_generated_;
		nbkit::Event<> on_simulation_stepped_;

	private:
		std::unique_ptr<SimulationPrinter> simulation_printer_;
		std::unique_ptr<UIPrinter> ui_printer_;
		Level* level_ = nullptr;
		WorldSpace world_space_;
		std::unordered_map<ISimulationEntity*, std::unique_ptr<ISimulationEntity>> entities_;
		std::list<ISimulationEntity*> to_remove_entities_;

		std::list<MoveRequest> move_requests_;

		//------------------------------------------------------------------------------------ methods
	public:
		void LoadLevel(Level& level);
		void UnloadLevel();
		void Step();
		bool TryAddEntity(std::unique_ptr<ISimulationEntity>&& entity_ptr);
		void RemoveEntity(ISimulationEntity* entity);
		void RequestMovement(GameObject* applicant_obj, Direction move_direction, double move_speed);
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
		bool TryMoveObjectAtDirection(GameObject* obj, Direction direction);
		bool CanEntityBeAdded(ISimulationEntity* entity) const;
		bool IsEntityInSimulation(const ISimulationEntity* entity) const;
		void UpdateObjectEndedCollisions(Collider* colliding_obj);
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

		void MarkAreaToReprint(GameObject* obj_area);
		void MarkAreaToReprintAfterMovement(GameObject* obj, int old_pos_x, int old_pos_y);
		void UnmarkObjectToReprint(GameObject* obj) { obj->must_be_reprinted_ = false; }
	};
}
