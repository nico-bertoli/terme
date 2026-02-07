#include "obstacle.h"
#include <terme/managers/audio_manager.h>
#include <terme/managers/time_manager.h>

using std::shared_ptr;
using terme::Model;
using terme::Direction;

namespace Platformer
{
    Model Obstacle::model_ = {};

    void Obstacle::OnCollisionEnter(shared_ptr<Collider> other, Direction collision_dir)
    {
        terme::AudioManager::Instance().PlayFx("resources/hit.wav", 0.2);
    }

    void Obstacle::InitModel()
    {
        if (model_.GetSizeY() == 0)
            model_ = CreteModelUsingChar('#', size_t(4), size_t(2));
        SetModel(model_);
    }

}