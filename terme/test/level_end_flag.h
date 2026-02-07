#pragma once
#include <terme/entities/collider.h>
#include <terme/general/vector2_int.h>
#include <terme/entities/level.h>

namespace Engine {class Level;}

namespace Platformer
{
    class LevelEndFlag : public terme::Collider
    {
        //---------------------------------------------------------- Fieldds

    public:
        nbkit::Event<> OnTouchBunny;

    private:
        static inline const terme::Model kFlagModel = terme::Model
        (
            2,
            {
                '|','\\',
                '|','/',
                '|',' '
            }
        );
        terme::Level* level_;

        //---------------------------------------------------------- Methods
    public:
        LevelEndFlag(terme::Level* level, int x_pos, int y_pos);
        terme::TerminalColor GetColor() const override { return terme::color::kGreen; }

    protected:
        bool CanExitScreenSpace() const override { return false; }
        double GetGravityScale() const override { return 0; }
        void OnCollisionEnter(std::shared_ptr<Collider> other, terme::Direction collision_dir) override;
        void InitModel() override { SetModel(kFlagModel); }
    };
}