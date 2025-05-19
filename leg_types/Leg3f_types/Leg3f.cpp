#include "Leg3f.h"

Leg3f::Leg3f( vec3f Pos0, float Len ): pos0(Pos0), len(Len), prev(nullptr), next(nullptr)
{
    //ctor
}

Leg3f::Leg3f(): pos0(), len(1.0f), prev(nullptr), next(nullptr)
{
    //ctor
}

vec3f Leg3f::T( float s )const// provides unit tangent to curve at s
{
    static const float ds = 0.01f;
    vec3f v;// retVal
    v.x = x(s+ds) - x(s-ds);
    v.y = y(s+ds) - y(s-ds);
    v.z = z(s+ds) - z(s-ds);
    v /= v.mag();
    return v;
}

/*
const Leg* Leg::updateGrav( vec3f& pos, float& s, float& v, float dt, float g )const
{
    float dy = y(s+v) - y(s);
    float dv = g*dy/v;
//    if( v < g && v > -g ) dv = g;
//    v += dv;
    if( v < 0.0f && v > -g*dt ) v = g*dt;
    else if( v > 0.0f && v < g*dt ) v = -g*dt;
    else v += dv*dt;
    return update( rx, ry, s, v, dt );
}   */

const Leg3f* Leg3f::update( vec3f& pos, float& s, float v, float dt )const
{
    s+= v*dt;
 //   s+= ( v + acc/2.0f )*dt;
 //   v += acc*dt;
    const Leg3f* pNewLeg = this;
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
        pos = pNewLeg->getPos(s);
    }
    else// rest at end of current Leg
    {
        pos = getPos(s);
    }

    return pNewLeg;
}

// static function
void Leg3f::destroy3fPath( Leg3f*& pPath )// for dynamically allocated legs owned by this
{
    if( !pPath ) return;
    Leg3f* it = pPath;
    Leg3f* itNext = it->next;
    int n = 0;
    do
    {
        itNext = it->next;
        delete it;
        ++n;
        if( itNext == pPath ) break;
        it = itNext;
    }while( it );// accounts for closed paths

    pPath = nullptr;// safe off from further use
    std::cout << "\n destroy3fPath() # of legs deleted = " << n;
}

// Leg3f_adapter
void Leg3f_adapter::update( float dt )
{
    if( !pLegCurr ) return;
    if( !pSetPosition ) return;

    vec3f pos;
    pLegCurr = pLegCurr->update( pos, s, v, dt );
    if( pLegCurr )
    {
        if( pTu ) *pTu = pLegCurr->T(s);
        if( pVel ) *pVel = pLegCurr->T(s)*v;
        if( pAccel ) *pAccel = pLegCurr->accel(s,v);
    }
    pSetPosition( pos );
}

void Leg3f_adapter::init( const Leg3f& rLeg0, std::function<void(vec3f)> p_SetPosition )
{
    pLeg0 = &rLeg0;
    pLegCurr = pLeg0;
    pSetPosition = p_SetPosition;
    s = 0.0f;
}

void Leg3f_adapter::reset()
{
    s = 0.0f;
    pLegCurr = pLeg0;
    if( pLeg0 && pSetPosition ) pSetPosition( pLeg0->pos0 );
}

vec3f Leg3f_adapter::accel()const
{
    if( !pLegCurr ) return vec3f();
    return pLegCurr->accel(s,v);
}
