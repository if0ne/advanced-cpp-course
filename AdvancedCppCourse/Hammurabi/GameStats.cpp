#include "GameStats.h"

void GameStats::Print() {
    if (round == 0) {
        std::cout << "�� ���������� �������� ������ - ��������\n";
        std::cout << "��� ����� ����������� "
            << new_city.citizens
            << " �����, "
            << new_city.acres
            << " ����� ����� � "
            << new_city.wheats
            << " ������� �������.\n"
            << "���� 1 ���� ����� "
            << new_city.acr_price << " ������� �������."
            << std::endl;

        return;
    }

    std::cout << "----------------------------------------\n";
    std::cout << "��� ����������, �������� �������� ����\n";
    std::cout << "� ���� " << round << " ������ ����������� ���������\n";

    if (dead_citizens) {
        std::cout << dead_citizens << " ������� ������ � ������;\n";
    }

    if (new_citizens) {
        std::cout << new_citizens << " ������� ������ � ��� ������� �����;\n";
    }

    if (was_plague) {
        std::cout << "���� ���������� �������� ���������;\n";
    }

    std::cout << "��������� ������ ������ ���������� " << new_city.citizens << " �������;\n";
    std::cout << "�� ������� " << gathered_wheats << " ������� �������, �� " << wheats_per_arc << " ������ � ����;\n";
    std::cout << "����� ��������� " << eaten_by_rats << " ������� �������, ������� " << new_city.wheats << " ������ � �������;\n";
    std::cout << "����� ������ �������� " << new_city.acres << " �����;\n";
    std::cout << "1 ��� ����� ����� ������ " << new_city.acr_price << " ������.\n";
    std::cout << std::endl;
}