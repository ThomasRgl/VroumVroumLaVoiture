#include "Circuit.hpp"
#include "../csv/csv.hpp"


Circuit::Circuit(std::string fileName){
    std::vector<std::pair<std::string, std::vector<int>>> test = read_csv("circuits/"+fileName);
    if(test[0].second.size() != test[1].second.size())
    exit(-1);
    if( test[0].second.size() < 6 || test[0].second.size() % 2 == 1)
    exit(-1);


    for (size_t i = 0; i != test[0].second.size(); i++){
        // std::cout << test[0].second[i] << ',' <<  test[1].second[i] << std::endl;
        sf::Vertex vertex;
        vertex.position = sf::Vector2f(test[0].second[i], test[1].second[i]);
        vertex.color = sf::Color(128,128,128);
        vertices.push_back(vertex);
        // vertices.push_back(sf::Vertex(test[0].second[i], test[1].second[i]));
    }

    sizeCircuit = (vertices.size() - 2)/2;
}

void Circuit::highlightZone( size_t n ){
    if( n < 0 || n >= sizeCircuit ) {
        std::cout << "impossible de surligner cette zone" << std::endl;
        return;
    }

    for (size_t i = 0; i != vertices.size(); i++){
        vertices[i].color = sf::Color(128,128,128);
    }

    vertices[n * 2 + 0].color = sf::Color::Green;
    vertices[n * 2 + 1].color = sf::Color::Green;
    vertices[n * 2 + 2].color = sf::Color::Green;
    vertices[n * 2 + 3].color = sf::Color::Green;
}

sf::Vector2f Circuit::getZoneCenter(){
    return sf::Vector2f( ( vertices[0].position.x + vertices[1].position.x + vertices[2].position.x + vertices[3].position.x)/4  ,
                         ( vertices[0].position.y + vertices[1].position.y + vertices[2].position.y + vertices[3].position.y)/4 ) ;

}
