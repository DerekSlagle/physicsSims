#include "persDoor.h"


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

 //   Wtxt = static_cast<float>( pDoorTxt->getSize().x );
 //   Htxt = static_cast<float>( pDoorTxt->getSize().y );// width and height of texture
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

/*
// utility for user - positions will be assigned in a clockwise direction from upper left
void persDoor::get_quadMap( vec3f** pQuadPos )
{
    if(  moveDirection == 'L' )// quadPos are CW from dn left
    {
    //    pQuadPos[0] = &quadPos[2];
        pQuadPos[0] = &framePos[1];
        pQuadPos[1] = &framePos[2];
        pQuadPos[2] = &framePos[3];
        pQuadPos[3] = &framePos[0];
     //   pQuadPos[3] = &quadPos[3];
    }
    else if(  moveDirection == 'R' )// quadPos are CCW from dn right
    {
        pQuadPos[0] = &framePos[2];
        pQuadPos[1] = &quadPos[2];
        pQuadPos[2] = &quadPos[3];
        pQuadPos[3] = &framePos[3];
    }
    else if(  moveDirection == 'U' )// quadPos are CW from up left, same as pQuadPos
    {
        pQuadPos[0] = &quadPos[3];
        pQuadPos[1] = &quadPos[2];
        pQuadPos[2] = &framePos[2];
        pQuadPos[3] = &framePos[3];
    }
    else if(  moveDirection == 'D' )// quadPos are CW from dn right
    {
        pQuadPos[0] = &framePos[2];
        pQuadPos[1] = &framePos[3];
        pQuadPos[2] = &quadPos[3];
        pQuadPos[3] = &quadPos[2];
    }
}


void persDoor::initFill( std::function<void(float)> update_OutFromIn, std::function<void(sf::RenderTarget&)> draw_OutFromIn,
                  std::function<void(float)> update_InFromOut, std::function<void(sf::RenderTarget&)> draw_InFromOut )
{
    // imagery from inside looking out
    updateOutFromIn = update_OutFromIn;// from level
    drawOutFromIn = draw_OutFromIn;
    // imagery from outside looking in
    updateInFromOut = update_InFromOut;// room may supply
    drawInFromOut = draw_InFromOut;

    // one good pair is enough to create the canvas
    if( ( updateOutFromIn && drawOutFromIn ) || ( update_InFromOut && draw_InFromOut ) )
    {
   //     get_quadMap( pCorner );
        // assign colors
        for( unsigned int n = 0; n < 4; ++n ) pictQuad[n].color = sf::Color::White;
        // assign pt positions
   //     for( unsigned int n = 0; n < 4; ++n ) pictQuad[n].position = persPt::get_xyw( *pCorner[n] );

         pictQuad[0].position = persPt::get_xyw( pos + W*wu + H*hu  );
        pictQuad[1].position = persPt::get_xyw( pos - W*wu + H*hu  );
        pictQuad[2].position = persPt::get_xyw( pos - W*wu - H*hu  );
        pictQuad[3].position = persPt::get_xyw( pos + W*wu - H*hu  );

        pictQuad[0].texCoords.x = 1.0f;// up left
        pictQuad[0].texCoords.y = 1.0f;
        pictQuad[1].texCoords.x = 2.0f*W - 1.0f;// up right
        pictQuad[1].texCoords.y = 1.0f;
        pictQuad[2].texCoords.x = 2.0f*W - 1.0f;// dn right
        pictQuad[2].texCoords.y = 2.0f*H - 1.0f;// dn right
        pictQuad[3].texCoords.x = 1.0f;// dn left
        pictQuad[3].texCoords.y = 2.0f*H - 1.0f;

  //      unsigned int pictW = static_cast<unsigned int>( 2.0f*W );
   //     unsigned int pictH = static_cast<unsigned int>( 2.0f*H );
  //      picture.create( pictW, pictH );
     //   sf::View pictView( sf::Vector2f( persPt::X0, persPt::Yh ), sf::Vector2f(  2.0f*W,  2.0f*H ) );// center pos, size
    //    picture.setView( pictView );
    }

}



void persDoor::update_fill( float dt )
{
    static vec3f camPosLast = persPt::camPos;

    if( doDraw && fractionOpenCurr > 0.03f )
    {
        vec3f dPos = persPt::camPos - camPosLast;
        vec3f sep = pos - persPt::camPos;

        if( dPos.dot( dPos ) > 0.1f )// motion detection
        {
            // adjust the view
       //     sf::View pictView( picture.getView() );

            sf::Vector2f newCenter;
            float U = sep.dot( Nu );
            if( U < 0.0f ) U *= -1.0f;
            newCenter.x = persPt::X0 - sep.dot( wu );//*persPt::Z0/U;
            newCenter.y = persPt::Yh + sep.dot( hu );//*persPt::Z0/U;

        //    pictView.setCenter( newCenter );
       //     picture.setView( pictView );
        }

        vec3f CxuSv = persPt::xu;
        vec3f CyuSv = persPt::yu;
        vec3f CdirSv = persPt::camDir;


        doDrawFill = false;// unless assigned true below
        if( ( sep.dot( Nu ) < 0.0f ) && updateInFromOut && drawInFromOut )// camera is outside of room
        {
            doDrawFill = true;
            pictQuad[0].position = persPt::get_xyw( pos + W*wu + H*hu  );
            pictQuad[1].position = persPt::get_xyw( pos - W*wu + H*hu  );
            pictQuad[2].position = persPt::get_xyw( pos - W*wu - H*hu  );
            pictQuad[3].position = persPt::get_xyw( pos + W*wu - H*hu  );

            // camera basis = level view straight through doorway
            persPt::camDir = -Nu;
            persPt::xu = persPt::yHat.cross( persPt::camDir );
            persPt::xu /= persPt::xu.mag();
            persPt::yu = persPt::camDir.cross( persPt::xu );
            // update
            updateInFromOut(dt);
            // restore basis
            persPt::camDir = CdirSv;// early?
            persPt::xu = CxuSv;
            persPt::yu = CyuSv;
            // draw
      //      picture.clear( sf::Color(0,0,0) );
      //      drawInFromOut( picture );
      //      picture.display();
        }
        else if( ( sep.dot( Nu ) > 0.0f ) && updateOutFromIn && drawOutFromIn )// camera is inside of room
        {
            doDrawFill = true;
            pictQuad[1].position = persPt::get_xyw( pos + W*wu + H*hu  );
            pictQuad[0].position = persPt::get_xyw( pos - W*wu + H*hu  );
            pictQuad[3].position = persPt::get_xyw( pos - W*wu - H*hu  );
            pictQuad[2].position = persPt::get_xyw( pos + W*wu - H*hu  );

            // camera basis = level view straight through doorway
            persPt::camDir = -Nu;
            persPt::xu = persPt::yHat.cross( persPt::camDir );
            persPt::xu /= persPt::xu.mag();
            persPt::yu = persPt::camDir.cross( persPt::xu );
            // update
            updateOutFromIn(dt);
            // restore basis
            persPt::camDir = CdirSv;// early?
            persPt::xu = CxuSv;
            persPt::yu = CyuSv;
            // draw
      //      picture.clear( sf::Color(0,0,0) );
       //     drawOutFromIn( picture );
      //      picture.display();

      //      persPt::camDir = sep/sep.mag();
       //     persPt::camDir = Nu;
       //     updateOutFromIn(dt);
        //    picture.clear( sf::Color(0,0,0) );
       //     drawOutFromIn( picture );
        //    picture.display();
        //    persPt::camDir = CdirSv;
       //     doDrawFill = true;
        }
    }

    camPosLast = persPt::camPos;
}
*/

