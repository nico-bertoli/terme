#pragma once
#include <terme/entities/level.h>

namespace Platformer
{
    class SortingLayerTestLevel : public terme::Level
    {
        //---------------------------------------------------------- Methods
    public:
        int GetWorldSizeX() const override { return 100; }
        int GetWorldSizeY() const override { return 30; }
        int GetScreenPadding() const override { return 1; }
        double ShowGameOverScreenDelay()const { return 0; }

        void LoadInSimulation() override;
    };
}