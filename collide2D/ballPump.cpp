#include "ballPump.h"

void ballPump::init( std::istream& is )
{
    is >> Rinlet >> Rmax >> ejectVel.x >> ejectVel.y >> Kr >> Ka;
    is >> posInlet.x >> posInlet.y;
    inletCS.setPosition( sf::Vector2f( posInlet.x, posInlet.y ) );
    inletCS.setRadius( Rinlet );
    inletCS.setOrigin( sf::Vector2f(Rinlet,Rinlet) );
    inletCS.setFillColor( sf::Color(0,0,200,100) );
    sf::Vector2f tmp;
    float w, h; is >> posOutlet.x >> posOutlet.y >> w >> h;// for outletRS
    outletRS.setSize( sf::Vector2f( w, h ) );
    outletRS.setOrigin( w/2.0f, h/2.0f );
    outletRS.setPosition( posOutlet.x, posOutlet.y );
    outletRS.setFillColor( sf::Color(100,100,100) );

    runColor = sf::Color(0,200,0,200);
    stopColor = sf::Color(200,0,0,200);
    ejectColor = sf::Color::White;

    runCS_inlet.setRadius( 3.0f );
    runCS_inlet.setOrigin( sf::Vector2f(3.0f,3.0f) );
    runCS_inlet.setPosition( sf::Vector2f( posInlet.x, posInlet.y + Rinlet*0.7f ) );
    runCS_inlet.setFillColor( stopColor );// pump is off

    runCS_outlet = runCS_inlet;
    runCS_outlet.setPosition( sf::Vector2f( posOutlet.x - w/4.0f, posOutlet.y ) );
}

bool ballPump::init_controls( std::istream& is )
{
    sf::Vector2f hbPos, csPos, csDims;
    is >> hbPos.x >> hbPos.y >> csPos.x >> csPos.y >> csDims.x >> csDims.y;

    paramsCS.init( hbPos, csPos, csDims, "params" );
    paramsCS.pButtVec.push_back( &paramsMS );
    button::RegisteredButtVec.push_back( &paramsCS );

    sf::Vector2f ofst(10.0f,10.0f);
    paramsMS.init( csPos + ofst, &paramsVOH, is );

    //
    if( paramsMS.sdataVec.size() != 6 ) { std::cout << "\nbad paramsMS data"; return false; }
    stripData* pSD = &paramsMS.sdataVec.front();
    // Rinlet
    pSD->pSetFunc =     [this](float x){ setInletRadius( x ); };
    paramsVOH.reInit( *pSD );
    Rinlet = pSD->xCurr;
     // Rmax
    (++pSD)->pSetFunc = [this](float x){ Rmax = x; };
    Rmax = pSD->xCurr;
    // Kr
    (++pSD)->pSetFunc = [this](float x){ Kr = x; };
    Kr = pSD->xCurr;
    // Ka
    (++pSD)->pSetFunc = [this](float x){ Ka = x; };
    Ka = pSD->xCurr;
    // ejectVel.x
    (++pSD)->pSetFunc = [this](float x){ set_ejectVel( vec2d( x, ejectVel.y ) ); };
    Kr = pSD->xCurr;
    // ejectVel.y
    (++pSD)->pSetFunc = [this](float x){ set_ejectVel( vec2d( ejectVel.x, x ) ); };
    Ka = pSD->xCurr;

    return true;
}

void ballPump::setInletRadius( float r )
{
    inletCS.setRadius(r);
    inletCS.setOrigin(r,r);
    sf::Vector2f dPos = runCS_inlet.getPosition() - inletCS.getPosition();
    runCS_inlet.setPosition( inletCS.getPosition() + (r/Rinlet)*dPos );
    Rinlet = r;
}

void ballPump::set_ejectVel( vec2d v )
{
    const float pi = 3.141593f;
    ejectVel = v;
    sf::FloatRect fr = outletRS.getLocalBounds();
    float w = fr.width, h = fr.height;
    float ang = v.x == 0.0f ? pi/2.0f : atan2f( v.y,v.x );
    outletRS.setRotation( ang*180.0f/pi );
    sf::Vector2f pos = outletRS.getPosition();
    pos.x += w*cosf(ang)/4.0f;
    pos.y += w*sinf(ang)/4.0f;
    runCS_outlet.setPosition( pos );
}

