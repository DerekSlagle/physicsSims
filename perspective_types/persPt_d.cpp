#include "persPt_d.h"

float persPt_d::X0 = 600.0f, persPt_d::Yh = 300.0f;// view point in window
double persPt_d::Z0 = 400.0;// for perspective scaling of x and y coordinates
vec3d persPt_d::camPos = vec3d(0.0,0.0,0.0);// camera position in scene
vec3d persPt_d::camDir = vec3d(0.0,0.0,1.0);// direction pointed
vec3d persPt_d::yu = vec3d(0.0,1.0,0.0);    // unit vector "up" (camera orientation)
vec3d persPt_d::xu = vec3d(1.0,0.0,0.0);    // unit vector "to right"
const vec3d persPt_d::xHat = persPt_d::xu;
const vec3d persPt_d::yHat = persPt_d::yu;
const vec3d persPt_d::zHat = persPt_d::camDir;
double persPt_d::angle = 0.0;// yaw
double persPt_d::pitchAngle = 0.0;// pitch
double persPt_d::rollAngle = 0.0;// roll

// static functions
// get the window position from the vec3f position in the scene
sf::Vector2f persPt_d::get_xyw( vec3d Pos )
{
    vec3d Rcp = Pos - camPos;// from camera to Pos
    double U = Rcp.dot( camDir );// projection along camDir = "z - Zcam"
    vec3d Rcp_perp = Rcp - U*camDir;// component of Rcp in plane perpendicular to camDir
    vec3d L = ( Z0/U )*Rcp_perp;// perspective transform applied
//    vec3f L = ( Z0/Rcp.mag() )*Rcp_perp;// perspective transform applied
    if( U < 0.0 ) L *= -1.0;// NEW - L flips when U -> + to -. This line counters.
    return sf::Vector2f( X0 + static_cast<float>( L.dot( xu ) ), Yh - static_cast<float>( L.dot( yu ) ) );
}

double persPt_d::changeCamDir( double dAy, double dAp, double dAr )// returns compass heading
{
    camDir = camDir*cos(dAy) + xu*sin(dAy);// yaw
    xu = yu.cross( camDir );

    camDir = camDir*cos(dAp) - yu*sin(dAp);// pitch
    yu = camDir.cross( xu );

    yu = yu*cos(dAr) + xu*sin(dAr);// roll
    yu /= yu.mag();// renormalize: this has proven necessary - NEW
    xu = yu.cross( camDir );

    // compass heading
    vec3d Rh = camDir - camDir.y*yHat;// component of camDir in x,z plane
    Rh /= Rh.mag();// normalize
    double a = acos( Rh.dot( zHat ) );
    if( Rh.dot( xHat ) < 0.0 ) a *= -1.0;
    return a*180.0/3.14159;
}

void persPt_d::calibrateCameraAxes()
{
    camDir /= camDir.mag();
    xu = yu.cross( camDir );
    xu /= xu.mag();
//    persPt::yu = persPt::xu.cross( persPt::camDir );
    yu = camDir.cross( xu );
}

void persPt_d::sortByDistance( std::vector<persPt_d*>& pPtVec )
{
    for( size_t iterCurr = 0; iterCurr + 1 < pPtVec.size(); ++ iterCurr )
    {
        if( !pPtVec[iterCurr]->doDraw ) continue;// just leave it in place and move to the next

        size_t iterMax = iterCurr;
        for( size_t i = iterCurr + 1; i < pPtVec.size(); ++i )
        {
            if( !pPtVec[iterCurr]->doDraw ) continue;
            if( pPtVec[i]->getDistance() > pPtVec[iterMax]->getDistance() )
                iterMax = i;
        }

        if( iterMax > iterCurr )// swap values
        {
            persPt_d* pPtTemp = pPtVec[ iterCurr ];
            pPtVec[ iterCurr ] = pPtVec[ iterMax ];
            pPtVec[ iterMax ] = pPtTemp;
        }
    }
}

// regular mfs
// persPt
void persPt_d::update_doDraw()
{
    doDraw = false;
    double U = camDir.dot( pos - camPos );
    if( U < Rbound ) return;// don't draw objects that are behind the camera
    sf::Vector2f winPos = get_xyw();
    float dim = static_cast<float>( Z0*Rbound/U );
    if( winPos.x + dim < 0.0f ) return;// left of window
    if( winPos.x - dim > 2.0f*X0 ) return;// right of window
    if( winPos.y + dim < 0.0f ) return;// above window
    if( winPos.y - dim > 2.0f*Yh ) return;// below window
    doDraw = true;
}

// persBall_d
void persBall_d::init( std::istream& is )
{
    vec3d Pos, Vel; is >> Pos.x >> Pos.y >> Pos.z >> Vel.x >> Vel.y >> Vel.z;
    double R0; is >> R0;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    init( Pos, R0, sf::Color(rd,gn,bu), Vel );
}

