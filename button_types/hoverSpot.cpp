#include "hoverSpot.h"

void hoverSpot::init( sf::Vector2f Pos, sf::Vector2f Ofst, float Radius, const char* Msg, unsigned int fontSz, sf::Color spotClr, sf::Color txtClr, sf::Color bkgdClr )
{
    pos = Pos;
    ofst = Ofst;
    radius = Radius;
    boxClr = bkgdClr;

    img.setRadius(radius);
    img.setOrigin( radius, radius );
    img.setPosition(pos.x, pos.y);
    img.setFillColor( spotClr );

    if( button::pFont )
    {
        msg.setString( Msg );
        msg.setFillColor( txtClr );
        msg.setFont( *button::pFont );
        msg.setCharacterSize( fontSz );
        msg.setPosition( pos + ofst );
    }
}

void hoverSpot::init_delayBox()
{
    float xLt = pos.x + ofst.x;
    float yUp = pos.y + ofst.y;

    button::moValtxt = msg;
//    button::moValtxt.setPosition( xLt, yUp );
    sf::Vector2f boxSz;
    boxSz.x = msg.getLocalBounds().width;
    boxSz.y = msg.getLocalBounds().height;
    moValBox[0].position.x = xLt;
    moValBox[0].position.y = yUp;// up lt
    moValBox[1].position.x = xLt + boxSz.x;
    moValBox[1].position.y = yUp;// up rt
    moValBox[2].position.x = xLt + boxSz.x;
    moValBox[2].position.y = yUp + boxSz.y;// dn rt
    moValBox[3].position.x = xLt;
    moValBox[3].position.y = yUp + boxSz.y;// dn lt
 //   moValBox[0].color = moValBox[1].color = moValBox[2].color = moValBox[3].color = boxClr;
}

bool hoverSpot::hit()const
{
    float dx = mseX - pos.x;
    float dy = mseY - pos.y;
    return dx*dx + dy*dy < radius*radius;
}

void hoverSpot::draw( sf::RenderTarget& rRW )const
{
    rRW.draw( img );

    if( mseOver && button::delayCnt >= button::moDelay )
    {
        rRW.draw( button::moValBox, 4, sf::Quads );
        rRW.draw( button::moValtxt );
    }

    return;
}

void hoverSpot::setPosition( sf::Vector2f Pos )
{
    pos = Pos;
    img.setPosition( pos );
    msg.setPosition( pos + ofst );
}
