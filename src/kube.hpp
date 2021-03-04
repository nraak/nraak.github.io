#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <string>
#include <math.h>
#include "..\..\LinearAlgebra\linearAlgebra.h"

#ifdef DEBUG
#include <iostream>
#include <exception>
#endif

class Kube {
    private:
        const unsigned int maxTrayCount = 200;
        const float fpsTarget = 144;
        const float tickRateTarget = 60;

        unsigned long trayCount;
        unsigned int frameDebugs;
        unsigned int width;
        unsigned int height;
        int exitCode;
        double rollAngle;
        double pitchAngle;
        double yawAngle;
        bool rollStop;
        bool pitchStop;
        bool yawStop;
        bool stop;
        bool cursorFocus;
        bool trailRender;

        bool camForward;
        bool camBackward;
        bool camLeft;
        bool camRight;
        bool camUp;
        bool camDown;

        sf::Clock KubeClock;
        sf::Font KubeFont;
        sf::RenderWindow* window;
        std::vector<kVector> cubeVectors;
        std::vector<kVector> trayVectors;
        kCamera* camera;

        void drawCube(std::vector<sf::Vertex> *vertices);
        void drawTrail(std::vector<sf::Vertex> *vertices);
        void drawAxes(std::vector<sf::Vertex> *vertices);
        void drawGrid(std::vector<sf::Vertex> *vertices, int xSize, int ySize, int zSize, kVector xyzDistances);
        void debug(std::string message);
        void renderThread();
    public:
        Kube();
        ~Kube();
        int start();
};
