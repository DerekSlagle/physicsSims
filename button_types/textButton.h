#ifndef TEXTBUTTON_H_INCLUDED
#define TEXTBUTTON_H_INCLUDED

#include "buttonRect.h"

class textButton : public buttonRect
{
    public:
    sf::Text txtMsg;
    std::string strMsg;
    sf::Vertex cursor[2];
    bool cursorToRight = false;// text appears to left of cursor if true
    unsigned int maxCharCount = 20;// # to display
    std::string* pStr = nullptr;// writes here when enter pressed
    std::function<void(const char*)> pStrFunc = nullptr;

    textButton(){}
    ~textButton(){}
    void init( float x, float y, float W, float H, const sf::Text& Title );
    textButton( float x, float y, float W, float H, const sf::Text& Title ) { init( x, y, W, H, Title ); }
    void init( float x, float y, float W, float H, const char* label, unsigned int MaxCharCount, bool CursorToRight = false );
    textButton( float x, float y, float W, float H, const char* label, unsigned int MaxCharCount, bool CursorToRight = false ) { init( x, y, W, H, label, MaxCharCount, CursorToRight ); }
    void setPosition( sf::Vector2f Pos );
    virtual bool hitLeft();// alters sel - overload in ValOnHit
    virtual void setSel( bool Sel );
    virtual void draw( sf::RenderTarget& rRW )const;
//    virtual void pushChar( sf::Keyboard::Key K );
    virtual void pushChar( sf::Event::KeyEvent KV );
    virtual void pushChar( sf::Event::TextEvent TV );
};

#endif // TEXTBUTTON_H_INCLUDED
