#include "kube.hpp"

Kube::~Kube() {
    delete camera;
    delete window;
}

Kube::Kube() {
    window = new sf::RenderWindow(sf::VideoMode(400, 400), "Kube splash window", sf::Style::None, sf::ContextSettings(0, 0, 0, 2, 0));
    sf::Texture texture;
    if (texture.loadFromFile("splash.png")){
        window->create(sf::VideoMode(texture.getSize().x, texture.getSize().y), "Kube splash screen", sf::Style::None, sf::ContextSettings(0, 0, 0, 2, 0));
        sf::Sprite sprite;
        sprite.setTexture(texture);
        window->draw(sprite);
    }
    window->display();
    //loading start
#ifdef DEBUG
    std::cout << "loading start..." << std::endl;
#endif
    exitCode = 0;
    frameDebugs = 0;
    stop = false;

    width = 800;
    height = 800;

    rollAngle = 0;
    pitchAngle = 0;
    yawAngle = 0;

    rollStop = false;
    pitchStop = false;
    yawStop = false;
    cursorFocus = false;
    trailRender = false;

    camForward = false;
    camBackward = false;
    camLeft = false;
    camRight = false;
    camUp = false;
    camDown = false;

    camera = new kCamera({double(width), double(height)}, {0, 0, 200}, {0, 0, 0}, 90);

    cubeVectors = {
        {-50, -50, -50},
		{ 50, -50, -50},
		{ 50,  50, -50},
		{-50,  50, -50},
		{-50, -50,  50},
		{ 50, -50,  50},
		{ 50,  50,  50},
		{-50,  50,  50}
    };

    exitCode = KubeFont.loadFromFile("C:\\WINDOWS\\fonts\\arial.ttf") ? exitCode : -2;
    KubeClock.restart();
#ifdef DEBUG
    std::cout << "loading end!" << std::endl;
#endif
    //loading end
    if (exitCode == 0){
        window->create(sf::VideoMode(width, height), "Kube", sf::Style::Titlebar | sf::Style::Close);
        window->setActive(false);
    }
}

int Kube::start() {
    if (exitCode != 0) {return exitCode;}
    sf::Event event;

    sf::Thread rThread(&Kube::renderThread, this);
    rThread.launch();
    while (!stop) {
        while (window->pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed :
                    stop = true;
                    exitCode = 0;
                    break;
                case sf::Event::LostFocus :
                    cursorFocus = false;
                    break;
                case sf::Event::KeyPressed :
                    switch(event.key.code){
                        case sf::Keyboard::W :
                            rollStop = !rollStop;
                            break;
                        case sf::Keyboard::X :
                            pitchStop = !pitchStop;
                            break;
                        case sf::Keyboard::C :
                            yawStop = !yawStop;
                            break;
                        case sf::Keyboard::Escape :
                            rollStop = true;
                            pitchStop = true;
                            yawStop = true;
                            rollAngle = 0;
                            pitchAngle = 0;
                            yawAngle = 0;
                            camera->setDistance(140);
                            camera->setPosition({0, 0, 0});
                            camera->setRotation({0, 0, 0});
                            cursorFocus = false;
                            window->setMouseCursorVisible(!cursorFocus);
                            break;
                        case sf::Keyboard::Up :
                            camera->setDistance(camera->getDistance()+5.f);
                            break;
                        case sf::Keyboard::Down :
                            camera->setDistance(camera->getDistance()-5.f);
                            break;
                        case sf::Keyboard::Z :          camForward  = true; break;
                        case sf::Keyboard::Q :          camLeft     = true; break;
                        case sf::Keyboard::S :          camBackward = true; break;
                        case sf::Keyboard::D :          camRight    = true; break;
                        case sf::Keyboard::Space :      camUp       = true; break;
                        case sf::Keyboard::LControl :   camDown     = true; break;
                        case sf::Keyboard::F11 :
                            cursorFocus = !cursorFocus;
                            window->setMouseCursorVisible(!cursorFocus);
                            break;
                        case sf::Keyboard::F2 :
                            trailRender = !trailRender;
                            break;
                        default :
                            break;
                    }
                    break;
                case sf::Event::KeyReleased :
                    switch(event.key.code){
                        case sf::Keyboard::Z :          camForward  = false; break;
                        case sf::Keyboard::Q :          camLeft     = false; break;
                        case sf::Keyboard::S :          camBackward = false; break;
                        case sf::Keyboard::D :          camRight    = false; break;
                        case sf::Keyboard::Space :      camUp       = false; break;
                        case sf::Keyboard::LControl :   camDown     = false; break;
                        default :
                            break;
                    }
                    break;
                default :
                    break;
            }
        }
        if(trailRender){
            if(trayVectors.size() >= maxTrayCount) {
                trayVectors[trayCount%maxTrayCount] = project(rotateVector(cubeVectors[0], rollAngle, pitchAngle, yawAngle), *camera);
                trayCount++;
            }else{
                unsigned int missingParts = maxTrayCount-trayVectors.size();
                for(unsigned int i = 0; i < missingParts; i++){
                    trayVectors.push_back(project(rotateVector(cubeVectors[0], rollAngle, pitchAngle, yawAngle), *camera));
                }
                trayCount += missingParts;
            }
        }

        camera->setPosition(camera->getPosition()-kVector{
            (camLeft        ? 5.f : 0.f) - (camRight    ? 5.f : 0.f),
            (camUp          ? 5.f : 0.f) - (camDown     ? 5.f : 0.f),
            (camBackward    ? 5.f : 0.f) - (camForward  ? 5.f : 0.f)
        });

        float anglePerTick = 1.5*(1.f/fpsTarget);
        rollAngle = rollStop ? rollAngle : rollAngle + anglePerTick;
        pitchAngle = pitchStop ? pitchAngle : pitchAngle + anglePerTick;
        yawAngle = yawStop ? yawAngle : yawAngle + anglePerTick;

        sf::sleep(sf::seconds(1.f/tickRateTarget));//60 event / sec
    }
    rThread.wait();
    return exitCode;
}

