#include "level_end_flag.h"
#include "bunny.h"
#include <terme/entities/level.h>

using terme::Model;
using terme::Direction;
using terme::Level;
using std::shared_ptr;

namespace Platformer
{
    LevelEndFlag::LevelEndFlag(Level* level, int x_pos, int y_pos) : Collider(x_pos, y_pos), level_(level) {}

    void LevelEndFlag::OnCollisionEnter(shared_ptr<Collider> other, Direction collision_dir)
    {
        if(std::static_pointer_cast<Bunny>(other))
            OnTouchBunny.Notify();
    }
}