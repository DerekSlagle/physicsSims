#ifndef KEYPAD_H_INCLUDED
#define KEYPAD_H_INCLUDED

#include "button.h"

class keyPad : public button
{
    public:
        static sf::Color selClr, unSelClr, moClr, unMoClr;

        sf::Vector2f sz;
//        sf::RectangleShape R[4];
        sf::RectangleShape R;
        sf::RectangleShape Rsel;// selected area
        unsigned int selIdx = 0;
        unsigned int rows = 1, cols = 1;// a single button
        sf::Text title;

        keyPad() {}
        virtual ~keyPad(){}
        virtual bool hit()const;
        virtual void draw( sf::RenderTarget& rRW )const;
        virtual void setPosition( sf::Vector2f Pos );

        virtual bool MseOver();// find cell over. selIdx?
        virtual bool hitLeft();// find + use selIdx in function call
        virtual void setSel( bool Sel );// sel = open to hits
        virtual void setMO( bool MO ) { mseOver = MO; R.setOutlineColor( (mseOver ? moClr : unMoClr ) ); }

   //     void init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title = sf::Text() );
     //   void init( float x, float y, float W, float H, const sf::Text& Title = sf::Text() );
        void init( float x, float y, float W, float H, unsigned int Rows, unsigned int Cols, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() );
        keyPad( float x, float y, float W, float H, unsigned int Rows, unsigned int Cols, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() ) { init( x, y, W, H, Rows, Cols, p_HitFunc, Title ); }
};

#endif // KEYPAD_H_INCLUDED
