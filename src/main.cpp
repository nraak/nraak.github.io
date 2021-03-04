#include "kube.hpp"

#ifdef DEBUG
#include <iostream>
#endif

int main() {
    #ifdef DEBUG
        std::cout << "main start..." << std::endl;
        int code = -1;
        try {
            Kube game;
            code = game.start();
        }catch(std::exception& e) {
            std::cout << "exception catched: " << e.what() << std::endl;
        }
        std::cout << "main end... (code : " << code << ")";
        return code;
    #else
        Kube game;
        return game.start();
    #endif
}
