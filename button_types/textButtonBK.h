#ifndef TEXTBUTTON_H_INCLUDED
#define TEXTBUTTON_H_INCLUDED

#include "buttonRect.h"

class textButton : public buttonRect
{
    public:
    sf::Text txtMsg;
    std::string strMsg;
    sf::Vertex cursor[2];
    std::string* pStr = nullptr;// writes here when enter pressed
    std::function<void(const char*)> pStrFunc = nullptr;

    textButton(){}
    ~textButton(){}
    void init( float x, float y, float W, float H, const sf::Text& Title );
    textButton( float x, float y, float W, float H, const sf::Text& Title ) { init( x, y, W, H, Title ); }
    void setPosition( sf::Vector2f Pos );
    virtual bool hitLeft();// alters sel - overload in ValOnHit
    virtual void draw( sf::RenderTarget& rRW )const;
//    virtual void pushChar( sf::Keyboard::Key K );
    virtual void pushChar( sf::Event::KeyEvent KV );
    virtual void pushChar( sf::Event::TextEvent TV );
};

#endif // TEXTBUTTON_H_INCLUDED
