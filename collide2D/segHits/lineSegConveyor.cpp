#include "lineSegConveyor.h"
#include "../mvHits/mvHit.h"

 void lineSegConveyor::init( std::istream& fin )
 {
    lineSeg::init( fin );// base overload
    fin >> vel >> Cf;
    uVel = ( vel/L.mag() )*L;
    if( vel < 0.0f ) uVel *= -1.0f;
    friction_on = true;

    const float Ron = 6.0f;
    onCS.setRadius( Ron );
    onCS.setOrigin( Ron, Ron );
    onCS.setPosition( sf::Vector2f( pos.x + 0.5f*L.x, pos.y + 0.5f*L.y + Ron ) );
    onCS.setFillColor( sf::Color::Green );// on
 }

void lineSegConveyor::to_file( std::ofstream& fout )
{
    fout << "C\n";
    segHit::to_file(fout);
    fout << ' ' << pos.x + L.x << ' ' << pos.y + L.y << '\n';
    unsigned int rd = vtx[0].color.r, gn = vtx[0].color.g, bu = vtx[0].color.b;
    fout << rd << ' ' << gn << ' ' << bu << '\n';
    fout << vel << '\n';
}

bool lineSegConveyor::hit( mvHit& mh )
{
    if( isRunning ) mh.v -= vel*uVel;
    bool Hit = lineSeg::hit( mh );
    if( isRunning ) mh.v += vel*uVel;

 //   bool Hit = lineSeg::hit( mh );
 //   if( Hit && mh.v.dot( uVel ) < vel )
  //      mh.v += Cf*vel*uVel;

    return Hit;
}

void lineSegConveyor::start()
{
    isRunning = true;
    onCS.setFillColor( sf::Color::Green );
}

void lineSegConveyor::stop()
{
    isRunning = false;
    onCS.setFillColor( sf::Color::Red );
}

void lineSegConveyor::draw( sf::RenderTarget& RT )const
{
    lineSeg::draw(RT);
    RT.draw( onCS );
}

void lineSegConveyor::update( float dt )
{
    if( !button::pButtMse )
    {
        if( button::clickEvent_Rt() == 1 )// right click
        {
            sf::Vector2f Rh = button::msePos() - onCS.getPosition();
            if( Rh.x*Rh.x + Rh.y*Rh.y < onCS.getRadius()*onCS.getRadius() )
            {
                if( isRunning ) stop();
                else start();
            }
        }
    }
}
