#include "linLeg.h"

// initial leg
linLeg::linLeg( float X0, float Y0, float Xf, float Yf )
    : Leg( X0, Y0, sqrt((Xf-X0)*(Xf-X0)+(Yf-Y0)*(Yf-Y0)) ), xf(Xf), yf(Yf)
{

}

void linLeg::init( float X0, float Y0, float Xf, float Yf )
{
    x0 = X0; y0 = Y0;
    xf = Xf; yf = Yf;
    len = sqrt((Xf-X0)*(Xf-X0)+(Yf-Y0)*(Yf-Y0));
    prev = next = nullptr;
}

// mid legs
linLeg::linLeg( Leg& rPrev, float Xf, float Yf ): Leg( rPrev.x(rPrev.len), rPrev.y(rPrev.len), 1.0f ), xf(Xf), yf(Yf)
{
    len = sqrt((xf-x0)*(xf-x0)+(yf-y0)*(yf-y0));
    rPrev.next = this;
    prev = &rPrev;
}

void linLeg::init( Leg& rPrev, float Xf, float Yf )
{
    x0 = rPrev.x(rPrev.len); xf = Xf;
    y0 = rPrev.y(rPrev.len); yf = Yf;
    len = sqrt((xf-x0)*(xf-x0)+(yf-y0)*(yf-y0));
    rPrev.next = this;
    prev = &rPrev;
}

// last (tie) leg
linLeg::linLeg( Leg& rPrev, Leg& rNext )
    : Leg( rPrev.x(rPrev.len), rPrev.y(rPrev.len), 1.0f ), xf(rNext.x(0.0f)), yf(rNext.y(0.0f))
{
    len = sqrt((xf-x0)*(xf-x0)+(yf-y0)*(yf-y0));
    rPrev.next = this;
    prev = &rPrev;
    rNext.prev = this;
    next = &rNext;
}

// entry leg
linLeg::linLeg( float X0, float Y0, Leg& rNext ): Leg( X0, Y0, 1.0f ), xf(rNext.x0), yf(rNext.y0)
{
    len = sqrt((xf-x0)*(xf-x0)+(yf-y0)*(yf-y0));
    next = &rNext;
    prev = nullptr;
}

linLeg::~linLeg()
{
    //dtor
}

void linLeg::T( float s, float& Tx, float& Ty )const// provide unit tangent to curve at s
{
    (void)s;
    Tx = (xf-x0)/len;
    Ty = (yf-y0)/len;
}
