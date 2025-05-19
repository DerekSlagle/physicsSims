#include "mouseLeg.h"

float mouseLeg::xLast = 0.0f, mouseLeg::xNow = 0.0f;
float mouseLeg::yLast = 0.0f, mouseLeg::yNow = 0.0f;

const Leg* mouseLeg::update( float& rx, float& ry, float& s, float& v, float dt )const
{
    mouseLeg::xLast = rx;  mouseLeg::yLast = ry;// not data members so ok to write value in const qualified function
    rx = mouseLeg::xNow = button::mseX;
    ry = mouseLeg::yNow = button::mseY;
    v = sqrtf( (rx-xLast)*(rx-xLast) + (ry-yLast)*(ry-yLast) )/dt;
    return this;
}

void mouseLeg::T( float s, float& Tx, float& Ty )const// provide unit tangent to curve at s
{
 //   float xNow = button::mseX, yNow = button::mseY;
    if( abs(xNow-xLast) > 1.0f || abs(yNow-yLast) > 1.0f )// long enough to find a T
    {
        float disp = sqrtf( (xNow-xLast)*(xNow-xLast) + (yNow-yLast)*(yNow-yLast) );
        Tx = (xNow-xLast)/disp;
        Ty = (yNow-yLast)/disp;
    }
    else// fake it
    {
        Tx = 1.0f;
        Ty = 0.0f;
    }
}

/*
void mouseLeg::T( float s, float& Tx, float& Ty )const// provide unit tangent to curve at s
{
    float xNow = button::mseX, yNow = button::mseY;
    if( abs(xNow-xLast) > 1.0f || abs(yNow-yLast) > 1.0f )// long enough to find a T
    {
        float disp = sqrtf( (xNow-xLast)*(xNow-xLast) + (yNow-yLast)*(yNow-yLast) );
        Tx = (xNow-xLast)/disp;
        Ty = (yNow-yLast)/disp;
    }
    else// fake it
    {
        Tx = 1.0f;
        Ty = 0.0f;
    }
}
*/
