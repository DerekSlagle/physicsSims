#include "flexLeg.h"

// initial leg
flexLeg::flexLeg( float X0, float Y0, float Xf, float Yf, float K )
    : linLeg( X0, Y0, Xf, Yf ), k(K), nx((Xf-X0)/len), ny((Yf-Y0)/len)
{

}

// mid legs
flexLeg::flexLeg( Leg& rPrev, float Xf, float Yf, float K )
    : linLeg( rPrev, Xf, Yf ), k(K)
{
    nx = ( Xf - rPrev.x(rPrev.len) )/len;
    ny = ( Yf - rPrev.y(rPrev.len) )/len;
}

// last (tie) leg
flexLeg::flexLeg( Leg& rPrev, Leg& rNext, float K )
    : linLeg( rPrev, rNext ), k(K)
{
    nx = ( rNext.x(0.0f) - rPrev.x(rPrev.len) )/len;
    ny = ( rNext.y(0.0f) - rPrev.y(rPrev.len) )/len;
}

flexLeg::~flexLeg()
{
    //dtor
}

float flexLeg::x( float s )const
{
    const float dev = 2.0f*s/len - 1.0f;
    return linLeg::x(s) - ny*k*( 1.0f - dev*dev );
}

float flexLeg::y( float s )const
{
    const float dev = 2.0f*s/len - 1.0f;
    return linLeg::y(s) + nx*k*( 1.0f - dev*dev );
}