void persDoor::setPosition( vec3f  Pos )
{

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

    // the fill image
 //   update_fill(dt);

    // update_doDraw
    doDraw = true;
    // becomes false if any point is behind the camera
    for( unsigned int j = 0; j < 4; ++j )
        if( ( framePos[j] - persPt::camPos ).dot( persPt::camDir ) < 0.0f ) { doDraw = false; return; }
}

void persDoor::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
 //   if( doDrawFill && (drawInFromOut || drawOutFromIn) && (fractionOpenCurr > 0.03f) ) RT.draw( pictQuad, 4, sf::Quads, &( picture.getTexture() ) );
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

/*
void persDoor::get_opening( vec3f* cPos )
{
    if( moveDirection == 'L' )
    {

        cPos[0] = framePos[1];// up left
        cPos[1] = framePos[2];// up right
        cPos[2] = framePos[3];// dn right
        cPos[3] = framePos[0];// dn left
    }
}
*/
/*
// check sense of corner assignments
    vec3f Vc0 = *pCorner[0] - persPt::camPos;
    vec3f Vc1 = *pCorner[1] - persPt::camPos;
    if( ( Vc0.cross(Vc1) ).dot( persPt::yHat ) < 0.0f )// wrong way!
    {
        // swap pCorner[0] with pCorner[1]
        vec3f* temp = pCorner[0];
        pCorner[0] = pCorner[1];
        pCorner[1] = temp;
        // swap pCorner[2] with pCorner[3]
        temp = pCorner[2];
        pCorner[2] = pCorner[3];
        pCorner[3] = temp;
        std::cout << "\n OutFromIn corner flip";
    }
*/
