#pragma once
#include <terme/entities/collider.h>
#include <terme/general/vector2_int.h>

namespace Platformer
{
    class PressurePlate : public terme::Collider
    {
        //---------------------------------------------------------- Fields
    public:
        nbkit::Event<> on_press;
        nbkit::Event<> on_release;

    private:
        terme::Model pressed_model_;
        terme::Model unpressed_model_;

        //---------------------------------------------------------- Methods
    public:
        PressurePlate(int x_pos, int y_pos, size_t width);

        bool CanExitScreenSpace() const override { return false; }
        double GetGravityScale() const override { return 0; }
        terme::TerminalColor GetColor() const override { return terme::color::kCyan; }

    protected:
        void OnCollisionEnter(std::shared_ptr<terme::Collider> other, terme::Direction collision_dir) override;
        void OnCollisionExit(terme::Direction ending_collision_dir) override;
        void InitModel() override { SetModel(unpressed_model_); }
    };
}