#pragma once
#include <terme/config.h>
#include <terme/core/direction.h>
#include <terme/entities/i_simulation_entity.h>
#include <terme/terminal/terminal.h>
#include <terme/general/vector2_int.h>
#include <nbkit/matrix.h>
#include <nbkit/event.h>

namespace terme
{
	using Model = nbkit::Matrix<char>;

	class GameObject : public ISimulationEntity
	{
		friend class Simulation;

		//---------------------------------------------------------- fields
	public:
		nbkit::Event<GameObject*, Direction> on_move;
		// generic on destroy event could be added

	protected:
		bool can_move_ = true;

	private:
		int x_pos_;
		int y_pos_;

		double x_pos_continuous_;
		double y_pos_continuous_;

		/// never set this value directly, use simulation MarkObjectToReprint
		bool must_be_reprinted_ = true;
		const Model* model_ = nullptr;

		//---------------------------------------------------------- methods
	public:
		GameObject(int x_pos, int y_pos);

		virtual TerminalColor GetColor() const { return nullptr; }
		virtual TerminalColor GetBackColor() const { return nullptr; }
		virtual size_t GetSortingLayer() const { return 100; }
		void Init() { InitModel(); }
		int GetPosX() const { return x_pos_; }
		int GetPosY() const { return y_pos_; }
		int GetMaxPosX()const { return x_pos_ + static_cast<int>(GetModelWidth()) - 1; }
		int GetMaxPosY()const { return y_pos_ + static_cast<int>(GetModelHeight()) - 1; }
		int GetMidPosX() { return GetPosX() + static_cast<int>(GetModelWidth()) / 2; }
		int GetMidPosY() { return GetPosY() + static_cast<int>(GetModelHeight()) / 2; }
		size_t GetModelWidth()const { return model_->GetSizeX(); };
		size_t GetModelHeight()const { return model_->GetSizeY(); }
		const Model& GetModel()const { return *model_; }
		virtual bool CanExitScreenSpace() const = 0;

	protected:
		virtual void InitModel() = 0;
		virtual double GetGravityScale() const = 0;
		virtual void OnDestroy() {} // called by simulation
		virtual void TryMove(Direction direction, double move_speed);
		void Update()override;
		Model CreteModelUsingChar(char model_char, size_t size_x, size_t size_y) const;
		void SetModel(const Model& new_model);

	private:
		void ResetPartialMovement() { x_pos_continuous_ = x_pos_; y_pos_continuous_ = y_pos_; }
		void ApplyGravity();
		void CalledBySimMove(Direction direction);
	};


}