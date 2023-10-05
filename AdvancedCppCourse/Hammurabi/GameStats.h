#pragma once
#include <iostream>
//#include <fstream>

#include "City.h"

class GameStats
{
public:
    GameStats() : new_city({ 0, 0, 0 }) {
        round = 0;
        dead_citizens = 0;
        new_citizens = 0;
        gathered_wheats = 0;
        wheats_per_arc = 0;
        was_plague = false;
        eaten_by_rats = 0;
        sum_dead_citizens_procent = 0.0f;
        dead_percent = 0.0f;
    }

    int round;
    City new_city;
    Citizens dead_citizens;
    Citizens new_citizens;
    Wheats gathered_wheats;
    Wheats wheats_per_arc;
    bool was_plague;
    Wheats eaten_by_rats;
    float dead_percent;

    float sum_dead_citizens_procent;

    void PrintCurrentStats();
    void PrintGreeting();
};


/*std::ifstream& operator>>(std::ifstream& in, GameStats& game_stats) {
    in >> game_stats.round
        >> game_stats.dead_citizens
        >> game_stats.gathered_wheats
        >> game_stats.dead_citizens
        >> game_stats.new_citizens
        >> game_stats.wheats_per_arc
        >> game_stats.was_plague
        >> game_stats.eaten_by_rats
        >> game_stats.dead_percent
        >> game_stats.sum_dead_citizens_procent;

    return in;
}

std::ofstream& operator<<(std::ofstream& out, GameStats& game_stats) {
    out
        << game_stats.round << " "
        << game_stats.dead_citizens << " "
        << game_stats.gathered_wheats << " "
        << game_stats.dead_citizens << " "
        << game_stats.new_citizens << " "
        << game_stats.wheats_per_arc << " "
        << game_stats.was_plague << " "
        << game_stats.eaten_by_rats << " "
        << game_stats.dead_percent << " "
        << game_stats.sum_dead_citizens_procent;

    return out;
}*/