void ballPump::setPosition_inlet( sf::Vector2f pos )
{
    //sf::Vector2f dPos = runCS_inlet.getPosition() - sf::Vector2f( posInlet.x, posInlet.y );
    sf::Vector2f dPos = pos - inletCS.getPosition();
    inletCS.setPosition( pos );
    posInlet.x += dPos.x;
    posInlet.y += dPos.y;
    runCS_inlet.setPosition( runCS_inlet.getPosition() + dPos );
}

void ballPump::setPosition_outlet( sf::Vector2f pos )
{
    sf::Vector2f dPos = pos - outletRS.getPosition();
    outletRS.setPosition( pos );
    posOutlet.x += dPos.x;
    posOutlet.y += dPos.y;
    runCS_outlet.setPosition( runCS_outlet.getPosition() + dPos );
}

void ballPump::update( std::vector<ball>& bVec, float dt )
{
    if( !is_pumping ) return;

    for( ball& b : bVec )
    {
        vec2d sep = posInlet - b.pos;
        float Rsep = sqrtf( sep.dot( sep ) );
        if( Rsep > Rmax ) continue;

        if( Rsep < Rinlet )// captured!
        {
            b.v = ejectVel;
            b.setPosition( posOutlet );
            delayCount = 0.0f;
            runCS_inlet.setFillColor( ejectColor );
            runCS_outlet.setFillColor( ejectColor );
            b.img.setFillColor( sf::Color::Green );
        }
        else// draw in
        {
            vec2d dVel = Kr*sep*dt/(Rsep*Rsep);// component along sep
            dVel -= Ka*b.v.cross( sep )*sep.get_LH_norm()/(Rsep*Rsep);// component perp. to sep
            b.v += dVel;
        }
    }

    updateEjectTimer( dt );
}

bool ballPump::updateEjectTimer( float dt )
{
    if( delayCount < delayLimit )// timer for ejectColor
    {
        delayCount += dt;
        if( delayCount == delayLimit )
        {
            runCS_inlet.setFillColor( runColor );
            runCS_outlet.setFillColor( runColor );
            return true;
        }
    }

    return false;
}

bool ballPump::suckUp( ball& b, float dt )
{
    if( !is_pumping ) return false;

    vec2d sep = posInlet - b.pos;
    float Rsep = sqrtf( sep.dot( sep ) );
    if( Rsep > Rmax ) return false;

    if( Rsep < Rinlet )// captured!
    {
        b.v = ejectVel;
        b.setPosition( posOutlet );
        delayCount = 0.0f;
        runCS_inlet.setFillColor( ejectColor );
        runCS_outlet.setFillColor( ejectColor );
        return true;
    }
    // draw in
    vec2d dVel = Kr*sep*dt/(Rsep*Rsep);// component along sep
    dVel -= Ka*b.v.cross( sep )*sep.get_LH_norm()/(Rsep*Rsep);// component perp. to sep
    b.v += dVel;
    return false;
}

void ballPump::draw( sf::RenderTarget& RT )const
{
    RT.draw( inletCS );
    RT.draw( runCS_inlet );
    RT.draw( outletRS );
    RT.draw( runCS_outlet );
}
void ballPump::draw_inlet( sf::RenderTarget& RT )const// before drawing balls
{
    RT.draw( inletCS );
    RT.draw( runCS_inlet );
}

void ballPump::draw_outlet( sf::RenderTarget& RT )const// after drawing balls
{
    RT.draw( outletRS );
    RT.draw( runCS_outlet );
}

void ballPump::turnOn( bool on )
{
    is_pumping = on;
    sf::Color clr = on ? runColor : stopColor;
    runCS_inlet.setFillColor( clr );
    runCS_outlet.setFillColor( clr );
}
