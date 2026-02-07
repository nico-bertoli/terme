#include <terme/entities/level.h>
#include <terme/core/simulation.h>
#include "bunny.h"
#include "sorting_layer_test_object.h"
#include "sorting_layer_test_level.h"

using std::shared_ptr;

namespace Platformer
{
    void SortingLayerTestLevel::LoadInSimulation()
    {
        terme::Level::LoadInSimulation();
        terme::Simulation& simulation = terme::Simulation::Instance();

        //----------------- bunny setup
        shared_ptr<Bunny> bunny = std::make_shared<Bunny>(9,5);
        bunny->on_obstacle_hit.Subscribe([this]() { OnGameOver(); });
        simulation.TryAddEntity(bunny);

        //----------------- front object
        auto sortingLayerTestObjFront = std::make_shared<SortingLayerTestObject>
        (
            10,     //posx
            1,      //posy
            30,     //sizex
            5,      //sizey
            '#',    //char
            terme::color::kRed,
            200     //sorting layer
        );
        simulation.TryAddEntity(sortingLayerTestObjFront);

        //----------------- back object
        auto sortingLayerTestObjBack = std::make_shared<SortingLayerTestObject>
        (
            60,     //posx
            1,      //posy
            30,      //sizex
            5,      //sizey
            '#',     //char
            terme::color::kBlue,
            0       //sorting layer
        );
        simulation.TryAddEntity(sortingLayerTestObjBack);
    }
}