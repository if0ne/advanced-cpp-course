#pragma once
#include <iostream>

#include "City.h"
#include "GameStats.h"
#include "Utils.h"

class GameRound
{
public:
    GameRound(City city, int round) :  
        new_city_(city),
        round_(round)
    {}

    City Run(GameStats& game_stats);
private:
    const Wheats kWheatsPerCitizen = 20;
    const Acres kAcresPerCitizen = 10;
    const Acres kAcrPerWheat = 2;

    const float kPlagueChance = 0.15f;
    const float kRatRate = 0.07f;

    City new_city_;

    Acres acres_to_sow_;
    Wheats wheats_to_eat_;

    Wheats gathered_wheats_;
    Wheats wheats_per_acr_;
    Wheats eaten_by_rats_;
    Citizens dead_citizens_;
    Citizens new_citizens_;
    bool was_plague_;
    float dead_percent_;
    
    int round_;

    GameRound& BuyAcres();
    GameRound& SellAcres();
    GameRound& SetWheatsToEat();
    GameRound& SetAcresToProduction();
    GameRound& InputData();
    GameRound& SowWheats();
    GameRound& RunRatsAttack();
    GameRound& EatWheats();
    GameRound& AttractNewCitizens();
    GameRound& TriggerPlague();
    City EndRound(GameStats& game_stats);
};

