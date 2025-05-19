#include "radioButton.h"

sf::Color radioButton::bkgdColor = sf::Color::White;
sf::Color radioButton::borderColor = sf::Color(136,136,136);// grey;

void radioButton::init( std::istream& is )
{
    std::string label; is >> label;
    sf::Text Title;
    Title.setFont( *button::pFont );
    Title.setCharacterSize(12);
    Title.setFillColor( sf::Color::Black );
    Title.setString( label.c_str() );

    float x, y, W, H;
    is >> x >> y >> W >> H;
    size_t numButts; is >> numButts;
    int iSel; is >> iSel;
    init( x, y, W, H, numButts, Title, iSel );
}

void radioButton::init( float x, float y, float W, float H, size_t numButts, const sf::Text& Title, int iSel )
{
    buttonRect::init( x, y, W, H, nullptr, Title );
 //   R[0].setFillColor( bkgdColor );
 //   R[0].setOutlineColor( borderColor );
    R.setFillColor( bkgdColor );
    R.setOutlineColor( borderColor );
    title.setPosition( x, y+H );

    sz.x = W; sz.y = H;
    pos.x = x; pos.y = y;
    isHoriz = W > H;
    r = isHoriz ? 0.7f*H : 0.7f*W;
    r /= 2.0f;
    Nbutts = numButts;
    alwaysSel = iSel >= 0;
    sel = alwaysSel;// true by default
    selIdx = alwaysSel ? (size_t)iSel : 0;
    float len = isHoriz ? W : H;
    sep = len/(Nbutts+1);

    buttVec.reserve( Nbutts );
    for( size_t i=0; i<Nbutts; ++i )
    {
        sf::CircleShape tmp;
        tmp.setRadius(r);
        tmp.setOrigin( r, r );
        tmp.setFillColor( sf::Color::White );
        tmp.setOutlineColor( sf::Color::Blue );
        tmp.setOutlineThickness( 2.0f );

        if( isHoriz ) tmp.setPosition( pos.x + (i+1)*sep, pos.y + H/2.0f );
        else tmp.setPosition( pos.x + W/2.0f, pos.y + (i+1)*sep );
        buttVec.push_back( tmp );
    }

    dot.setRadius(r/2.0f);
    dot.setOrigin( r/2.0f, r/2.0f );
    dot.setFillColor( sf::Color::Black );
    dot.setPosition( buttVec[selIdx ].getPosition() );
}

void radioButton::setButtLabels( float offX, float offY, const std::vector<std::string>& labelVec, unsigned int fontSz, sf::Color labelColor )
{
    if( labelVec.empty() ) return;
    buttLabelVec.clear();
    buttLabelVec.reserve( labelVec.size() );
    sf::Vector2f ofst( offX, offY );
    for( unsigned int i = 0; i < labelVec.size() && i < buttVec.size(); ++i )
    {
        buttLabelVec.push_back( sf::Text( labelVec[i].c_str(), *button::pFont, fontSz ) );
        buttLabelVec.back().setPosition( buttVec[i].getPosition() + ofst );
        buttLabelVec.back().setFillColor( labelColor );
    //    std::cout << labelVec[i];
    }
}

bool radioButton::hitLeft()
{
    if( !mseDnLt ) return false;

    // is a button hit?
    for( size_t i=0; i<buttVec.size(); ++i )
    {
        sf::Vector2f bPos = buttVec[i].getPosition();
        if( (mseX - bPos.x)*(mseX - bPos.x) + (mseY - bPos.y)*(mseY - bPos.y) < r*r )// this one hit
        {
            sel = true;
            selIdx = i;
            dot.setPosition( bPos );
            if( pFuncIdx ) pFuncIdx( i );
            return true;
        }
    }

    return true;
}

void radioButton::setIndex( size_t i )
{
    if( i >= buttVec.size() ) return;
    sf::Vector2f bPos = buttVec[i].getPosition();
    if( i != selIdx )// act only on change
    {
        selIdx = i;
        dot.setPosition( bPos );
        if( pFuncIdx ) pFuncIdx( i );
    }
    return;
}

void radioButton::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f oldPos = pos;
    buttonRect::setPosition( Pos );
    for( auto& x : buttVec ) x.setPosition( Pos - oldPos + x.getPosition() );
    for( auto& x : buttLabelVec ) x.setPosition( Pos - oldPos + x.getPosition() );
    dot.setPosition( buttVec[selIdx].getPosition() );
}

void radioButton::draw( sf::RenderTarget& RT )const
{
//    if( alwaysSel ) sel = true;
    RT.draw( title );
 //   RT.draw( R[0] );
    RT.draw( R );
    for( auto& butt : buttVec ) RT.draw( butt );
    if( drawButtLabels ) for( auto& txt : buttLabelVec ) RT.draw( txt );
    if( sel ) RT.draw( dot );
}

//state_ab* radioButton::newState()
//{ std::cout << "\nradioButt getState()"; return new state_fn( std::bind( &radioButton::getState, this, std::placeholders::_1 ), std::bind( &radioButton::setState, this, std::placeholders::_1 ), 1 ); }
