#ifndef INTERSECT_HH
#define INTERSECT_HH

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>


boost::optional<sf::Vertex> intersect( sf::Vertex A, sf::Vertex B, sf::Vertex C, sf::Vertex D);

boost::optional<sf::Vertex> intersectV2( sf::Vertex A, sf::Vertex B, sf::Vertex C, sf::Vertex D);



#endif
