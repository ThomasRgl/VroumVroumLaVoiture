#ifndef VOITURE_HH
#define VOITURE_HH

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>

#include "../circuit/Circuit.hpp"

static const float NUM_VIEW = 8;

class Voiture  {

public:
    sf::Vector2f position;
    sf::Vector2f oldPosition;

    sf::Texture texture;
    sf::Sprite sprite;

    std::vector<sf::Vector2f> view;
    double viewDistance[8];

    double distanceCP;
    sf::Vector2f pointCP;

    double apf = 250; //acceleration per frame
    double rpf = 100; // rotation per frame
    double DeltaFrotement = 3000;

    double speed;
    double angle;
    double acceleration;

    size_t zone;

    Voiture(sf::Texture _texture_, sf::Vector2f _position_ );

    void accelerate(double deltaTime, int direction);

    void turn(double deltaTime, int direction);

    void update(double deltaTime, Circuit map);

    void checkZone(Circuit map);

    void collide(Circuit map);

    void updateView(Circuit map);

    void updateDistanceToCheckpoint(Circuit map);

    double*  getInput();

    size_t getNumInput();

    sf::Sprite getSprite(){
        return sprite;
    }

    size_t getZone(){
        return zone;
    }

    sf::Vector2f getPosition(){
        return position;
    }

    sf::Vector2f getOldPosition(){
        return oldPosition;
    }

    std::vector<sf::Vector2f> getView(){
        return view;
    }
};


#endif
