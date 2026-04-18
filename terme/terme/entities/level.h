#pragma once

#include "terme/core/frame.h"
#include "terme/entities/i_simulation_entity.h"
#include "terme/terminal/terminal.h"

namespace terme
{
    class Level : public ISimulationEntity
    {
        friend class Simulation;

        //---------------------------------------------------------- settings
    private:
        static constexpr double kPressAnyKeyToTerminateGameDelay = 1.5;

        //---------------------------------------------------------- fields
    private:
        bool has_called_on_post_game_over_delay_ended_ = false;
        double game_over_time_ = -1;
        double level_started_time_ = 0;
        bool is_terminated_ = false;

    protected:
        terme::Frame game_over_window_;

        //---------------------------------------------------------- methods

    public:
        virtual int GetWorldSizeX() const = 0;
        virtual int GetWorldSizeY() const = 0;
        virtual int GetScreenPadding() const = 0;
        virtual const char* GetBackgroundFileName()const { return ""; }
        double GetLevelTime() const;
        bool IsGameOver() const { return game_over_time_ > -1; }
        bool IsTerminated() const { return is_terminated_; }

        virtual TerminalColor GetMarginsColor() const { return color::kBlueDark; }
        virtual TerminalColor GetDefaultFrontColor() const {return color::kWhite;}
        virtual TerminalColor GetDefaultBackColor() const { return color::kBgBlack; }
        virtual TerminalColor GetBackgroundCharsColor() const {return color::kWhite;}

    protected:
        virtual void OnGameOver();
        virtual double ShowGameOverScreenDelay() const = 0;
        void Update() override;
        virtual void OnPostGameOverDelayEnded() { has_called_on_post_game_over_delay_ended_ = true; }
        virtual void LoadInSimulation();
        virtual const char* GetGameOverWindowPath() { return ""; }
        virtual const char* GetPersistenceFilePath() { return ""; }
        virtual void ShowGameOverScreen(int score, int saved_best_score){};

        bool IsPostGameOverPauseEnded() const;
        bool CanPlayerPressKeyToRestartGame() const;
        void Terminate() { is_terminated_ = true; }

    };
}