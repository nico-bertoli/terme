#pragma once
// #include "config.h"
#include <terme/entities/collider.h>
#include <terme/general/vector2_int.h>
#include <string>

namespace Platformer
{
    class Bunny : public terme::Collider
    {
        enum class State
        {
            kIdle,
            kWalking,
            kJumpingUp,
            kJumpingDown,
            kDefeated
        };

        //---------------------------------------------------------- Model
    private:
        static constexpr char kCharNose = '.';
        static constexpr char kCharEye = 'O';
        static constexpr char kCharGameoverEye = '*';
        static constexpr char kCharChest = ':';
        static constexpr char kCharFootUp = '@';

        //todo make this iniline constexpr and init them directly in header
        static const terme::Model kModelWalkLeft;
        static const terme::Model kModelWalkRight;
        static const terme::Model kModelJumpRight;
        static const terme::Model kModelJumpLeft;
        static const terme::Model kModelDefeated;
        static const terme::Model kModelIdleRight;
        static const terme::Model kModelIdleLeft;

        //---------------------------------------------------------- Settings
    private:
        static constexpr size_t kJumpHeight = 10;
        static constexpr double kMoveUpSpeed = 24;
        static constexpr double kMoveDownSpeed = 24;
        static constexpr double kMoveDownControlledSpeed = 8;
        static constexpr double kSideMovementSpeed = 24;
        static constexpr double kStepAnimEverySeconds = 0.7f;

        //---------------------------------------------------------- Fields
    public:
        nbkit::Event<> on_obstacle_hit;
    private:
        State state;
        double last_time_moved_on_x_ = 0;
        int previous_position_x_;
        int jump_starting_y_ = -1;

        terme::Model active_model_jump_;
        terme::Model active_model_idle_;
        terme::Model active_model_walk_;
        //---------------------------------------------------------- Methods
    public:
        Bunny(int x_pos, int y_pos);

        bool CanExitScreenSpace() const override { return false; }
        double GetGravityScale() const override;
        terme::TerminalColor GetColor() const override { return terme::color::kGreen; }

    protected:
        void Update() override;
        void OnCollisionEnter(std::shared_ptr<Collider> other, terme::Direction collision_dir) override;
        void InitModel() override { SetModel(kModelIdleLeft); }

    private:
        bool IsJumping() const { return state == State::kJumpingDown || state == State::kJumpingUp; }
        void SetState(State new_state);
        void UpdateModel();
        void SwitchWalkIdleState();
        void HandleVerticalMovement();
        void HandleHorizontalMovement();
        void HandleSounds(State old_state, State new_state);
        bool IsTouchingGround() { return collisions_[terme::Direction::kDown].size() > 0; }
        bool IsJumping() { return state == State::kJumpingDown || state == State::kJumpingUp; }
        void ActivateLeftModels(bool activate);
        void OnMoveCallback(terme::Direction dir);
    };
}