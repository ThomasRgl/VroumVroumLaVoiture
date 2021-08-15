
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#include "circuit/Circuit.hpp"
#include "voiture/Voiture.hpp"
// #include "intersect/intersect.hpp"
extern "C"{
#include "neuralNetwork/neuralNetwork.h"
}

static const float VIEW_SIZE = 1000.f;


typedef struct VroumVroumArgs
{
    Circuit * circuit;
}VroumVroumArgs;

/*
Fonction jeu qui sera controllé par un nn
@param NeuralNetwork * nn : nn controllant le jeu
*/
void game(NeuralNetwork * nn, void * args){
    // VroumVroumArgs * vvargs = (VroumVroumArgs *) args;
    // Voiture voiture();

    // std::vector<sf::Vertex>* vertices = map.getVertices();
    // float pos_x = ( (*vertices)[0].position.x + (*vertices)[1].position.x + (*vertices)[2].position.x + (*vertices)[3].position.x)/4;
    // float pos_y = ( (*vertices)[0].position.y + (*vertices)[1].position.y + (*vertices)[2].position.y + (*vertices)[3].position.y)/4;
    double deltaTime = 0.1;

    Circuit * map = ((VroumVroumArgs *) args)->circuit;
    Voiture voiture(sf::Vector2f( map->getZoneCenter() ));
    voiture.updateView( *map );

    // int end = 0;

    int resultat = 4;
    while(voiture.isAlive() && voiture.getFuel() != 0 ){
        resultat = computeNN( nn, voiture.getInput() );
        //                      Affichage
        // jump(10);
        // printNetwork(population[i]);
        // afficherData(population[i]);
        // afficherJeu(resultat);
        // printf(">\n");
        // getchar();


        switch (resultat) {
            case 0:
                voiture.accelerate(deltaTime, 1 );
                break;
            case 1:
                voiture.accelerate(deltaTime, -1 );
            case 2:
                voiture.turn(deltaTime, -1 );
                break;
            case 3:
                voiture.turn(deltaTime, 1 );
                break;
            case 4:
                break;
            default:
                //break;
                //printf("fin du jeu\n" );
                printf("%d\n", resultat);
                exit(0);
                // end = 1;
                break;
        }

        voiture.update( deltaTime, *map );

    }

    setScore(nn, voiture.getScore() + 1, voiture.getScore() + 1);
    printf("%f\n", voiture.getScore() + 1 );

    return;
}

void ShowGame( NeuralNetwork * nn, void * args){


    sf::RenderWindow window(sf::VideoMode(VIEW_SIZE, VIEW_SIZE), "Vroum Vroum la voiture");

    // Sync Framerate and speed
    window.setFramerateLimit(10);

    //
    sf::View view(sf::Vector2f(0.0f,0.0f), sf::Vector2f( VIEW_SIZE, VIEW_SIZE));
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    // Font
    sf::Font font;
    font.loadFromFile("font/arial.ttf");

    //Load texture
    sf::Texture voiture_texture;
    voiture_texture.loadFromFile("sprite/voiture.png");

    //
    sf::Clock clock;
    Circuit * map = ((VroumVroumArgs *) args)->circuit;
    // double multiplier = 50;
    double deltaTime;

    // u_int window_size_x = window.getSize().x;
    // u_int window_size_y = window.getSize().y;

    // UI
    sf::Text fps;
    fps.setFont(font);
    fps.setCharacterSize(24);
    fps.setString("Hello world");
    fps.setFillColor(sf::Color::Red);
    fps.setPosition(10.f, 10.f);

    // World
    // std::vector<sf::Vertex>* vertices = map.getVertices();
    // float pos_x = ( (*vertices)[0].position.x + (*vertices)[1].position.x + (*vertices)[2].position.x + (*vertices)[3].position.x)/4;
    // float pos_y = ( (*vertices)[0].position.y + (*vertices)[1].position.y + (*vertices)[2].position.y + (*vertices)[3].position.y)/4;
    // Voiture voiture(voiture_texture, sf::Vector2f( pos_x, pos_y ));
    Voiture voiture( voiture_texture, sf::Vector2f( map->getZoneCenter() ));
    voiture.updateView(*map);

    int resultat = 4;
    // double deltaTime = 0.1;
    // int end = 0;


    while (window.isOpen())
    {

        //Event
        sf::Event event;
        while (window.pollEvent(event))
        {

            if( event.type == sf::Event::MouseWheelMoved ){
                    view.zoom(1 + event.mouseWheel.delta * -0.05f);
                }
            if( event.type == sf::Event::Closed ){
                window.close();
                }
            if( event.type == sf::Event::Resized ){

                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                float aspectRatio = (float) window.getSize().x / (float) window.getSize().y ;
                view.setSize(VIEW_SIZE * aspectRatio, VIEW_SIZE);
                // minimap.setSize(5*VIEW_SIZE * aspectRatio, 5*VIEW_SIZE);
                // std::cout << "width " << event.size.width <<" height " << event.size.height<< " aspect Ratio " << aspectRatio << std::endl;

            }
        }

        //time elapsed
        deltaTime = clock.restart().asSeconds();
        fps.setString("fps : " +  std::to_string(1.f / deltaTime));



            // std::cout << "zz" << std::endl;



        //process
        resultat = computeNN( nn, voiture.getInput() );

        switch (resultat) {
            case 0:
                voiture.accelerate(deltaTime, 1 );
                break;
            case 1:
                voiture.accelerate(deltaTime, -1 );
            case 2:
                voiture.turn(deltaTime, -1 );
                break;
            case 3:
                voiture.turn(deltaTime, 1 );
                break;
            case 4:
                break;
            default:
                //break;
                //printf("fin du jeu\n" );
                printf("%d\n", resultat);
                exit(0);
                // end = 1;
                break;
        }

        //Update
        voiture.update(deltaTime, *map);
        voiture.UpdateSprite();

        map->highlightZone(voiture.getZone());





        window.clear();

        //Views
        view.setCenter(voiture.sprite.getPosition());
        window.setView(view);

        //draw
        window.draw(*map);
        window.draw(voiture.getSprite());

        //draw view
        sf::Vertex voiture_point (voiture.getPosition() , sf::Color::Magenta);
        sf::Vertex line [2];
        line[0] = voiture_point;
        for(int i = 0; i != NUM_VIEW; i++){
            line[1] = sf::Vertex(voiture.getView()[i], sf::Color::Magenta);
            // std::cout << i << " x : "<< voiture.getView()[i].x << " y :"<< voiture.getView()[i].y << std::endl;
            window.draw(line,2,sf::Lines);
            // std::cout << i << " : " << voiture.viewDistance[i] << std::endl;
        }

        //draw nearest checkPoint
        sf::Vertex line2 [2];

        line2[0] = sf::Vertex(voiture.getPosition() , sf::Color::Blue);
        line2[1] = sf::Vertex(voiture.pointCP, sf::Color::Blue);
        window.draw(line2,2,sf::Lines);

        window.setView(window.getDefaultView());
        window.draw(fps);
        window.display();

        if( !(voiture.isAlive() && voiture.getFuel() > 0) ){
            window.close();
        }

    }

    return ;

}



