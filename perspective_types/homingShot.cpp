#include "homingShot.h"

void homingShot::init( std::istream& is )
{
    ball.init(is);
    pos0 = pos = ball.pos;
    vel0 = vel = ball.vel;
    tFlight = 0.0f;
    is >> Speed0 >> Kaccel >> tFlightMax;
    isMoving = true;
    ball.isMoving = true;
    ball.inUse = false;
}

void homingShot::setPosition( vec3f Pos )
{
    ball.setPosition( Pos );
    pos = pos0 = ball.pos;
}

void homingShot::update( float dt )
{
    if( !ball.inUse ) return;
    if( !pTarget ) return;

    update_doDraw();
 //   if( !doDraw ) return;

 //   std::cout << "\nHomer here!";
    vec3f N = ( persPt::camPos - ball.pos );
    tFlight += dt;
    if( tFlight > tFlightMax )
    {
    //    N = ( persPt::camPos - ball.pos );
        N /= N.mag();
        if( pGetAni ) pGetAni( ball, ball.pos + 10.0f*N );
        ball.inUse = doDraw = ball.doDraw  = false;
        std::cout << "\nHomer gone!";
        return;
    }

    if( pTarget )
    {
        // accelerate towards droneQuad
        float vMag = ball.vel.mag();
        ball.vel *= Speed0/vMag;// back to Speed0
        vMag = Speed0;
        vec3f vu = ball.vel*( 1.0f/vMag );
        vec3f sep = pTarget->pos - ball.pos;
        vec3f sepU = sep*( 1.0f/sep.mag() );
        vec3f acc = Kaccel*vMag*( vu.cross( sepU ) ).cross( vu );
        vec3f ipos = ball.pos;
        ball.pos += ball.vel*dt + acc*(0.5f*dt*dt);
        ball.vel += acc*dt;
        vec3f P, wu;
        if( pTarget->hit( ipos, ball.pos, P, wu ) )
        {
            N = ( persPt::camPos - ball.pos );
            if( pGetAni ) pGetAni( ball, ball.pos + N*( 10.0f/N.mag() ) );
            ball.inUse = doDraw = ball.doDraw  = false;
            std::cout << "\nHomer hit!";
        }
    }

    ball.update(dt);
}

void homingShot::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    ball.draw(RT);
}
