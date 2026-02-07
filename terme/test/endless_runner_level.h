#pragma once
// #include "config.h"
#include <terme/entities/level.h>
#include <string>
#include <array>

namespace Platformer
{
    class EndlessRunnerLevel : public terme::Level
    {
        //---------------------------------------------------------- Settings
    private:
        static inline constexpr std::array<const char*, 3> kMusicFiles =
        {
            "resources/music/1.wav",
            "resources/music/2.wav",
            "resources/music/3.wav"
        };

        int shown_time_ = -1;

        //---------------------------------------------------------- Methods
    public:
        int GetWorldSizeX() const override { return 90; }
        int GetWorldSizeY() const override { return 23; }
        int GetScreenPadding() const override { return 4; }
        const char* GetBackgroundFileName()const override { return "resources/backgrounds/endless_runner.txt"; }

        void LoadInSimulation() override;

    protected:
        void Update()override;
        double ShowGameOverScreenDelay() const override { return 1.5; }
        void OnPostGameOverDelayEnded() override;
        const char* GetPersistenceFilePath() override { return "resources/persistence/endless_runner.txt"; }
        const char* GetGameOverWindowPath() override { return "resources/game_over_windows/endless_runner.txt"; }
        void ShowGameOverScreen(int score, int bestScore) override;
        void OnGameOver() override;

    private:
        void PlayRandomMusic();
    };
}