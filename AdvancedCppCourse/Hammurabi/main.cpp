#include "HammurabiGame.h"

int main() {
    //setlocale(LC_ALL, "Russian");
    system("chcp 65001");

    HammurabiGame game{};

    game.Start();

    return 0;
}