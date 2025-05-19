#ifndef PERSQUADSPIN_H
#define PERSQUADSPIN_H

#include "persPt.h"


class persQuadSpin : public persQuad
{
    public:
    float M = 1.0f;
    float I = 10.0f;// moment of inertia
    char orient = 'H';// rotation axis is horizontal, or vertical = 'V'
    float Rcm = 0.5f*h;
    bool hitStops = true;
    float angMin = 0.0f, angMax = vec2f::PI;
    vec3f N0;// Nu when angle = 0
    float angVel = 0.0f, minAngVelMag = 0.1f;
    float angle = 0.0f;
    float dragTorque = 0.0f;// freely spinning
    vec3f rotAxis;
    bool isSpinning = false;// dynamic

    virtual void update( float dt );
    void update( vec3f grav, float dt );
    bool hitSpin( persBall& PB, float dt, float Cr, vec3f& P, vec3f& vu );
    void reset();// so that Nu = N0
    virtual void setPosition( vec3f Pos );
    void setRotation( float Angle );// set angle
    void setVtxPositions();

    void initSpin( std::istream& is, sf::Texture* p_Txt = nullptr  );// call instead of persQuad::init()
    persQuadSpin( std::istream& is, sf::Texture* p_Txt = nullptr ){ initSpin( is, p_Txt ); }

    persQuadSpin(){}
    virtual ~persQuadSpin(){}
};

#endif // PERSQUADSPIN_H
