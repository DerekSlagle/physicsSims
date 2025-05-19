#include "persCannon.h"

void persCannon::init( std::istream& is, bool& rTrigger, std::function<bool( vec3f, vec3f, bool )> p_FireShot )
{
    pTrigger = &rTrigger;
    pFireShot = p_FireShot;

    Base.init(is);// A round image
    is >> shotSpeed >> dtFire;
    is >> gravY;
    is >> moveSpeed >> barrelLength;
    Rbound = Base.Rbound;
    pos = Base.pos;
    isMoving = Base.isMoving = false;
    Base.update(0.0f);
    doDraw = Base.doDraw;

//    firePos = pos + persPt::zHat*( 3.0f*Rbound );
    firingEnabled = false;// respond to fire signals
    tElap = tFlight = 0.0f;
    aimingHigh = false;
}

void persCannon::setPosition( vec3f Pos )
{
    Base.setPosition( Pos );
    Base.update(0.0f);
    pos = Base.pos;
    doDraw = Base.doDraw;
}

void persCannon::update( float dt )
{
    Base.update(dt);
    pos = Base.pos;// must match for below to work
    update_doDraw();// base version uses Rbound and pos
 //   if( !doDraw ) return;//{ std::cout << "\n doDraw false"; return; }
    if( !( pTrigger && pFireShot ) ) return;// firing not possible

 //   vec3f firePos;
    if( firingEnabled )
    {
        if( *pTrigger )// is pulled
        {
            if( !prevTriggerState )// just pulled
            {
                if( getFiringVelocity() )
                {
                //    firePos = pos + shotVel*( barrelLength/shotVel.mag() );
               //     pFireShot( firePos, shotVel, true );
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
                //    firePos = pos + shotVel*( barrelLength/shotVel.mag() );
                //   pFireShot( firePos, shotVel, true );
                    pFireShot( pos, shotVel, false );
                    if( pGetFireball )  pGetFireball( pos + shotVel*( barrelLength/shotVel.mag() ) );
                }
                tElap = 0.0f;
            }
        }
    }

    prevTriggerState = *pTrigger;

    // animated motion
    if( isMoving )
    {
        // Base is moving. Check to stop.
        vec3f sep = movePt - pos;
        if( sep.dot( vel ) < 0.0f )// passed it!
        {
            pos = movePt;
            Base.setPosition( movePt );
            isMoving = Base.isMoving = false;
            vel = Base.vel = vec3f();
        }
    }
}

void persCannon::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    Base.draw(RT);
}

bool persCannon::getFiringVelocity()
{

    vec3f sep = AimPt - Base.pos;
 //   vec3f sep = AimPt - firePos;
 //   sep += fireOfst*persPt::yu;// correct for cannon offset from view point
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
    vel = Base.vel = moveSpeed*sep/sepMag;
    isMoving = Base.isMoving = true;
}
