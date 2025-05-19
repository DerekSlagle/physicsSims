#include "legAdapter.h"
#include "linLeg.h"

void legAdapter::update( float dt )
{
    if( !( pLeg && pSetPosition ) ) return;
    if( delay > 0.0f ) { delay -= dt; return;}
    pLeg = pLeg->update( x, y, s, v, dt );
    x += offX; y += offY;
    if( pBaseLA ){ x += pBaseLA->x; y += pBaseLA->y; }
    pSetPosition( x, y );
}

void legAdapter::init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x, float off_y )
{
    pLeg = pLeg0 = p_Leg;
    pSetPosition = p_SetPosition;
    v = vel;
    offX = off_x;
    offY = off_y;
    if( pLeg )
    {
        s =  v > 0.0f ? 0.0f : pLeg->len;
        if( pSetPosition )
        {
            x = pLeg->x(s) + offX;
            y = pLeg->y(s) + offY;
            pSetPosition( x, y );
        }
    }
}

/*
void legAdapter::init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x, float off_y )
{
    pLeg = pLeg0 = p_Leg;
    pSetPosition = p_SetPosition;
    v = vel;
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
*/


void legAdapter::init( linLeg& rLL, std::function<void(float,float)> p_SetPosition, float iPosx, float iPosy, float fPosx, float fPosy, float velFinal, float Acc, float Delay )
{
    rLL.init( iPosx, iPosy, fPosx, fPosy );
    rLL.acc = Acc;
    init( &rLL, p_SetPosition, rLL.V0( velFinal ) );
    delay = Delay;
}


void legAdapter::reset( float V0, float Delay )
{
    s =  V0 > 0.0f ? 0.0f : pLeg0->len;
    pLeg = pLeg0;
    v = V0;
    delay = Delay;
    x = pLeg->x(s) + offX;
    y = pLeg->y(s) + offY;
    pSetPosition(x, y);
}

void legAdapter::applyBrake( float k )
{
    if( abs(v) < k ) v = 0.0f;
    else if( v > 0.0f ) v -= k;
    else v += k;
}

// derived
void inertialAdapter::update( float dt )
{
 //   legAdapter::update(dt); return;

    if( !pSetPosition ) return;
    if( delay > 0.0f )
    {
        delay -= dt;
    //    std::cout << "\ndelay: " << delay;
        if( delay <= 0.0f ) onLaunch();
        else return;
    }

    if( !pLeg )// free motion
    {
        x1 += v1x*dt; y1 += v1y*dt;
        pSetPosition( x1, y1 );
        return;
    }
    // path motion
    float x0 = x, y0 = y;// save
    pLeg = pLeg->update( x, y, s, v, dt );
    x += offX; y += offY;
    if( pBaseLA ){ x += pBaseLA->x; y += pBaseLA->y; }
    float vpx = (x-x0)/dt, vpy = (y-y0)/dt;// find velocity on path
    float Fx = Cd*( vpx - v1x ) + Cs*( x - x1 );// acceleration
    float Fy = Cd*( vpy - v1y ) + Cs*( y - y1 );

    v1x += Fx*dt/m; v1y += Fy*dt/m;
    x1 += v1x*dt; y1 += v1y*dt;
    pSetPosition( x1, y1 );
}

void inertialAdapter::update( float dt, float accx, float accy )// apply external acceleration
{
    if( delay <= 0.0f ){ v1x += accx*dt; v1y += accy*dt; }
    update(dt);
}

void inertialAdapter::onLaunch()// sync motion with pBaseLA
{
    if( !(pBaseLA && pBaseLA->pLeg && pLeg) ) return;
    x = pLeg->x(s); x += pBaseLA->x; x += offX;
    y = pLeg->y(s); y += pBaseLA->y; y += offY;
    x1 = x; y1 = y;
    pSetPosition(x,y);
    pLeg->T( s, v1x, v1y );// match initial velocity
    v1x *= v;
    v1y *= v;
    float v2x, v2y;// components
    pBaseLA->pLeg->T( pBaseLA->s, v2x, v2y );
    v2x *= pBaseLA->v; v2y *= pBaseLA->v;
    v1x += v2x; v1y += v2y;
}

