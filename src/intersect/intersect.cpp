#include "intersect.hpp"


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
        // std::cout << "les 2 segments sont //" << std::endl;
        return boost::none;
    }

    float inv_det = 1 / det;
    float M_inv[2][2] = {inv_det * M[1][1], inv_det * -M[0][1], inv_det * -M[1][0], inv_det * M[0][0]};

    // M_inv * Y
    float X[2] = { M_inv[0][0] * V[0] + M_inv[0][1] * V[1], M_inv[1][0] * V[0] + M_inv[1][1] * V[1]  };

    //
    if( !(0 < X[0] && X[0] < 1 && 0 < X[1] && X[1] < 1)){
        // std::cout << "les 2 segments ne sont pas // mais ne se croisent pas" << std::endl;
        return boost::none;
    }

    sf::Vertex S ;
    S.position.x = A.position.x + X[0] * (B.position.x - A.position.x);
    S.position.y = A.position.y + X[0] * (B.position.y - A.position.y);

    return S;
}

boost::optional<sf::Vertex> intersectV2( sf::Vertex A, sf::Vertex B, sf::Vertex C, sf::Vertex D){

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
        // std::cout << "les 2 segments sont //" << std::endl;
        return boost::none;
    }

    float inv_det = 1 / det;
    float M_inv[2][2] = {inv_det * M[1][1], inv_det * -M[0][1], inv_det * -M[1][0], inv_det * M[0][0]};

    // M_inv * Y
    float X[2] = { M_inv[0][0] * V[0] + M_inv[0][1] * V[1], M_inv[1][0] * V[0] + M_inv[1][1] * V[1]  };

    //
    if( !(0 <= X[0] && X[0] <= 1 && 0 <= X[1] && X[1] <= 1)){
        // std::cout << "les 2 segments ne sont pas // mais ne se croisent pas" << std::endl;
        return boost::none;
    }

    sf::Vertex S ;
    S.position.x = A.position.x + X[0] * (B.position.x - A.position.x);
    S.position.y = A.position.y + X[0] * (B.position.y - A.position.y);

    return S;
}
