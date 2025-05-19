#ifndef OKBOX_H
#define OKBOX_H

#include "buttonRect.h"

class OkBox : public button
{
    public:

    sf::Vector2f sz;
    sf::RectangleShape bkgdRect;
    sf::Text msg;
    buttonRect okButt;
    char focus = 'N'; // 'N' = normal, 'X' = exclusive

    void activate();
    void activate( const char* MsgStr, char Focus );

    virtual bool hit()const;
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual void setPosition( sf::Vector2f Pos );

    virtual bool MseOver();// assigns mseOver - virtual due to ValOnHit live display feature
    virtual bool hitLeft();
    virtual void setSel( bool Sel );
    virtual void setMO( bool MO ) { okButt.setMO( MO ); }

    void init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const sf::Text& MsgTxt = sf::Text() );
    OkBox( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const sf::Text& MsgTxt = sf::Text() ) { init( x, y, W, H, p_HitFunc, MsgTxt ); }

    void init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const char* MsgStr );
    OkBox( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const char* MsgStr ) { init( x, y, W, H, p_HitFunc, MsgStr ); }

    OkBox(){}
    virtual ~OkBox(){}
};

#endif // OKBOX_H
