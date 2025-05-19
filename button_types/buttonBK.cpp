#include "button.h"
#include "textButton.h"

float button::mseX = 0.0f, button::mseY = 0.0f;
bool button::mseDnLt = false, button::mseDnRt = false;
bool button::mseDnLt_last = false, button::mseDnRt_last = false;
bool button::mseMoved = false;
bool button::didScroll = false;
float button::scrollAmount = 0.0f;
button* button::pButtMse = nullptr;// button under the mouse
textButton* button::pTxtButtFocus = nullptr;
std::vector<button*> button::RegisteredButtVec;
std::function<bool(void)> button::pOKtoRun = nullptr;
//bool button::usingMapView = false;

// supporting mouse over delay
int button::delayCnt = 0;       // is condition for updating & drawing moVals
sf::Text button::moValtxt;// real time value
sf::Vertex button::moValBox[4];// Quad as bkgd
const  sf::Font* button::pFont = nullptr;// about time?
int button::moDelay = 30;// about 1/2 second?

button::button(): mseOver(false), sel(false), pos(0.0f,0.0f) {}

button::~button() {}// dtor

bool button::MseOver()// assigns mseOver
{
    if(  hit() ) mseOver = true;
    else mseOver = false;

    if( !mseOver && mode == 'I' && sel ) // support NOHC operation
    {
        if( pHitFunc ) pHitFunc();
   //     sel = false;// older
        setSel(false);// new
    }

    return mseOver;
}

bool button::hitLeft()// alters sel
{
 //   if( !mseDnLt ) return false;
    if( ( mode == 'T' || mode == 'L' ) && !mseDnLt ) return false;// new

    if( hit() )
    {
   //     sel = !sel;// toggle
        setSel( !sel );// toggle - new
        if( mseDnLt && pHitFunc ) pHitFunc();// call the function!
        return true;
    }
    return false;
}

// *** static functions

void button::init( const sf::Font& rFont, int mo_delay, sf::Color hoverBoxColor )
{
    button::pFont = &rFont;
    button::moDelay = mo_delay;// about 1/2 second?

    sf::Vector2f defPos(0.0f,0.0f);
    button::moValtxt.setPosition(defPos);
    button::moValtxt.setFont(rFont);
    button::moValtxt.setCharacterSize(12);
    button::moValtxt.setFillColor( sf::Color::Cyan );
    button::moValtxt.setString("0.0");
    pButtMse = nullptr;
    pTxtButtFocus = nullptr;
    mseMoved = didScroll = false;
    scrollAmount = 0.0f;

//    sf::Vector2f& sz = buttonValOnHit::moValBoxSz;
    sf::Vertex* pV = button::moValBox;
    pV[0].color = pV[1].color = pV[2].color = pV[3].color = hoverBoxColor;
}

void button::handleEvent(  sf::Event& rEvent )
{
    if (rEvent.type == sf::Event::MouseMoved)// *** mouse moved
    {
        mseX = rEvent.mouseMove.x;
        mseY = rEvent.mouseMove.y;
        mseMoved = true;
    }
    else if (rEvent.type == sf::Event::MouseButtonPressed)
    {
        mseX = rEvent.mouseButton.x;
        mseY = rEvent.mouseButton.y;

        if (rEvent.mouseButton.button == sf::Mouse::Left) mseDnLt = true;
        else if (rEvent.mouseButton.button == sf::Mouse::Right) mseDnRt = true;
    }
    else if (rEvent.type == sf::Event::MouseButtonReleased)// lbutt up
    {
        mseX = rEvent.mouseButton.x;
        mseY = rEvent.mouseButton.y;

        if (rEvent.mouseButton.button == sf::Mouse::Left)  mseDnLt = false;
        else if (rEvent.mouseButton.button == sf::Mouse::Right) mseDnRt = false;
    }
    else if (rEvent.type == sf::Event::MouseWheelScrolled)
    {
        scrollAmount = (float)rEvent.mouseWheelScroll.delta;
        didScroll = true;
    }
 //   else if ( rEvent.type == sf::Event::KeyPressed )
 //   {
//        if( pTxtButtFocus ) pTxtButtFocus->pushChar( rEvent.key.code );
//        if( pTxtButtFocus ) pTxtButtFocus->pushChar( rEvent.key );
 //   }
    else if ( rEvent.type == sf::Event::TextEntered )
    {
//        if( pTxtButtFocus ) pTxtButtFocus->pushChar( rEvent.key.code );
        if( pTxtButtFocus ) pTxtButtFocus->pushChar( rEvent.text );
    }

    return;
}

void button::reset()
{
    mseDnRt_last = mseDnRt;
    mseDnLt_last = mseDnLt;
    mseMoved = didScroll = false;
    scrollAmount = 0.0f;
}

