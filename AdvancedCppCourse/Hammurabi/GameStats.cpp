#include "GameStats.h"

void GameStats::PrintGreeting() {
    std::cout << "Ты повелитель великого города - Вавилона\n";
    std::cout << "под твоим управлением "
        << new_city.citizens
        << " людей, "
        << new_city.acres
        << " акров земли и "
        << new_city.wheats
        << " бушелей пшеницы.\n"
        << "Цена 1 акра земли "
        << new_city.acr_price << " бушелей пшеницы."
        << std::endl;

    return;
}

void GameStats::PrintCurrentStats() {
    std::cout << "----------------------------------------\n";
    std::cout << "Мой повелитель, соизволь поведать тебе\n";
    std::cout << "в году " << round << " твоего высочайшего правления\n";

    if (dead_citizens) {
        std::cout << dead_citizens << " человек умерли с голоду;\n";
    }

    if (new_citizens) {
        std::cout << new_citizens << " человек прибыл в наш великий город;\n";
    }

    if (was_plague) {
        std::cout << "Чума уничтожила половину населения;\n";
    }

    std::cout << "Население города сейчас составляет " << new_city.citizens << " человек;\n";
    std::cout << "Мы собрали " << gathered_wheats << " бушелей пшеницы, по " << wheats_per_arc << " бушеля с арка;\n";
    std::cout << "Крысы истребили " << eaten_by_rats << " бушелей пшеницы, оставив " << new_city.wheats << " бушеля в амбарах;\n";
    std::cout << "Город сейчас занимает " << new_city.acres << " акров;\n";
    std::cout << "1 акр земли стоит сейчас " << new_city.acr_price << " бушель.\n";
    std::cout << std::endl;
}