void inertialAdapter::reset( float V0, float Delay )
{
    s = 0.0f;
    pLeg = pLeg0;
    if( !pLeg ) return;
    v = V0;
    delay = Delay;
    if( delay == 0.0f && pBaseLA ){ onLaunch(); return; }
    x = pLeg->x(0.0f) + offX;
    y = pLeg->y(0.0f) + offY;
 //   if( pBaseLA ) { x += pBaseLA->x; y += pBaseLA->y; }
    pSetPosition(x, y);
    x1 = x; y1 = y;
    pLeg->T( s, v1x, v1y );// match initial velocity
    v1x *= v;
    v1y *= v;
 /*   if( pBaseLA )// add velocity
    {
        float v2x, v2y;// components
        pBaseLA->pLeg->T( pBaseLA->s, v2x, v2y );
        v2x *= pBaseLA->v; v2y *= pBaseLA->v;
        v1x += v2x; v1y += v2y;
    }   */
}

bool inertialAdapter::hitRigid( float hitX, float hitY, float hitR )// collision response with fixed object
{
    float sepSq = ( hitX - x1 )*( hitX - x1 ) + ( hitY - y1 )*( hitY - y1 );
    if( sepSq > ( hitR + R )*( hitR + R ) ) return false;// too far apart
    if( sepSq < R*R ) return false;// too close. Abort

    float sep = sqrtf( sepSq );
    float sux = (x1 - hitX)/sep, suy = (y1 - hitY)/sep;// unit vector fit center to circ center
    x1 = hitX + sux*( R + hitR );// no penetration. Balls in contact
    y1 = hitY + suy*( R + hitR );
    pSetPosition( x1, y1 );

    float vMag = ( 1.0f + Cr )*( v1x*sux + v1y*suy );// bounce back
    v1x -= vMag*sux;
    v1y -= vMag*suy;

    return true;
}

bool inertialAdapter::hitFree( float& hitX, float& hitY, float& hitVx, float& hitVy, float hitR, float hitMass )
{
    float sepSq = ( hitX - x1 )*( hitX - x1 ) + ( hitY - y1 )*( hitY - y1 );
    if( sepSq > ( hitR + R )*( hitR + R ) ) return false;// too far apart
    if( sepSq < 1.0f ) return false;// too close. Abort

    float sep = sqrtf( sepSq );
    float sux = (x1 - hitX)/sep, suy = (y1 - hitY)/sep;// unit vector hit center to circ center
    float vRelX = v1x - hitVx, vRelY = v1y - hitVy;
    if( vRelX*sux + vRelY*suy > 0.0f ) return false;// moving apart )
    // center of mass
    float mTot = m + hitMass;
    float cmx = ( m*x1 + hitMass*hitX )/mTot;// position
    float cmy = ( m*y1 + hitMass*hitY )/mTot;
    float cmVx = ( m*v1x + hitMass*hitVx )/mTot;// velocity
    float cmVy = ( m*v1y + hitMass*hitVy )/mTot;
    // new position at R from cm
 //   x1 = cmx + R*sux; y1 = cmy + R*suy;
    x1 = cmx + (R+hitR)*hitMass*sux/mTot; y1 = cmy + (R+hitR)*hitMass*suy/mTot;
//    hitX = cmx - hitR*sux; hitY = cmy - hitR*suy;
    hitX = cmx - (R+hitR)*m*sux/mTot; hitY = cmy - (R+hitR)*m*suy/mTot;
    pSetPosition( x1, y1 );// on caller to setPosition the collider
    // new velocity. Reflect component in su direction in cm frame
    v1x -= cmVx; v1y -= cmVy;// to cm frame
    float vMag = ( 1.0f + Cr )*( v1x*sux + v1y*suy );// bounce back magnitude
    if( vMag < 0.0f ) { v1x -= vMag*sux; v1y -= vMag*suy; }// new in cm
    v1x += cmVx; v1y += cmVy;// back to screen frame

    hitVx -= cmVx; hitVy -= cmVy;//  to cm frame
    vMag = ( 1.0f + Cr )*( hitVx*sux + hitVy*suy );// bounce back
    hitVx -= vMag*sux; hitVy -= vMag*suy;// new in cm
    hitVx += cmVx; hitVy += cmVy;// back from cm frame
    return true;
}

