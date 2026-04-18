#pragma once

#include <chrono>

#include <nbkit/singleton.h>

namespace terme
{
    class TimeManager : public nbkit::Singleton<TimeManager>
    {
        friend class nbkit::Singleton<TimeManager>;

        //---------------------------------------------------------- fields
    private:
        std::chrono::steady_clock::time_point start_time_;

        double last_time_simulation_stepped_ = 0;
        double fixed_delta_time_; //updated every time simulation steps

        double last_time_frame_generated_ = 0;
        double delta_time_ = 1;  //updated only when any part of terminal is reprinted

        //---------------------------------------------------------- methods
    public:
        double GetDeltaTime() const { return delta_time_; }
        double GetFPS() const { return 1.0 / GetDeltaTime();}

        double GetFixedDeltaTime() const {return fixed_delta_time_;}

        void OnFrameGenerated();
        void OnSimulationStepped();
        
        bool IsTimeForFirstOfTwoModels(double change_model_every_seconds) const;
        double GetTime() const;

    private:
        TimeManager();
    };
}