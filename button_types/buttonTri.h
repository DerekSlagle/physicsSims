#ifndef BUTTONTRI_H
#define BUTTONTRI_H

#include "button.h"

class buttonTri : public button
{
    public:
        // 0=unsel, 1=sel, 2=unselMo, 3=selMo
        sf::Vertex P[14];// 2 triangles + 2*3 lines
        sf::Text title;

        buttonTri();
        virtual ~buttonTri();

        virtual bool hit()const;
        virtual void draw( sf::RenderTarget& RT )const;
        virtual void setPosition( sf::Vector2f Pos );// = position of P[0]

        virtual void setSel( bool Sel ) { sel = false; }

   //     void init( sf::Vertex Pt[12], float x, float y, const sf::Text& Title = sf::Text() );// from prepared set using texture
        void init( sf::Vertex Pt[3], float x, float y, sf::Color* pColor, const sf::Text& Title = sf::Text() );// from array of 4 colors

    protected:

    private:
};

float xProd( sf::Vector2f A, sf::Vector2f B );

#endif // BUTTONTRI_H
