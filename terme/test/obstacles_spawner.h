#pragma once
#include <terme/entities/i_simulation_entity.h>
#include <vector>

namespace Platformer
{
    class ObstaclesSpawner : public terme::ISimulationEntity
    {
        //---------------------------------------------------------- Fields
    private:
        int x_pos_;
        std::vector<double> min_spawn_delays_;
        std::vector<double> max_spawn_delays_;
        std::vector<double> speeds_;
        std::vector<int> y_spawn_points_;
        size_t spawn_intensity_ = 0;
        double stop_spawning_when_phase_changes_duration_;
        double increase_intensity_every_seconds_;
        double spawn_next_projectile_time_;
        double last_time_increased_intensity_ = 0;

        //---------------------------------------------------------- Methods
    public:
        ObstaclesSpawner
        (
            int x_pos,
            const std::vector<double>& min_spawn_delays,
            const std::vector<double>& max_spawn_delays,
            const std::vector<double>& speeds,
            const std::vector<int>& y_spawn_points,
            double increase_intensity_every_seconds = -1,
            double stop_spawning_when_phase_changes_duration = 0
        );

        ObstaclesSpawner
        (
            int x_pos,
            double spawn_delay,
            double speed,
            const std::vector<int>& y_spawn_points
        );

    protected:
        void Update() override;

    private:
        double GetCurrentMinSpawnDelay() const { return min_spawn_delays_[spawn_intensity_]; }
        double GetCurrentMaxSpawnDelay() const { return max_spawn_delays_[spawn_intensity_]; }
        double GetCurrentObstaclesSpeed() const { return speeds_[spawn_intensity_]; }
        double GetNextSpawnObstacleTime() const;
        void TryIncreaseIntensity(double time);
    };
}