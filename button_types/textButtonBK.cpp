#include "textButton.h"

void textButton::init( float x, float y, float W, float H, const sf::Text& Title )
{
    mseOver = sel = false;
    pos.x = x; pos.y = y;
    // buttonRect
    sz.x = W; sz.y = H;

 //   initButtRects( R, W, H, sf::Vector2f(x,y), sf::Color::White, sf::Color(88,88,88), sf::Color::White, sf::Color(164,164,164) );
    buttonRect::init( x, y, W, H, nullptr, Title );
    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
//    dPos.y = (sz.y + titleSz.height)/2.0f;
    dPos.y = sz.y + titleSz.height/2.0f - 6.0f;
    title.setPosition(pos+dPos);

 //   for( size_t i=0; i<4; ++i )
 //       R[i].setPosition(pos);

    title.setColor( sf::Color::Red );

    dPos.x = x + 5.0f; dPos.y = y + 3.0f;
    txtMsg.setPosition( dPos );
    if( button::pFont )
    {
        txtMsg.setString( "" );
        txtMsg.setColor( sf::Color::Black );
        txtMsg.setFont( *button::pFont );
        txtMsg.setCharacterSize( 16 );
    }

    // cursor
    cursor[0].position.x = cursor[1].position.x = pos.x + 15.0f;
    cursor[0].position.y = pos.y + 0.25f*sz.y;
    cursor[1].position.y = pos.y + 0.75f*sz.y;
    cursor[0].color = cursor[1].color = sf::Color::Black;
}

void textButton::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    txtMsg.setPosition( txtMsg.getPosition() + dPos );
    cursor[0].position += dPos;
    cursor[1].position += dPos;
    buttonRect::setPosition( Pos );
}

bool textButton::hitLeft()
{
 //   pTxtButtFocus = this;
    if( !button::mseDnLt ) return false;
    bool retVal = button::hitLeft();
    setSel(true);
    pTxtButtFocus = this;
 //   else pTxtButtFocus = nullptr;
    return retVal;
}

void textButton::draw( sf::RenderTarget& rRW )const
{
    buttonRect::draw(rRW);
    rRW.draw( txtMsg );
    // blinking cursor when selected
    static int blinkCnt = 0;
    const int blinkLimit = 50;
    if( sel )
    {
        if(  ++blinkCnt > blinkLimit ) blinkCnt = 0;
        if( blinkCnt < blinkLimit/2 ) rRW.draw( cursor, 2, sf::Lines );
    }
}
void textButton::pushChar( sf::Event::TextEvent TV )
{

    if( TV.unicode == 13 )// return
    {
        setSel(false);
        if( pStr ) *pStr = strMsg;
        if( pStrFunc ) pStrFunc( strMsg.c_str() );
        return;
    }

    if( TV.unicode == 8 && strMsg.size() > 0 ) strMsg.pop_back();// back space
    else if( TV.unicode > 31 && TV.unicode < 128 ) strMsg.push_back( (char)( TV.unicode ) );
    txtMsg.setString( strMsg.c_str() );
    cursor[0].position.x = cursor[1].position.x = txtMsg.getPosition().x + txtMsg.getLocalBounds().width;// + pos.x;
    return;

}

void textButton::pushChar( sf::Event::KeyEvent KV )
{
    sf::Keyboard::Key K = KV.code;
    const char charMap[] = { '[',']',';',',','.','\'','/','\\','~','=','-',' ' };// 12
 //   if ( K == sf::Keyboard::H ) strMsg.push_back('H');
    if( K >= 0 && K < 26 )
    {
        if( KV.shift ) strMsg.push_back( (char)( K + 'A' ) );
        else strMsg.push_back( (char)( K + 'a' ) );
    }
    else if( K >= 26 && K < 36 ) strMsg.push_back( (char)( K - 26 + '0' ) );
    else if( K >= 46 && K < 58 ) strMsg.push_back( charMap[K-46] );// fail
    else if( K == sf::Keyboard::BackSpace ) strMsg.pop_back();
    else if( K == -1 ) strMsg.push_back('?');
 //   else strMsg.push_back('?');
    txtMsg.setString( strMsg.c_str() );
  //  float endX = txtMsg.getLocalBounds().width + pos.x;
    cursor[0].position.x = cursor[1].position.x = txtMsg.getPosition().x + txtMsg.getLocalBounds().width;// + pos.x;
    return;
}

/*
void textButton::pushChar( sf::Keyboard::Key K )
{
    const char charMap[] = { '[',']',';',',','.','\'','/','\\','~','=','-',' ' };// 12
 //   if ( K == sf::Keyboard::H ) strMsg.push_back('H');
    if( K >= 0 && K < 26 ) strMsg.push_back( (char)( K + 'a' ) );
    else if( K >= 26 && K < 36 ) strMsg.push_back( (char)( K - 26 + '0' ) );
    else if( K >= 46 && K < 58 ) strMsg.push_back( charMap[K-46] );// fail
    else if( K == sf::Keyboard::BackSpace ) strMsg.pop_back();
    else strMsg.push_back('?');
    txtMsg.setString( strMsg.c_str() );
  //  float endX = txtMsg.getLocalBounds().width + pos.x;
    cursor[0].position.x = cursor[1].position.x = txtMsg.getPosition().x + txtMsg.getLocalBounds().width;// + pos.x;
    return;
}
*/
