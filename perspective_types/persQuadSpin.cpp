#include "persQuadSpin.h"

void persQuadSpin::initSpin( std::istream& is, sf::Texture* p_Txt )// call instead of persQuad::init()
{
    vec3f Pos; is >> pos.x >> pos.y >> pos.z;
    is >> w >> h;
    float angPol; is >> angPol;// orientation in x,z plane
 //   vec3f nu; is >> nu.x >> nu.y >> nu.z;


    angVel = 0.0f;
    is >> hitStops >> angMin >> angMax;
    is >> M >> I >> Rcm;
    is >> orient;

    pTxt = p_Txt;
    Rbound = sqrtf( w*w + h*h )/2.0f;

    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    for( size_t i = 0; i < 4; ++i ) vtx[i].color = sf::Color(rd,gn,bu);

 //   vtx[0].color = sf::Color::Red;
 //   vtx[1].color = sf::Color::Red;

    Nu.x = sinf( angPol );
    Nu.z = cosf( angPol );
    Nu.y = 0.0f;
    N0 = Nu;
    rotAxis = orient == 'H' ? persPt::yHat.cross( Nu ) : persPt::yHat;

 //  setNu(Nu);
   setVtxPositions();
}

void persQuadSpin::update( float dt )
{
    persQuad::update(dt);
    if( !isSpinning ) return;

    vec3f th = rotAxis.cross( Nu );
    pos -= th*0.5f*h;// to rotAxis
    angle += angVel*dt;
    float angAccel = dragTorque/I;
    if( angVel < 0.0f ) angAccel *= -1.0f;
    angVel -= angAccel*dt;

    if( angVel > 0.0f && angle > 2.0f*vec2f::PI ) angle -= 2.0f*vec2f::PI;
    if( angVel < 0.0f && angle < 0.0f ) angle += 2.0f*vec2f::PI;

    if( hitStops )
    {
        if( angVel < 0.0f && angle < angMin ){ angle = angMin; angVel = 0.0f; isSpinning = false; }
        else if( angVel > 0.0f && angle > angMax ){ angle = angMax; angVel = 0.0f; isSpinning = false; }
    }

    // slow enough to stop?
    if( angVel*angVel < minAngVelMag*minAngVelMag )
    {
        angVel = 0.0f;
        isSpinning = false;
        std::cout << "\n spinning stopped! angle = " << angle;
    }

//    Nu = cosf( ang )*N0 + sinf( ang )*persPt::yHat;
    Nu = N0.rotate_axis( rotAxis, angle );
    th = rotAxis.cross( Nu );
    pos += th*0.5f*h;// back to center
 //   setNu(Nu);
    setVtxPositions();
 //   static int cnt = 0;
 //   std::cout << "\nSpinning  " << ++cnt << "  ang = " << ang*(180.0f/vec2f::PI);
 //   std::cout << "\nSpinning ang = " << angle*(180.0f/vec2f::PI);
}

void persQuadSpin::update( vec3f grav, float dt )
{
    persQuad::update(dt);
    if( !isSpinning ) return;

    vec3f th = rotAxis.cross( Nu );
    pos -= th*0.5f*h;// to rotAxis

    // drag torque
    float angAccel = dragTorque/I;
    if( angVel < 0.0f ) angAccel *= -1.0f;
    angVel -= angAccel*dt;
    // torque due to gravity
    float gravTorque = rotAxis.dot( Rcm*th.cross( M*grav ) );
    angVel += gravTorque*dt/I;

    angle += angVel*dt;

    if( angVel > 0.0f && angle > 2.0f*vec2f::PI ) angle -= 2.0f*vec2f::PI;
    if( angVel < 0.0f && angle < 0.0f ) angle += 2.0f*vec2f::PI;

    if( hitStops )
    {
        if( angVel < 0.0f && angle < angMin ){ angle = angMin; angVel = 0.0f; isSpinning = false; }
        else if( angVel > 0.0f && angle > angMax ){ angle = angMax; angVel = 0.0f; isSpinning = false; }
    }

    // slow enough to stop?
    if( angVel*angVel < minAngVelMag*minAngVelMag && gravTorque*gravTorque < dragTorque*dragTorque )
    {
        angVel = 0.0f;
        isSpinning = false;
        std::cout << "\n spinning stopped! angle = " << angle;
    }

    Nu = N0.rotate_axis( rotAxis, angle );
    th = rotAxis.cross( Nu );
    pos += th*0.5f*h;// back to center
    setVtxPositions();
}

void persQuadSpin::setRotation( float Angle )
{
    if( hitStops && ( Angle < angMin || Angle > angMax ) ) return;

    vec3f th = rotAxis.cross( Nu );
    pos -= th*0.5f*h;// to rotAxis
    angle = Angle;
    Nu = N0.rotate_axis( rotAxis, angle );
    th = rotAxis.cross( Nu );
    pos += th*0.5f*h;// back to center
    setVtxPositions();
}

