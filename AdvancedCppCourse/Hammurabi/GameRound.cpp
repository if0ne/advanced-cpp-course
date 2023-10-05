#include "GameRound.h"

City GameRound::Run(GameStats& game_stats) {
    return
        InputData()
        .SowWheats()
        .RunRatsAttack()
        .EatWheats()
        .AttractNewCitizens()
        .TriggerPlague()
        .EndRound(game_stats);
}

GameRound& GameRound::BuyAcres() {
    Acres acres_to_buy;

    std::cout << "Сколько акров земли повелеваешь купить? ";
    std::cin >> acres_to_buy;

    while (acres_to_buy < 0 || acres_to_buy > new_city_.wheats / new_city_.acr_price) {
        std::cout << "Неверное количество акров земли\n";
        std::cout << "Сколько акров земли повелеваешь купить? ";
        std::cin >> acres_to_buy;
    }

    new_city_.acres += acres_to_buy;
    new_city_.wheats -= acres_to_buy * new_city_.acr_price;

    return *this;
}

GameRound& GameRound::SellAcres() {
    Acres acres_to_sell;

    std::cout << "Сколько акров земли повелеваешь продать? ";
    std::cin >> acres_to_sell;

    while (acres_to_sell < 0 || acres_to_sell >= new_city_.acres) {
        std::cout << "Неверное количество акров земли\n";
        std::cout << "Сколько акров земли повелеваешь продать? ";
        std::cin >> acres_to_sell;
    }

    new_city_.acres -= acres_to_sell;
    new_city_.wheats += acres_to_sell * new_city_.acr_price;

    return *this;
}

GameRound& GameRound::SetWheatsToEat() {
    Wheats wheats_to_eat;

    std::cout << "Сколько бушелей пшеницы повелеваешь съесть? ";
    std::cin >> wheats_to_eat;

    while (wheats_to_eat < 0 || wheats_to_eat > new_city_.wheats) {
        std::cout << "Неверное количество бушелей пшеницы\n";
        std::cout << "Сколько бушелей пшеницы повелеваешь съесть? ";
        std::cin >> wheats_to_eat;
    }

    new_city_.wheats -= wheats_to_eat;

    wheats_to_eat_ = wheats_to_eat;

    return *this;
}

GameRound& GameRound::SetAcresToProduction() {
    Acres acres_to_sow;

    std::cout << "Сколько акров земли повелеваешь засеять? ";
    std::cin >> acres_to_sow;

    while (acres_to_sow < 0 ||
        acres_to_sow > new_city_.citizens * kAcresPerCitizen ||
        acres_to_sow / kAcrPerWheat > new_city_.wheats ||
        acres_to_sow > new_city_.acres) {
        std::cout << "Неверное количество акров земли\n";
        std::cout << "Сколько акров земли повелеваешь засеять? ";
        std::cin >> acres_to_sow;
    }

    acres_to_sow_ = acres_to_sow;

    return *this;
}

GameRound& GameRound::InputData() {
    return
        BuyAcres().
        SellAcres().
        SetWheatsToEat().
        SetAcresToProduction();
}

GameRound& GameRound::SowWheats() {
    wheats_per_acr_ = random(1, 6);
    gathered_wheats_ = acres_to_sow_ * wheats_per_acr_;
    new_city_.wheats += gathered_wheats_;

    return *this;
}

GameRound& GameRound::RunRatsAttack() {
    Wheats eaten_wheats = static_cast<int>(new_city_.wheats * random(0.0f, kRatRate));
    eaten_by_rats_ = eaten_wheats;
    new_city_.wheats -= eaten_wheats;

    return *this;
}

GameRound& GameRound::EatWheats() {
    Citizens survived = wheats_to_eat_ / kWheatsPerCitizen;
    dead_citizens_ = std::abs(new_city_.citizens - survived);
    dead_percent_ = dead_citizens_ / static_cast<float>(new_city_.citizens);
    new_city_.citizens = survived;

    return *this;
}

GameRound& GameRound::AttractNewCitizens() {
    // A lot of magic constants
    Citizens new_citizens = dead_citizens_ / 2 + (5 - wheats_per_acr_) * new_city_.wheats / 600 + 1;
    new_citizens = std::min(50, std::max(0, new_citizens));
    new_citizens_ = new_citizens;
    new_city_.citizens += new_citizens;

    return *this;
}

GameRound& GameRound::TriggerPlague() {
    was_plague_ = random(0.0f, 1.0f) < kPlagueChance;

    if (was_plague_) {
        new_city_.citizens /= 2;
    }

    return *this;
}

City GameRound::EndRound(GameStats& game_stats) {
    new_city_.acr_price = random(new_city_.kMinAcrPrice, new_city_.kMaxAcrPrice);

    game_stats.round = round_;
    game_stats.new_city = new_city_;
    game_stats.dead_citizens = dead_citizens_;
    game_stats.new_citizens = new_citizens_;
    game_stats.gathered_wheats = gathered_wheats_;
    game_stats.wheats_per_arc = wheats_per_acr_;
    game_stats.was_plague = was_plague_;
    game_stats.eaten_by_rats = eaten_by_rats_;

    game_stats.dead_percent = dead_percent_;
    game_stats.sum_dead_citizens_procent += 1.0f - dead_percent_;

    return new_city_;
}