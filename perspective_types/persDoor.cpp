#include "persDoor.h"

float persDoor::fracOpenMin = 0.03f;// for fill image to be updated and drawn

void persDoor::init( std::istream& is, spriteSheet& rSS )
{
    unsigned int SetNum, FrIdx; is >> SetNum >> FrIdx;

    vec3f Pos, nu; is >> Pos.x >> Pos.y >> Pos.z >> nu.x >> nu.y >> nu.z;
    char MoveDirection; is >> MoveDirection;
    is >> moveMode >> moveSpeed;
    float Scale; is >> Scale;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    init( rSS, SetNum, FrIdx, MoveDirection, Pos, nu, Scale, sf::Color(rd,gn,bu) );
}

void persDoor::init( spriteSheet& rSS, unsigned int SetNum, unsigned int FrIdx, char MoveDirection, vec3f Pos, vec3f nu, float Scale, sf::Color frameColor )
{
    pDoorTxt = &(rSS.txt);
    sf::IntRect srcRect_I = rSS.getFrRect( FrIdx, SetNum, false );
    srcRect.left = static_cast<float>( srcRect_I.left );
    srcRect.top = static_cast<float>( srcRect_I.top );
    srcRect.width = static_cast<float>( srcRect_I.width );
    srcRect.height = static_cast<float>( srcRect_I.height );

    Wtxt = static_cast<float>( srcRect.width );
    Htxt = static_cast<float>( srcRect.height );// width and height of texture

    moveDirection = MoveDirection;
    pos = Pos;
    scale = Scale;
    W = Scale*Wtxt*0.5f;
    H = Scale*Htxt*0.5f;
    nu /= nu.mag();
    Nu = nu;
    wu = persPt::yHat.cross( Nu );
    hu = Nu.cross( wu );

    for( unsigned int j = 0; j < 5; ++j ) frame[j].color = frameColor;
    for( unsigned int j = 0; j < 4; ++j ) doorQuad[j].color = sf::Color::White;

    std::cout << "\n persDoor::init(): moveDirection = " << moveDirection << " moveMode = " << moveMode;

    if( moveDirection == 'L' )// right edge moves
    {
        // 0,1 texCoords and 2,3 positions vary
        quadPos[0] = pos + wu*W - hu*H;// dn left
        doorQuad[0].texCoords = sf::Vector2f( srcRect.left, srcRect.top + Htxt );
        quadPos[1] = pos + wu*W + hu*H;// up left
        doorQuad[1].texCoords = sf::Vector2f( srcRect.left, srcRect.top );
        quadPos[2] = pos - wu*W + hu*H;// up right
        doorQuad[2].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top );
        quadPos[3] = pos - wu*W - hu*H;// dn right
        doorQuad[3].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top + Htxt );
    }
    else if( moveDirection == 'R' )// left edge moves
    {
        quadPos[0] = pos - wu*W - hu*H;// dn right
        doorQuad[0].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top + Htxt );
        quadPos[1] = pos - wu*W + hu*H;// up right
        doorQuad[1].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top );
        quadPos[2] = pos + wu*W + hu*H;// up left
        doorQuad[2].texCoords = sf::Vector2f( srcRect.left, srcRect.top );
        quadPos[3] = pos + wu*W - hu*H;// dn left
        doorQuad[3].texCoords = sf::Vector2f( srcRect.left, srcRect.top + Htxt );
    }
    else if( moveDirection == 'U' )// bottom edge moves
    {
        quadPos[0] = pos + wu*W + hu*H;// up left
        doorQuad[0].texCoords = sf::Vector2f( srcRect.left, srcRect.top );
        quadPos[1] = pos - wu*W + hu*H;// up right
        doorQuad[1].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top );
        quadPos[2] = pos - wu*W - hu*H;// dn right
        doorQuad[2].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top + Htxt );
        quadPos[3] = pos + wu*W - hu*H;// dn left
        doorQuad[3].texCoords = sf::Vector2f( srcRect.left, srcRect.top + Htxt );
    }
    else if( moveDirection == 'D' )// upper edge moves
    {
        quadPos[0] = pos - wu*W - hu*H;// dn right
        doorQuad[0].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top + Htxt );
        quadPos[1] = pos + wu*W - hu*H;// dn left
        doorQuad[1].texCoords = sf::Vector2f( srcRect.left, srcRect.top + Htxt );
        quadPos[2] = pos + wu*W + hu*H;// up left
        doorQuad[2].texCoords = sf::Vector2f( srcRect.left, srcRect.top );
        quadPos[3] = pos - wu*W + hu*H;// up right
        doorQuad[3].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top );
    }
    else
    {
        std::cout << "\n Bad moveDirection = " << moveDirection;
        return;
    }

    // assign positions
    for( unsigned int j = 0; j < 4; ++j )
    {
        framePos[j] = quadPos[j];
        doorQuad[j].position = persPt::get_xyw( quadPos[j] );
    }
    framePos[4] = framePos[0];

    for( unsigned int j = 0; j < 5; ++j )
        frame[j].position = persPt::get_xyw( framePos[j] );

    return;
}

