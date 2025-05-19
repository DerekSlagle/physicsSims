#include "colorPicker.h"
//#include "textButton.h"// for focus stealing in setSel()

void colorPicker::init( float x, float y, float W, float H )
{
    pos.x = x; pos.y = y;
    if( W < 100.0f ) W = 100.0f;
    if( H < 120.0f ) H = 120.0f;
    sz.x = W; sz.y = H;

    bkgdRect.setSize( sz );
    bkgdRect.setPosition( pos );
    bkgdRect.setFillColor( sf::Color::White );

    // big color rect
    vtxRange[0].position.x = pos.x + 10.0f; vtxRange[0].position.y = pos.y + 10.0f;// up lt
    vtxRange[1].position.x = pos.x + 10.0f + sz.x - 70.0f; vtxRange[1].position.y = pos.y + 10.0f;// up rt
    vtxRange[2].position.x = pos.x + 10.0f + sz.x - 70.0f; vtxRange[2].position.y = pos.y + sz.y - 80.0f;// dn rt
    vtxRange[3].position.x = pos.x + 10.0f; vtxRange[3].position.y = pos.y + sz.y - 80.0f;// dn lt
    vtxRange[0].color = vtxRange[1].color = sf::Color::White;
    vtxRange[2].color = vtxRange[3].color = sf::Color::Black;

    float R = 4.0f;
    colorSpot.setRadius(R);
    colorSpot.setFillColor( sf::Color::White );
    colorSpot.setOutlineColor( sf::Color::Green );
    colorSpot.setOutlineThickness( 1.0f );
    colorSpot.setOrigin( R,R );
    colorSpot.setPosition( vtxRange[0].position.x, vtxRange[0].position.y );

    // color preview
    previewRect.setSize( sf::Vector2f( 60.0f, 30.0f ) );
    previewRect.setPosition( sf::Vector2f( pos.x + 10.0f, pos.y + sz.y - 70.0f ) );
    previewRect.setFillColor( sf::Color(100,100,100) );

    sf::Vector2f stripPos( pos.x + sz.x - 40.0f, vtxRange[0].position.y );
    float stripHeight = vtxRange[2].position.y - vtxRange[0].position.y;
//    sf::Vector2f dPos( stripHeight/6.0f, 30.0f );
    sf::Vector2f dPos( 30.0f, stripHeight/6.0f );
    float yVtx = stripPos.y;

    // rainbow strip
    for( size_t i=0; i<=12; i += 2 )
    {
        vtxStrip[i].position.x = stripPos.x;
        vtxStrip[i].position.y = yVtx;
        vtxStrip[i+1].position.x = stripPos.x + dPos.x;
        vtxStrip[i+1].position.y = yVtx;
        yVtx += dPos.y;
    }

    vtxStrip[0].color = vtxStrip[1].color = sf::Color(255,0,0);
    vtxStrip[2].color = vtxStrip[3].color = sf::Color(255,0,255);
    vtxStrip[4].color = vtxStrip[5].color = sf::Color(0,0,255);
    vtxStrip[6].color = vtxStrip[7].color = sf::Color(0,255,255);
    vtxStrip[8].color = vtxStrip[9].color = sf::Color(0,255,0);
    vtxStrip[10].color = vtxStrip[11].color = sf::Color(255,255,0);
    vtxStrip[12].color = vtxStrip[13].color = sf::Color(255,0,0);

    // color labels
    if( button::pFont )
    {
        txtRed.setFont( *button::pFont );
        txtRed.setCharacterSize(14);
        txtRed.setString( "Red" );
        txtRed.setColor( sf::Color::Red );
        txtRed.setPosition( sf::Vector2f( vtxStrip[12].position.x - 50.0f, vtxStrip[12].position.y + 10.0f ) );
        txtRedVal = txtRed;
        txtRedVal.setPosition( txtRed.getPosition().x + 50.0f, txtRed.getPosition().y );

        txtGreen.setFont( *button::pFont );
        txtGreen.setCharacterSize(14);
        txtGreen.setString( "Green" );
        txtGreen.setColor( sf::Color::Green );
        txtGreen.setPosition( sf::Vector2f( vtxStrip[12].position.x - 50.0f, vtxStrip[12].position.y + 30.0f ) );
        txtGreenVal = txtGreen;
        txtGreenVal.setPosition( txtGreen.getPosition().x + 50.0f, txtGreen.getPosition().y );

        txtBlue.setFont( *button::pFont );
        txtBlue.setCharacterSize(14);
        txtBlue.setString( "Blue" );
        txtBlue.setColor( sf::Color::Blue );
        txtBlue.setPosition( sf::Vector2f( vtxStrip[12].position.x - 50.0f, vtxStrip[12].position.y + 50.0f ) );
        txtBlueVal = txtBlue;
        txtBlueVal.setPosition( txtBlue.getPosition().x + 50.0f, txtBlue.getPosition().y );

        // OKbutt and CancelButt
        sf::Text txt( "OK", *button::pFont, 10);
        txt.setColor( sf::Color(0,0,0) );
        OKbutt.init( pos.x + 10.0f, pos.y + sz.y - 30.0f, 40.0f, 20.0f, txt );
        txt.setString("Cancel");
        CancelButt.init( pos.x + 60.0f, pos.y + sz.y - 30.0f, 40.0f, 20.0f, txt );
    }


}

    bool colorPicker::hit()const
    {
        return hitFloatRect( bkgdRect.getGlobalBounds() );
    }

    bool colorPicker::hitLeft()
    {
        if( sel && button::mseDnLt )// control is open
        {
            if( OKbutt.hitLeft() )
            {
                sel = false;
                OKbutt.setSel(false);
                if( pColor ) *pColor = finalColor;
            //    if( pColorFunc ) pColorFunc( finalColor );
                return true;
            }
            if( CancelButt.hitLeft() )
            {
                sel = false;
                CancelButt.setSel(false);
                return true;
            }

        //    std::cout << "\nmissed butts";

            if( getStripColor( stripColor ) )
            {
                std::cout << "\nHit strip";
                previewRect.setFillColor( stripColor );
                vtxRange[1].color = stripColor;
                to_SF_string( txtRedVal, (int)stripColor.r );
                to_SF_string( txtBlueVal, (int)stripColor.b );
                to_SF_string( txtGreenVal, (int)stripColor.g );
                colorSpot.setFillColor( stripColor );
                colorSpot.setPosition( vtxRange[1].position.x, vtxRange[1].position.y );
                finalColor = stripColor;
                return true;
            }

            // interpolate in big color box
            float offX = button::mseX - vtxRange[0].position.x;
            float W = vtxRange[1].position.x - vtxRange[0].position.x;
            float offY = button::mseY - vtxRange[0].position.y;
            float H = vtxRange[2].position.y - vtxRange[0].position.y;
            if( offX >= 0.0f && offX <= W && offY >= 0.0f && offY <= H )// hit
            {
                sf::Color horizColor;
                horizColor.r = sf::Color::White.r - ( offX/W )*( sf::Color::White.r - stripColor.r );
                horizColor.g = sf::Color::White.g - ( offX/W )*( sf::Color::White.g - stripColor.g );
                horizColor.b = sf::Color::White.b - ( offX/W )*( sf::Color::White.b - stripColor.b );
                std::cout << "\n stripColor: " << (int)stripColor.r << ' ' << (int)stripColor.g << ' ' << (int)stripColor.b;
                std::cout << "\n horizColor: " << (int)horizColor.r << ' ' << (int)horizColor.g << ' ' << (int)horizColor.b;
                finalColor.r = ( 1.0f - offY/H )*horizColor.r;
                finalColor.g = ( 1.0f - offY/H )*horizColor.g;
                finalColor.b = ( 1.0f - offY/H )*horizColor.b;
                std::cout << "\n finalColor: " << (int)finalColor.r << ' ' << (int)finalColor.g << ' ' << (int)finalColor.b;
                to_SF_string( txtRedVal, (int)finalColor.r );
                to_SF_string( txtBlueVal, (int)finalColor.b );
                to_SF_string( txtGreenVal, (int)finalColor.g );
                colorSpot.setFillColor( finalColor );
                colorSpot.setPosition( button::mseX, button::mseY );
                previewRect.setFillColor( finalColor );
            }
        }

        return false;
    }

    void colorPicker::draw( sf::RenderTarget& rRW )const
    {
        if( sel )// control is open
        {
            rRW.draw( bkgdRect );
            rRW.draw( previewRect );
            rRW.draw( vtxStrip, 14, sf::TrianglesStrip );
            rRW.draw( vtxRange, 4, sf::Quads );
            rRW.draw( colorSpot );
            rRW.draw( txtRed );
            rRW.draw( txtGreen );
            rRW.draw( txtBlue );
            rRW.draw( txtRedVal );
            rRW.draw( txtGreenVal );
            rRW.draw( txtBlueVal );
            OKbutt.draw(rRW);
            CancelButt.draw(rRW);
            if( is_Modal )
            {
                static sf::Color shadeColor = sf::Color( 0, 0, 200, 50 );
                static sf::RectangleShape shadeRect( sf::Vector2f( rRW.getSize().x, rRW.getSize().y ) );
                shadeRect.setFillColor( shadeColor );
                rRW.draw( shadeRect );
            }
        }
    }

    bool colorPicker::MseOver()
    {
        if( !sel ) return false;
        if( button::MseOver() )
        {
            OKbutt.MseOver();
            CancelButt.MseOver();
            return true;
        }

        return is_Modal;
    }

    void colorPicker::setSel( bool Sel )
    {
        sel = Sel;
        if( is_Modal )// lock down focus
        {
            if( sel )
            {
           //     if( button::pButtMse ) { button::pButtMse->setMO( false ); button::pButtMse->setSel(false); button::pButtMse = this; }
           //     if( button::pTxtButtFocus ) { button::pTxtButtFocus->setSel(false); button::pTxtButtFocus = nullptr; }
                grabFocus( this );
                setMO( true );
                CancelButt.setMO( false );
                OKbutt.setMO( false );
            }
            else
                if( button::pButtMse == this ) button::pButtMse = nullptr;
        }
    }

    void colorPicker::setPosition( sf::Vector2f Pos )
    {
     //   sf::Vector2f dPos = Pos - pos;
        pos = Pos;
    }

    void colorPicker::update()
    {
        return;
    }

    bool colorPicker::getStripColor( sf::Color& retVal )const
    {
        if( hitFloatRect( sf::FloatRect( vtxStrip[0].position.x, vtxStrip[0].position.y, vtxStrip[1].position.x - vtxStrip[0].position.x, vtxStrip[12].position.y - vtxStrip[0].position.y ) ) )// the strip
        {
            float H = vtxStrip[12].position.y - vtxStrip[0].position.y;
            float dy = H/6.0f;
            int i0 = (int)( (button::mseY - vtxStrip[0].position.y )/dy );
            std::cout << "\n i0 = " << i0;
            if( 10 == 6 ) { retVal = sf::Color::Red; return true; }// red at bottom

            float x = button::mseY - vtxStrip[0].position.y - i0*dy;
            x /= dy;// as fraction
            i0 *= 2;
            retVal = vtxStrip[i0].color;
            retVal.r += x*( vtxStrip[i0+2].color.r - vtxStrip[i0].color.r );
            retVal.b += x*( vtxStrip[i0+2].color.b - vtxStrip[i0].color.b );
            retVal.g += x*( vtxStrip[i0+2].color.g - vtxStrip[i0].color.g );

            return true;
        }

        return false;
    }
