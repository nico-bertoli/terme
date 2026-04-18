#include "terme/entities/static_collider.h"

namespace terme
{
    StaticCollider::StaticCollider
    (
        int x_pos,
        int y_pos,
        size_t size_x,
        size_t size_y,
        char model_char
    ) : VariableSizeCollider(x_pos, y_pos, size_x, size_y, model_char)
    {
        can_move_ = false;
    }
}