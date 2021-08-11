#include "intersect.hpp"


boost::optional<sf::Vector2f> intersect( sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D){

    /*
    M
    [ B.x - A.x , C.x - D.x ]
    [ B.y - A.y , C.y - D.y ]

    Y
    (C.x - A.x, C.y - A.y)

    */

    float M[2][2] = {{B.x - A.x , C.x - D.x},{B.y - A.y , C.y - D.y}};
    float V[2] = {C.x - A.x, C.y - A.y};
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

    sf::Vector2f S ;
    S.x = A.x + X[0] * (B.x - A.x);
    S.y = A.y + X[0] * (B.y - A.y);

    return S;
}

boost::optional<sf::Vector2f> intersectV2( sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D){
    /*
    M
    [ B.x - A.x , C.x - D.x ]
    [ B.y - A.y , C.y - D.y ]

    Y
    (C.x - A.x, C.y - A.y)

    */

    float M[2][2] = {{B.x - A.x , C.x - D.x},{B.y - A.y , C.y - D.y}};
    float V[2] = {C.x - A.x, C.y - A.y};
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

    sf::Vector2f S ;
    S.x = A.x + X[0] * (B.x - A.x);
    S.y = A.y + X[0] * (B.y - A.y);

    return S;
}

sf::Vector2f minimum_distance(sf::Vector2f v, sf::Vector2f w, sf::Vector2f p) {
  // Return minimum distance between line segment vw and point p
  const float l2 = pow(v.x - w.x, 2) + pow(v.y - w.y,2);  // i.e. |w-v|^2 -  avoid a sqrt
  if (l2 == 0.0) return w;   // v == w case
  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  double t = std::max(0.f, std::min(1.f, ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2));
  sf::Vector2f projection; //= v + t * (w - v);  // Projection falls on the segment
  projection.x = v.x + t * (w.x - v.x);
  projection.y = v.y + t * (w.y - v.y);

  std::cout << "dot : " << (p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y) << std::endl;
  std::cout << "l2 : " << l2 << std::endl;
  std::cout << "dot/l2 : " << ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2 << std::endl;

  std::cout << "v.x : " << v.x << " v.y : " << v.y << std::endl;
  std::cout << "w.x : " << w.x << " v.y : " << w.y << std::endl;
  std::cout << "p.x : " << p.x << " v.y : " << p.y << std::endl;

  std::cout << "x : " << projection.x << " y : " << projection.y << std::endl;
  return projection;
}

// function pDistance(sf::Vector2f v, sf::Vector2f w, sf::Vector2f p) {
//
//   float dot = (p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y);
//   float len_sq = pow(v.x - w.x, 2) + pow(v.y - w.y);
//   var param = -1;
//   if (len_sq != 0) //in case of 0 length line
//       param = dot / len_sq;
//
//   sf::Vector2f projection;
//
//   if (param < 0) {
//     projection.x = x1;
//     projection.y = y1;
//   }
//   else if (param > 1) {
//     projection.x = x2;
//     projection.y = y2;
//   }
//   else {
//     projection.x = x1 + param * (w.x - v.x);
//     projection.y = y1 + param * (w.y - v.y);
//   }
//
//   var dx = x - xx;
//   var dy = y - yy;
//   return Math.sqrt(dx * dx + dy * dy);
// }
