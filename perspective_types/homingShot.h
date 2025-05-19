#ifndef HOMINGSHOT_H
#define HOMINGSHOT_H

#include "persPt.h"

class homingShot : public persPt
{
    public:
    persBall ball;// the shot
    vec3f pos0;
    vec3f vel0;
    float tFlight = 0.0f;
    float tFlightMax = 10.0f;
    float Kaccel = 1000.0f;
    float Speed0 = 2000.0f;
    persPt *pTarget = nullptr;// has pos and vel to go by
    std::function<bool(persBall&,vec3f)> pGetAni = nullptr;// when ball inUse ends in update()

    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void update_doDraw() { ball.update_doDraw(); doDraw = ball.doDraw; }

    void init( std::istream& is );
    homingShot( std::istream& is ) { init(is); }
    homingShot(){}
    virtual ~homingShot(){}
};

#endif // HOMINGSHOT_H
