#pragma once
#include <iostream>
#include <fstream>

#include "GameRound.h"
#include "City.h"

class HammurabiGame
{
public:
    HammurabiGame() : 
        city_({ kStartCitizens, kStartAcres, kStartWheats }), 
        game_stats_({}),
        seed_(time(nullptr)),
        round_(1)
    {
        srand(seed_);
        game_stats_.new_city = city_;
    }

    void Start();

private:
    const Citizens kStartCitizens = 100;
    const Acres kStartAcres = 1000;
    const Wheats kStartWheats = 2800;
    const int kMaxRound = 10;
    const float kSurvivedToLose = 0.45;

    const char* kSaveFilename = "stat.save";

    City city_;
    GameStats game_stats_;

    int seed_;
    int round_;

    bool requeust_to_quit_;

    bool RequestToUserAction(const std::string& request_string);
    void TryLoad();
    void Save();
};

