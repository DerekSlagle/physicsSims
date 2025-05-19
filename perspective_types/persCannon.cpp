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
    is >> moveSpeed >> barrelLength >> barrelWidth;
    is >> rd >> gn >> bu;// barrel color
    is >> tFuse >> yawRate >> pitchRate;

    isMoving = false;
    firingEnabled = false;// respond to fire signals
    tElap = tFlight = 0.0f;
    aimingHigh = false;

    vtx[0].color = vtx[1].color = vtx[2].color = vtx[3].color = sf::Color(rd,gn,bu);
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

 //   update_doDraw();// base version uses Rbound and pos
 //   if( doDraw )
 //   {
 //       update_rw();
 //   }

    if( firingEnabled )
    {
        if( *pTrigger )// is pulled
        {
            if( !prevTriggerState )// just pulled
            {
                bool gotVel = mortarMode ? getMotarVelocity() : getFiringVelocity();
                if( gotVel )
                {
                    bool gotShot = fragMode ? pFireFragShot( pos, shotVel, tFlight - tFuse ) : pFireShot( pos, shotVel, false );
                    if( gotShot )
                    {
                        vec3f fbPos = pos + shotVel*( ( barrelLength + 40.0f )/shotVel.mag() );
                        if( pGetFireball )  pGetFireball( fbPos );
                    }
                }
                tElap = 0.0f;
            }

            tElap += dt;
            if( tElap > dtFire )// fire!
            {
                bool gotVel = mortarMode ? getMotarVelocity() : getFiringVelocity();
                if( gotVel )
                {
                    bool gotShot = fragMode ? pFireFragShot( pos, shotVel, tFlight - tFuse ) : pFireShot( pos, shotVel, false );
                    if( gotShot )
                    {
                        vec3f fbPos = pos + shotVel*( ( barrelLength + 40.0f )/shotVel.mag() );
                        if( pGetFireball )  pGetFireball( fbPos );
                    }
                }
                tElap = 0.0f;
            }
        }
    }

    prevTriggerState = *pTrigger;

    update_doDraw();// base version uses Rbound and pos
    if( doDraw )
    {
        update_rw();
        setBarrelOrientation();
    }
}

void persCannon::setMortarMode( bool MM )
{
    mortarMode = MM;
    if( !MM ) return;// nothing to do

    // trigger barrel elevation animation
    vec3f vu = shotVel/shotSpeed;
    vec3f Rxz = vu - ( vu.dot( persPt::yHat ) )*persPt::yHat;
    vec3f vum = sinf( firingAngle )*persPt::yHat + cosf( firingAngle )*Rxz;
    vum /= vum.mag();
    angle = acosf( vu.dot(vum) );
    Trot = vu.cross(vum);
    Trot /= Trot.mag();
    varAngle = 0.0f;
    rotRate = pitchRate;
    isAiming = true;
    isYawing = false;
    firingEnabled = false;
    fireOnceAimed = false;
}


void persCannon::startAiming( vec3f aimPt )
{
    // yaw 1st. common assigns
    varAngle = 0.0f;
    isAiming = isYawing = true;
    firingEnabled = false;
    rotRate = yawRate;
    if( pTrigger ) { fireOnceAimed = *pTrigger ? true : false; }

    if( mortarMode )
    {
        // yaw only
        vec3f sep = AimPt - pos;
        vec3f P0 = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;// xz component of sep
        AimPt = newAimPt = aimPt;
        sep = AimPt - pos;
        vec3f Pf = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;// xz component of sep
        P0 /= P0.mag();
        Pf /= Pf.mag();
        Trot = P0.cross( Pf );
        Trot /= Trot.mag();
        angle = acosf( Pf.dot(P0) );
    }
    else// not for mortar mode
    {
        AimPt = newAimPt = aimPt;
        // find mid point at end of yaw motion = newPt - y component of delta
        vec3f P = shotVel;// start orientation
        P /= P.mag();// normalize
        if( !getFiringVelocity() ){ std::cout << "\n new AimPt out of range"; return; }
        vec3f Pf = shotVel/shotSpeed;
        vec3f P1 = Pf - ( Pf - P ).dot( persPt::yHat )*persPt::yHat;
        angle = asinf( P.cross(P1).dot( persPt::yHat ) );// pos or neg value
        Trot = P.cross(P1);
        Trot /= Trot.mag();
        shotVel = P;// must restore. Rotation will be off this.
    }
}

