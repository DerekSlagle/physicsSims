#ifndef BUTTONSLIP_H_INCLUDED
#define BUTTONSLIP_H_INCLUDED

#include "buttonImg.h"

class buttonSlip : public buttonImg
{
    public:
    sf::FloatRect hitRect[2];// 0=unsel, 1=sel

    buttonSlip();
    virtual ~buttonSlip();

    virtual bool hit()const;
    virtual void init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title = sf::Text(), float scale = 1.0f );
};


#endif // BUTTONSLIP_H_INCLUDED
