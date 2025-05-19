#ifndef HOVERSPOT_H_INCLUDED
#define HOVERSPOT_H_INCLUDED

#include "button.h"

class hoverSpot : public button
{
    public:
    float radius;
    sf::Vector2f ofst;
    sf::CircleShape img;
    sf::Text msg;
    sf::Color boxClr;

    hoverSpot(){}
    virtual ~hoverSpot(){}
    virtual void init_delayBox();
    virtual bool hit()const;
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual void setPosition( sf::Vector2f Pos );
    void init( sf::Vector2f Pos, sf::Vector2f Ofst, float Radius, const char* Msg, unsigned int fontSz, sf::Color spotClr = sf::Color::Cyan, sf::Color txtClr = sf::Color::White, sf::Color bkgdClr = sf::Color::Black );
    hoverSpot( sf::Vector2f Pos, sf::Vector2f Ofst, float Radius, const char* Msg, unsigned int fontSz, sf::Color spotClr = sf::Color::Cyan, sf::Color txtClr = sf::Color::White, sf::Color bkgdClr = sf::Color::Black )
    { init( Pos, Ofst, Radius, Msg, fontSz, spotClr, txtClr, bkgdClr ); }
};

#endif // HOVERSPOT_H_INCLUDED
