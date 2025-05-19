#include "subMenuABS.h"

bool subMenuABS::init( std::istream& is )
{
    sf::Vector2f iPos;// 1st option message position
    is >> iPos.x >> iPos.y;
    float msgSpacing;
    is >> msgSpacing;
    size_t fontSz = 30;
    is >> fontSz;
    unsigned int r,g,b;
    is >> r >> g >> b;
//    msgClrReg.setColor( sf::Color(r,g,b) );
    msgClrReg = sf::Color(r,g,b);
    is >> r >> g >> b;
//    msgClrMseOver.setColor( sf::Color(r,g,b) );
    msgClrMseOver = sf::Color(r,g,b);

    is >> numOptions;
    msgVec.reserve(numOptions);
    std::string msgIn;

    std::cout << "numOptions = " << numOptions << '\n';


    is.ignore(256,'\n');
    sf::Text tmpTxt;
    if( button::pFont ) tmpTxt.setFont( *button::pFont );
    tmpTxt.setFillColor(msgClrReg);
    tmpTxt.setCharacterSize(fontSz);

    for( size_t i=0; i<numOptions; ++i )
    {
        getline( is, msgIn );
        tmpTxt.setString( msgIn.c_str() );// = "Ek = ";
        tmpTxt.setPosition( iPos.x, iPos.y + msgSpacing*i );
        std::cout << msgIn.c_str() << " at " << iPos.x << ", " << iPos.y + msgSpacing*i << '\n';
        msgVec.push_back( tmpTxt );

        sf::Rect<float> tmpRect;
        tmpRect.left = iPos.x;
        tmpRect.top = iPos.y + msgSpacing*i;
        tmpRect.width = 500.0f;
        tmpRect.height = 30.0f;
        rectVec.push_back( tmpRect );
    }

    sf::Text label("Menu", *button::pFont, 12);
    label.setFillColor( sf::Color(0,0,255) );
    gotoMeButt.init( Level::winW - 80.0f, 140.0f, 60.0f, 28.0f, [this](){ reset(); }, label );
 // save the button*s
    myButtVec.push_back( &Level::quitButt );
    myButtVec.push_back( &Level::goto_MMButt );

    reset();
    return true;
}

void subMenuABS::reset()
{
    if( p_subLevel ) delete p_subLevel;
        p_subLevel = nullptr;

    button::RegisteredButtVec.clear();
    for( button* pButt : myButtVec )
        button::RegisteredButtVec.push_back( pButt );
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    gotoMeButt.setSel(false);
    button::mseDnLt = false;// so not down on return from a level
    std::cout << "\n subMenuABS::reset() called";
}

bool subMenuABS::handleEvent( sf::Event& rEvent )// mouse input launches rB
{
    if( p_subLevel )
        return p_subLevel->handleEvent( rEvent );

    size_t j = numOptions;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if( rEvent.key.code == sf::Keyboard::Num1 ) { j = 0; lvlNum = 1; }
        else if( rEvent.key.code == sf::Keyboard::Num2 ) { j = 1; lvlNum = 2; }
        else if( rEvent.key.code == sf::Keyboard::Num3 ) { j = 2; lvlNum = 3; }
        else if( rEvent.key.code == sf::Keyboard::Num4 ) { j = 3; lvlNum = 4; }
        else if( rEvent.key.code == sf::Keyboard::Num5 ) { j = 4; lvlNum = 5; }
        else if( rEvent.key.code == sf::Keyboard::Num6 ) { j = 5; lvlNum = 6; }
        else if( rEvent.key.code == sf::Keyboard::Num7 ) { j = 6; lvlNum = 7; }
        else if( rEvent.key.code == sf::Keyboard::Num8 ) { j = 7; lvlNum = 8; }
        else if( rEvent.key.code == sf::Keyboard::Num9 ) { j = 8; lvlNum = 9; }

  //      if( lvlNum > numOptions ) levelBox.setSel(true);
    }
    else if (rEvent.type == sf::Event::MouseMoved)
    {
        for( size_t i=0; i<numOptions; ++i )
        {
            if( hitRect( rectVec[i], sf::Vector2f( button::mseX, button::mseY ) ) ) msgVec[i].setFillColor(msgClrMseOver);
            else msgVec[i].setFillColor(msgClrReg);
        }

    }
    else if (rEvent.type == sf::Event::MouseButtonPressed)// lbutt down
    {
        if (rEvent.mouseButton.button == sf::Mouse::Left)
            for( size_t i=0; i<numOptions; ++i )
                if( hitRect( rectVec[i], sf::Vector2f( button::mseX, button::mseY ) ) ) j = i;
    }

    if( j < numOptions )// new level
    {
        msgVec[j].setFillColor(msgClrReg);
        if( !loadLevel(j) )
        {
            std::cout << "\nLevel load fail";
            reset();
        }
    }

    return true;
}

void subMenuABS::update( float dt )
{
    if( p_subLevel )
    {
        p_subLevel->update( dt );
        return;
    }

    return;
}

void subMenuABS::draw( sf::RenderTarget& RT ) const
{
    if( p_subLevel )
    {
        p_subLevel->draw(RT);
        return;
    }

    for( auto& x : msgVec ) RT.draw( x );
    return;
}

void subMenuABS::cleanup()
{
    if( p_subLevel ) delete p_subLevel;
    return;
}