// called if map view buttons present
void button::MseOverAll( float ofstX , float ofstY )
{
    static button* pButtMse_last = nullptr;
    pButtMse_last = pButtMse;

    if( pButtMse )// perhaps still under?
    {
        if( pButtMse->inDefView )
        {
            if( !pButtMse->MseOver() ) pButtMse = nullptr;
        }
        else// map view
        {
            pButtMse->mseOver = false;
            pButtMse = nullptr;
        }
    }

   if( !pButtMse )
   {
    for( auto& pButt : RegisteredButtVec )
        if( pButt->inDefView && pButt->MseOver() )// break;
        {
            pButtMse = pButt;
         //   break;// found it!
        }
   }

    if( !pButtMse )// perhaps a map view one?
    {
        mseX += ofstX; mseY += ofstY;
        for( auto& pButt : RegisteredButtVec )
            if( !pButt->inDefView && pButt->MseOver() )// break;
            {
                pButtMse = pButt;
            //    break;// found it!
            }

        if( !pButtMse ) { mseX -= ofstX; mseY -= ofstY; }
    }

    if( !pButtMse && pButtMse_last )// mouse slid off button. Reset buttons to up?
    {
        mseDnLt = mseDnLt_last = mseDnRt = mseDnRt_last = false;
        delayCnt = 0;
    }

    if( pButtMse_last != pButtMse ) delayCnt = 0;

    if( clickEvent_Lt() == 1 || clickEvent_Rt() == 1 )// presume no textButton hit. Call to hitLeft() will restore
    {
        if( pTxtButtFocus ) pTxtButtFocus->setSel(false);// so it will be selected if clicked again
        pTxtButtFocus = nullptr;// click missed all buttons
    }

    if( pOKtoRun && !pOKtoRun() ) { pButtMse = nullptr; return; }// ignore click activity
    if( pButtMse )// mouse is over a button
    {
        if( clickEvent_Lt() != 0 ) pButtMse->hitLeft();// press and release
        if( clickEvent_Rt() != 0 ) pButtMse->hitRight();// right button just pressed

        if( pButtMse && ( delayCnt < button::moDelay ) && ++delayCnt == button::moDelay )
        pButtMse->init_delayBox();

        if( pButtMse && !pButtMse->inDefView ) { mseX -= ofstX; mseY -= ofstY; }
    }

    for( auto& pButt : RegisteredButtVec ) if( pButt->inDefView ) pButt->update();
    mseX += ofstX; mseY += ofstY;
    for( auto& pButt : RegisteredButtVec ) if( !pButt->inDefView ) pButt->update();
    mseX -= ofstX; mseY -= ofstY;
}

void button::MseOverAll()// all in default view
{
    static button* pButtMse_last = nullptr;
    pButtMse_last = pButtMse;

    // check if stiil over last
    if( pButtMse && !pButtMse->MseOver() ) pButtMse = nullptr;

    if( !pButtMse )// nope. Check all
        for( auto& pButt : RegisteredButtVec )
            if( pButt->MseOver() ) pButtMse = pButt;

    if( !pButtMse && pButtMse_last )// mouse slid off button. Reset buttons to up?
    {
        mseDnLt = mseDnLt_last = mseDnRt = mseDnRt_last = false;
        delayCnt = 0;
    }

    if( pButtMse_last != pButtMse ) delayCnt = 0;

    if( clickEvent_Lt() == 1 || clickEvent_Rt() == 1 )// presume no textButton hit. Call to hitLeft() will restore
    {
        if( pTxtButtFocus ) pTxtButtFocus->setSel(false);// so it will be selected if clicked again
        pTxtButtFocus = nullptr;// click missed all buttons
    }

    if( pButtMse )// mouse is over a button
    {
        if( clickEvent_Lt() != 0 ) pButtMse->hitLeft();// press and release
        if( clickEvent_Rt() != 0 ) pButtMse->hitRight();// right button just pressed

        if( pButtMse && ( delayCnt < button::moDelay ) && ++delayCnt == button::moDelay )
        pButtMse->init_delayBox();
    }

    for( auto& pButt : RegisteredButtVec ) { pButt->update(); }//
}

void button::drawAll_def( sf::RenderTarget& rRT )
{
    for( auto& pButt : RegisteredButtVec )
        if( pButt->inDefView ) pButt->draw(rRT);
}

void button::drawAll_map( sf::RenderTarget& rRT )
{
    for( auto& pButt : RegisteredButtVec )
        if( !pButt->inDefView ) pButt->draw(rRT);
}

void button::unRegisterMe( button* pButton )// erase the pointer in RegisteredButtVec
{
    std::vector<button*>::iterator itButt = RegisteredButtVec.begin();
    while( itButt != RegisteredButtVec.end() && *itButt != pButton ) ++itButt;

    if( itButt != RegisteredButtVec.end() )// found it!
    {
        RegisteredButtVec.erase(itButt);// no more automatic MseOver, draw calls for you
        if( pButtMse == pButton ) pButtMse = nullptr;// it will be honored next frame however, if you're still around
    }
}

bool button::registerUnique( button* pButton )// returns true if newly registered
{
    bool pcFound = false;
    for( auto& pButt : RegisteredButtVec ) if( pButt == pButton ){ pcFound = true; break; }
    if( !pcFound ) { button::RegisteredButtVec.push_back( pButton ); }
    return !pcFound;
}

int button::clickEvent_Lt()// 0= no event +1= down -1= up
{
    if( mseDnLt == mseDnLt_last ) return 0;
    return mseDnLt ? 1 : -1;// down: up
}

int button::clickEvent_Rt()
{
    if( mseDnRt == mseDnRt_last ) return 0;
    return mseDnRt ? 1 : -1;// down: up
}

void button::grabFocus( button* pButt )
{
    if( button::pButtMse )
    {
        button::pButtMse->setMO( false );
        button::pButtMse->setSel(false);
        button::pButtMse = pButt;
    }
    if( button::pTxtButtFocus )
    {
        button::pTxtButtFocus->setSel(false);
        button::pTxtButtFocus = nullptr;
    }
}

// utility
bool button::hitFloatRect( const sf::FloatRect& FR )
{
    if( mseX < FR.left ) return false;
    if( mseX > FR.left + FR.width ) return false;
    if( mseY < FR.top ) return false;
    if( mseY > FR.top + FR.height ) return false;
    return true;
}