bool persCannon::update_turret( float dt )
{
    if( !isAiming ) return false;

    vec3f P = shotVel.rotate_axis( Trot, rotRate*dt );
    setAimPt(P);// assigns AimPt
    shotVel = P;

 //   vec3f P = shotVel.rotate_axis( Trot, varAngle );
 //   setAimPt(P);// assigns AimPt

    if( angle > 0.0f && ( varAngle += rotRate*dt ) < angle ) return true;
    if( angle < 0.0f && ( varAngle -= rotRate*dt ) > angle ) return true;

    if( isYawing )
    {
        // stop yawing and start pitching
        varAngle = 0.0f;
        isYawing = false;
        if( mortarMode )// finished with yaw = done
        {
            isAiming = false;
            firingEnabled = true;
            isAiming = false;
            AimPt = newAimPt;
            if( fireOnceAimed ) fireAshot();
            return false;
        }
        // on to pitch animation
        P /= P.mag();
        AimPt = newAimPt;
        getFiringVelocity();// again.
    //    if( mortarMode ) getMotarVelocity();
     //   else getFiringVelocity();
        vec3f Pf = shotVel/shotSpeed;
    //    Trot = P.cross( Pf );
        Trot = P.cross( Pf );
        Trot /= Trot.mag();
        rotRate = pitchRate;
        shotVel = P;// must restore. Rotation will be off this.
        angle = asinf( P.cross(Pf).dot( Trot ) );
        return true;
    }

    // motion is finished
    setAimPt( newAimPt );
    isAiming = false;
    firingEnabled = true;
    if( fireOnceAimed )
    {
        fireAshot();
     /*   bool gotVel = mortarMode ? getMotarVelocity() : getFiringVelocity();
        if( gotVel )
        {
            bool gotShot = fragMode ? pFireFragShot( pos, shotVel, tFlight - tFuse ) : pFireShot( pos, shotVel, false );
            if( gotShot )
            {
                vec3f fbPos = pos + shotVel*( ( barrelLength + 40.0f )/shotVel.mag() );
                if( pGetFireball )  pGetFireball( fbPos );
            }
        }   */
    }

    return false;
}

bool persCannon::fireAshot()
{
    bool gotVel = mortarMode ? getMotarVelocity() : getFiringVelocity();
    if( gotVel )
    {
        bool gotShot = fragMode ? pFireFragShot( pos, shotVel, tFlight - tFuse ) : pFireShot( pos, shotVel, false );
        if( gotShot )
        {
            vec3f fbPos = pos + shotVel*( ( barrelLength + 40.0f )/shotVel.mag() );
            if( pGetFireball )  pGetFireball( fbPos );
            return true;
        }
    }

    return false;
}

void persCannon::update_rw()// scale and position the baseImg
{
    persPt::set_rw( pos, Rbound, baseImg );
}

void persCannon::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
 //   RT.draw( baseImg );

    vec3f sep = pos - persPt::camDir;
    if( sep.dot( shotVel ) > 0.0f )// barrel is behind base
    {
        RT.draw( vtx, 4, sf::Quads );
        RT.draw( baseImg );
    }
    else
    {
        RT.draw( baseImg );
        RT.draw( vtx, 4, sf::Quads );
    }
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

bool persCannon::getMotarVelocity()
{
    vec3f sep = AimPt - pos;
    float h = sep.dot( persPt::yHat );
    vec3f rxz = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;
    float R = rxz.mag();// horizontal
    float u = tanf( firingAngle );
    if( u < h/R ) return false;// shot is too steep
    rxz /= R;// now unit length
    float gy = gravY < 0.0f ? -gravY : gravY;// must be positive
    float V0 = R*sqrtf( 0.5f*gy*( 1.0f + u*u )/( u*R - h ) );
    float sA = u/sqrtf( 1.0f + u*u ), cA = sqrtf( 1.0f - sA*sA );
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

void persCannon::setAimPt( vec3f aimPt )// assigns barrel ( vtx[] ) positions
{
    AimPt = aimPt;
    // find Tw, Th
    vec3f vu = shotVel/shotVel.mag();// aka Th. future dm?
    vec3f P = pos + vu*( Rbound + 0.5f*barrelLength );
    vec3f sep = persPt::camPos - P;
    vec3f Tw = sep.cross( vu );
    Tw /= Tw.mag();
    setBarrelOrientation( P, Tw, vu );
}

// assign vtx[].positions and return surface normal Nu
vec3f persCannon::setBarrelOrientation( vec3f ctrPos, vec3f Tw, vec3f Th )// general orientation: pt[0], pt[1] toward Tu
{
    float H = 0.5f*barrelLength, W = 0.5f*barrelWidth;
    vec3f pt[4];
    pt[0] = ctrPos + H*Th - W*Tw;
    pt[1] = ctrPos + H*Th + W*Tw;
    pt[2] = ctrPos - H*Th + W*Tw;
    pt[3] = ctrPos - H*Th - W*Tw;
    for( size_t i = 0; i < 4; ++i )
        vtx[i].position = persPt::get_xyw( pt[i] );

    return Th.cross( Tw );
}

void persCannon::setBarrelOrientation()// calls above based on shotVel and camPos
{
    // find Tw, Th
    vec3f vu = shotVel/shotVel.mag();// aka Th. future dm?
    vec3f P = pos + vu*( Rbound + 0.5f*barrelLength );
    vec3f sep = persPt::camPos - P;
    vec3f Tw = sep.cross( vu );
    Tw /= Tw.mag();
    setBarrelOrientation( P, Tw, vu );
}