bool persQuadSpin::hitSpin(  persBall& PB, float dt, float Cr, vec3f& P, vec3f& vu )
{
    vec3f relVel = PB.vel;
    if( isSpinning ) relVel -= 0.5f*h*angVel*Nu;// to project position in the previous frame
//    if( isSpinning ) relVel += 0.5f*h*angVel*Nu;// to project position in the previous frame

    if( persQuad::hit( PB.pos - relVel*dt, PB.pos, P, vu ) )
    {
        // backside hit - just bounce off
    //    if( PB.vel.dot( N0 ) > 0.0f ) return true;


        vec3f P1 = P - pos;// local
        vec3f th = rotAxis.cross( Nu );
        float rHit = P1.dot( th ) + 0.5f*h;
        float Vtn = -PB.vel.dot( Nu );
        float Kr = 1.0 + Cr;

        if( isSpinning )
        {
            float J = 0.5*rHit*Kr + I/(PB.mass*rHit);// was -
         //   angVel = ( Kr*Vtn + angVel*J )/( 0.5*rHit*Kr + I/(PB.mass*rHit) );// ??

            angVel += Kr*Vtn/J;// works
        }
        else
        {
            angVel = PB.mass*rHit*Vtn*( 1.0f + Cr )/( PB.mass*rHit*rHit + I );
            isSpinning = true;
        }


        // adjust vu to reflect reduction
        std::cout << "\nhitSpin(): angVel = " << angVel;
     //   float ang = acosf( Nu.dot( N0 ) );
     //   angle = acosf( Nu.dot( N0 ) );
        std::cout << "  ang = " << angle*(180.0f/vec2f::PI);

        if( hitStops )
        {
            if( angVel > 0.0 && angle >= angMax ) { angle = angMax; angVel = 0.0; isSpinning = false; return true; }
            else if( angVel < 0.0 && angle <= angMin ) { angle = angMin; angVel = 0.0; isSpinning = false; return true; }
        }

        // adjust vu
        PB.vel -= Kr*( PB.vel.dot(Nu) + angVel*rHit )*Nu;
        vu = PB.vel/PB.vel.mag();

        return true;
    }

    return false;
}

/*
bool persQuadSpin::hitSpin(  persBall& PB, float dt, float Cr, vec3f& P, vec3f& vu )
{

    if( persQuad::hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        // backside hit - just bounce off
        if( PB.vel.dot( N0 ) > 0.0f ) return true;

        isSpinning = true;
        vec3f P1 = P - pos;// local
        vec3f th = rotAxis.cross( Nu );
        float rHit = P1.dot( th ) + 0.5f*h;
        float Vtn = -PB.vel.dot( Nu );
    //    if( Vtn < 0.0f ) Vtn *= -1.0f;// new
        angVel = PB.mass*rHit*Vtn*( 1.0f + Cr )/( PB.mass*rHit*rHit + I );
   //     angVel *= -1.0f;
        // adjust vu to reflect reduction
        std::cout << "\nhitSpin(): angVel = " << angVel;
     //   float ang = acosf( Nu.dot( N0 ) );
        angle = acosf( Nu.dot( N0 ) );
        std::cout << "  ang = " << angle*(180.0f/vec2f::PI);
        return true;
    }

    return false;
}
*/

void persQuadSpin::setPosition( vec3f Pos )
{
    pos = Pos;
    setVtxPositions();
}

// call instead of setNu(Nu)
void persQuadSpin::setVtxPositions()
{
    vec3f th = rotAxis.cross( Nu );
    pt[0] = pos + 0.5f*h*th - 0.5f*w*rotAxis;
    pt[1] = pos + 0.5f*h*th + 0.5f*w*rotAxis;
    pt[2] = pos - 0.5f*h*th + 0.5f*w*rotAxis;
    pt[3] = pos - 0.5f*h*th - 0.5f*w*rotAxis;
    vtx[0].position = persPt::get_xyw( pt[0] );
    vtx[1].position = persPt::get_xyw( pt[1] );
    vtx[2].position = persPt::get_xyw( pt[2] );
    vtx[3].position = persPt::get_xyw( pt[3] );
}

void persQuadSpin::reset()// so that Nu = N0
{
    vec3f th = rotAxis.cross( Nu );
    pos -= th*(0.5f*w);
    Nu = N0;
    th = rotAxis.cross( Nu );
 //   pos += tw*(0.5f*w);
    pos += th*(0.5f*w);
 //   setNu(Nu);
    setVtxPositions();
    isSpinning = false;
    angVel = 0.0f;
    angle = 0.0f;
}
