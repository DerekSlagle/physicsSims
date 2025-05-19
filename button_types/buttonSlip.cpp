#include "buttonSlip.h"

 buttonSlip::buttonSlip() {}// ctor
 buttonSlip::~buttonSlip() {}// dtor

bool buttonSlip::hit()const
{
    const sf::FloatRect& rHR = sel ? hitRect[1] : hitRect[0];

    if( mseX < pos.x + rHR.left || mseX > pos.x + rHR.left + rHR.width ) return false;
    if( mseY < pos.y + rHR.top || mseY > pos.y + rHR.top + rHR.height ) return false;
    return true;
}

void buttonSlip::init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title, float scale )
{
    sf::IntRect hr =  pR[4].getTextureRect();
    hitRect[0].left = hr.left; hitRect[0].top = hr.top; hitRect[0].width = hr.width; hitRect[0].height = hr.height;
    hr =  pR[5].getTextureRect();
    hitRect[1].left = hr.left; hitRect[1].top = hr.top; hitRect[1].width = hr.width; hitRect[1].height = hr.height;

    for( unsigned int i = 0; i < 2; ++i )
    { hitRect[i].left *= scale; hitRect[i].top *= scale; hitRect[i].width *= scale; hitRect[i].height *= scale; }

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