void persBall_d::init( vec3d Pos, double R0, sf::Color color, vec3d Vel )
{
    pos = Pos;
    Rbound = R0;
    vel = Vel;
    B.setRadius( R0 );
    B.setOrigin(R0,R0);
    B.setFillColor( color );
    update(0.0f);
}

void persBall_d::setPosition( vec3d Pos )
{
    pos = Pos;
    update(0.0f);
}

void persBall_d::update_rw()
{
    double U = ( pos - persPt_d::camPos ).dot( persPt_d::camDir );
    if( U < 0.0 ) U *= -1.0;
    float R = static_cast<float>( Rbound*persPt_d::Z0/U );
    B.setRadius(R);
    B.setOrigin(R,R);
    B.setPosition( get_xyw() );
}

void persBall_d::update( float dt )
{
    if( !inUse ) return;
    update_doDraw();

    if( doDraw || isMoving )
    {
        if( isMoving ) pos += vel*dt;
        update_rw();
    }
}

void persBall_d::update( vec3d grav, float dt )
{
    if( !inUse ) return;
    if( isMoving ) vel += grav*dt;
    update(dt);
}

void persBall_d::reset()
{
    inUse = true;
}

void persBall_d::reset( vec3d Pos, vec3d Vel )
{
    inUse = true;
    vel = Vel;
    setPosition( Pos );
}

void persBall_d::draw( sf::RenderTarget& RT ) const
{
    if( inUse && doDraw ) RT.draw( B );
}

bool persBall_d::hitFixed( vec3d posA, double r1, vec3d posB, vec3d& P, vec3d& vu )const// *this is stationary
{
    if( isMoving ) return false;

    vec3d rA = posA - pos, rB = posB - pos;
    vec3d Tu = posB - posA; Tu /= Tu.mag();
    vec3d Tn = rB.cross( Tu ); Tn /= Tn.mag();
    vec3d T1 = Tu.cross( Tn );
    double r2 = Rbound;
    if( rB.mag() > 3.0*( r1 + r2 ) ) return false;// too far away
    double h = rA.dot( T1 );
    if(  h >= r1 + r2 ) return false;// miss. ball centers are too far apart
    double w = sqrt( (r1+r2)*(r1+r2) - h*h );
    P = h*T1 - w*Tu;
    if( Tu.dot( P - rA ) < 0.0 || Tu.dot( P - rB ) > 0.0 ) return false;// posA and posB are on the same side of P
    // It's a hit!
    vec3d pu = P/P.mag();
    P += pos;// for return
    vec3d qu = Tn.cross( pu );
    vu = (Tu.dot(qu))*qu - (Tu.dot(pu))*pu;
    return true;
}

bool persBall_d::hitFree( persBall_d& Ball, double Cr, float dt )// both are moving. Collision is handled within function
{
    if( !Ball.isMoving ) return false;

    vec3d P, vu;
    if( !isMoving )// call hit fixed
    {
        if( hitFixed( Ball.pos - Ball.vel*dt, Ball.Rbound, Ball.pos, P, vu ) )
        {
            Ball.setPosition( P + 5.0*vu );
            Ball.vel = Cr*vu*Ball.vel.mag();
            return true;
        }
        return false;
    }
    // find P in frame where *this is at rest
    vec3d Vf = Ball.vel - vel, t1 = Vf/Vf.mag();
    vec3d A = Ball.pos - Vf*dt, B = Ball.pos;
    vec3d rA = A - pos, rB = B - pos;
    vec3d tn = rB.cross( t1 ); tn /= tn.mag();
    vec3d t2 = t1.cross( tn );
    double h = rB.dot( t2 );
    if(  h > Rbound + Ball.Rbound ) return false;
    double w = sqrt( (Ball.Rbound+Rbound)*(Ball.Rbound+Rbound) - h*h );
    P = h*t2 - w*t1;
    if( t1.dot( P - rA ) < 0.0 ) return false;
    if( t1.dot( rB - P ) < 0.0 ) return false;
    // find velocities in cm frame
    vec3d pu = P/P.mag();
    double M = mass + Ball.mass;
    vec3d Vcm = ( Ball.mass*Ball.vel + mass*vel )/M;
    vel -= Vcm; Ball.vel -= Vcm;// shift to cm frame
    vel = vel - ( 1.0 + Cr )*( vel.dot( pu ) )*pu;
    Ball.vel = Ball.vel - ( 1.0 + Cr )*( Ball.vel.dot( pu ) )*pu;
    vel += Vcm; Ball.vel += Vcm;// shift back to lab frame

    Ball.setPosition( P + pos + 5.0*Ball.vel/Ball.vel.mag() );

    return true;
}

// static function - mutual collision handling
void persBall_d::hitAll( std::vector<persBall_d>& PBvec, double Cr, float dt )
{
    if( PBvec.size() < 2 ) return;

    for( size_t i = 0; i + 1 < PBvec.size(); ++i )
        for( size_t j = i + 1; j < PBvec.size(); ++j )
            PBvec[i].hitFree( PBvec[j], Cr, dt );
}
