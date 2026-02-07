#include "collisions_test_level.h"
// #include "config.h"
#include <terme/core/simulation.h>
#include "obstacle.h"
#include "obstacles_spawner.h"
#include "bunny.h"

using std::shared_ptr;
using std::vector;

namespace Platformer
{
    void CollisionsTestLevel::LoadInSimulation()
    {
        terme::Level::LoadInSimulation();
        terme::Simulation& simulation = terme::Simulation::Instance();

        std::shared_ptr<Bunny> bunny = std::make_shared<Bunny>(5, 8);
        simulation.TryAddEntity(bunny);

        //----------------------- spawners settings
        double spawnDelay = 0.01;
        double speed = 2;

        //----------------------- right spawner
        int xPosRight = GetWorldSizeX() - 2;

        vector<int>ySpawnPoints;
        for(int i = 4; i<GetWorldSizeY()-2; i+=2)
            ySpawnPoints.push_back(i);

        shared_ptr<ObstaclesSpawner> spawnerRight = std::make_shared<ObstaclesSpawner>
        (
            xPosRight,
            spawnDelay,
            -speed,
            ySpawnPoints
        );
        simulation.TryAddEntity(spawnerRight);

        //----------------------- left spawner
        int xPosLeft = 2;
        vector<double>speeds2 = { 8, 1 };
        shared_ptr<ObstaclesSpawner> spawnerLeft = std::make_shared<ObstaclesSpawner>
        (
            xPosLeft,
            spawnDelay,
            speed,
            ySpawnPoints
        );
        simulation.TryAddEntity(spawnerLeft);
    }
}

