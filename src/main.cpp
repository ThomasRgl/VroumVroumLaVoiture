
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <boost/optional.hpp>
#include "csv/csv.hpp"

static const float VIEW_SIZE = 1000.f;


class Voiture  {

public:
    sf::Vector2f position;
    sf::Vector2f OldPosition;

    sf::Texture texture;
    sf::Sprite sprite;

    double apf = 250; //acceleration per frame
    double rpf = 100; // rotation per frame
    double DeltaFrotement = 3000;

    double speed;
    double angle;
    double acceleration;

    size_t zone;

    Voiture(sf::Texture _texture_, sf::Vector2f _position_ ){
        texture = _texture_;
        position = _position_;
        OldPosition = position;

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

    void accelerate(double deltaTime, int direction){
        acceleration += apf * direction * 0.05;
        // ax +=  apf * cos_x * deltaTime * direction;
        // ay +=  apf * sin_y * deltaTime * direction;
    }

    void turn(double deltaTime, int direction){
        if(speed >= 0)
            angle += deltaTime * rpf * direction;
        else
            angle += deltaTime * -rpf * direction;
        sprite.setRotation(angle);
    }

    void update(double deltaTime){

        OldPosition = position;

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

    sf::Sprite getSprite(){
        return sprite;
    }

    size_t getZone(){
        return zone;
    }

};

class Circuit : public sf::Drawable {
private:
public:
    std::vector<sf::Vertex> vertices;
    Circuit(std::string fileName){
        std::vector<std::pair<std::string, std::vector<int>>> test = read_csv("circuits/"+fileName);
        if(test[0].second.size() != test[1].second.size())
        exit(-1);
        if( test[0].second.size() < 6 || test[0].second.size() % 2 == 1)
        exit(-1);

        for (int i = 0; i != test[0].second.size(); i++){
            std::cout << test[0].second[i] << ',' <<  test[1].second[i] << std::endl;
            sf::Vertex vertex;
            vertex.position = sf::Vector2f(test[0].second[i], test[1].second[i]);
            vertex.color = sf::Color(128,128,128);
            vertices.push_back(vertex);
            // vertices.push_back(sf::Vertex(test[0].second[i], test[1].second[i]));
        }
    }
    Circuit(){}

    std::vector<sf::Vertex>* getVertices(){
        return &vertices;
    }

    void highlightZone( size_t n ){
        if( n < 0 || n > ((vertices.size() - 4)/2 - 1) ) {
            std::cout << "impossible de surligner cette zone" << std::endl;
            return;
        }

        for (int i = 0; i != vertices.size(); i++){
            vertices[i].color = sf::Color(128,128,128);
        }

        vertices[n * 2 + 0].color = sf::Color::Green;
        vertices[n * 2 + 1].color = sf::Color::Green;
        vertices[n * 2 + 2].color = sf::Color::Green;
        vertices[n * 2 + 3].color = sf::Color::Green;
    }

    void checkZone(Voiture voiture){
        size_t zone = voiture.getZone();

    }

    // void export(){}
    // virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const{
        target.draw(&vertices[0], vertices.size(), sf::TriangleStrip, states);
    }
};

boost::optional<sf::Vertex> intersect( sf::Vertex A, sf::Vertex B, sf::Vertex C, sf::Vertex D){
    /*
    M
    [ B.x - A.x , C.x - D.x ]
    [ B.y - A.y , C.y - D.y ]

    Y
    (C.x - A.x, C.y - A.y)

    */

    float M[2][2] = {{B.position.x - A.position.x , C.position.x - D.position.x},{B.position.y - A.position.y , C.position.y - D.position.y}};
    float V[2] = {C.position.x - A.position.x, C.position.y - A.position.y};
    float det = M[0][0] * M[1][1] - M[1][0] * M[0][1];
    if( det == 0){
        std::cout << "les 2 segments sont //" << std::endl;
        return boost::none;
    }

    float inv_det = 1 / det;
    float M_inv[2][2] = {inv_det * M[1][1], inv_det * -M[0][1], inv_det * -M[1][0], inv_det * M[0][0]};

    // M_inv * Y
    float X[2] = { M_inv[0][0] * V[0] + M_inv[0][1] * V[1], M_inv[1][0] * V[0] + M_inv[1][1] * V[1]  };

    //
    if( !(0 < X[0] && X[0] < 1 && 0 < X[1] && X[1] < 1)){
        std::cout << "les 2 segments ne sont pas // mais ne se croisent pas" << std::endl;
        return boost::none;
    }

    sf::Vertex S ;
    S.position.x = A.position.x + X[0] * (B.position.x - A.position.x);
    S.position.y = A.position.y + X[0] * (B.position.y - A.position.y);

    return S;
}

int main()
{

    sf::Vertex A;
    sf::Vertex B;
    sf::Vertex C;
    sf::Vertex D;

    A.position.x = 0;
    A.position.y = 0.5;

    B.position.x = 1;
    B.position.y = 0.5;

    C.position.x = 0;
    C.position.y = 0;

    D.position.x = 0;
    D.position.y = 1;

    boost::optional<sf::Vertex> S = intersect(A,B,C,D);

    if(S){
        std::cout << "x : " << S->position.x << std::endl;
        std::cout << "y : " << S->position.y << std::endl;
    }

    return 0 ;



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

    (*vertices)[0].color = sf::Color::Green;
    (*vertices)[1].color = sf::Color::Green;
    (*vertices)[2].color = sf::Color::Green;
    (*vertices)[3].color = sf::Color::Green;

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
