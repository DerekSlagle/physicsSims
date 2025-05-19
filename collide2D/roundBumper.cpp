#include "roundBumper.h"

unsigned int* roundBumper::pScore = nullptr;

bool roundBumper::init(std::ifstream& is )
{
    float rCore = 0.0f;
    is >> pos.x >> pos.y >> rCore >> rRing;
    is >> bumpSpeed >> bumpSteps >> scoreValue;

    unsigned int rd,gn,bu;
    is >> rd >> gn >> bu;
    normalColor = sf::Color(rd,gn,bu);
    litColor = sf::Color(255,255,255);
    core.setRadius(rCore);
    core.setPosition(pos.x, pos.y);
    core.setFillColor( normalColor );
    core.setOrigin( rCore,rCore );

    is >> rd >> gn >> bu;
    ring.setRadius(rRing);
    ring.setPosition(pos.x, pos.y);
    ring.setFillColor( sf::Color(rd,gn,bu) );
    ring.setOrigin( rRing, rRing );

    pFuncHit = nullptr;// assigned later

    return true;
}

bool roundBumper::hit( ball& b )
{
    vec2d sep = b.pos - pos;
    if( sep.dot( sep ) > ( b.r + rRing )*( b.r + rRing ) ) return false;
    float sepMag = sep.mag();
    if( sepMag < 1.0f ) return false;// shouldn't happen
    if( sep.dot( b.v ) > 0.0f ) return false;// moving away

    vec2d uSep = sep/sepMag;
    vel = bumpSpeed*uSep;
    b.v += vel - 2.0f*uSep.dot( b.v )*uSep;
    b.setPosition( pos + ( rRing + b.r )*uSep );
    state = 1;
    bumpIdx = 0;
    if( pFuncHit ) pFuncHit();// play sounds

    if( isLit() && roundBumper::pScore ) *roundBumper::pScore += scoreValue;

    return true;
}

void roundBumper::draw( sf::RenderTarget& RT )const
{
    RT.draw( ring );
    RT.draw( core );
}

void roundBumper::update( float dt )
{
    if( litTimer > 0.0f )
    {
        litTimer -= dt;
        if( litTimer <= 0.0f )
        {
            litTimer = 0.0f;
            core.setFillColor( normalColor );
        }
    }

    if( state == 0 ) return;

    if( state == 1 )// going up
    {
        if( ++bumpIdx >= bumpSteps ) state = -1;
    }
    else// going down
    {
        if( --bumpIdx <= 0 ) state = 0;// motion done
    }

    ring.setPosition( sf::Vector2f( pos.x + (float)bumpIdx*vel.x*dt, pos.y + (float)bumpIdx*vel.y*dt ) );
}

void roundBumper::getLit( float lightTime )
{
    if( lightTime == 0.0f ) return;// scoring not active
    litTimer = lightTime;
    core.setFillColor( litColor );
}

void roundBumper::getUnlit()// called when game over and at game start
{
    litTimer = 0.0f;
    core.setFillColor( normalColor );
}
