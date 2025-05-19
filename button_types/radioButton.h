#ifndef RADIOBUTTON_H_INCLUDED
#define RADIOBUTTON_H_INCLUDED

#include "buttonRect.h"
//#include "../collider/stateTypes.h"

class radioButton : public buttonRect
{
    public:
    std::vector< sf::CircleShape > buttVec;
    std::vector< sf::Text > buttLabelVec;
    bool drawButtLabels = true;
    sf::CircleShape dot;// in selected circle
    float sep = 1.0f;// distance between buttons
    float r;// button radius
    bool isHoriz = true;
    bool alwaysSel = true;// some button always selected
    size_t selIdx = 0;// index to currently selected
    size_t Nbutts = 1;// = buttVec.size()
    std::function<void(size_t)> pFuncIdx = nullptr;

    static sf::Color bkgdColor, borderColor;

    radioButton(){}
    virtual ~radioButton(){}
    void init( float x, float y, float W, float H, size_t numButts, const sf::Text& Title, int iSel = -1 );
    radioButton( float x, float y, float W, float H, size_t numButts, const sf::Text& Title, int iSel = -1 )
    { init( x, y, W, H, numButts, Title, iSel ); }

    void init( std::istream& is );
    radioButton( std::istream& is ) { init( is ); }

    virtual bool hitLeft();
    virtual void setSel( bool Sel ) { sel = Sel || alwaysSel; }
    virtual void setPosition( sf::Vector2f Pos );
    virtual void setPosition( float x, float y ) { setPosition( sf::Vector2f(x,y) ); }
    virtual void draw( sf::RenderTarget& RT )const;
    void setButtLabels( float offX, float offY, const std::vector<std::string>& labelVec, unsigned int fontSz = 12, sf::Color labelColor = sf::Color::Black );

    void setIndex( size_t i );

    // for recording state in 1 float
  //  state_ab* newState();
    void setState( const float* pf ) { setIndex( (size_t)pf[0] ); }
    void getState( float* pf )const { *pf = (float)selIdx + 0.5f; }
};


#endif // RADIOBUTTON_H_INCLUDED
