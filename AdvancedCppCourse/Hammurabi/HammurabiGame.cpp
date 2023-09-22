#include "HammurabiGame.h"

void HammurabiGame::Start() {
    TryLoad();

    do
    {
        game_stats.Print();

        if (RequestToUserAction("��������� � �����? (y/n) ")) {
            requeust_to_quit_ = true;
            Save();
            return;
        }

        GameRound round{ city_, round_ };
        city_ = round.Run(game_stats);
        round_++;
    } while (
        round_ <= kMaxRound &&
        !requeust_to_quit_ &&
        game_stats.dead_percent < kSurvivedToLose
        );

    if (game_stats.dead_percent >= kSurvivedToLose) {
        std::cout << "�� ��������� �� " << round_ << " ����." << std::endl;
    }
    else {
        float p = game_stats.sum_dead_citizens_procent / kMaxRound;
        float l = city_.acres / city_.citizens;

        if (p > 0.33 && l < 0.07) {
            std::cout << "��-�� ����� ���������������� � ����������, ����� ������� ����, � ������ ��� �� ������.������ �� ��������� ������� ������ ������������� � ��������" << std::endl;
        }
        else if (p > 0.1 && l < 0.09) {
            std::cout << "��� ������� �������� �����, ������� ������ � ����� ��������.����� �������� � �����������, � ����� ������ �� ������ ������ ��� ����������" << std::endl;
        }
        else if (p > 0.03 && l < 0.1) {
            std::cout << "��� ���������� ������ �������, � ���, �������, ���� ���������������, �� ������ ������ �� ������� ��� �� ����� ������ �����" << std::endl;
        }
        else {
            std::cout << "����������! ���� �������, �������� � ���������� ������ �� ���������� �� �����" << std::endl;
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

        if (!RequestToUserAction("������ ����, ���������? (y/n) ")) {
            return;
        }

        file
            >> seed_
            >> round_
            >> city_.citizens
            >> city_.acres
            >> city_.wheats
            >> city_.acr_price
            >> game_stats.round
            >> game_stats.dead_citizens
            >> game_stats.gathered_wheats
            >> game_stats.dead_citizens
            >> game_stats.new_citizens
            >> game_stats.wheats_per_arc
            >> game_stats.was_plague
            >> game_stats.eaten_by_rats
            >> game_stats.dead_percent
            >> game_stats.sum_dead_citizens_procent;
        game_stats.new_city = city_;

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

    file.close();
}