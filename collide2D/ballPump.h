#ifndef BALLPUMP_H
#define BALLPUMP_H

#include<vector>
#include "mvHits/ball.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"

class ballPump
{
    public:
//    sf::Texture outletTexture;
    sf::CircleShape inletCS;// image
    sf::CircleShape runCS_inlet, runCS_outlet;// a small indicator light on each port
    sf::RectangleShape outletRS;

    vec2d posInlet, posOutlet, ejectVel;
    bool is_pumping = false;
    float Rinlet = 20.0f, Rmax = 100.0f;
    float Kr = 50.0;// attractive force constant acc = -k/r
    float Ka = 50.0;// acceleration perpendicular to sep acc = Ka*vPerp/r
    float delayLimit = 20.0f, delayCount = delayLimit;

    sf::Color runColor, stopColor, ejectColor;

    controlSurface paramsCS;
    multiSelector paramsMS;
    buttonValOnHit paramsVOH;
    bool init_controls( std::istream& is );

    void init( std::istream& is );
    void update( std::vector<ball>& bVec, float dt );
    bool suckUp( ball& b, float dt );// as above but one by one
    bool updateEjectTimer( float dt );
    void draw( sf::RenderTarget& RT )const;
    void draw_inlet( sf::RenderTarget& RT )const;// before drawing balls
    void draw_outlet( sf::RenderTarget& RT )const;// after drawing balls
    void turnOn( bool on );
    void setInletRadius( float r );
    void set_ejectVel( vec2d v );
    void set_ejectVel( float vx, float vy ){ set_ejectVel( vec2d(vx,vy) ); }
    void setPosition_inlet( sf::Vector2f pos );
    void setPosition_outlet( sf::Vector2f pos );
    ballPump(){}
    ~ballPump(){}
};

#endif // BALLPUMP_H
