#include "buttStrip.h"

void buttStrip::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    ocButt.setPosition( ocButt.pos + dPos );
    voh.setPosition( voh.pos + dPos );
    pos = Pos;
}

void buttStrip::setMO( bool MO )
{
    mseOver = MO;
 //   if( ocButt.hit() ) ocButt.setMO(MO);
 //   else if( sel && voh.hit() ) voh.setMO(MO);
}

bool buttStrip::MseOver()
{
    if( ocButt.MseOver() ) { pButt_mse = &ocButt; return true; }
    if( sel && voh.MseOver() ) { pButt_mse = &voh; return true; }

    pButt_mse = nullptr;
    return false;
}

bool buttStrip::hitLeft()
{
    if( pButt_mse )
    {
        pButt_mse->hitLeft();
        if( pButt_mse == &ocButt && doFade )
        {
            if( ocButt.sel ) { growState = 1; growVal = voh.fVal_min; }// fade in
            else { growState = -1; growVal = voh.fVal; }// fade out
        }
        return true;
    }
    return false;
}

void buttStrip::update()
{
    ocButt.update();
    if( sel ) voh.update();

    if( !voh.pSetFunc ) return;
    if( doFade && growState != 0 )
    {
        if( growState == 1 ) { if( (growVal+=growSpeed) >= voh.fVal ) { growVal = voh.fVal; growState = 0; } }// fade in
        else { if( (growVal-=growSpeed) <= voh.fVal_min ) { growVal = voh.fVal_min; growState = 0; } }// fade out
        voh.pSetFunc( growVal );
    }
}

void buttStrip::init( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, float* p_val )
{
    mseOver = sel = false;
    pos = Pos;
    sf::Text label( OCtitle, *button::pFont, 12);
    label.setFillColor( sf::Color::Black );
    ocButt.init( pos.x, pos.y, 50.0f, H, [this](){ sel = ocButt.sel; }, label );
    label.setString( VOHtitle );
    voh.init( Pos.x+ofstToVOH.x, Pos.y+ofstToVOH.y, W, H, label, val_min, val_max, init_val, p_val );
}

void buttStrip::init( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc )
{
    voh.pSetFunc = p_SetFunc;
    mseOver = sel = false;
    pos = Pos;
    sf::Text label( OCtitle, *button::pFont, 12);
    label.setFillColor( sf::Color::Black );
    ocButt.init( pos.x, pos.y, 50.0f, H, nullptr, label );
    label.setString( VOHtitle );
    voh.init( Pos.x+ofstToVOH.y, Pos.x+ofstToVOH.y, W, H, label, val_min, val_max, init_val, nullptr );
 }

buttStrip::buttStrip( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, float* p_val )
{ init( Pos, ofstToVOH, W, H, OCtitle, VOHtitle, val_min, val_max, init_val, p_val ); }

buttStrip::buttStrip( sf::Vector2f Pos, sf::Vector2f ofstToVOH, float W, float H, const char* OCtitle, const char* VOHtitle, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc )
{ init( Pos, ofstToVOH, W, H, OCtitle, VOHtitle, val_min, val_max, init_val, p_SetFunc ); }
