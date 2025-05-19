#ifndef PERSDRONE_H
#define PERSDRONE_H

#include<queue>
#include "persPt.h"

class persDrone : public persPt
{
    public:
    persQuad droneQuad;
 //   sf::Vertex vtx[4];// the "Quad"
    float accelFactor = 1.0f;
    vec3f accel, Tu;// + vel are assigned by adapter
    vec3f accelSum;// sum of queue elements
    std::queue<vec3f> accelQueue;// use a running average acceleration to smooth Leg transitions

    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    void updateGrav( vec3f grav, float dt );
    vec3f getQueueAccel();// return running average over size of queue
    virtual void draw( sf::RenderTarget& RT ) const;
 //   virtual void update_doDraw();
 //   virtual bool hit( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const;// also persQuad

    void init( std::istream& is, spriteSheet* pSS );
    persDrone( std::istream& is, spriteSheet* pSS ){ init( is, pSS ); }
    persDrone(){}
    virtual ~persDrone(){}
};

#endif // PERSDRONE_H
