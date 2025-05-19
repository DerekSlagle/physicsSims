#ifndef INTSELECTOR_H_INCLUDED
#define INTSELECTOR_H_INCLUDED

#include "button.h"

class intSelector : public button
{
    public:
        sf::Vector2f sz  = sf::Vector2f(140.0f,180.0f);// bkgdRect dimensions
        sf::RectangleShape bkgdRect, digitRect;
        sf::Text title, countText, hoverText;
        sf::Text minCountText, countLimitText;
        sf::Vertex hoverLine[2];

        int countValue = 0, minCount = 0, countLimit = 9;
        int hoverValue = 0;
        float yVal = 0.0f, dyVal = 5.0f;// at integer value, increment distance
        float yValHover = 0.0f;
        std::function<void(unsigned int)> p_setFunc = nullptr;

        intSelector(): sz(140.0f,180.0f){};
        virtual ~intSelector(){};
        virtual bool hit()const;
        virtual void draw( sf::RenderTarget& rRW )const;
        virtual void setPosition( sf::Vector2f Pos );
        void setValue( int value );
        void setCountLimit( int limit );

        virtual bool MseOver();// assigns mseOver - virtual due to ValOnHit live display feature
        virtual bool hitLeft();
        virtual void setSel( bool Sel ) { sel = false; }

        void init( float x, float y, float W, float H, int MinCount, int CountLimit, int iCount, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() );
        intSelector( float x, float y, float W, float H, int MinCount, int CountLimit, int iCount, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() ) { init( x, y, W, H, MinCount, CountLimit, iCount, p_HitFunc, Title ); }

        void init( float x, float y, float W, float H, int MinCount, int CountLimit, int iCount, std::function<void(void)> p_HitFunc, const char* label );
        intSelector( float x, float y, float W, float H, int MinCount, int CountLimit, int iCount, std::function<void(void)> p_HitFunc, const char* label ) { init( x, y, W, H, MinCount, CountLimit, iCount, p_HitFunc, label ); }

        void init( std::istream& is, std::function<void(void)> p_HitFunc );
};

#endif // INTSELECTOR_H_INCLUDED
