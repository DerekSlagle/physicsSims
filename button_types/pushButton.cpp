#include "pushButton.h"

sf::Color pushButton::selColor = sf::Color::White;
sf::Color pushButton::unSelColor = sf::Color( 150,150,150 );
sf::Color pushButton::moColor = sf::Color::Green;
sf::Color pushButton::noMoColor = sf::Color::Blue;

void pushButton::init( sf::Vector2f Pos, sf::Vector2f Sz, char Mode, const std::string& name, std::function<void(void)> pFunc_close, std::function<void(void)> pFunc_open )
{
    pos = Pos;
    sz = Sz;
    mode = Mode;
    pFuncClose = pFunc_close;
    pFuncOpen = pFunc_open;
    bodyRS.setPosition( pos );
    bodyRS.setSize( sz );
    mseOver = false;
    bodyRS.setOutlineThickness( -2.0f );
    bodyRS.setOutlineColor( noMoColor );
    sel = (mode == 'C') ? true : false;
    bodyRS.setFillColor( sel ? selColor : unSelColor );
    label.setFont( *button::pFont );
    label.setCharacterSize( 12 );
    label.setFillColor( sf::Color::Black );
    label.setString( name.c_str() );
    sf::FloatRect szL = label.getLocalBounds();
    sf::Vector2f posLabel;
    posLabel.x = pos.x + ( sz.x - szL.width )/2.0f;
    posLabel.y = pos.y + ( sz.y - szL.height )/2.0f - 3.0f;
    label.setPosition( posLabel );
}

void pushButton::init( std::istream& is, char Mode, std::function<void(void)> pFunc_close, std::function<void(void)> pFunc_open )
{
    std::string name; is >> name;
    sf::Vector2f Pos, Sz;
    is >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    init( Pos, Sz, Mode, name, pFunc_close, pFunc_open );
}

void pushButton::setSel( bool Sel )
{
    if( sel == Sel ) return;// no state change
    sel = Sel;
    bodyRS.setFillColor( sel ? selColor : unSelColor );
    if( sel && pFuncClose ) pFuncClose();
    else if( pFuncOpen ) pFuncOpen();
}

bool pushButton::hit()const// pv
{
    if( button::mseX < pos.x ) return false;// left of
    if( button::mseX > pos.x + sz.x ) return false;// right of
    if( button::mseY < pos.y ) return false;// above
    if( button::mseY > pos.y + sz.y ) return false;// below

    return true;// It's a hit!
}
void pushButton::draw( sf::RenderTarget& RT )const// pv
{
    RT.draw( bodyRS );
    RT.draw( label );
}

bool pushButton::MseOver()// assigns mseOver - and handles mouse cursor "slide offs" (release button if held)
{
    if( status != Status::active ) { mseOver = false; return false; }// NEW

    bool moNow = hit();
    if( mseOver == moNow ) return mseOver;
    bodyRS.setOutlineColor( moNow ? moColor : noMoColor );
    // handle slide off while held down. Return to normal state
    if( mseOver && !moNow )
    {
        if( mode == 'O' && sel ) setSel(false);// release to open
        else if( mode == 'C' && !sel ) setSel(true);// release  to closed
    }

    mseOver = moNow;
    return mseOver;
}

bool pushButton::hitLeft()
{
    if( status != Status::active ) { mseOver = false; return false; }
    if( !hit() ) return false;

    if( button::clickEvent_Lt() == 1 )// mouse button pressed
    {
        if( mode == 'O' ) setSel(true);
        else setSel(false);
        return true;
    }

    if( button::clickEvent_Lt() == -1 )// mouse button released
    {
        if( mode == 'O' ) setSel(false);
        else setSel(true);
        return true;
    }

    return false;
}

void pushButton::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    bodyRS.setPosition( Pos );
    label.setPosition( label.getPosition() + dPos );
}
