#include "persCannon.h"

void persCannon::init( std::istream& is, bool& rTrigger, std::function<bool( vec3f, vec3f, bool )> p_FireShot )
{
    pTrigger = &rTrigger;
    pFireShot = p_FireShot;

    // baseImg
    is >> pos.x >> pos.y >> pos.z >> vel.x >> vel.y >> vel.z;
    float R0; is >> R0;
    Rbound = R0;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    baseImg.setFillColor( sf::Color(rd,gn,bu) );
    baseImg.setRadius( R0 );
    baseImg.setOrigin( R0, R0 );
    update_doDraw();
    if( doDraw ) update_rw();

    is >> shotSpeed >> dtFire;
    is >> gravY;
    is >> moveSpeed >> barrelLength;

    isMoving = false;
    firingEnabled = false;// respond to fire signals
    tElap = tFlight = 0.0f;
    aimingHigh = false;
}

void persCannon::setPosition( vec3f Pos )
{
    pos = Pos;
    update_doDraw();
    if( doDraw ) update_rw();
}

void persCannon::update( float dt )
{
    if( !( pTrigger && pFireShot ) ) return;// firing not possible

    // animated motion
    if( isMoving )
    {
        pos += vel*dt;

        vec3f sep = movePt - pos;
        if( sep.dot( vel ) < 0.0f )// passed it!
        {
            pos = movePt;
            isMoving = false;
            vel = vec3f();
        }
    }

    update_doDraw();// base version uses Rbound and pos
    if( doDraw ) update_rw();

    if( firingEnabled )
    {
        if( *pTrigger )// is pulled
        {
            if( !prevTriggerState )// just pulled
            {
                if( getFiringVelocity() )
                {
                    pFireShot( pos, shotVel, false );
                    if( pGetFireball )  pGetFireball( pos + shotVel*( barrelLength/shotVel.mag() ) );
                }
                tElap = 0.0f;
            }

            tElap += dt;
            if( tElap > dtFire )// fire!
            {
                if( getFiringVelocity() )
                {
                    pFireShot( pos, shotVel, false );
                    if( pGetFireball )  pGetFireball( pos + shotVel*( barrelLength/shotVel.mag() ) );
                }
                tElap = 0.0f;
            }
        }
    }

    prevTriggerState = *pTrigger;
}

void persCannon::update_rw()// scale and position the baseImg
{
    persPt::set_rw( pos, Rbound, baseImg );
 //   float U = ( pos - persPt::camPos ).dot( persPt::camDir );
 //   if( U < 0.0f ) U *= -1.0f;
 //   float R = Rbound*persPt::Z0/U;
 //   baseImg.setRadius(R);
 //   baseImg.setOrigin(R,R);
 //   baseImg.setPosition( get_xyw(pos) );
}

void persCannon::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    RT.draw( baseImg );
}

bool persCannon::getFiringVelocity()
{

    vec3f sep = AimPt - pos;
    float h = sep.dot( persPt::yHat );// - fireOfst*persPt::yu.dot( persPt::yHat );// vertical component
    vec3f rxz = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;
    float R = rxz.mag();// horizontal
    rxz /= R;// now unit length
    float V0 = shotSpeed;// + camVel; it's not that simple
    float k = -gravY*R*R/(2.0f*shotSpeed*shotSpeed);
    float deter = R*R - 4.0f*k*( h + k );
    if( deter < 0.0f )
    {
     //   std::cout << "\nOut of range";
        tFlight = 0.0f;// temp cheat
        return false;
    }

    float tanAngle = aimingHigh ? R + sqrtf( deter ) : R - sqrtf( deter );
    tanAngle /= 2.0f*k;
    float sA = tanAngle/sqrtf( 1.0f + tanAngle*tanAngle ), cA = sqrtf( 1.0f - sA*sA );
    shotVel = V0*( cA*rxz + sA*persPt::yHat );// bingo
    tFlight = R/( V0*cA );
    return true;
}

void persCannon::startMove( vec3f movePos )// trigger
{
    movePt = movePos;
    vec3f sep = movePt - pos;
    float sepMag = sep.mag();
    vel = moveSpeed*sep/sepMag;
    isMoving = true;
}
