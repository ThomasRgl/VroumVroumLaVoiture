#ifndef INTERSECT_HH
#define INTERSECT_HH

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>


boost::optional<sf::Vector2f> intersect( sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D);

boost::optional<sf::Vector2f> intersectV2( sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D);

sf::Vector2f minimum_distance(sf::Vector2f v, sf::Vector2f w, sf::Vector2f p);

#endif
