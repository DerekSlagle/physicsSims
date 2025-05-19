#include "OkBox.h"

void OkBox::activate()
{
    status = button::Status::active;
}

void OkBox::activate( const char* MsgStr, char Focus )
{
    msg.setString( MsgStr );
    focus = Focus;
    status = button::Status::active;

    sf::FloatRect msgSz = msg.getLocalBounds();
    sf::Vector2f dPos( floor( (sz.x - msgSz.width)/2.0f ), 10.0f );
    msg.setPosition( pos + dPos );
}

bool OkBox::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void OkBox::draw( sf::RenderTarget& rRW )const
{
    rRW.draw( bkgdRect );
    rRW.draw( msg );
    okButt.draw(rRW);
}

void OkBox::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    msg.setPosition( msg.getPosition() + dPos );
    okButt.setPosition( okButt.pos + dPos );
    bkgdRect.setPosition(pos);
}

bool OkBox::MseOver()
{
    if( status == button::Status::hidden ) mseOver = false;
    else
    {
        mseOver = okButt.MseOver() || hit();
        if( focus == 'X' )
        {
            mseOver = true;
            if( button::pButtMse && button::pButtMse != this )
            {
                pButtMse->setMO( false );// not working as intended. call to okButt.MseOver() assigns
                button::pButtMse = this;// grab focus
            }
        }
    }

    return mseOver;
}

/*
bool OkBox::MseOver()
{
    if( status == button::Status::hidden ) mseOver = false;
    else mseOver = okButt.MseOver() || hit();
    return mseOver;
}
*/

bool OkBox::hitLeft()
{
    if( okButt.hitLeft() )
    {
        status = button::Status::hidden;
        okButt.setSel( false );
        okButt.setMO( false );
        return true;
    }
    return false;
}

void OkBox::setSel( bool Sel )
{
    okButt.setSel( Sel );
}

void OkBox::init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const char* MsgStr )
{
    sf::Text MsgTxt( MsgStr, *button::pFont, 12 );
    MsgTxt.setFillColor( sf::Color::Black );
    init( x, y, W, H, p_HitFunc, MsgTxt );
}

void OkBox::init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const sf::Text& MsgTxt )
{
    pos.x = x;
    pos.y = y;
    sz.x = W; sz.y = H;

    msg = MsgTxt;
    sf::FloatRect msgSz = msg.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = floor( (sz.x - msgSz.width)/2.0f );
    dPos.y = 10.0f;
 //   dPos += pos;// new
 //   dPos.x = floor( dPos.x );// new
 //   dPos.y = floor( dPos.y - 2.0f );// new
    msg.setPosition( pos + dPos );

    bkgdRect.setSize( sf::Vector2f(W,H) );
    bkgdRect.setPosition( pos );
    bkgdRect.setOutlineThickness( -3.0f );
    bkgdRect.setFillColor( sf::Color::White );
    bkgdRect.setOutlineColor( sf::Color::Blue );

    okButt.init( pos.x + sz.x/2.0f - 30.0f, pos.y + sz.y - 40.0f, 60.0f, 30.0f, p_HitFunc, "OK" );
    status = button::Status::hidden;
}
