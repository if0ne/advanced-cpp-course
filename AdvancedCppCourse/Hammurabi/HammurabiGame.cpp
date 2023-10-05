#include "HammurabiGame.h"

void HammurabiGame::Start() {
    TryLoad();

    do
    {
        if (round_ == 1) {
            game_stats_.PrintGreeting();
        } else {
            game_stats_.PrintCurrentStats();
        }
        

        if (RequestToUserAction("Сохранить и выйти? (y/n) ")) {
            Save();
            return;
        }

        GameRound round{ city_, round_ };
        city_ = round.Run(game_stats_);
        round_++;
    } while (
        round_ <= kMaxRound &&
        game_stats_.dead_percent < kSurvivedToLose
    );

    if (game_stats_.dead_percent >= kSurvivedToLose) {
        std::cout << "Вы проиграли на " << round_ << " году." << std::endl;
    } else {
        float p = game_stats_.sum_dead_citizens_procent / kMaxRound;
        float l = city_.acres / city_.citizens;

        if (p > 0.33 && l < 0.07) {
            std::cout << "Из-за вашей некомпетентности в управлении, народ устроил бунт, и изгнал вас их города.Теперь вы вынуждены влачить жалкое существование в изгнании" << std::endl;
        }
        else if (p > 0.1 && l < 0.09) {
            std::cout << "«Вы правили железной рукой, подобно Нерону и Ивану Грозному.Народ вздохнул с облегчением, и никто больше не желает видеть вас правителем" << std::endl;
        }
        else if (p > 0.03 && l < 0.1) {
            std::cout << "«Вы справились вполне неплохо, у вас, конечно, есть недоброжелатели, но многие хотели бы увидеть вас во главе города снова" << std::endl;
        }
        else {
            std::cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше" << std::endl;
        }
    }
}

bool HammurabiGame::RequestToUserAction(const std::string& request_string) {
    std::string in;
    std::cout << request_string;
    std::cin >> in;

    return in == "y";
}

void HammurabiGame::TryLoad() {
    std::ifstream file(kSaveFilename);

    if (file.good()) {

        if (!RequestToUserAction("Найден файл, загрузить? (y/n) ")) {
            return;
        }

        file
            >> seed_
            >> round_
            >> city_.citizens
            >> city_.acres
            >> city_.wheats
            >> city_.acr_price
            >> game_stats_.round
            >> game_stats_.dead_citizens
            >> game_stats_.gathered_wheats
            >> game_stats_.dead_citizens
            >> game_stats_.new_citizens
            >> game_stats_.wheats_per_arc
            >> game_stats_.was_plague
            >> game_stats_.eaten_by_rats
            >> game_stats_.dead_percent
            >> game_stats_.sum_dead_citizens_procent;
        game_stats_.new_city = city_;

        srand(seed_);
    }

    file.close();
}

void HammurabiGame::Save() {
    std::ofstream file(kSaveFilename);

    file.clear();

    file
        << seed_ << " "
        << round_ << " "
        << city_.citizens << " "
        << city_.acres << " "
        << city_.wheats << " "
        << city_.acr_price << " "
        << game_stats_.round << " "
        << game_stats_.dead_citizens << " "
        << game_stats_.gathered_wheats << " "
        << game_stats_.dead_citizens << " "
        << game_stats_.new_citizens << " "
        << game_stats_.wheats_per_arc << " "
        << game_stats_.was_plague << " "
        << game_stats_.eaten_by_rats << " "
        << game_stats_.dead_percent << " "
        << game_stats_.sum_dead_citizens_procent;

    file.close();
}