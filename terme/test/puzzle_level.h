#pragma once
#include <terme/entities/level.h>

namespace Platformer
{
    class PuzzleLevel : public terme::Level
    {
        //---------------------------------------------------------- Methods
    public:
        double ShowGameOverScreenDelay() const override { return 1.5; }
        int GetWorldSizeX() const override { return 120; }
        int GetWorldSizeY() const override { return 30; }
        int GetScreenPadding() const override { return 4; }

        void LoadInSimulation() override;
        void OnGameOver() override;
        void OnPostGameOverDelayEnded() override;
    };
}