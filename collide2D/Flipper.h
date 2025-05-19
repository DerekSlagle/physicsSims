#ifndef FLIPPER_H
#define FLIPPER_H

#include "segHits/lineSegFlip.h"
#include "mvHits/ball.h"

class Flipper
{
    public:
    lineSegFlip flipLine;
    lineSeg topSeg, bottomSeg;
    ball centerBall, endBall;

    void init( std::istream& fin );
    bool hit( ball& b );
    void update( float dt );
    void draw( sf::RenderTarget& rRW )const;
    void setPosition( vec2d pos );
    void setPosition( float x, float y ) { setPosition( vec2d(x,y) ); }
    void resetEdges();

    bool is_up()const;
    void init_up();// trigger actions
    void init_dn();
    void set_up();// set state
    void set_dn();

    Flipper(){}
    ~Flipper(){}

};

#endif // FLIPPER_H