//void persDoor::initFill( sf::RenderTexture& rRendTxt, std::function<void( float, sf::RenderTarget& )> updateDrawInFront, std::function<void( float, sf::RenderTarget& )> updateDrawBehind )
void persDoor::initFill( std::function<void( sf::RenderTarget& )> updateDrawInFront, std::function<void( sf::RenderTarget& )> updateDrawBehind )
{
 //   pRendTxt = &rRendTxt;
    updateDraw_inFront = updateDrawInFront;
    updateDraw_behind = updateDrawBehind;

    // prepare the canvas
    unsigned int pictW = static_cast<unsigned int>( 2.0f*W );
    unsigned int pictH = static_cast<unsigned int>( 2.0f*H );
    pRendTxt = new sf::RenderTexture;
    pRendTxt->create( pictW, pictH );
    sf::View pictView( sf::Vector2f( persPt::X0, persPt::Yh ), sf::Vector2f(  2.0f*W,  2.0f*H ) );// center pos, size
    pRendTxt->setView( pictView );

    for( unsigned int n = 0; n < 4; ++n ) pictQuad[n].color = sf::Color::White;
    pictQuad[0].texCoords = sf::Vector2f( 0.0f, 0.0f );// up left
    pictQuad[1].texCoords = sf::Vector2f( 2.0f*W, 0.0f );// up right
    pictQuad[2].texCoords = sf::Vector2f( 2.0f*W, 2.0f*H );// dn right
    pictQuad[3].texCoords = sf::Vector2f( 0.0f, 2.0f*H );// dn left
}

void persDoor::update( float dt )
{
    if( moving )
    {
        if( fractionOpenCurr < fractionOpen )// opening
        {
            fractionOpenCurr += moveSpeed*dt;
            if( fractionOpenCurr >= fractionOpen )
            {
                fractionOpenCurr = fractionOpen;
                moving = false;
            }
        }
        else if( fractionOpenCurr > fractionOpen )// closing
        {
            fractionOpenCurr -= moveSpeed*dt;
            if( fractionOpenCurr <= fractionOpen )
            {
                fractionOpenCurr = fractionOpen;
                moving = false;
            }
        }

        setOpenValue( fractionOpenCurr );
    }

    // update positions
    for( unsigned int j = 0; j < 4; ++j ) doorQuad[j].position = persPt::get_xyw( quadPos[j] );
    for( unsigned int j = 0; j < 5; ++j ) frame[j].position = persPt::get_xyw( framePos[j] );

    // update_doDraw
    doDraw = true;
    // becomes false if any point is behind the camera
    for( unsigned int j = 0; j < 4; ++j )
        if( ( framePos[j] - persPt::camPos ).dot( persPt::camDir ) < 0.0f ) { doDraw = false; break; }

    updateDrawFill();
}

