#ifndef MOUSELEG_H_INCLUDED
#define MOUSELEG_H_INCLUDED

#include "Leg.h"
#include "../button_types/button.h"

class mouseLeg : public Leg
{
    public:
        static float xLast, yLast;
        static float xNow, yNow;
        mouseLeg(): Leg(0.0f,0.0f,1.0f) {}

        virtual float x( float s )const { return button::mseX; }
        virtual float y( float s )const { return button::mseY; }
        virtual ~mouseLeg(){}

        virtual const Leg* update( float& rx, float& ry, float& s, float& v, float dt )const;
        virtual void T( float s, float& Tx, float& Ty )const;// provide unit tangent to curve at s
};

#endif // MOUSELEG_H_INCLUDED