int main()
{
    srand(time(NULL));

    struct timespec start, finish;
    double elapsed;




    NewConfig(
        4,       // size_t taille_population,
        100,        //size_t nombre de generation

        Voiture::getNumInput(),          // size_t nbNeuronsInput,
        8,          // size_t nbNeuronsHidden,
        5,          // size_t nbNeuronsOutput,

        1,          // size_t nbHiddenLayer,

        0.3,        // double mutationRate,
        0.05,       // double sigmaMutation,
        0.3,        // double crossoverRate,

        2           //size_t nbThread
    );

    Circuit map1("test.csv");
    Circuit * p_map1;
    p_map1 = &map1;

    VroumVroumArgs myArgs;
    VroumVroumArgs * p_myArgs;
    p_myArgs = &myArgs;

    p_myArgs->circuit = p_map1;

    //lance l'ia et chronometre le temps d'execution
    clock_gettime(CLOCK_MONOTONIC, &start);
    runPere( (char* )"score/score.csv", game, ShowGame, (void *) &myArgs  );
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    // printf("%ld, %lf\n",i,elapsed );



    return 0;

    /*
    sigmoid(20);

    sf::RenderWindow window(sf::VideoMode(VIEW_SIZE, VIEW_SIZE), "Vroum Vroum la voiture");

    // Sync Framerate and speed
    window.setFramerateLimit(100);

    //
    sf::View view(sf::Vector2f(0.0f,0.0f), sf::Vector2f( VIEW_SIZE, VIEW_SIZE));
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

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

    voiture.updateView(map);

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



        //Update Shapes ZQSD
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z) )
            voiture.accelerate(deltaTime, 1 );
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) )
            voiture.accelerate(deltaTime, -1 );
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q) )
            voiture.turn(deltaTime, -1 );
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)  )
            voiture.turn(deltaTime, 1 );

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
        voiture.update(deltaTime, map);


        map.highlightZone(voiture.getZone());




        window.clear();

        //Views
        view.setCenter(voiture.sprite.getPosition());
        window.setView(view);

        //draw
        window.draw(map);
        window.draw(voiture.getSprite());

        //draw view
        sf::Vertex voiture_point (voiture.getPosition() , sf::Color::Magenta);
        sf::Vertex line [2];
        line[0] = voiture_point;
        for(int i = 0; i != NUM_VIEW; i++){
            line[1] = sf::Vertex(voiture.getView()[i], sf::Color::Magenta);
            // std::cout << i << " x : "<< voiture.getView()[i].x << " y :"<< voiture.getView()[i].y << std::endl;
            window.draw(line,2,sf::Lines);
            // std::cout << i << " : " << voiture.viewDistance[i] << std::endl;
        }

        //draw nearest checkPoint
        sf::Vertex line2 [2];

        line2[0] = sf::Vertex(voiture.getPosition() , sf::Color::Blue);
        line2[1] = sf::Vertex(voiture.pointCP, sf::Color::Blue);
        window.draw(line2,2,sf::Lines);

        window.setView(window.getDefaultView());
        window.draw(fps);
        window.display();
    }

    return 0;
    */
}
