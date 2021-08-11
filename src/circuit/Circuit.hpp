#ifndef CIRCUIT_HH
#define CIRCUIT_HH

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>

class Circuit : public sf::Drawable {
private:
    size_t sizeCircuit;
public:
    std::vector<sf::Vertex> vertices;
    Circuit(std::string fileName);

    void highlightZone( size_t n );

    size_t getSize(){
        return sizeCircuit;
    }

    std::vector<sf::Vertex>* getVertices(){
        return &vertices;
    }

    sf::Vertex GetVertices(size_t i){
        if(i > vertices.size() || i < 0)
        exit(-1);
        return vertices[i];
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const{
        target.draw(&vertices[0], vertices.size(), sf::TriangleStrip, states);
    }
};

#endif
