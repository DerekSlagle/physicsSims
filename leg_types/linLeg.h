#ifndef LINLEG_H
#define LINLEG_H

#include "Leg.h"

class linLeg : public Leg
{
    public:
        float xf, yf;
        linLeg(): Leg(0.0f,0.0f,1.0f), xf(1.0f), yf(0.0f) {}
        linLeg( float X0, float Y0, float Xf, float Yf );// 1st leg
        void init( float X0, float Y0, float Xf, float Yf );
        linLeg( Leg& rPrev, float Xf, float Yf );// mid legs
        void init( Leg& rPrev, float Xf, float Yf );// mid legs
        linLeg( Leg& rPrev, Leg& rNext );// last (tie) leg
        linLeg( float X0, float Y0, Leg& rNext );// entry leg

        virtual float x( float s )const { return x0 + s*(xf-x0)/len; }
        virtual float y( float s )const { return y0 + s*(yf-y0)/len; }
        virtual ~linLeg();

        virtual void T( float s, float& Tx, float& Ty )const;// provide unit tangent to curve at s

    protected:

    private:
};

#endif // LINLEG_H
