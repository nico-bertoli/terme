#include "terme/entities/level.h"
#include "terme/core/simulation.h"
#include "terme/input_manager/input_manager.h"
#include "terme/managers/time_manager.h"

namespace terme
{
    double Level::GetLevelTime() const
    {
        if (IsGameOver())
            return game_over_time_ - level_started_time_;
        else
            return TimeManager::Instance().GetTime() - level_started_time_;
    }

    void Level::LoadInSimulation()
    {
        is_terminated_ = false;
        game_over_time_ = -1;
        level_started_time_ = TimeManager::Instance().GetTime();
        has_called_on_post_game_over_delay_ended_ = false;

        game_over_window_.ReadFromFile(GetGameOverWindowPath());
    }

    bool Level::IsPostGameOverPauseEnded() const
    {
        return game_over_time_ > 0 && TimeManager::Instance().GetTime() - game_over_time_ > ShowGameOverScreenDelay();
    }

    bool Level::CanPlayerPressKeyToRestartGame() const
    {
        return TimeManager::Instance().GetTime() - game_over_time_ > ShowGameOverScreenDelay() + kPressAnyKeyToTerminateGameDelay;
    }

    void Level::OnGameOver()
    {
        if (IsGameOver())
            return;

        game_over_time_ = TimeManager::Instance().GetTime();
    }

    void Level::Update()
    {
        if (game_over_time_ < 0)
            return;

        if (IsPostGameOverPauseEnded() && has_called_on_post_game_over_delay_ended_ == false)
            OnPostGameOverDelayEnded();
        else if (CanPlayerPressKeyToRestartGame() && InputManager::Instance().IsAnyKeyPressed())
            Terminate();
    }
}