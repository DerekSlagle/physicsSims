#include "coordinateAxes.h"

std::function<sf::Vector2f(vec3f)> coordinateAxes::get_xyw = nullptr;// mapping function given

coordinateAxes::coordinateAxes()
{
    //ctor
}

coordinateAxes::~coordinateAxes()
{
    //dtor
}

void coordinateAxes::init( std::istream& is, std::function<sf::Vector2f(vec3f)> get_xyw, const sf::Font* pFont )
{
    float length, offL, offW;
    is >> length >> offL >> offW;
    unsigned int charSz, rd, gn, bu;
    is >> charSz >> rd >> gn >> bu;

    init( sf::Color(rd,gn,bu), length, offL, offW, charSz, get_xyw, pFont );
}

void coordinateAxes::init( sf::Color axesColor, float length, float offL, float offW, unsigned int charSz, std::function<sf::Vector2f(vec3f)> getXyw, const sf::Font* pFont )
{
    // pre assigned
    if( !pXu ) return;
    if( !pYu ) return;
    if( !pZu ) return;
    if( !pOrigin ) return;
 //   if( !pDoDraw ) return;
    // passed here
 //   if( !getXyw ) return;
    if( !pFont ) return;

    isValid = true;
    get_xyw = getXyw;

    // x axis
    axisPt[0] = vec3f( -length, 0.0f, 0.0f );
    axisPt[1] = vec3f( length, 0.0f, 0.0f );
    axisPt[2] = axisPt[1];
    axisPt[3] = vec3f( length - offL, offW, 0.0f );
    axisPt[4] = axisPt[1];
    axisPt[5] = vec3f( length - offL, -offW, 0.0f );
    // y axis
    axisPt[6] = vec3f( 0.0f, -length, 0.0f );
    axisPt[7] = vec3f( 0.0f, length, 0.0f );
    axisPt[8] = axisPt[7];
    axisPt[9] = vec3f( offW, length - offL, 0.0f );
    axisPt[10] = axisPt[7];
    axisPt[11] = vec3f( -offW, length - offL, 0.0f );
    // z axis
    axisPt[12] = vec3f( 0.0f, 0.0f, 0.0f );
    axisPt[13] = vec3f( 0.0f, 0.0f, length );
    axisPt[14] = axisPt[13];
    axisPt[15] = vec3f( offW, 0.0f, length - offL );
    axisPt[16] = axisPt[13];
    axisPt[17] = vec3f( -offW, 0.0f, length - offL );

    for( unsigned int j = 0; j < Sz; ++j )
    {
        axisVtx[j].color = axesColor;
        vec3f Pos = *pOrigin + *pXu*axisPt[j].x + *pYu*axisPt[j].y + *pZu*axisPt[j].z;
        if( get_xyw )
            axisVtx[j].position = get_xyw( Pos );
        else
        {
            axisVtx[j].position.x = Pos.x;
            axisVtx[j].position.y = Pos.y;
        }
    }

    // axis labels
    if( pFont )
    {
        xLbl.setFont( *pFont );
        xLbl.setFillColor( axesColor );
        xLbl.setCharacterSize( charSz );
        yLbl = zLbl = xLbl;

        xLbl.setString("X");
        xLbl.setPosition( axisVtx[1].position );
        yLbl.setString("Y");
        yLbl.setPosition( axisVtx[7].position );
        zLbl.setString("Z");
        zLbl.setPosition( axisVtx[13].position );
    }
}

void coordinateAxes::update()
{
    if( !isValid ) return;

    if( get_xyw )
    {
        for( unsigned int j = 0; j < Sz; ++j )
            axisVtx[j].position = get_xyw( *pOrigin + *pXu*axisPt[j].x + *pYu*axisPt[j].y + *pZu*axisPt[j].z );
    }
    else
    {
        for( unsigned int j = 0; j < Sz; ++j )
        {
            vec3f Pos = *pOrigin + *pXu*axisPt[j].x + *pYu*axisPt[j].y + *pZu*axisPt[j].z;
            axisVtx[j].position.x = Pos.x;
            axisVtx[j].position.y = Pos.y;
        }
    }

    // axis labels
    xLbl.setPosition( axisVtx[1].position );
    yLbl.setPosition( axisVtx[7].position );
    zLbl.setPosition( axisVtx[13].position );
}

void coordinateAxes::draw( sf::RenderTarget& RT ) const
{
    if( !isValid ) return;
    if( pDoDraw && *pDoDraw == false ) return;

    RT.draw( axisVtx, Sz, sf::Lines );
    RT.draw( xLbl );
    RT.draw( yLbl );
    RT.draw( zLbl );
}
