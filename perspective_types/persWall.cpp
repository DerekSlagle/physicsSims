#include "persWall.h"



void persWall::init( vec3f Pos, float Width, float Height, vec3f nu, sf::Color color, const sf::Texture* p_Txt )
{
    W = Width;
    H = Height;

    wallQuad[0].color = wallQuad[1].color = wallQuad[2].color = wallQuad[3].color = color;
    sides[0].color = sides[1].color = sides[2].color = sides[3].color = sf::Color::Blue;

//    { lineWQ[0].pt[0], lineWQ[0].pt[1], lineWQ[1].pt[0], lineWQ[1].pt[1] }
    pt[0] = lineWQ[0].pt[0];
    pt[1] = lineWQ[0].pt[1];
    pt[2] = lineWQ[1].pt[0];
    pt[3] = lineWQ[1].pt[1];

    setNu( nu );
}

void persWall::init( vec3f posStUp, vec3f posEndUp, vec3f posStDn, vec3f posEndDn, vec3f nu, sf::Color color )
{
    Nu = nu/nu.mag();
    pos = ( posStUp + posStDn )*0.5f;
    wallQuad[0].color = wallQuad[1].color = wallQuad[2].color = wallQuad[3].color = color;
    sides[0].color = sides[1].color = sides[2].color = sides[3].color = sf::Color::Black;
    lineWQ[0].init( posStUp, posEndUp, sf::Color::Black );
    lineWQ[1].init( posStDn, posEndDn, sf::Color::Black );

    pt[0] = lineWQ[0].pt[0];
    pt[1] = lineWQ[0].pt[1];
    pt[2] = lineWQ[1].pt[0];
    pt[3] = lineWQ[1].pt[1];
}

void persWall::setSidePositions()
{
    sides[0].position = lineWQ[0].vtx[0].position;
    sides[1].position = lineWQ[1].vtx[1].position;
    sides[2].position = lineWQ[0].vtx[1].position;
    sides[3].position = lineWQ[1].vtx[0].position;
}

void persWall::update( float dt )
{
    lineWQ[0].update(dt);
    lineWQ[1].update(dt);
    doDraw = lineWQ[0].doDraw && lineWQ[1].doDraw;

    pt[0] = lineWQ[0].pt[0];
    pt[1] = lineWQ[0].pt[1];
    pt[2] = lineWQ[1].pt[0];
    pt[3] = lineWQ[1].pt[1];

    for( unsigned int i = 0; i < 4; ++i )
        wallQuad[i].position = persPt::get_xyw( pt[i] );

    setSidePositions();
}

void persWall::draw( sf::RenderTarget& RT ) const
{
    if( doDraw )
    {
        RT.draw( wallQuad, 4, sf::Quads );
        lineWQ[0].draw(RT);
        lineWQ[1].draw(RT);
        RT.draw( sides, 4, sf::Lines );
    }
}

void persWall::setPosition( vec3f Pos )
{

}

void persWall::setNu( vec3f nu )
{
    nu /= nu.mag();// assuring normalization
    Nu = nu;
    vec3f wu = nu.cross( persPt::yHat );
    wu /= wu.mag();
    vec3f hu = wu.cross( nu );
    vec3f StartPos = pos + (H/2.0f)*hu + (W/2.0f)*wu;
    vec3f EndPos = pos + (H/2.0f)*hu - (W/2.0f)*wu;
    lineWQ[0].init( StartPos, EndPos, sf::Color::Magenta );
    StartPos = pos - (H/2.0f)*hu + (W/2.0f)*wu;
    EndPos = pos - (H/2.0f)*hu - (W/2.0f)*wu;
//    lineWQ[1].init( StartPos, EndPos, lineWQ[1].vtx[0].color );
    lineWQ[1].init( StartPos, EndPos, sf::Color::Green );
    setSidePositions();
}
