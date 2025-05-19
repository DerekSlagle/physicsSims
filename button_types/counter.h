#ifndef COUNTER_H_INCLUDED
#define COUNTER_H_INCLUDED

#include "buttonTri.h"

class counter : public button
{
    public:
        static unsigned int incrementDelayLimit, incrementDelayTime;
        sf::Vector2f sz  = sf::Vector2f(140.0f,180.0f);// bkgdRect dimensions

        sf::RectangleShape bkgdRect, digitRect;
        sf::Text title, countText;

        buttonTri Up, Down;
        unsigned int countValue = 0, countLimit = 10;

        counter();
        virtual ~counter();
        virtual bool hit()const;
        virtual void draw( sf::RenderTarget& rRW )const;
        virtual void setPosition( sf::Vector2f Pos );

        virtual bool MseOver();// assigns mseOver - virtual due to ValOnHit live display feature
        virtual bool hitLeft();
        virtual void setSel( bool Sel ) { sel = false; }

   //     void init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title = sf::Text() );
     //   void init( float x, float y, float W, float H, const sf::Text& Title = sf::Text() );
        void init( float x, float y, float W, float H, unsigned int CountLimit, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() );
        counter( float x, float y, float W, float H, unsigned int CountLimit, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() ) { init( x, y, W, CountLimit, H, p_HitFunc, Title ); }

        void init( float x, float y, float W, float H, unsigned int CountLimit, std::function<void(void)> p_HitFunc, const char* label );
        counter( float x, float y, float W, float H, unsigned int CountLimit, std::function<void(void)> p_HitFunc, const char* label ) { init( x, y, W, H, CountLimit, p_HitFunc, label ); }
};

#endif // COUNTER_H_INCLUDED
