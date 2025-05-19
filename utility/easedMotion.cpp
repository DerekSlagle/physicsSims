#include "easedMotion.h"

void easedMotion::init( double x0, double xf, double t_Final, unsigned int BasePow )
{
    X0 = x0;
    Xf = xf;
    delX = xf - x0;
    tElap = tFinal = t_Final;
    basePow = BasePow;
}

void easedMotion::start()// start after init
{
    tElap = u = 0.0;
    X = X0;
}

bool easedMotion::update( double dt )// true if tElap < tFinal after incrementation
{

    if( tElap < tFinal )
    {
        // increment
        bool done = false;
        tElap += dt;
        u = tElap/tFinal;
        if( u > 1.0 )
        {
            u = 1.0;
            done = true;

        }
        // evaluate
        Y = u;
        if( cVec.size() > 0 )
        {
            Y = cVec[0];
            double uPow = u;
            for( unsigned int j = 1; j < cVec.size(); ++j )
            {
                Y += uPow*cVec[j];
                uPow *= u;
            }
        }

        for( unsigned int j = 0; j < basePow; ++j )
            Y *= u;

        X = X0 + Y*delX;// update value

        if( done && pNext ) pNext->start();
        return done;
    }

    return false;
}
