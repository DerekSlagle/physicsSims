#include "counter.h"

unsigned int counter::incrementDelayLimit = 50, counter::incrementDelayTime = 7;

counter::counter(): sz(140.0f,180.0f)
{
    //ctor
}

counter::~counter()
{
    //dtor
}

bool counter::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void counter::draw( sf::RenderTarget& rRW )const
{
    rRW.draw( bkgdRect );
 //   rRW.draw( digitRect );
    rRW.draw( countText );
    rRW.draw(title);
    Up.draw( rRW );
    Down.draw( rRW );
}

void counter::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    title.setPosition( title.getPosition() + dPos );
    bkgdRect.setPosition( pos );
    digitRect.setPosition( digitRect.getPosition() + dPos );
    countText.setPosition( countText.getPosition() + dPos );
    Up.setPosition( Up.pos + dPos );
    Down.setPosition( Down.pos + dPos );
}

bool counter::MseOver()
{
    if( !button::MseOver() ) return false;

    bool moLast = Up.mseOver || Down.mseOver;
    if( Up.MseOver() && countValue >= countLimit ) Up.setMO( false );
    if( Down.MseOver() && countValue < 1 ) Down.setMO( false );

    static unsigned int delayCount = 0, timeCount = 0;// counts = 0 await left click before incrementing
    if( Up.mseOver || Down.mseOver )
    {
        // stop incrementing
        if( !moLast ) delayCount = timeCount = 0;// just mouse over
        if( button::clickEvent_Lt() == -1 ) delayCount = timeCount = 0;// release
        // start incrementing
        else if( button::clickEvent_Lt() == 1 ) { delayCount = 1; timeCount = 0; }// press

        if( delayCount > 0 )
        {
            if( delayCount < counter::incrementDelayLimit )
                ++delayCount;
            else
            {
                if( timeCount < counter::incrementDelayTime )
                    ++timeCount;
                else// increment
                {
                    if( Up.mseOver )
                    {
                        if( countValue < countLimit )
                        {
                            to_SF_string( countText, ++countValue );
                            countText.setPosition( sf::Vector2f( floor( (sz.x - countText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y + 10.0f ) ) );
                        }

                    }
                    else if( countValue > 0 )// Down
                    {
                        to_SF_string( countText, --countValue );
                        countText.setPosition( sf::Vector2f( floor( (sz.x - countText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y + 10.0f ) ) );
                    }

                    timeCount = 0;
                }
            }
        }
    }
    else delayCount = timeCount = 0;

    return mseOver;
}

/*
bool counter::MseOver()
{
    if( !button::MseOver() ) return false;

    bool moLast = Up.mseOver || Down.mseOver;
 //   button::MseOver();
    if( Up.MseOver() && countValue >= countLimit ) Up.setMO( false );
    if( Down.MseOver() && countValue < 1 ) Down.setMO( false );

    static unsigned int delayCount = 0, timeCount = 0;// counts = 0 await left click before incrementing

    if( Up.mseOver || Down.mseOver )
    {
        // stop incrementing
     //   if( button::delayCnt == 0 ) delayCount = timeCount = 0;// just mouse over
        if( !moLast ) delayCount = timeCount = 0;// just mouse over
        if( button::clickEvent_Lt() == -1 ) delayCount = timeCount = 0;// release
        // start incrementing
        else if( button::clickEvent_Lt() == 1 ) { delayCount = 1; timeCount = 0; }// press

        if( delayCount > 0 )
        {
         //   std::cout << '*';
            if( delayCount < counter::incrementDelayLimit )
                ++delayCount;
            else
            {
                if( timeCount < counter::incrementDelayTime )
                    ++timeCount;
                else// increment
                {
                    if( Up.mseOver )
                    {
                        if( countValue < countLimit )
                        {
                            to_SF_string( countText, ++countValue );
                        }
                    }
                    else// Down
                    {
                        if( countValue > 0 )
                        {
                            to_SF_string( countText, --countValue );
                        }
                    }

                    timeCount = 0;
                }

              //  delayCount = 1;
            }
        }
    }
    else delayCount = timeCount = 0;

    return mseOver;
}
*/

bool counter::hitLeft()
{
    button::hitLeft();
    if( Up.hitLeft() )
    {
        if( countValue < countLimit )
        {
            to_SF_string( countText, ++countValue );
            countText.setPosition( sf::Vector2f( floor( (sz.x - countText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y + 10.0f ) ) );
        }
    }
    else if( Down.hitLeft() && countValue > 0 )
    {
        to_SF_string( countText, --countValue );
        countText.setPosition( sf::Vector2f( floor( (sz.x - countText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y + 10.0f ) ) );
    }

    return mseOver;
}

void counter::init( float x, float y, float W, float H, unsigned int CountLimit, std::function<void(void)> p_HitFunc, const char* label )
{
    sf::Text Title( label, *button::pFont, 12 );
    Title.setFillColor( sf::Color::Black );
    init( x, y, W, H, CountLimit, p_HitFunc, Title );
}

void counter::init( float x, float y, float W, float H, unsigned int CountLimit, std::function<void(void)> p_HitFunc, const sf::Text& Title )
{
    pos.x = x;
    pos.y = y;
    sz.x = W; sz.y = H;
    pHitFunc = p_HitFunc;
    countLimit = CountLimit;

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

    digitRect.setSize( sf::Vector2f(40.0f,40.0f) );
    digitRect.setPosition( sf::Vector2f(pos.x + 15.0, pos.y + 10.0f ) );
 //   digitRect.setOutlineThickness( -3.0f );
    digitRect.setFillColor( sf::Color::Red );
 //   digitRect.setOutlineColor( sf::Color::Green );

    sf::Text label("+", *button::pFont, 14);
    label.setFillColor( sf::Color::White );
    sf::Vertex Pt[3]; Pt[0].position = sf::Vector2f(0.0f,16.0f); Pt[1].position = sf::Vector2f(0.0f,0.0f);  Pt[2].position = sf::Vector2f(12.0f,8.0f);
//    sf::Vertex Pt[3]; Pt[0].position = sf::Vector2f(0.0f,20.0f); Pt[1].position = sf::Vector2f(0.0f,0.0f);  Pt[2].position = sf::Vector2f(17.0f,10.0f);
 //   sf::Color clr[4]; clr[0] = sf::Color::Blue; clr[2] = sf::Color::Green; clr[1] = sf::Color::Red; clr[3] = sf::Color::Yellow;
    // 0=unsel, 1=sel, 2=unselMo, 3=selMo
    sf::Color clr[4]; clr[0] = sf::Color(0,0,0); clr[1] = sf::Color::Green; clr[2] = sf::Color::Red; clr[3] = sf::Color::Yellow;
    Up.init( Pt, pos.x + sz.x - 16.0f, pos.y + sz.y - 22.0f, clr, label );
    Up.title.setPosition( Up.pos.x - 0.0f, Up.pos.y - 0.0f );
    label.setString("-");
    Pt[2].position = sf::Vector2f(-12.0f,8.0f);
 //   Pt[2].position = sf::Vector2f(-17.0f,10.0f);
    Down.init( Pt, pos.x + 16.0f, pos.y + sz.y - 22.0f, clr, label );
    Down.title.setPosition( Down.pos.x - 7.0f, Down.pos.y - 0.0f );

 //   countText.setPosition( sf::Vector2f(pos.x + 20.0, pos.y + 10.0f ) );
    countText.setFillColor( sf::Color::White );
    countText.setFont( *button::pFont );
    countText.setCharacterSize( 18 );
    to_SF_string( countText, countValue );
    countText.setPosition( sf::Vector2f( floor( (sz.x - countText.getLocalBounds().width)/2.0f + pos.x ), floor( pos.y + 10.0f ) ) );
}
