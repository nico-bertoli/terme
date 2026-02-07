#pragma once
#include <terme/entities/collider.h>
#include <terme/managers/audio_manager.h>
#include <terme/core/direction.h>

#include "bunny.h"

namespace Platformer
{
    class PushableObject : public terme::Collider
    {
        //---------------------------------------------------------- Fields
    private:
        terme::Model model_ = CreteModelUsingChar('@', 4, 2);
        //----------------------------------------------F------------ Methods
    public:
        using Collider::Collider;
        bool CanExitScreenSpace() const override { return false; }
        double GetGravityScale() const override { return 8; }
        terme::TerminalColor GetColor() const override { return terme::color::kBlue; }

    protected:
        void InitModel() override { SetModel(model_); }
        void OnCollisionEnter(std::shared_ptr<Collider> other, terme::Direction collision_dir)override
        {
            if (collision_dir == terme::Direction::kRight || collision_dir == terme::Direction::kLeft)
            {
                TryMove(terme::direction_utils::GetInverseDirection(collision_dir), 9999);
                collisions_[collision_dir].clear();
            }
            else
            {
                if(std::dynamic_pointer_cast<Bunny>(other))
                   terme::AudioManager::Instance().PlayFx("resources/sounds/hit.wav", 0.2);
            }
        }
    };
}