bool inertialAdapter::hitFreeLine( float& hitX, float& hitY, float& hitVx, float& hitVy, float hitR, float hitMass )
{
    float vMag = sqrtf( v1x*v1x + v1y*v1y );
    if( vMag < ( R < hitR ? R/2.0f : hitR/2.0f ) && hitFree( hitX, hitY, hitVx, hitVy, hitR, hitMass ) )// half of smaller radius
    {
     //   if( pSetPosition ) pSetPosition(x1,y1);
        return true;// low speed algo employed
    }

    // high speed ray algo
    float Ax = x1 - v1x, Ay = y1 - v1y;// last frame position
    float sepSq = ( hitX - Ax )*( hitX - Ax ) + ( hitY - Ay )*( hitY - Ay );
    float sep = sqrtf( sepSq );
    float sux = ( hitX - Ax )/sep, suy = ( hitY - Ay )/sep;// unit vector hit center to circ center
    if( v1x*sux + v1y*suy < 0.0f ) return false;// receding

    float vux = v1x/vMag, vuy = v1y/vMag;
    float Rs = R + hitR;
    float vDotS = vux*sux + vuy*suy;// unit vectors = cosine
    if( vDotS*vDotS <= 1.0f - Rs*Rs/sepSq ) return false;// will miss

    float a = sep*vDotS - sqrtf( sep*sep*vDotS*vDotS + Rs*Rs - sep*sep );
    if( a > vMag ) return false;// target too far away

    // hit!
    x1 = Ax + a*vux; y1 = Ay + a*vuy;
    if( pSetPosition ) pSetPosition(x1,y1);

    // center of mass
    float mTot = m + hitMass;
    float cmVx = ( m*v1x + hitMass*hitVx )/mTot;// velocity
    float cmVy = ( m*v1y + hitMass*hitVy )/mTot;
    sux = (x1 - hitX)/Rs; suy = (y1 - hitY)/Rs;

    v1x -= cmVx; v1y -= cmVy;// to cm frame
    vMag = ( 1.0f + Cr )*( v1x*sux + v1y*suy );// bounce back magnitude
    if( vMag < 0.0f ) { v1x -= vMag*sux; v1y -= vMag*suy; }// new in cm
    v1x += cmVx; v1y += cmVy;// back to screen frame

    hitVx -= cmVx; hitVy -= cmVy;//  to cm frame
    vMag = ( 1.0f + Cr )*( hitVx*sux + hitVy*suy );// bounce back
    hitVx -= vMag*sux; hitVy -= vMag*suy;// new in cm
    hitVx += cmVx; hitVy += cmVy;// back from cm frame

    return true;
}

bool inertialAdapter::keepInBox( float left, float right, float top, float bottom )// handle collisions with inside of box
{
    bool Hit = false;

    if( x1 - R < left )
    {
        setPosition( left + R, y1 );
        if( v1x < 0.0f ) v1x *= -Cr;
        Hit = true;
    }
    else if( x1 + R > right )
    {
        setPosition( right - R, y1 );
        if( v1x > 0.0f ) v1x *= -Cr;
        Hit = true;
    }

    if( y1 - R < top )
    {
        setPosition( x1, top + R );
        if( v1y < 0.0f ) v1y *= -Cr;
        Hit = true;
    }
    else if( y1 + R > bottom )
    {
        setPosition( x1, bottom - R );
        if( v1y > 0.0f ) v1y *= -Cr;
        Hit = true;
    }

    return Hit;
}

// type impactTarget
float impactTarget::FceToDmgGrad = 0.1f;// damage = FceToDmgGrad*( v1 - vLast - vMin )
float impactTarget::dVelMin = 2.0f;// damage = FceToDmgGrad*( v1 - vLast - vMin )

