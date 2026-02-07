#pragma once
#include <terme/entities/level.h>

namespace Platformer
{
    class CollisionsTestLevel : public terme::Level
    {
        //---------------------------------------------------------- Methods
    public:
        int GetWorldSizeX() const override { return 400; }
        int GetWorldSizeY() const override { return 120; }
        int GetScreenPadding() const override { return 4; }
        void LoadInSimulation() override;
        double ShowGameOverScreenDelay()const override { return 0; }
    };
}