
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>


#include "circuit/Circuit.hpp"
#include "voiture/Voiture.hpp"
// #include "intersect/intersect.hpp"


static const float VIEW_SIZE = 1000.f;


int main()
{

    // return 0;
    sf::RenderWindow window(sf::VideoMode(VIEW_SIZE, VIEW_SIZE), "Vroum Vroum la voiture");

    // Sync Framerate and speed
    window.setFramerateLimit(100);

    //
    sf::View view(sf::Vector2f(0.0f,0.0f), sf::Vector2f( VIEW_SIZE, VIEW_SIZE));
    // sf::View minimap(sf::Vector2f(-2.5f * VIEW_SIZE , -2.5f * VIEW_SIZE), sf::Vector2f( VIEW_SIZE*5, VIEW_SIZE*5));
    // sf::View view(sf::Vector2f(0.0f,0.0f), sf::Vector2f( VIEW_SIZE, VIEW_SIZE));

    //
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    // minimap.setViewport(sf::FloatRect(0.75f, 0.f, 0.25f, 0.25f));


    // Font
    sf::Font font;
    font.loadFromFile("font/arial.ttf");

    //Load texture
    sf::Texture voiture_texture;
    voiture_texture.loadFromFile("sprite/voiture.png");

    //
    sf::Clock clock;
    Circuit map("test.csv");
    double multiplier = 50;
    double deltaTime;
    float move = 0.f;

    u_int window_size_x = window.getSize().x;
    u_int window_size_y = window.getSize().y;

    // UI
    sf::Text fps;
    fps.setFont(font);
    fps.setCharacterSize(24);
    fps.setString("Hello world");
    fps.setFillColor(sf::Color::Red);
    fps.setPosition(10.f, 10.f);

    // World
    std::vector<sf::Vertex>* vertices = map.getVertices();
    float pos_x = ( (*vertices)[0].position.x + (*vertices)[1].position.x + (*vertices)[2].position.x + (*vertices)[3].position.x)/4;
    float pos_y = ( (*vertices)[0].position.y + (*vertices)[1].position.y + (*vertices)[2].position.y + (*vertices)[3].position.y)/4;
    Voiture voiture(voiture_texture, sf::Vector2f( pos_x, pos_y ));

    // (*vertices)[0].color = sf::Color::Green;
    // (*vertices)[1].color = sf::Color::Green;
    // (*vertices)[2].color = sf::Color::Green;
    // (*vertices)[3].color = sf::Color::Green;

    // map.vertices[0].color = sf::Color::Green;
    // map.vertices[1].color = sf::Color::Green;
    // map.vertices[2].color = sf::Color::Green;
    // map.vertices[3].color = sf::Color::Green;

    while (window.isOpen())
    {

        //Event
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type){

                case sf::Event::MouseWheelMoved:
                    view.zoom(1 + event.mouseWheel.delta * -0.05f);
                    break;

                case sf::Event::Closed :
                    window.close();
                    break;

                case sf::Event::Resized:
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    float aspectRatio = (float) window.getSize().x / (float) window.getSize().y ;
                    view.setSize(VIEW_SIZE * aspectRatio, VIEW_SIZE);
                    // minimap.setSize(5*VIEW_SIZE * aspectRatio, 5*VIEW_SIZE);
                    // std::cout << "width " << event.size.width <<" height " << event.size.height<< " aspect Ratio " << aspectRatio << std::endl;
                    break;
            }
        }

        //time elapsed
        deltaTime = clock.restart().asSeconds();
        fps.setString("fps : " +  std::to_string(1.f / deltaTime));

        //Update
        move = 10.f * multiplier * deltaTime;
        // tic++;

        //Update Shapes ZQSD
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z) )
            voiture.accelerate(deltaTime, 1 );
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) )
            voiture.accelerate(deltaTime, -1 );
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q) )
            voiture.turn(deltaTime, -1 );
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)  )
            voiture.turn(deltaTime, 1 );

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) )
            map.highlightZone(0);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::X) )
            map.highlightZone(1);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::C) )
            map.highlightZone(2);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)  )
            map.highlightZone(3);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::M) ){
            std::cout << "enregistrement en cours, appuyez sur echap pour finir" << std::endl;
            while (! sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                sf::CircleShape circle;
                while (window.pollEvent(event))
                {
                    switch (event.type){


                        case sf::Event::Closed :
                            window.close();
                            break;


                        case sf::Event::MouseButtonReleased :
                            window.setView(view);
                            window.setView(window.getDefaultView());
                            if (event.mouseButton.button == sf::Mouse::Left)
                            {
                                std::cout << event.mouseButton.x << "," << event.mouseButton.y <<  std::endl;
                            }

                            // sf::CircleShape circle = new sf::CircleShape(5.f);
                            circle.setRadius(5.f);
                            circle.setPosition(event.mouseButton.x, event.mouseButton.y);
                            window.draw(circle);
                            window.display();
                            window.setView(window.getDefaultView());
                            break;

                        case sf::Event::Resized :
                            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                            window.setView(sf::View(visibleArea));
                            float aspectRatio = (float) window.getSize().x / (float) window.getSize().y ;
                            view.setSize(VIEW_SIZE * aspectRatio, VIEW_SIZE);
                            // minimap.setSize(5*VIEW_SIZE * aspectRatio, 5*VIEW_SIZE);
                            // std::cout << "width " << event.size.width <<" height " << event.size.height<< " aspect Ratio " << aspectRatio << std::endl;
                            break;
                    }
                }
            }
            std::cout << "Fin." << std::endl;
        }
            // std::cout << "zz" << std::endl;


        //Update
        voiture.update(deltaTime);
        voiture.checkZone(map);
        voiture.collide(map);
        map.highlightZone(voiture.getZone());
        // map.checkZone(voiture);

        //Collision

        window.clear();

        //Views
        view.setCenter(voiture.sprite.getPosition());
        window.setView(view);

        //draw
        window.draw(map);
        window.draw(voiture.getSprite());

        //
        window.setView(window.getDefaultView());
        window.draw(fps);
        window.display();
    }

    return 0;
}
