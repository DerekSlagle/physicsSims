# include "floatSpot.h"

// call inherited init first
void floatSpot::init2( float minX, float maxX, float xi, const char* voh_label, std::function<void(float)> p_SetFunc )
{
    sf::Text Title( msg );
    Title.setString( voh_label );
    voh.init( pos.x, pos.y + ofst.y + 50.0f, 120.0f, 18.0f, Title, minX, maxX, xi, p_SetFunc );
    voh.drawValBox = false;
    voh.title.setPosition(  msg.getPosition() + sf::Vector2f(0.0f,20.0f) );// below
    voh.txtVal.setPosition(  voh.title.getPosition() + sf::Vector2f(voh.title.getGlobalBounds().width + 5.0f,0.0f) );// to right
}

void floatSpot::init2( float minX, float maxX, float xi, const char* voh_label, float* pVal )
{
    sf::Text Title( msg );
    Title.setString( voh_label );
    voh.init( pos.x, pos.y + ofst.y + 50.0f, 120.0f, 18.0f, Title, minX, maxX, xi, pVal );
    voh.drawValBox = false;
    voh.title.setPosition(  msg.getPosition() + sf::Vector2f(0.0f,20.0f) );// below
    voh.txtVal.setPosition(  voh.title.getPosition() + sf::Vector2f(voh.title.getGlobalBounds().width + 5.0f,0.0f) );// to right
}

void floatSpot::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    hoverSpot::setPosition( Pos );
    voh.setPosition( voh.pos + dPos );
}

bool floatSpot::hit()const
{
    if( sel )
    {
        if( mseX < moValBox[0].position.x ) return false;// left of
        if( mseX > moValBox[1].position.x ) return false;// right of
        if( mseY < moValBox[0].position.y ) return false;// above
        if( mseY > moValBox[2].position.y ) return false;// below
        return true;
    }

    return hoverSpot::hit();
}

bool floatSpot::MseOver()
{
    button::MseOver();
    if( sel ) voh.MseOver();
    if( mseOver ) pButtMse = this;// proxy for voh too
    if( !mseOver && sel ) setSel(false);// proxy for voh too
    return mseOver;
}

bool floatSpot::hitLeft()
{
    if( button::delayCnt < button::moDelay ) return false;

    if( sel )// voh open
    {
        if( voh.hitLeft() )
        {
            setSel(false);// new value entered
            button::delayCnt = 0;// stop final draw
        }
    }
    else // hover acting
        setSel(true);// expose voh control

    return mseOver;
}

bool floatSpot::hitRight()
{
    if( button::delayCnt < button::moDelay ) return false;

    if( sel )// voh open
    {
        if( voh.hitRight() )
        {
            setSel(false);// new value entered
            button::delayCnt = 0;// stop final draw
        }
    }
    else // hover acting
        setSel(true);// expose voh control

    return mseOver;
}

/*
bool floatSpot::hitLeft()
{
    if( sel )// voh open
    {
        if( voh.hitLeft() )
            setSel(false);// new value entered
    }
    else if( button::delayCnt >= button::moDelay )// hover acting
        setSel(true);// expose voh control

    return mseOver;
}
*/

void floatSpot::setSel( bool Sel )
{
    sel = Sel;
    voh.mseOver = false;
    if( sel ) makeBox( true );// big box
    else makeBox( false );// small box
}

void floatSpot::init_delayBox()// need extra space for float display
{
    voh.init_delayBox();
    makeBox( false );
}

void floatSpot::draw( sf::RenderTarget& rRW )const
{
 //   static int afterDelay = 0;
    if( pButtMse == this && button::delayCnt >= button::moDelay )// hover acting
    {
        rRW.draw( button::moValBox, 4, sf::Quads );
        if( sel ) voh.draw(rRW);
        rRW.draw( msg );// draw txtVal
        rRW.draw( voh.title );// draw txtVal
        rRW.draw( voh.txtVal );// draw txtVal
//        afterDelay = 20;
    }

 /*   if( afterDelay > 0 )
    {
        --afterDelay;
        rRW.draw( msg );// draw txtVal
        rRW.draw( voh.title );// draw txtVal
        rRW.draw( voh.txtVal );// draw txtVal
    }   */

    rRW.draw( img );
}

void floatSpot::makeBox( bool Big ) const// utility
{
    float xLt = pos.x;
    float yUp = pos.y - radius;
    sf::Vector2f boxSz;

    if( Big )// big box
    {
        xLt = pos.x - radius;
        yUp = pos.y - radius;
    //    boxSz.x = voh.sz.x;
        boxSz.x = voh.pos.x + voh.sz.x - xLt + radius;
        boxSz.y = voh.pos.y + voh.sz.y - yUp;
    }
    else// small box
    {
        xLt = pos.x + ofst.x;
        yUp = pos.y + ofst.y;
        boxSz.x = msg.getGlobalBounds().width;
        boxSz.y = 2.0f*msg.getGlobalBounds().height;
    }

    moValBox[0].position.x = xLt;
    moValBox[0].position.y = yUp;// up lt
    moValBox[1].position.x = xLt + boxSz.x;
    moValBox[1].position.y = yUp;// up rt
    moValBox[2].position.x = xLt + boxSz.x;
    moValBox[2].position.y = yUp + boxSz.y;// dn rt
    moValBox[3].position.x = xLt;
    moValBox[3].position.y = yUp + boxSz.y;// dn lt
    moValBox[0].color = moValBox[1].color = moValBox[2].color = moValBox[3].color = boxClr;
}
