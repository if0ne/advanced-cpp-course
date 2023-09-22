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

    void Print();
};

/*
std::ifstream& operator>>(std::ifstream& in, GameStats& game_stats) {
    in >> game_stats.round;
    in >> game_stats.dead_citizens;
    in >> game_stats.gathered_wheats;
    in >> game_stats.dead_citizens;
    in >> game_stats.new_citizens;
    in >> game_stats.wheats_per_arc;
    in >> game_stats.was_plague;
    in >> game_stats.eaten_by_rats;
    in >> game_stats.survived_percent;

    return in;
}

std::ofstream& operator<<(std::ofstream& out, GameStats& game_stats) {
    out << game_stats.round;
    out << game_stats.dead_citizens;
    out << game_stats.gathered_wheats;
    out << game_stats.dead_citizens;
    out << game_stats.new_citizens;
    out << game_stats.wheats_per_arc;
    out << game_stats.was_plague;
    out << game_stats.eaten_by_rats;
    out << game_stats.survived_percent;

    return out;
}
*/