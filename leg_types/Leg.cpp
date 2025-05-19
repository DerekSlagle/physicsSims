#include "Leg.h"
#include "linLeg.h"

Leg::Leg( float X0, float Y0, float Len ): x0(X0), y0(Y0), len(Len), prev(nullptr), next(nullptr)
{
    //ctor
}

Leg::~Leg()
{
    //dtor
}

void Leg::T( float s, float& Tx, float& Ty )const// providde unit tangent to curve at s
{
    static const float ds = 0.01f;
    Tx = x(s+ds) - x(s-ds);
    Ty = y(s+ds) - y(s-ds);
    float magT = sqrtf( Tx*Tx + Ty*Ty );
    Tx /= magT;
    Ty /= magT;
}

const Leg* Leg::updateGrav( float& rx, float& ry, float& s, float& v, float dt, float g )const
{
    float dy = y(s+v) - y(s);
    float dv = g*dy/v;
//    if( v < g && v > -g ) dv = g;
//    v += dv;
    if( v < 0.0f && v > -g*dt ) v = g*dt;
    else if( v > 0.0f && v < g*dt ) v = -g*dt;
    else v += dv*dt;
    return update( rx, ry, s, v, dt );
}

const Leg* Leg::update( float& rx, float& ry, float& s, float& v, float dt )const
{
 //   s+= v*dt;
    s+= ( v + acc/2.0f )*dt;
    v += acc*dt;
    const Leg* pNewLeg = this;
    if( v < 0.0f )
    {
        if( s < 0.0f )
        {
            pNewLeg = prev;
            if( prev )
                s += pNewLeg->len;
            else s = 0.0f;
        }

    }
    else// v > 0
    {
        if( s > len )
        {
            pNewLeg = next;
            if( next )
                s -= len;
            else s = len;
        }
    }

    if( pNewLeg )
    {
        rx = pNewLeg->x(s);
        ry = pNewLeg->y(s);
    }
    else// rest at end of current Leg
    {
        rx = x(s);
        ry = y(s);
    }

    return pNewLeg;
}

// LegAdapter
/*
void LegAdapter::update( float dt )
{
    if( !( pLeg && pSetPosition ) ) return;
    if( delay > 0.0f ) { delay -= dt; return;}
//    v += acc*dt/2.0f;
    pLeg = pLeg->update( x, y, s, v, dt );
//    v += acc*dt/2.0f;
    pSetPosition( x + offX, y + offY );
}

void LegAdapter::init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x, float off_y )
{
    pLeg = pLeg0 = p_Leg;
    pSetPosition = p_SetPosition;
//    v = v1 = vel;
    v = vel;
 //   acc = 0.0f;
    offX = off_x;
    offY = off_y;
    s = 0.0f;
    if( pLeg && pSetPosition )
    {
        x = pLeg->x0 + offX;
        y = pLeg->y0 + offY;
        pSetPosition( x, y );
    }
}

void LegAdapter::init( linLeg& rLL, std::function<void(float,float)> p_SetPosition, float iPosx, float iPosy, float fPosx, float fPosy, float velFinal, float Acc, float Delay )
{
    rLL.init( iPosx, iPosy, fPosx, fPosy );
    //acc = Acc;
    rLL.acc = Acc;
    init( &rLL, p_SetPosition, rLL.V0( velFinal ) );
    delay = Delay;
}

void LegAdapter::reset( float V0, float Delay )
{
    s = 0.0f;
    pLeg = pLeg0;
    v = V0;
    delay = Delay;
    x = pLeg->x(0.0f);
    y = pLeg->y(0.0f);
    pSetPosition(x, y);
}

void LegAdapter::applyBrake( float k )
{
    if( abs(v) < k ) v = 0.0f;
    else if( v > 0.0f ) v -= k;
    else v += k;
}
*/