void persDoor::updateDrawFill()
{
    if( !doDraw || !pRendTxt ) return;
    // update and draw fill image
    if( fractionOpenCurr < persDoor::fracOpenMin ) return;

        // detect camera movement
        static vec3f camPosLast = persPt::camPos;
        vec3f dCamPos = persPt::camPos - camPosLast;
        const float dPosMin = 0.2f;// ??

        vec3f sep = pos - persPt::camPos;
        float sepNu = sep.dot( Nu );

        // pictQuad positions
        if( sepNu < 0.0f )
        {
            pictQuad[0].position = persPt::get_xyw( pos + W*wu + H*hu  );
            pictQuad[1].position = persPt::get_xyw( pos - W*wu + H*hu  );
            pictQuad[2].position = persPt::get_xyw( pos - W*wu - H*hu  );
            pictQuad[3].position = persPt::get_xyw( pos + W*wu - H*hu  );
        }
        else
        {
            pictQuad[1].position = persPt::get_xyw( pos + W*wu + H*hu  );
            pictQuad[0].position = persPt::get_xyw( pos - W*wu + H*hu  );
            pictQuad[3].position = persPt::get_xyw( pos - W*wu - H*hu  );
            pictQuad[2].position = persPt::get_xyw( pos + W*wu - H*hu  );
        }

        if( !doDrawFill ) return;

        // update image if camera moved
        if( dCamPos.dot( dCamPos ) > dPosMin )
        {
            if( sepNu > 0.0f )
            {
                if( !updateDraw_inFront ) return;
            }
            else if( !updateDraw_behind ) return;

            vec3f CposSv = persPt::camPos;
            vec3f CdirSv = persPt::camDir;
            vec3f CxuSv = persPt::xu;
            vec3f CyuSv = persPt::yu;

            persPt::camDir = Nu;
            persPt::xu = wu;
            persPt::yu = hu;

            if( sepNu < 0.0f )
            {
                persPt::camDir *= -1.0f;
                persPt::xu *= -1.0f;
            }

            sf::Vector2f newCtr( persPt::get_xyw( pos ) );
            float zf = persPt::Z0/sepNu;
            if( zf < 0.0f ) zf *= -1.0f;
            if( zf > 1.0f ) zf = 1.0f;
            sf::Vector2f newSz(  2.0f*zf*W,  2.0f*zf*H );
            sf::View newView( newCtr, newSz );

            pRendTxt->setView( newView );

        //    pRendTxt->clear( sf::Color(0,0,0,160) );
        //    pRendTxt->clear( sf::Color(0,0,200) );

            if( sepNu > 0.0f )// camera is behind door
            {

                pRendTxt->clear( clearColor_inFront );
                if( updateDraw_inFront ) updateDraw_inFront( *pRendTxt );
            }
            else// in front
            {
                pRendTxt->clear( clearColor_behind );
                if( updateDraw_behind ) updateDraw_behind( *pRendTxt );
            }

            pRendTxt->display();

            // restore camera state
            persPt::camPos = CposSv;
            persPt::camDir = CdirSv;
            persPt::xu = CxuSv;
            persPt::yu = CyuSv;
        }// end if camera moved

}

/*
void persDoor::updateDrawFill( float dt )
{
    // update and draw fill image
    if( pRendTxt && doDraw && fractionOpenCurr > persDoor::fracOpenMin )
    {
        vec3f sep = pos - persPt::camPos;
        float sepNu = sep.dot( Nu );

        if( sepNu > 0.0f )// camera is behind door
        {
            if( updateDraw_inFront ) updateDraw_inFront( dt, *pRendTxt );
        }
        else// in front
        {
            if( updateDraw_behind ) updateDraw_behind( dt, *pRendTxt );
        }
    }
}
*/

void persDoor::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
 //   if( pVtxFill && pTxtFill && ( fractionOpenCurr >= persDoor::fracOpenMin ) ) RT.draw( pVtxFill, 4, sf::Quads, pTxtFill );
    if( pRendTxt && ( fractionOpenCurr >= persDoor::fracOpenMin ) )
    {
        if( doDrawFill ) RT.draw( pictQuad, 4, sf::Quads, &( pRendTxt->getTexture() ) );
        else RT.draw( pictQuad, 4, sf::Quads );
    }
    if( pDoorTxt ) RT.draw( doorQuad, 4, sf::Quads, pDoorTxt );
    RT.draw( frame, 5, sf::LinesStrip );
}

