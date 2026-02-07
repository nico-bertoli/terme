#include "obstacles_spawner.h"
#include "obstacle.h"
#include <terme/core/simulation.h>
#include <terme/managers/time_manager.h>
#include <nbkit/random_utils.h>
#include <terme/entities/collider.h>
#include <cstdlib>

using terme::Collider;
using terme::Direction;
using std::shared_ptr;

namespace Platformer
{
    ObstaclesSpawner::ObstaclesSpawner
    (
        int x_pos,
        const std::vector<double>& min_spawn_delays,
        const std::vector<double>& max_spawn_delays,
        const std::vector<double>& speeds,
        const std::vector<int>& y_spawn_points,
        double increase_intensity_every_seconds,
        double stop_spawning_when_phase_changes_duration
    ) :
        x_pos_(x_pos),
        min_spawn_delays_(min_spawn_delays),
        max_spawn_delays_(max_spawn_delays),
        speeds_(speeds),
        increase_intensity_every_seconds_(increase_intensity_every_seconds),
        stop_spawning_when_phase_changes_duration_(stop_spawning_when_phase_changes_duration),
        y_spawn_points_(y_spawn_points)
    {
        assert(max_spawn_delays_.size() == speeds_.size() && min_spawn_delays_.size() == speeds_.size());
        spawn_next_projectile_time_ = GetNextSpawnObstacleTime();
        last_time_increased_intensity_ = terme::TimeManager::Instance().GetTime();
    }

    ObstaclesSpawner::ObstaclesSpawner
    (
        int x_pos,
        double spawn_delay,
        double speed,
        const std::vector<int>& y_spawn_points
    ) :
        ObstaclesSpawner(
            x_pos,
            std::vector<double>{spawn_delay},
            std::vector<double>{spawn_delay},
            std::vector<double>{speed},
            y_spawn_points
        )
    {
    }

    void ObstaclesSpawner::Update()
    {
        double time = terme::TimeManager::Instance().GetTime();
        if (time >= spawn_next_projectile_time_)
        {
            TryIncreaseIntensity(time);

            if (time - last_time_increased_intensity_ < stop_spawning_when_phase_changes_duration_)
                return;

            spawn_next_projectile_time_ = GetNextSpawnObstacleTime();

            int random_index = nbkit::random_utils::GetRandomInt(0, static_cast<int>(y_spawn_points_.size()) - 1);
            int random_pos_y = y_spawn_points_[random_index];

            double obstacle_speed = GetCurrentObstaclesSpeed();
            Direction direction = obstacle_speed > 0 ? Direction::kRight : Direction::kLeft;
            obstacle_speed = abs(obstacle_speed);
            shared_ptr<Obstacle> obstacle = std::make_shared<Obstacle>(x_pos_, random_pos_y, direction, obstacle_speed);
            terme::Simulation::Instance().TryAddEntity(std::dynamic_pointer_cast<Collider>(obstacle));
        }
    }

    void ObstaclesSpawner::TryIncreaseIntensity(double time)
    {
        if (increase_intensity_every_seconds_ == -1)
            return;

        if (spawn_intensity_ < speeds_.size() - 1 && (time - last_time_increased_intensity_) > increase_intensity_every_seconds_ + stop_spawning_when_phase_changes_duration_)
        {
            ++spawn_intensity_;
            last_time_increased_intensity_ = time;
        }
    }

    double ObstaclesSpawner::GetNextSpawnObstacleTime() const
    {
        return terme::TimeManager::Instance().GetTime() + nbkit::random_utils::GetRandomDouble(GetCurrentMinSpawnDelay(), GetCurrentMaxSpawnDelay());
    }
}