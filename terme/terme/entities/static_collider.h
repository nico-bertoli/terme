#pragma once
#include <terme/settings.h>
#include <terme/entities/variable_size_collider.h>

namespace terme
{
    class StaticCollider : public VariableSizeCollider
    {
        //---------------------------------------------------------- methods
    public:
        StaticCollider
        (
            int x_pos,
            int y_pos,
            size_t size_x,
            size_t size_y,
            char model_char
        );

        bool CanExitScreenSpace() const override { return true; }
        double GetGravityScale() const override { return 0; }
    };
}