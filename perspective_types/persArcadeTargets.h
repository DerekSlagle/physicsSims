#ifndef PERSARCADETARGETS_H
#define PERSARCADETARGETS_H

//#include "persPt.h"
#include "persQuadSpin.h"

class persArcadeTargets : public persPt
{
    public:
    persQuad leftPQ, rightPQ, frontPQ;
    std::vector<persQuadSpin> PQSvec;

    float Length = 400.0f, Height = 140.0f;
 //   float frontWidth = 400.0f, sideWidth = 80.0f;// for frontPQ, leftPQ and rightPQ
    float speed = 3.0f;
    bool isRunning = false;
    vec3f Nf;// normal to front face

    virtual void setPosition( vec3f Pos );
 //   void setPosition( float X, float Y, float Z ) { setPosition( vec3f(X,Y,Z) ); }
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void update_doDraw();
    bool hit( persBall& PB, float dt, float Cr, vec3f& P, vec3f& vu );
    void resetTargets();

    void init( std::istream& is, std::vector<spriteSheet>& SSvec );
    persArcadeTargets(){}
    virtual ~persArcadeTargets(){}
};

#endif // PERSARCADETARGETS_H
