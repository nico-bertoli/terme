#include "puzzle_level.h"
#include <terme/core/simulation.h>
#include "bunny.h"
#include <terme/entities/static_collider.h>
#include "automatic_door.h"
#include "pressure_plate.h"
#include "level_end_flag.h"
#include <terme/managers/audio_manager.h>
#include "pushable_object.h"

using StaticCollider = terme::StaticCollider;

namespace Platformer
{
    void PuzzleLevel::LoadInSimulation()
    {
        terme::Level::LoadInSimulation();
        terme::Simulation& simulation = terme::Simulation::Instance();

        //------------ bunny
        std::shared_ptr<Bunny> bunny = std::make_shared<Bunny>(5, 8);
        simulation.TryAddEntity(bunny);

        //------------ left platform
        auto platform = std::make_shared<StaticCollider>(24, 11, 12, 1, '#');
        simulation.TryAddEntity(platform);

        //------------ right flag platform
        auto flagPlatform = std::make_shared<StaticCollider>(63, 15, 16, 1, '#');
        simulation.TryAddEntity(flagPlatform);

        //------------ flag
        auto flag = std::make_shared<LevelEndFlag>(this, 70, 16);
        flag->OnTouchBunny.Subscribe([this]() { OnGameOver(); });
        simulation.TryAddEntity(flag);

        //------------ automatic door
        auto automaticDoor = std::make_shared<AutomaticDoor>(60, 4, 2, 6, '|', 4);
        simulation.TryAddEntity(automaticDoor);

        //------------ automatic door container
        auto automaticDoorTopRight = std::make_shared<StaticCollider>(62, 10, 1, 18, '#');
        simulation.TryAddEntity(automaticDoorTopRight);

        auto automaticDoorTopLeft = std::make_shared<StaticCollider>(59, 10, 1, 18, '#');
        simulation.TryAddEntity(automaticDoorTopLeft);

        //------------ pressure plate left
        auto pressurePlate1 = std::make_shared<PressurePlate>(14, 4, 8);
        pressurePlate1->on_press.Subscribe
        (
            [this]()
            {
                auto pushableObj1 = std::make_shared<PushableObject>(32, GetWorldSizeY() - 4);
                terme::Simulation::Instance().TryAddEntity(pushableObj1);
            }
        );
        simulation.TryAddEntity(pressurePlate1);

        //------------ pressure plate center
        auto openDoorPressurePlateLeft = std::make_shared<PressurePlate>(37, 4, 8);
        openDoorPressurePlateLeft->on_press.Subscribe([automaticDoor]() { automaticDoor->AddEnergySource(); });
        openDoorPressurePlateLeft->on_release.Subscribe([automaticDoor]() { automaticDoor->RemoveEnergySource(); });
        simulation.TryAddEntity(openDoorPressurePlateLeft);

        //------------ pressure plate right
        auto openDoorPressurePlateRight = std::make_shared<PressurePlate>(86, 4, 8);
        openDoorPressurePlateRight->on_press.Subscribe([automaticDoor]() { automaticDoor->AddEnergySource(); });
        openDoorPressurePlateRight->on_release.Subscribe([automaticDoor]() { automaticDoor->RemoveEnergySource(); });
        simulation.TryAddEntity(openDoorPressurePlateRight);
    }

    void PuzzleLevel::OnGameOver()
    {
        terme::Level::OnGameOver();
        terme::AudioManager::Instance().PlayFx("resources/sounds/level_won.wav");
    }

    void PuzzleLevel::OnPostGameOverDelayEnded()
    {
        terme::Level::OnPostGameOverDelayEnded();
        Terminate();
    }
}