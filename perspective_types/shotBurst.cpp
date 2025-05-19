#include "shotBurst.h"

//std::vector<persBall> shotBurst::fragPool;
std::function<bool( vec3f, vec3f, bool )> shotBurst::pFireShot = nullptr;
std::function<bool( shotBurst& rSB )> shotBurst::pGetFireBall = nullptr;

std::vector<vec3f> shotBurst::velVecArr[shotBurst::numPatterns];// unit fragment velocities
std::vector<vec3f> shotBurst::velVec;// unit fragment velocities
//bool shotBurst::anyFragsInUse = false;

// static functions
void shotBurst::init_stat()
{
    velVecArr[0].reserve( 22 );
    const float Pi = 3.141593f;
    vec3f temp;
    //                                   polar angle = theta   azimuth = phi
    velVecArr[0].push_back( temp.from_spherePolar( 1.0f, 0.0f, 0.0f ) );// straight up
    velVecArr[0].push_back( temp.from_spherePolar( 1.0f, 0.0f, Pi ) );// straight down

    unsigned N = 6;
    float theta = 0.0f;
    float phi = Pi/4.0f;
    for( unsigned n = 0; n < N; ++n )// 12
    {
        velVecArr[0].push_back( temp.from_spherePolar( 1.0f, theta, phi ) );
        velVecArr[0].push_back( temp.from_spherePolar( 1.0f, theta, -phi ) );
        theta += 2.0f*Pi/(float)N;
    }

    N = 8; theta = 0.0f; phi = Pi/4.0f;
    for( unsigned n = 0; n < N; ++n )
    {
        velVecArr[0].push_back( temp.from_spherePolar( 1.0f, theta, phi ) );
        theta += 2.0f*Pi/(float)N;
    }

    // conical
    velVecArr[1].reserve( 21 );
    velVecArr[1].push_back( temp.from_spherePolar( 1.0f, 0.0f, 0.0f ) );// straight up
    N = 6; theta = 0.0f; phi = Pi*0.05f;
 //   float dTheta = Pi/6.0f;
    for( unsigned n = 0; n < N; ++n )
    {
        velVecArr[1].push_back( temp.from_spherePolar( 1.0f, theta, phi ) );
        velVecArr[1].push_back( temp.from_spherePolar( 1.0f, theta + Pi/6.0f, phi ) );
        theta += 2.0f*Pi/(float)N;
    }
    N = 8; theta = 0.0f; phi = Pi*0.08f;
    for( unsigned n = 0; n < N; ++n )
    {
        velVecArr[1].push_back( temp.from_spherePolar( 1.0f, theta, phi ) );
        theta += 2.0f*Pi/(float)N;
    }

    // flat
    velVecArr[2].reserve( 24 );
    N = 8; theta = 0.0f; phi = Pi/2.0f;
    for( unsigned n = 0; n < N; ++n )
    {
        velVecArr[2].push_back( temp.from_spherePolar( 1.0f, theta, phi ) );
        velVecArr[2].push_back( temp.from_spherePolar( 1.0f, theta + 0.125f*Pi, phi )*0.67f );// at 2/3 speed
        velVecArr[2].push_back( temp.from_spherePolar( 1.0f, theta + 0.375f*Pi, phi )*0.33f );// at 1/3 speed
        theta += 2.0f*Pi/(float)N;
    }
}

// regular member functions
void shotBurst::init( std::istream& is )
{
    persBall::init(is);
    is >> tBoom >> Vfire >> Vfrags;
    isMoving = true;
    inUse = false;
}

void shotBurst::fire( vec3f Pos, vec3f Vel )// call to fire *this shot
{
    Vel *= Vfire/Vel.mag();
    persBall::reset( Pos, Vel );
    tElap = 0.0f;
}

void shotBurst::getBurst()// assigns a standard velocity pattern
{
    if( !shotBurst::pFireShot ) return;
  //  for( vec3f v3f : shotBurst::velVec )
    if( velPattern < shotBurst::numPatterns )
    {
        for( vec3f v3f : shotBurst::velVecArr[velPattern] )
        {
            v3f = v3f.x*persPt::xu + v3f.y*persPt::yu + v3f.z*persPt::camDir;
            shotBurst::pFireShot( pos + v3f*Rbound, vel + Vfrags*v3f, false );
        }
        if( shotBurst::pGetFireBall ) shotBurst::pGetFireBall( *this );
    }
}

void shotBurst::updateFuse( float dt )
{
    if( fuseMode == 'T' )
    {
        tElap += dt;
        if( tElap > tBoom )
        {
            getBurst();
            inUse = false;
            doDraw = false;
            tElap = 0.0f;
        }
    }
    else if( fuseMode == 'E' )
    {
        if( vel.dot( persPt::yHat ) < 0.0f && pos.y < Rdet )
        {
            getBurst();
            inUse = false;
            doDraw = false;
        }
    }
    else if( pTargetPos && fuseMode == 'P' )
    {
        float dist = ( *pTargetPos - pos ).mag();
        if( dist < Rdet )
        {
            getBurst();
            inUse = false;
            doDraw = false;
        }
    }
}

void shotBurst::update( float dt )
{
    if( !inUse ) return;

    persBall::update(dt);
    updateFuse(dt);
}

void shotBurst::update( vec3f grav, float dt )
{
    if( !inUse ) return;

    persBall::update( grav, dt );
    updateFuse(dt);
}
