#pragma once

#include "terme/entities/collider.h"

namespace terme
{
    class VariableSizeCollider : public Collider
    {
        //---------------------------------------------------------- fields
        char model_char_;
        size_t model_size_x_;
        size_t model_size_y_;
        Model model_;
        //---------------------------------------------------------- methods
    public:
        VariableSizeCollider
        (
            int x_pos,
            int y_pos,
            size_t model_size_x,
            size_t model_size_y,
            char model_char
        ) :
            Collider(x_pos, y_pos),
            model_size_x_(model_size_x),
            model_size_y_(model_size_y),
            model_char_(model_char)
        {
            model_ = CreateModelUsingChar(model_char, model_size_x, model_size_y);
        }

    protected:
        void InitModel() override { SetModel(model_); }
    };
}