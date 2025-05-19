#ifndef PART3D_H
#define PART3D_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <iostream>
#include "../vec2f.h"

class part3d
{
    public:
    static float winHeight;
    std::vector<vec3f> ptVec;// displacements in the Xu, Yu, Zu directions. 1st point is origin for part
    sf::VertexArray outlineVA, fillVA;
    std::vector<unsigned int> visitOrder;// for TriangleFan draw
    void setVisitOrder();// call after part is complete
    bool isComplete = false;
    sf::Color outlineColor, fillColor;
    vec3f BBctrPos;// bound box center
    void findBBctrPos();// assigns above
    // get a modified part
    part3d flipX()const;// all ptVec.y *= -1.0f
    part3d flipY()const;// all ptVec.x *= -1.0f
    part3d toBasis( vec3f Xu, vec3f Yu, vec3f Zu )const;

    void init( sf::Color OutlineColor, sf::Color FillColor );
    void addPoint( vec3f pt );
    void clear();
    void update( vec3f Origin, vec3f Xu, vec3f Yu, vec3f Zu );
    void draw( sf::RenderTarget& RT ) const;

    part3d(){}
    ~part3d(){}
};

#endif // PART3D_H
