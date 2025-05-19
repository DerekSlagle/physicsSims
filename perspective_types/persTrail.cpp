#include "persTrail.h"


void persTrail::init( vec3f StartPos, vec3f EndPos, float Width, sf::Color edgeColor, sf::Color surfaceColor )
{
    // find unit vector across trail
    vec3f wu = persPt::yHat.cross( EndPos - StartPos );
    wu /= wu.mag();
    line[0].init( StartPos - wu*(0.5f*Width), EndPos - wu*(0.5f*Width), edgeColor );// left
    line[1].init( StartPos + wu*(0.5f*Width), EndPos + wu*(0.5f*Width), edgeColor );// right
    for( unsigned int k = 0; k < 4; ++k ) sfc[k].color = surfaceColor;
    update(0.0f);// assign vertex positions, doDraw
}

void persTrail::init( std::istream& is )
{
    vec3f StartPos, EndPos; is >> StartPos.x >> StartPos.y >> StartPos.z >> EndPos.x >> EndPos.y >> EndPos.z;
    float Width; is >> Width;
    unsigned int rd, gn, bu;
    is >> rd >> gn >> bu;// line colors
    sf::Color edgeColor(rd,gn,bu);
    is >> rd >> gn >> bu;// surface color
    sf::Color sfcColor(rd,gn,bu);
    init( StartPos, EndPos, Width, edgeColor, sfcColor );
}

void persTrail::update( float dt )
{
    line[0].update(dt);
    line[1].update(dt);
    doDraw = line[0].doDraw && line[1].doDraw;

    if( doDraw )
    {
        sfc[0].position = persPt::get_xyw( line[0].pt[0] );// left begin
        sfc[1].position = persPt::get_xyw( line[0].pt[1] );// left end
        sfc[2].position = persPt::get_xyw( line[1].pt[1] );// right end
        sfc[3].position = persPt::get_xyw( line[1].pt[0] );// right begin closes the quad
    }
}

void persTrail::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    RT.draw( sfc, 4, sf::Quads );
    line[0].draw(RT);
    line[1].draw(RT);
}
