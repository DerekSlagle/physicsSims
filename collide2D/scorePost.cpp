#include "scorePost.h"

unsigned int* scorePost::pScore = nullptr;
//sf::Text* scorePost::pScoreMsg = nullptr;

bool scorePost::init( std::istream& is )
{
    if( button::pFont ) valueMsg.setFont( *button::pFont );
    else return false;

    postSeg.init( is );
    is >> value >> countDownLimit;
    unsigned int r,g,b;
    is >> r >> g >> b;// normalColor
    normalColor = sf::Color(r,g,b);
    is >> r >> g >> b;// normalColor
    hitColor = sf::Color(r,g,b);
    postSeg.vtx[1].color = postSeg.vtx[0].color = normalColor;
    valueMsg.setCharacterSize( 20 );
    valueMsg.setFillColor( sf::Color::White );
    valueMsg.setPosition( postSeg.pos.x, postSeg.pos.y - 20.0f );
    to_SF_string( valueMsg, value );
    pFuncHit = nullptr;// play sounds

    return true;
}

bool scorePost::hit( ball& b )
{
    if( !scorePost::pScore ) return false;

    if( postSeg.hit(b) && postSeg.L.cross( b.pos - postSeg.pos ) > 0.0f )// striking scoring side
    {
        *scorePost::pScore += value;
        countDown = countDownLimit;
        postSeg.vtx[1].color = postSeg.vtx[0].color = hitColor;
        if( pFuncHit ) pFuncHit();
        return true;
    }

    return false;
}

void scorePost::update()
{
    if( countDown == 0 ) return;

    if( --countDown == 0 ) postSeg.vtx[1].color = postSeg.vtx[0].color = normalColor;
}

void scorePost::draw( sf::RenderTarget& RT )const
{
    postSeg.draw( RT );
    RT.draw( valueMsg );
}
