#ifndef BUTTONIMG_H
#define BUTTONIMG_H

#include "button.h"


class buttonImg : public button
{
    public:
        sf::Vector2f sz;
        sf::RectangleShape R[4];
        sf::Text title;

        buttonImg(){}
        virtual ~buttonImg(){}

        virtual bool hit()const;
        virtual void draw( sf::RenderTarget& RT )const;
        virtual void setPosition( sf::Vector2f Pos );

        virtual void init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title = sf::Text(), float scale = 1.0f );
};

#endif // BUTTONIMG_H