void Kube::debug(std::string message){
#ifdef DEBUG
    sf::Text text(message, KubeFont);

    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);
    text.setPosition(0, 12*frameDebugs);

    window->draw(text);
    frameDebugs++;
#endif
}

void Kube::drawCube(std::vector<sf::Vertex> *vertices){
    kVector tmpA = {0, 0};
    kVector tmpB = {0, 0};
    for (unsigned int i = 0; i <= 3; i++) {
        //part 1/3
        tmpA = project(rotateVector(cubeVectors[i], rollAngle, pitchAngle, yawAngle), *camera);
        tmpB = project(rotateVector(cubeVectors[(i + 1) % 4], rollAngle, pitchAngle, yawAngle), *camera);
        vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
        vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
        vertices->operator[](vertices->size()-2).color = sf::Color::Red;
        vertices->operator[](vertices->size()-1).color = sf::Color::Red;
        //part 2/3
        tmpA = project(rotateVector(cubeVectors[i + 4], rollAngle, pitchAngle, yawAngle), *camera);
        tmpB = project(rotateVector(cubeVectors[((i + 1) % 4) + 4], rollAngle, pitchAngle, yawAngle), *camera);
        vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
        vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
        vertices->operator[](vertices->size()-2).color = sf::Color::Blue;
        vertices->operator[](vertices->size()-1).color = sf::Color::Blue;
        //part 3/3false
        tmpA = project(rotateVector(cubeVectors[i], rollAngle, pitchAngle, yawAngle), *camera);
        tmpB = project(rotateVector(cubeVectors[i + 4], rollAngle, pitchAngle, yawAngle), *camera);
        vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
        vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
        vertices->operator[](vertices->size()-2).color = sf::Color::Green;
        vertices->operator[](vertices->size()-1).color = sf::Color::Green;
    }
}

void Kube::drawTrail(std::vector<sf::Vertex> *vertices){
    if(trayVectors.size() == maxTrayCount){
        for (unsigned int i = 0; i < trayVectors.size()-1; i++){
            vertices->push_back(sf::Vector2f{float(trayVectors[(trayCount-1-i)%maxTrayCount][0]), float(trayVectors[(trayCount-1-i)%maxTrayCount][1])});
            vertices->push_back(sf::Vector2f{float(trayVectors[(trayCount-2-i)%maxTrayCount][0]), float(trayVectors[(trayCount-2-i)%maxTrayCount][1])});
            unsigned int gray = 255-(float(i)/(trayVectors.size()-1)*255);
            vertices->operator[](vertices->size()-2).color = sf::Color(gray, gray, gray, 255);
            vertices->operator[](vertices->size()-1).color = sf::Color(gray, gray, gray, 255);
        }
    }
}

