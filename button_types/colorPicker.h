#ifndef COLORPICKER_H_INCLUDED
#define COLORPICKER_H_INCLUDED

#include "buttonRect.h"
#include<sstream>
#include<iostream>
//#include<functional>

class colorPicker : public button
{
    public:

    sf::Vector2f sz;
    sf::RectangleShape bkgdRect, previewRect;
    sf::CircleShape colorSpot;
    sf::Color stripColor = sf::Color::White;
    sf::Color finalColor = sf::Color::White;

    sf::Vertex vtxStrip[14];// rainbow triangle fan
    sf::Vertex vtxRange[4];// big color box
    sf::Text txtRed, txtGreen, txtBlue;// numeric labels
    sf::Text txtRedVal, txtGreenVal, txtBlueVal;// numeric labels
    unsigned int *pRedVal = nullptr, *pGreenVal = nullptr, *pBlueVal = nullptr;
    sf::Color* pColor = nullptr;
 //   std::function<void(sf::Color)> pColorFunc = nullptr;
    buttonRect OKbutt, CancelButt;
    bool is_Modal = false;// retains focus

    // funcs
    colorPicker() {}// default = invalid object
    virtual ~colorPicker() {}

    virtual bool hit()const;
    virtual bool hitLeft();
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual bool MseOver();
    virtual void setPosition( sf::Vector2f Pos );
    virtual void update();
    virtual void setSel( bool Sel );


    void init( float x, float y, float W, float H );
    bool getStripColor( sf::Color& retVal )const;
};


#endif // COLORPICKER_H_INCLUDED
