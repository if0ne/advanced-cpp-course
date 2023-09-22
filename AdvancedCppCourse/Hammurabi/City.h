#pragma once
#include <iostream>
//#include <fstream>

#include "Utils.h"

typedef int Citizens;
typedef int Acres;
typedef int Wheats;

class City
{
public:
    const Wheats kMinAcrPrice = 17;
    const Wheats kMaxAcrPrice = 26;

    City(Citizens citizens, Acres acres, Wheats wheats) : citizens(citizens), acres(acres), wheats(wheats) {
        acr_price = random(kMinAcrPrice, kMaxAcrPrice);
    }

    Citizens citizens;
    Acres acres;
    Wheats wheats;

    Wheats acr_price;

    void operator=(const City& other) {
        citizens = other.citizens;
        acres = other.acres;
        wheats = other.wheats;
        acr_price = other.acr_price;
    }
};

/*
std::ifstream& operator>>(std::ifstream& in, City& city) {
    in >> city.citizens;
    in >> city.acres;
    in >> city.wheats;
    in >> city.acr_price;

    return in;
}

std::ofstream& operator<<(std::ofstream& out, City& city) {
    out << city.citizens;
    out << city.acres;
    out << city.wheats;
    out << city.acr_price;

    return out;
}
*/