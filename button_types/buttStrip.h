#ifndef BUTTSTRIP_H_INCLUDED
#define BUTTSTRIP_H_INCLUDED

#include "button.h"
#include "buttonValOnHit.h"

class buttStrip : public button// button + strip
{
    public:
    buttonRect ocButt;// open/close
    buttonValOnHit voh;
    button* pButt_mse = nullptr;// note in MseOver, use in hitLeft, hitRight

    bool doFade = false;
    int growState = 0;// for roll on/off following open/close
    float growSpeed = 1.0f, growVal = 0.0f;// range: minVal -> currVal or reverse

    void init( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, float* p_val );
    void init( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc ); //   buttonValOnHit( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, bool isFloat, void* p_val = nullptr )
    buttStrip( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, float* p_val );
    buttStrip( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc );

    buttStrip(){}
    virtual ~buttStrip(){}

    virtual void init_delayBox() { if( sel ) voh.init_delayBox(); return; }
    virtual void setSel( bool Sel ) { ocButt.setSel(Sel); sel = Sel; }// over in buttonList to open/close list, okBox, colorPicker
    virtual void setMO( bool MO );
    virtual bool hit()const { return ocButt.hit() || ( sel && voh.hit() ); }
    virtual void draw( sf::RenderTarget& RT )const { ocButt.draw(RT); if( sel ) { voh.draw(RT); } }
    virtual bool MseOver();
    virtual bool hitLeft();
    virtual bool hitRight() { if( pButt_mse ) pButt_mse->hitRight(); return mseOver; }// overload in joyButton
    virtual void setPosition( sf::Vector2f Pos );// { return; }
    virtual void update();

    // extras
    float fVal() const { return voh.fVal; }
};

#endif // BUTTSTRIP_H_INCLUDED
