#ifndef MATADAPTER_H
#define MATADAPTER_H

#include "persMat.h"

class matAdapter
{
public:
//    double x0 = 1.0f, x = 1.0f, x10 = 0.0f, x1 = 0.0f;
//    double z0 = 1.0f, z = 1.0f, z10 = 0.0f, z1 = 0.0f;
 //   double Etot = 1.0f;
    double Grav = 1.0;
    unsigned int numReps = 1;
    bool doAnimate = false;
    persMat* pPM = nullptr;
    persPt* pRider = nullptr;// for setPosition
 //   std::function<double(double,double,double,double)> pFx2 = nullptr;// d2x/dt2( x, x1, z, z1 )
 //   std::function<double(double,double,double,double)> pFz2 = nullptr;// d2z/dt2( x, x1, z, z1 )

    virtual void update( float dt ) = 0;
    virtual void reset() = 0;// to initial conditions
    virtual void setRiderPosition()const = 0;// { if( pPM && pRider ) pRider->setPosition( pPM->toWorldPos( x, z ) ); }
    virtual vec3f getVelocity()const = 0;
    // using pf, pf1, pf2
    void init( persMat& rPM, persPt& rRider ) { pPM = &rPM; pRider = &rRider; }// assign pFx2, pFz2 separately

    matAdapter( persMat& rPM, persPt& rRider ){ init( rPM, rRider ); }
    matAdapter(){}
    ~matAdapter(){}
};

#endif // MATADAPTER_H
