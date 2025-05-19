#include "keyPad.h"

sf::Color keyPad::selClr = sf::Color::White, keyPad::unSelClr = sf::Color(200,200,200);
sf::Color keyPad::moClr = sf::Color::Green, keyPad::unMoClr = sf::Color::Blue;

bool keyPad::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void keyPad::draw( sf::RenderTarget& RT )const
{
//    size_t idx = 0;
 //   if( sel ) idx = 2;
 //   if( mseOver ) ++idx;
    if( !sel ) return;
    RT.draw( R );
    if( selIdx < rows*cols ) RT.draw(Rsel);
    RT.draw(title);
}

void keyPad::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    title.setPosition( title.getPosition() + dPos );

    R.setPosition(pos);
}

bool keyPad::MseOver()
{
    if( !sel ) return false;

    if( hit() )
    {
        mseOver = true;
        float wCell = sz.x/cols, hCell = sz.y/rows;
        unsigned int c = floor( ( button::msePos().x - pos.x )/wCell ), r = floor( ( button::msePos().y - pos.y )/hCell );
        selIdx = c + cols*r;
        Rsel.setPosition( pos.x + c*wCell, pos.y + r*hCell  );
        Rsel.setSize( sf::Vector2f(wCell, hCell) );
    }
    else
    {
        mseOver = false;
        selIdx = rows*cols;
    }

    return mseOver;
}

bool keyPad::hitLeft()
{
    if( !sel ) return false;
    if( !button::mseDnLt ) return false;
    if( selIdx >= rows*cols ) return false;
    pHitFunc();
    return mseOver;
}

void keyPad::setSel( bool Sel )
{
    button::setSel( Sel );
    R.setFillColor( (sel ? selClr : unSelClr ) );
}

void keyPad::init( float x, float y, float W, float H, unsigned int Rows, unsigned int Cols, std::function<void(void)> p_HitFunc, const sf::Text& Title )
{
    pos.x = x;
    pos.y = y;
    sz.x = W; sz.y = H;
    pHitFunc = p_HitFunc;
    rows = Rows;
    cols = Cols;
    sel = mseOver = false;
    selIdx = rows*cols;

    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    dPos += pos;// new
    dPos.x = floor( dPos.x );// new
    dPos.y = floor( dPos.y - 2.0f );// new
    title.setPosition(dPos);

    R.setSize( sf::Vector2f(W,H) );
    R.setPosition( pos );
    R.setOutlineThickness( -3.0f );
    R.setFillColor( (sel ? selClr : unSelClr) );
    R.setOutlineColor( (mseOver ? moClr : unMoClr) );
    Rsel.setFillColor( sf::Color(150,0,0,120) );

 //   sf::RectangleShape pR[4];
 //   initButtRects( pR, W, H );
 //   buttonRect::init( pR, x, y, Title );
}