void Kube::drawGrid(std::vector<sf::Vertex> *vertices, int xSize, int ySize, int zSize, kVector xyzDistances){
    kVector tmpA = {0, 0};
    kVector tmpB = {0, 0};

    /*for(unsigned int x = 0; x < xSize){
        for(unsigned int y = 0; y < ySize){
            for(unsigned int z = 0; z < zSize){
            }
        }
    }*/

    for (int x = 0; x < (xSize*2+1); x++){
        for (int y = 0; y < (ySize*2+1); y++){
            for (int z = 0; z < (zSize*2+1); z++){
                tmpA = project({(x - xSize)*xyzDistances[0], ySize * xyzDistances[1], (z - zSize)*xyzDistances[0]}, *camera);
                tmpB = project({(x - xSize)*xyzDistances[0], -ySize * xyzDistances[1], (z - zSize)*xyzDistances[0]}, *camera);
                vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
                vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
                vertices->operator[](vertices->size()-2).color = sf::Color::Red;
                vertices->operator[](vertices->size()-1).color = sf::Color::Red;

                tmpA = project({xSize * xyzDistances[0], (y - ySize)*xyzDistances[1], (z - zSize)*xyzDistances[0]}, *camera);
                tmpB = project({-xSize * xyzDistances[0], (y - ySize)*xyzDistances[1], (z - zSize)*xyzDistances[0]}, *camera);
                vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
                vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
                vertices->operator[](vertices->size()-2).color = sf::Color::Green;
                vertices->operator[](vertices->size()-1).color = sf::Color::Green;

                tmpA = project({(x - xSize)*xyzDistances[0], (y - ySize)*xyzDistances[1], zSize * xyzDistances[0]}, *camera);
                tmpB = project({(x - xSize)*xyzDistances[0], (y - ySize)*xyzDistances[1], -zSize * xyzDistances[0]}, *camera);
                vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
                vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
                vertices->operator[](vertices->size()-2).color = sf::Color::Blue;
                vertices->operator[](vertices->size()-1).color = sf::Color::Blue;
            }
       }
    }
}

void Kube::drawAxes(std::vector<sf::Vertex> *vertices){
    kVector tmpA = {0, 0};
    kVector tmpB = {0, 0};

    tmpA = project({0, 0, 0}, *camera);
    tmpB = project({20, 0, 0}, *camera);
    vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
    vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
    vertices->operator[](vertices->size()-2).color = sf::Color::Red;
    vertices->operator[](vertices->size()-1).color = sf::Color::Red;

    tmpA = project({0, 0, 0}, *camera);
    tmpB = project({0, 20, 0}, *camera);
    vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
    vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
    vertices->operator[](vertices->size()-2).color = sf::Color::Green;
    vertices->operator[](vertices->size()-1).color = sf::Color::Green;

    tmpA = project({0, 0, 0}, *camera);
    tmpB = project({0, 0, 20}, *camera);
    vertices->push_back(sf::Vector2f{float(tmpA[0]), float(tmpA[1])});
    vertices->push_back(sf::Vector2f{float(tmpB[0]), float(tmpB[1])});
    vertices->operator[](vertices->size()-2).color = sf::Color::Blue;
    vertices->operator[](vertices->size()-1).color = sf::Color::Blue;
}

void Kube::renderThread() {
    window->setActive(true);
    kVector cursorPos = {0, 0};
    kVector oldCursorPos = {0, 0};
    kVector cursorChange = {0, 0};
    while (!stop) {
        frameDebugs = 0;
        window->clear(sf::Color::Black);
        cursorPos = {double(sf::Mouse::getPosition(*window).x), double(sf::Mouse::getPosition(*window).y)};
        cursorChange = cursorPos - oldCursorPos;
        oldCursorPos = cursorPos;
        if(cursorFocus){
            sf::Mouse::setPosition(sf::Vector2i(width/2, height/2), *window);
            oldCursorPos = {width/2.f, height/2.f};
            kVector rotation = camera->getRotation();
            double delta = 1.f/fpsTarget;
            rotation[1] += cursorChange[0] * delta;
            rotation[0] -= cursorChange[1] * delta;
            camera->setRotation(rotation);
        }

        std::vector<sf::Vertex> vertices;
        drawCube(&vertices);
        if(trailRender){
            drawTrail(&vertices);
        }
        drawAxes(&vertices);
        drawGrid(&vertices, 3, 0, 3, {100, 100, 100});
        window->draw(vertices.data(), vertices.size(), sf::Lines);

        this->debug(std::string(rollStop   ? "stop" : "turn") + " | roll: " + std::to_string(rollAngle));
        this->debug(std::string(pitchStop  ? "stop" : "turn") + " | pitch: " + std::to_string(pitchAngle));
        this->debug(std::string(yawStop    ? "stop" : "turn") + " | yaw: " + std::to_string(yawAngle));
        this->debug("camera fov?: " + std::to_string(camera->getDistance()));
        this->debug("camera position: " + camera->getPosition().toString());
        this->debug("camera rotation: " + camera->getRotation().toString());
        this->debug("cursor change: " + cursorChange.toString());

        window->display();

        sf::sleep(sf::seconds(sf::seconds(1.f)/sf::seconds(fpsTarget)));
    }
    window->close();
    window->setActive(false);
}
