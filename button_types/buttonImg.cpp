#include "buttonImg.h"

bool buttonImg::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void buttonImg::draw( sf::RenderTarget& RT )const
{
    size_t idx = 0;
    if( sel ) idx = 2;
    if( mseOver ) ++idx;

    if( mode == 'L' )// latching appearance
    {
        if( mseOver && button::mseDnLt ) idx = 1;
        else idx = sel ? 2 : 0;
    }

    RT.draw( R[idx] );
    RT.draw(title);
}

void buttonImg::setPosition( sf::Vector2f Pos )
{
    pos = Pos;
    sf::Vector2f dPos;
    sf::FloatRect titleSz = title.getLocalBounds();
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
        R[i].setPosition(pos);
}

void buttonImg::init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title, float scale )
{

    pos.x = x;
    pos.y = y;
    sz = pR->getSize();
    sz *= scale;

    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
    {
        R[i] = pR[i];
        R[i].setSize( sz );
        R[i].setPosition(pos);
    }
}