void impactTarget::onLaunch()// sync motion with pBaseLA
{
    inertialAdapter::onLaunch();
    vLastX = v1x;
    vLastY = v1y;
}

void impactTarget::reset( float V0, float Delay )
{
    inertialAdapter::reset( V0, Delay );
    vLastX = v1x;
    vLastY = v1y;
}

void impactTarget::update( float dt )
{
    if( health <= 0.0f ) return;
    inertialAdapter::update(dt);// or even sharp acceleration here
    float dVx = v1x - vLastX;// vx1 maybe changed in hit() since last call
    float dVy = v1y - vLastY;// vx1 maybe changed in hit() since last call
//    inertialAdapter::update(dt);// ignores accel on path when called here
    float dVel = sqrtf( dVx*dVx + dVy*dVy );
    if( dVel > dVelMin )
    {
        health -= impactTarget::FceToDmgGrad*dVel;
        if( pSetHealth ) pSetHealth( health );
    }
    vLastX = v1x;
    vLastY = v1y;
}

bool impactTarget::keepInBox_soft( float left, float right, float top, float bottom )// handle collisions with inside of box
{
    bool Hit = false;

    if( x1 - R < left )
    {
     //   setPosition( left + R, y1 );
        v1x += 0.8f*dVelMin;
        Hit = true;
    }
    else if( x1 + R > right )
    {
    //    setPosition( right - R, y1 );
        v1x -= 0.8f*dVelMin;
        Hit = true;
    }

    if( y1 - R < top )
    {
    //    setPosition( x1, top + R );
        v1y += 0.8f*dVelMin;
        Hit = true;
    }
    else if( y1 + R > bottom )
    {
    //    setPosition( x1, bottom - R );
        v1y -= 0.8f*dVelMin;
        Hit = true;
    }

    return Hit;
}

bool freeMover::hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass )
{
    float Ax = x - vx, Ay = y - vy;// last frame position
    float sepSq = ( hitX - Ax )*( hitX - Ax ) + ( hitY - Ay )*( hitY - Ay );
    float sep = sqrtf( sepSq );
    float sux = ( hitX - Ax )/sep, suy = ( hitY - Ay )/sep;// unit vector hit center to circ center
    if( vx*sux + vy*suy < 0.0f ) return false;// receding

    float vMag = sqrtf( vx*vx + vy*vy );
    float vux = vx/vMag, vuy = vy/vMag;
    float Rs = R + hitR;
    float vDotS = vux*sux + vuy*suy;// unit vectors = cosine
    if( vDotS*vDotS <= 1.0f - Rs*Rs/sepSq ) return false;// will miss

    float a = sep*vDotS - sqrtf( sep*sep*vDotS*vDotS + Rs*Rs - sep*sep );
    if( a > vMag ) return false;// target too far away

    // hit!
    x = Ax + a*vux; y = Ay + a*vuy;
    if( pSetPosition ) pSetPosition(x,y);

    // center of mass
    float mTot = m + hitMass;
    float cmVx = ( m*vx + hitMass*hitVx )/mTot;// velocity
    float cmVy = ( m*vy + hitMass*hitVy )/mTot;
    sux = (x - hitX)/Rs; suy = (y - hitY)/Rs;

    vx -= cmVx; vy -= cmVy;// to cm frame
    vMag = ( 1.0f + Cr )*( vx*sux + vy*suy );// bounce back magnitude
    if( vMag < 0.0f ) { vx -= vMag*sux; vy -= vMag*suy; }// new in cm
    vx += cmVx; vy += cmVy;// back to screen frame

    hitVx -= cmVx; hitVy -= cmVy;//  to cm frame
    vMag = ( 1.0f + Cr )*( hitVx*sux + hitVy*suy );// bounce back
    hitVx -= vMag*sux; hitVy -= vMag*suy;// new in cm
    hitVx += cmVx; hitVy += cmVy;// back from cm frame

    return true;
}