void persDoor::startMotion( float FractionOpen )
{
    fractionOpenCurr = fractionOpen;
    fractionOpen = FractionOpen;
    moving = true;
//    std::cout << "\n startMotion(): FractionOpen = " << FractionOpen;
}

void persDoor::set_doDrawFill( bool ddf )
{
    doDrawFill = ddf;
    sf::Color fillColor = sf::Color::White;
    if( !doDrawFill )
    {
        vec3f sep = pos - persPt::camPos;
        fillColor = ( sep.dot( Nu ) > 0.0f ) ? clearColor_inFront : clearColor_behind;
    }
    for( unsigned int n = 0; n < 4; ++n ) pictQuad[n].color = fillColor;
}

void persDoor::setOpenValue( float FractionOpen )
{
    float Pi = 3.1416f;
    // moveMode: 'S'=sliding, 'I'=swinging in, 'O'=swinging out
    vec3f Wu = wu*( 1.0f - FractionOpen ), Hu = hu*( 1.0f - FractionOpen );// if door is sliding

    vec3f nu = ( moveDirection == 'D' || moveDirection == 'R' ) ? Nu : -Nu;// sense flips
    if( moveMode == 'O' )
    {
        Wu = wu*cosf( Pi*FractionOpen ) + nu*sinf( Pi*FractionOpen );// opens out
        Hu = hu*cosf( Pi*FractionOpen ) + nu*sinf( Pi*FractionOpen );// opens out
    }
    else if( moveMode == 'I' )
    {
        Wu = wu*cosf( Pi*FractionOpen ) - nu*sinf( Pi*FractionOpen );// opens in
        Hu = hu*cosf( Pi*FractionOpen ) - nu*sinf( Pi*FractionOpen );// opens in
    }

    // 0,1 texCoords vary  2,3 positions vary
    // by direction of motion
    if( moveDirection == 'L' )// working
    {
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords = sf::Vector2f( srcRect.left + FractionOpen*Wtxt, srcRect.top + Htxt );// dn left
            doorQuad[1].texCoords = sf::Vector2f( srcRect.left + FractionOpen*Wtxt, srcRect.top );// up left
        }
        quadPos[2] = pos + H*hu + W*wu - 2.0f*W*Wu;// up right
        quadPos[3] = pos - H*hu + W*wu - 2.0f*W*Wu;// dn right

    }
    else if( moveDirection == 'R' )// working
    {
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords = sf::Vector2f( srcRect.left + Wtxt - FractionOpen*Wtxt, srcRect.top + Htxt );// dn right
            doorQuad[1].texCoords = sf::Vector2f( srcRect.left + Wtxt - FractionOpen*Wtxt, srcRect.top );// up right
        }
        quadPos[2] = pos + H*hu - W*wu + 2.0f*W*Wu;// up left
        quadPos[3] = pos - H*hu - W*wu + 2.0f*W*Wu;// dn left
    }
    else if( moveDirection == 'U' )// working
    {
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords = sf::Vector2f( srcRect.left       , srcRect.top + FractionOpen*Htxt );// up left
            doorQuad[1].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top + FractionOpen*Htxt );// up right
        }
        quadPos[2] = pos - W*wu + H*hu - 2.0f*H*Hu;// dn right
        quadPos[3] = pos + W*wu + H*hu - 2.0f*H*Hu;// dn left
    }
    else if( moveDirection == 'D' )// working
    {
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords = sf::Vector2f( srcRect.left + Wtxt, srcRect.top + Htxt - FractionOpen*Htxt );// dn right
            doorQuad[1].texCoords = sf::Vector2f( srcRect.left       , srcRect.top + Htxt - FractionOpen*Htxt );// dn left
        }
        quadPos[2] = pos + W*wu - H*hu + 2.0f*H*Hu;// up left
        quadPos[3] = pos - W*wu - H*hu + 2.0f*H*Hu;// up right
    }

    return;
}
