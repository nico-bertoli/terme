#pragma once
#include <terme/entities/moving_straight_object.h>

namespace Platformer
{
    class Obstacle : public terme::MovingStraightObject
    {
    private:
        static terme::Model model_;
        //---------------------------------------------------------- Methods
    public:
        using MovingStraightObject::MovingStraightObject;

        bool CanExitScreenSpace() const override { return true; }
        double GetGravityScale() const override { return 0; }
        terme::TerminalColor GetColor() const override { return terme::color::kRed; }

    protected:
        void OnCollisionEnter(std::shared_ptr<Collider> other, terme::Direction collision_dir) override;
        void InitModel() override;
    };
}