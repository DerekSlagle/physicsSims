#ifndef FLEXLEG_H_INCLUDED
#define FLEXLEG_H_INCLUDED

#include "linLeg.h"

class flexLeg : public linLeg
{
    public:
        float k;// deflection  from linLeg
        float nx, ny;// unit normal components, until a vec2d class gets here
        flexLeg( float X0, float Y0, float Xf, float Yf, float K=0.0f );// 1st leg
        flexLeg( Leg& rPrev, float Xf, float Yf, float K=0.0f );// mid legs
        flexLeg( Leg& rPrev, Leg& rNext, float K=0.0f );// last (tie) leg
        virtual float x( float s )const;
        virtual float y( float s )const;
        virtual ~flexLeg();

    protected:

    private:
};


#endif // FLEXLEG_H_INCLUDED
