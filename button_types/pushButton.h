#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "button.h"


class pushButton : public button
{
    public:

    static sf::Color selColor, unSelColor, moColor, noMoColor;

    sf::RectangleShape bodyRS;
    sf::Vector2f sz;// dimensions of bodyRS
    sf::Text label;
    std::function<void(void)> pFuncClose = nullptr, pFuncOpen = nullptr;
    // char mode; inherited - 2 values here: 'O' NOHC or 'C' NCHO
    // sf::Vector2f pos;
    // bool mseOver, sel;

    void init( std::istream& is, char Mode, std::function<void(void)> pFunc_close, std::function<void(void)> pFunc_open );
    void init( sf::Vector2f Pos, sf::Vector2f Sz, char Mode, const std::string& name, std::function<void(void)> pFunc_close, std::function<void(void)> pFunc_open );
    virtual void setSel( bool Sel );
    virtual bool hit()const;// pv
    virtual void draw( sf::RenderTarget& RT )const;// pv
    virtual bool MseOver();// assigns mseOver - and handles mouse cursor "slide offs" (release button if held)
    virtual bool hitLeft();// alters sel - overload in ValOnHit
    virtual void setPosition( sf::Vector2f Pos );// pv

    pushButton(){}
    pushButton( sf::Vector2f Pos, sf::Vector2f Sz, char Mode, const std::string& name, std::function<void(void)> pFunc_close, std::function<void(void)> pFunc_open )
    { init( Pos, Sz, Mode, name, pFunc_close, pFunc_open ); }
    virtual ~pushButton(){}

};

#endif // PUSHBUTTON_H
