#include "intSelector.h"

bool intSelector::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void intSelector::draw( sf::RenderTarget& rRW )const
{
    rRW.draw( bkgdRect );
    rRW.draw( digitRect );
    rRW.draw( countText );
    rRW.draw( minCountText );
    rRW.draw( countLimitText );
    rRW.draw(title);
    if( mseOver )
    {
        rRW.draw( hoverText );
        rRW.draw( hoverLine, 2, sf::Lines );
    }
}

void intSelector::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    title.setPosition( title.getPosition() + dPos );
    bkgdRect.setPosition( pos );
    digitRect.setPosition( digitRect.getPosition() + dPos );
    countText.setPosition( countText.getPosition() + dPos );
    hoverText.setPosition( hoverText.getPosition() + dPos );
}

bool intSelector::MseOver()
{
    if( !button::MseOver() ) return false;

    yValHover = pos.y + sz.y - button::mseY;

    int hoverValueNow = floor( yValHover/dyVal ) + minCount;
    if( hoverValueNow > countLimit ) hoverValueNow = countLimit;
    if( hoverValueNow != hoverValue )
    {
        hoverValue = hoverValueNow;
        to_SF_string( hoverText, hoverValue );
        hoverLine[0].position.y = hoverLine[1].position.y = pos.y + sz.y - ( hoverValue - minCount )*dyVal;
    }

    return mseOver;
}

bool intSelector::hitLeft()
{
    if( button::clickEvent_Lt() == -1 ) return false;
    setValue( hoverValue );
    button::hitLeft();// calls pHitFunc()
    return mseOver;
}

void intSelector::setValue( int value )
{
    if( value >= minCount && value <= countLimit )
    {
//        countValue = hoverValue = value;
        countValue = value;
        to_SF_string( countText, countValue );
        if( countValue == countLimit ) yVal = sz.y;
        else if( countValue == minCount ) yVal = 0.0f;
        else yVal = dyVal*( countValue - minCount );

        digitRect.setSize( sf::Vector2f( sz.x, yVal ) );
        digitRect.setPosition( sf::Vector2f( pos.x, pos.y + sz.y - yVal ) );

        if( p_setFunc ) p_setFunc( countValue );
    }
}

void intSelector::setCountLimit( int limit )// sets upper count limit
{
    if( limit < minCount + 1 ) return;// limit too low
    countLimit = limit;
    dyVal = sz.y/( countLimit - minCount + 1 );
    to_SF_string( countLimitText, countLimit );
    if( countValue > countLimit ) setValue( countLimit );
}

void intSelector::init( float x, float y, float W, float H, int MinCount, int CountLimit, int iCount, std::function<void(void)> p_HitFunc, const char* label )
{
    sf::Text Title( label, *button::pFont, 12 );
    Title.setFillColor( sf::Color::Black );
    init( x, y, W, H, MinCount, CountLimit, iCount, p_HitFunc, Title );
}

void intSelector::init( float x, float y, float W, float H, int MinCount, int CountLimit, int iCount, std::function<void(void)> p_HitFunc, const sf::Text& Title )
{
    pos.x = x;
    pos.y = y;
    sz.x = W; sz.y = H;
    pHitFunc = p_HitFunc;
    if( CountLimit < MinCount + 1 ) CountLimit = MinCount + 1;
    minCount = MinCount;
    countLimit = CountLimit;
    countValue = iCount;
    hoverValue = iCount;

    dyVal = H/( countLimit - minCount + 1 );
    yVal = dyVal*( iCount - minCount );

    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = sz.y + 5.0f;
    dPos += pos;// new
    dPos.x = floor( dPos.x );// new
    dPos.y = floor( dPos.y );// new
    title.setPosition(dPos);

    bkgdRect.setSize( sf::Vector2f(W,H) );
    bkgdRect.setPosition( pos );
    bkgdRect.setOutlineThickness( 3.0f );
    bkgdRect.setFillColor( sf::Color::Blue );
    bkgdRect.setOutlineColor( sf::Color::Green );

    digitRect.setSize( sf::Vector2f( W, (iCount - minCount)*dyVal ) );
    digitRect.setPosition( sf::Vector2f( pos.x, pos.y + sz.y - ( iCount - minCount )*dyVal ) );
    digitRect.setFillColor( sf::Color(170,0,0,170) );

    hoverLine[0].color = hoverLine[1].color = sf::Color::Green;
    hoverLine[0].position.x = pos.x; hoverLine[1].position.x = pos.x + sz.x;
    hoverLine[0].position.y = hoverLine[1].position.y = pos.y + sz.y - ( hoverValue - minCount )*dyVal;

 //   sf::Text label("+", *button::pFont, 20);
 //   label.setFillColor( sf::Color::White );


    countText.setFillColor( sf::Color::White );
    countText.setFont( *button::pFont );
    countText.setCharacterSize( 20 );
    to_SF_string( countText, countValue );
    countText.setPosition( sf::Vector2f( floor( (sz.x - countText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y + 10.0f ) ) );

  //  hoverText.setPosition( sf::Vector2f(pos.x + 5.0, pos.y - 20.0f ) );
    hoverText.setFillColor( sf::Color::White );
    hoverText.setFont( *button::pFont );
    hoverText.setCharacterSize( 14 );
    to_SF_string( hoverText, countValue );
    hoverText.setPosition( sf::Vector2f( floor( (sz.x - hoverText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y - 20.0f ) ) );

    minCountText.setFillColor( sf::Color::White );
    minCountText.setFont( *button::pFont );
    minCountText.setCharacterSize( 14 );
    to_SF_string( minCountText, minCount );
    minCountText.setPosition( sf::Vector2f( pos.x + sz.x + 10.0f, pos.y + sz.y - 10.0f ) );

    countLimitText.setFillColor( sf::Color::White );
    countLimitText.setFont( *button::pFont );
    countLimitText.setCharacterSize( 14 );
    to_SF_string( countLimitText, countLimit );
    countLimitText.setPosition( sf::Vector2f( pos.x + sz.x + 10.0f, pos.y - 10.0f ) );
}

void intSelector::init( std::istream& is, std::function<void(void)> p_HitFunc )
{
    float x, y, W, H;
    std::string name; is >> name;
    is >> x >> y >> W >> H;
    int MinCount, CountLimit, iCount;
    is >> MinCount >> CountLimit >> iCount;
    intSelector::init( x, y, W, H, MinCount, CountLimit, iCount, p_HitFunc, name.c_str() );
}
