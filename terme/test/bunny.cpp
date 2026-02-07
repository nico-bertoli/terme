#include "bunny.h"
#include <terme/entities/level.h>
#include <terme/input_manager/input_manager.h>
#include <terme/managers/time_manager.h>
#include <terme/managers/audio_manager.h>
#include <terme/core/simulation.h>
#include "obstacle.h"

using terme::Model;
using terme::Direction;
using std::weak_ptr;
using std::shared_ptr;


namespace Platformer
{
    //---------------------------------------------------------- Models
    const Model Bunny::kModelWalkLeft
    (
        5,
        {
            '(', '\\', '(', '\\', ' ' ,
            '(', kCharEye, kCharNose, kCharEye, ')',
            '/', kCharFootUp, kCharChest, '_', '|'
        }
    );


    const Model Bunny::kModelWalkRight
    (
        5,
        {
            ' ', '/', ')', '/', ')' ,
            '(', kCharEye, kCharNose, kCharEye, ')' ,
            '|', '_', kCharChest, kCharFootUp, '\\'
        }
    );


    const Model Bunny::kModelJumpRight
    (
        5,
        {
            ' ', '/', ')', '/', ')' ,
             '(', kCharEye, kCharNose, kCharEye, ')' ,
             '\\', kCharFootUp, kCharChest, kCharFootUp, '/'
        }
    );

    const Model Bunny::kModelJumpLeft
    (
        5,
        {
            '(', '\\', '(', '\\', ' ',
            '(', kCharEye, kCharNose, kCharEye, ')',
            '\\', kCharFootUp, kCharChest, kCharFootUp, '/'
        }
    );

    const Model Bunny::kModelDefeated
    (
        5,
        {
            ' ', '/', ')', '/', ')',
            '(', kCharGameoverEye, kCharNose, kCharGameoverEye, ')',
            '|', kCharFootUp, kCharChest, kCharFootUp, '|'
        }
    );

    const Model Bunny::kModelIdleRight
    (
        5,
        {
            ' ', '/', ')', '/', ')',
            '(', kCharEye, kCharNose, kCharEye, ')',
            '|', '_', kCharChest, '_', '|'
        }
    );

    const Model Bunny::kModelIdleLeft
    (
        5,
        {
         '(', '\\', '(', '\\', ' ',
         '(', kCharEye, kCharNose, kCharEye, ')',
         '|', '_', kCharChest, '_', '|'
        }
    );

    //---------------------------------------------------------- Methods

    Bunny::Bunny(int x_pos, int y_pos) : Collider(x_pos, y_pos)
    {
        SetState(State::kIdle);
        ActivateLeftModels(true);
        previous_position_x_ = GetPosX();
        on_move.Subscribe([this](weak_ptr<GameObject> _, Direction dir) { OnMoveCallback(dir); });
    }

    void Bunny::Update()
    {
        Collider::Update();

        SwitchWalkIdleState();

        if (state == State::kWalking && GetPosX() != previous_position_x_)
        {
            previous_position_x_ = GetPosX();
            bool is_time_for_left_model = terme::TimeManager::Instance().IsTimeForFirstOfTwoModels(kStepAnimEverySeconds);
            ActivateLeftModels(is_time_for_left_model);
        }

        //prevent movement when game is over
        if (terme::Simulation::Instance().GetActiveLevel()->IsGameOver() == false)
        {
            HandleVerticalMovement();
            HandleHorizontalMovement();
        }

        UpdateModel();
    }

    void Bunny::ActivateLeftModels(bool activate)
    {
        active_model_jump_ = activate ? kModelJumpLeft : kModelJumpRight;
        active_model_idle_ = activate ? kModelIdleLeft : kModelIdleRight;
        active_model_walk_ = activate ? kModelWalkLeft : kModelWalkRight;
    }

    void Bunny::UpdateModel()
    {
        switch (state)
        {
        case State::kDefeated:
            SetModel(kModelDefeated);
            break;

        case State::kIdle:
            SetModel(active_model_idle_);
            break;

        case State::kJumpingUp:
        case State::kJumpingDown:
            SetModel(active_model_jump_);
            break;

        case State::kWalking:
            SetModel(active_model_walk_);
            break;
        }
    }

    void Bunny::SwitchWalkIdleState()
    {
        if (terme::TimeManager::Instance().GetTime() - last_time_moved_on_x_ > 0.2)
        {
            if (state == State::kWalking)
                SetState(State::kIdle);
            return;
        }
        else if (state == State::kIdle)
        {
            SetState(State::kWalking);
        }
    }

    void  Bunny::HandleVerticalMovement()
    {
        bool is_pressing_space = terme::InputManager::Instance().IsKeyPressed(terme::Key::kSpace);

        if (is_pressing_space == false && IsTouchingGround() && IsJumping())
            SetState(State::kWalking);


        switch (state)
        {
        case State::kIdle:
        case State::kWalking:
            if (is_pressing_space)
            {
                jump_starting_y_ = GetPosY();
                SetState(State::kJumpingUp);
            }
            break;

        case State::kJumpingUp:
            if (is_pressing_space)
            {
                TryMove(Direction::kUp, kMoveUpSpeed);
            }
            else
            {
                SetState(State::kJumpingDown);
                return;
            }


            if (GetPosY() == jump_starting_y_ + kJumpHeight)
                SetState(State::kJumpingDown);
            break;
        }
    }

    void Bunny::HandleHorizontalMovement()
    {
        bool is_pressing_a = terme::InputManager::Instance().IsKeyPressed(terme::Key::kA);
        bool is_pressing_d = terme::InputManager::Instance().IsKeyPressed(terme::Key::kD);

        if ((is_pressing_a && is_pressing_d) || state == State::kDefeated)
            return;

        if (is_pressing_a)
            TryMove(Direction::kLeft, kSideMovementSpeed);
        if (is_pressing_d)
            TryMove(Direction::kRight, kSideMovementSpeed);
    }

    void Bunny::SetState(State new_state)
    {
        if (state == new_state)
            return;

        if (state == State::kDefeated)
            return;

        HandleSounds(state, new_state);

        state = new_state;
    }

    void Bunny::HandleSounds(State old_state, State new_state)
    {
        if (new_state == State::kJumpingUp)
            terme::AudioManager::Instance().PlayFx("resources/sounds/jump.wav", 0.1);
    }

    double Bunny::GetGravityScale() const
    {
        if (state == State::kJumpingUp)
            return 0;

        if (terme::InputManager::Instance().IsKeyPressed(terme::Key::kSpace) && state == State::kJumpingDown)
            return kMoveDownControlledSpeed;

        return kMoveDownSpeed;
    }

    void Bunny::OnMoveCallback(Direction dir)
    {
        if (dir == Direction::kRight || dir == Direction::kLeft)
            last_time_moved_on_x_ = terme::TimeManager::Instance().GetTime();
    }

    void Bunny::OnCollisionEnter(shared_ptr<Collider> other, Direction collision_dir)
    {
        if(std::dynamic_pointer_cast<Obstacle>(other))
        {
            SetState(State::kDefeated);
            on_obstacle_hit.Notify();
            return;
        }

        switch (collision_dir)
        {
        case Direction::kDown:
            SetState(State::kIdle);
            break;
        case Direction::kUp:
            if (state == State::kJumpingUp)
                SetState(State::kJumpingDown);
            break;
        }
    }
}