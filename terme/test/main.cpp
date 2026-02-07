#include <terme/core/simulation.h>
#include <terme/input_manager/input_manager.h>
#include <terme/terminal/terminal.h>
#include "test_level.h"

using terme::Simulation;
using terme::InputManager;
using terme::Key;

int main()
{
    auto level = std::make_shared<terme::TestLevel>();
    terme::Simulation::Instance().LoadLevel(level);
    
    while (level->IsTerminated() == false)
    {
        terme::Simulation::Instance().Step();
        if (InputManager::Instance().IsKeyPressed(Key::kEsc))
            break;
    }
    
    return 0;
}
