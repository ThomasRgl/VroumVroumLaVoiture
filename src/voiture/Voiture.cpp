#include "Voiture.hpp"
#include "../intersect/intersect.hpp"


Voiture::Voiture(sf::Texture _texture_, sf::Vector2f _position_ ){
    texture = _texture_;
    position = _position_;
    oldPosition = position;

    //
    speed = 0;
    angle = -90;
    acceleration = 0;
    zone = 0;

    sprite.setTexture(texture);
    sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2 );
    sprite.setPosition(position);
    sprite.setRotation(angle);
    sprite.scale( 0.1, 0.1  );  //mettre le scale après position et origine sinon c'est fuck up

}

void Voiture::accelerate(double deltaTime, int direction){
    acceleration += apf * direction  * deltaTime;
    // ax +=  apf * cos_x * deltaTime * direction;
    // ay +=  apf * sin_y * deltaTime * direction;
}

void Voiture::turn(double deltaTime, int direction){
    if(speed >= 0)
        angle += deltaTime * rpf * direction;
    else
        angle += deltaTime * -rpf * direction;
    sprite.setRotation(angle);
}

void Voiture::update(double deltaTime){

    oldPosition = position;

    //frotement
    // acceleration += -speed * DeltaFrotement * 1/deltaTime * 0.001;
    // acceleration += speed * deltaTime * 0.95;
    speed += acceleration * deltaTime;
    // speed *= 0.995;

    // std::cout <<"a        : "<< acceleration - speed * DeltaFrotement<<std::endl;
    // std::cout <<"vitesse  : "<< speed / deltaTime <<std::endl;
    // std::cout <<"angle    : "<< angle  <<std::endl;
    // std::cout <<"DTime    : "<< deltaTime  <<std::endl;
    // std::cout <<"frotement: "<< speed * DeltaFrotement <<std::endl;

    acceleration = 0;

    float radiant  = angle * (3.142 / 180.0);
    position.x += cos(radiant) * speed;
    position.y += sin(radiant) * speed;

    sprite.setPosition(position);
}

void Voiture::checkZone(Circuit map){

    sf::Vertex A = position;
    sf::Vertex B = oldPosition;
    sf::Vertex C = map.GetVertices(2 * zone);
    sf::Vertex D = map.GetVertices(2 * zone + 1);

    if(intersectV2(A,B,C,D)){
        do {
            if(zone == 0)
                zone = map.getSize() - 1;
            else
                zone -= 1;

            C = map.GetVertices(2 * zone); // zone precedante
            D = map.GetVertices(2 * zone + 1);

        } while( intersectV2(A,B,C,D) );

        return;
    }


    C = map.GetVertices(2 * zone + 2); // zone suivante
    D = map.GetVertices(2 * zone + 3);

    if(intersectV2(A,B,C,D)){
        do {
            if(zone == map.getSize() - 1)
                zone = 0;
            else
                zone += 1;

            C = map.GetVertices(2 * zone + 2); // zone suivante
            D = map.GetVertices(2 * zone + 3);

        } while( intersectV2(A,B,C,D) );

        return;
    }

}

void Voiture::collide(Circuit map){

    sf::Vertex A = position;
    sf::Vertex B = oldPosition;
    sf::Vertex C = map.GetVertices(2 * zone);
    sf::Vertex D = map.GetVertices(2 * zone + 2);

    boost::optional<sf::Vertex> I = intersectV2(A,B,C,D);
    if( I ){
        sf::Vector2f p = oldPosition;
        oldPosition = position;
        position = p;
        speed = 0;
        checkZone(map);
        oldPosition = position;
        std::cout << "Collision - x :  " << I->position.x << "  y : " << I->position.y << std::endl;

        }



    C = map.GetVertices(2 * zone + 1);
    D = map.GetVertices(2 * zone + 3);

    I = intersectV2(A,B,C,D);
    if( I ){
        sf::Vector2f p = oldPosition;
        oldPosition = position;
        position = p;
        speed = 0;
        checkZone(map);
        oldPosition = position;
        std::cout << "Collision - x :  " << I->position.x << "  y : " << I->position.y << std::endl;
        }

    return;
}
