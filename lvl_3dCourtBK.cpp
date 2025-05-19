#include "lvl_3dCourt.h"

bool lvl_3dCourt::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
//    Level::clearColor = sf::Color(200,200,200);
 //   button::setHoverBoxColor( Level::clearColor );
    // static members
    persPt::camPos.x = persPt::camPos.y = persPt::camPos.z = 0.0f;
    persPt::camDir.x = 0.0f; persPt::camDir.y = 0.0f; persPt::camDir.z = 1.0f;
    persPt::yu.x = 0.0f; persPt::yu.y = 1.0f; persPt::yu.z = 0.0f;
    persPt::xu.x = 1.0f; persPt::xu.y = 0.0f; persPt::xu.z = 0.0f;
    persPt::angle = persPt::pitchAngle = persPt::rollAngle = 0.0f;

    std::ifstream fin("include/levels/lvl_3dCourt/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    persPt::init_stat( fin );
    persPt::camPos.x = 0.0f;//0.5f*Level::winW;
    persPt::camPos.y = 0.0f;//0.5f*Level::winH;
    persPt::camPos.z = 0.0f;//0.5f*Level::winH;

    fin >> ZboxBack >> ZboxFront;
    std::cout << "\n ZboxFront = " << ZboxFront;

    // All good now
    float halfH = 0.5f*Level::winH, halfW = 0.5f*Level::winW;
    persPt::X0 = halfW;
    persPt::Yh = halfH;
    boxCorner[0] = boxCorner[4] = vec3f( -halfW, -halfH, persPt::Z0 );// down left
    boxCorner[1] = boxCorner[5] = vec3f( -halfW, halfH, persPt::Z0 );// up left
    boxCorner[2] = boxCorner[6] = vec3f( halfW, halfH, persPt::Z0 );// up right
    boxCorner[3] = boxCorner[7] = vec3f( halfW, -halfH, persPt::Z0 );// down right
    boxCorner[4].z = boxCorner[5].z = boxCorner[6].z = boxCorner[7].z = ZboxBack;

    boxSideVtxVec.reserve(20);// 5 x sf::Quads
    boxEdgeVtxVec.reserve(12);// 6 x sf::Lines
    unsigned int rd, gn, bu;
    sf::Color sideColor;
    fin >> rd >> gn >> bu;// back
    sf::Vertex temp;
    temp.color = sf::Color(rd,gn,bu);
    temp.position = persPt::get_xyw( boxCorner[4] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[5] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[6] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[7] );
    boxSideVtxVec.push_back( temp );
    // left side
    fin >> rd >> gn >> bu;
    temp.color = sf::Color(rd,gn,bu);
    temp.position = persPt::get_xyw( boxCorner[0] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[1] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[5] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[4] );
    boxSideVtxVec.push_back( temp );
    // right side
 //   fin >> rd >> gn >> bu;
 //   temp.color = sf::Color(rd,gn,bu);
    temp.position = persPt::get_xyw( boxCorner[3] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[2] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[6] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[7] );
    boxSideVtxVec.push_back( temp );
    // bottom
    fin >> rd >> gn >> bu;
    temp.color = sf::Color(rd,gn,bu);
    temp.position = persPt::get_xyw( boxCorner[0] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[4] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[7] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[3] );
    boxSideVtxVec.push_back( temp );
    // top
    fin >> rd >> gn >> bu;
    temp.color = sf::Color(rd,gn,bu);
    temp.position = persPt::get_xyw( boxCorner[1] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[5] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[6] );
    boxSideVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[2] );
    boxSideVtxVec.push_back( temp );

    // edges
    fin >> rd >> gn >> bu;
    temp.color = sf::Color(rd,gn,bu);
    temp.position = persPt::get_xyw( boxCorner[0] );// down left
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[4] );
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[1] );// up left
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[5] );
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[3] );// down right
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[7] );
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[2] );// up right
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[6] );
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[4] );// down back
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[7] );
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[5] );// up back
    boxEdgeVtxVec.push_back( temp );
    temp.position = persPt::get_xyw( boxCorner[6] );
    boxEdgeVtxVec.push_back( temp );

    // scoring
    scoreMsg.setFont( *button::pFont );
    float posX, posY, ofstX; fin >> posX >> posY >> ofstX;// ofstY is to NumMsg from posY
    unsigned int charSz; fin >> charSz;
    scoreMsg.setCharacterSize(charSz);
    fin >> rd >> gn >> bu;
    scoreMsg.setFillColor( sf::Color(rd,gn,bu) );
    scoreNumMsg = scoreMsg;
    scoreMsg.setPosition( posX, posY );
    scoreMsg.setString( "SCORE" );
    scoreNumMsg.setPosition( posX + ofstX, posY );
    scoreNumMsg.setString( "0" );


    ball.init( fin );
    ballSpeed = ball.vel.z;
    ball.isMoving = true;

    // ball path
    float Rdot; fin >> Rdot;
    fin >> rd >> gn >> bu;
    pathDot.setRadius( Rdot );
    pathDot.setOrigin( Rdot, Rdot );
    pathDot.setFillColor( sf::Color(rd,gn,bu) );
    ballPath[0].color = ballPath[1].color = ballPath[2].color = sf::Color(rd,gn,bu);

    // aiming
    aimPath[0].color = aimPath[1].color = sf::Color::Red;
    float aimR = 4.0f;
    aimDot.setRadius( aimR );
    aimDot.setOrigin( aimR, aimR );
    aimDot.setFillColor( sf::Color::Red );

    if( !initPaddle( fin ) ){ std::cout << "\n paddle init fail"; return false; }

    persBlockGrid::pFont = button::pFont;
    persBlockGrid::pBall = &ball;

    std::string fName; fin >> fName;
    std::ifstream fin2( fName.c_str() );
    if( !init_grids( fin2 ) ){ std::cout << "\n init_grids fail"; return false; }
    fin2.close();
    pGridA = &gridVec.front();

    fin >> fName;
    fin2.open( fName.c_str() );
    if( !init_controls( fin2 ) ){ std::cout << "\n init_controls fail"; return false; }

    pPersSorted.reserve( 1 + gridVec.size() );// plus the ball
    pPersSorted.push_back( &ball );
    for( persBlockGrid& PBG : gridVec )
        pPersSorted.push_back( &PBG );

    return true;
}

bool lvl_3dCourt::hitBox()
{
    // exit left
    if( ball.pos.x < boxCorner[0].x + ball.Rbound && ball.vel.x < 0.0f )
    {
        ball.vel.x *= -1.0f;
        ball.pos.x = boxCorner[0].x + ball.Rbound;
        return true;
    }
    // exit right
    if( ball.pos.x > boxCorner[3].x - ball.Rbound && ball.vel.x > 0.0f )
    {
        ball.vel.x *= -1.0f;
        ball.pos.x = boxCorner[3].x - ball.Rbound;
        return true;
    }
    // exit bottom
    if( ball.pos.y < boxCorner[0].y + ball.Rbound && ball.vel.y < 0.0f )
    {
        ball.vel.y *= -1.0f;
        ball.pos.y = boxCorner[0].y + ball.Rbound;
        return true;
    }
    // exit top
    if( ball.pos.y > boxCorner[1].y - ball.Rbound && ball.vel.y > 0.0f )
    {
        ball.vel.y *= -1.0f;
        ball.pos.y = boxCorner[1].y - ball.Rbound;
        return true;
    }
    // exit front
    if( ball.pos.z < ZboxFront && ball.vel.z < 0.0f )
    {
        ball.vel.z *= -1.0f;
        ball.pos.z = ZboxFront;
        showDot = false;
        return true;
    }
    // exit back
    if( ball.pos.z > ZboxBack - ball.Rbound && ball.vel.z > 0.0f )
    {
        ball.vel.z *= -1.0f;
        ball.pos.z = ZboxBack - ball.Rbound;
        return true;
    }

    return false;
}

// intersecting box wall
vec3f lvl_3dCourt::findAimPt( vec3f P0, vec3f Vel, vec3f& Nu )const// writes new Vel and Pos = hitPt
{
    float Rb = ball.Rbound;
    // time to back
    float tz = ( ZboxBack - Rb - P0.z )/Vel.z;
    // time to side
    float tx = 0.0f;
    if( Vel.x < 0.0f ) tx = ( boxCorner[0].x + Rb - P0.x )/Vel.x;
    else tx = ( boxCorner[3].x - Rb - P0.x )/Vel.x;
    // time to bottom or top
    float ty = 0.0f;
    if( Vel.y < 0.0f ) ty = ( boxCorner[0].y + Rb - P0.y )/Vel.y;
    else ty = ( boxCorner[1].y - Rb - P0.y )/Vel.y;
    // find least t
    bool tzLess = tz < tx;
    float t = tz < tx ? tz : tx;

    bool tyLeast = ty < t;
    t = ty < t ? ty : t;// t is least value

    if( tyLeast ) Nu = (Vel.y < 0.0f) ? persPt::yu : -persPt::yu;
    else if( tzLess ) Nu = (Vel.z < 0.0f) ? persPt::camDir : -persPt::camDir;
    else Nu = (Vel.x < 0.0f) ? persPt::xu : -persPt::xu;

    return P0 + Vel*t;
}

// true if HitPt at paddlePos.z
bool lvl_3dCourt::findHitPt( vec3f& Pos, vec3f& Vel )const// writes new Vel and Pos = hitPt
{
    if( Vel.z > 0.0f ) return false;

    // project to paddle plane first
    float t = ( paddlePos.z - Pos.z )/Vel.z;
    vec3f HitPt = Pos + t*Vel;
    bool retVal = true;// until false below
    char bounceDir = 'N';// none

    // check if left of box
    if( HitPt.x < boxCorner[0].x + ball.Rbound )
    {
        t = ( boxCorner[0].x + ball.Rbound - Pos.x )/Vel.x;
        HitPt = Pos + t*Vel;// to left wall
        retVal = false;
        bounceDir = 'H';
    }
    else if( HitPt.x > boxCorner[3].x - ball.Rbound )
    {
        t = ( boxCorner[3].x - ball.Rbound - Pos.x )/Vel.x;
        HitPt = Pos + t*Vel;// to right wall
        retVal = false;
        bounceDir = 'H';
    }

    // check if it goes through the floor or ceiling 1st
    if( HitPt.y < boxCorner[0].y + ball.Rbound )
    {
        t = ( boxCorner[0].y + ball.Rbound - Pos.y )/Vel.y;
        HitPt = Pos + t*Vel;// to floor
        retVal = false;
        bounceDir = 'V';
    }
    else if( HitPt.y > boxCorner[1].y - ball.Rbound )//  or ceiling
    {
        t = ( boxCorner[1].y - ball.Rbound - Pos.y )/Vel.y;
        HitPt = Pos + t*Vel;// to ceiling
        retVal = false;
        bounceDir = 'V';
    }

    if( bounceDir == 'H' ) Vel.x *= -1.0f;
    else if( bounceDir == 'V' ) Vel.y *= -1.0f;
    Pos = HitPt;
    return retVal;
}

void lvl_3dCourt::assignPath()// call when ball hits a wall
{
    if( ball.vel.z > 0.0f ) return;

    vec3f Vel = ball.vel, Pos = ball.pos;
    ballPath[0].position = persPt::get_xyw( Pos );
    showDot = findHitPt( Pos, Vel );
    ballPath[1].position = persPt::get_xyw( Pos );

    if( !showDot )
    {
        showDot = findHitPt( Pos, Vel );
        ballPath[2].position = persPt::get_xyw( Pos );

    }
    else ballPath[2].position = ballPath[1].position;

    pathDot.setPosition( ballPath[2].position );


    /*
    // project to paddle plane first
    float t = ( paddlePos.z - ball.pos.z )/ball.vel.z;
    hitPt = ball.pos + t*ball.vel;
    showDot = true;// until false below
    ballPath[0].position = persPt::get_xyw( ball.pos );
    ballPath[1].position = persPt::get_xyw( hitPt );

    // check if left of box
    if( hitPt.x < boxCorner[0].x + ball.Rbound )
    {
        t = ( boxCorner[0].x + ball.Rbound - ball.pos.x )/ball.vel.x;
        hitPt = ball.pos + t*ball.vel;// to left wall
        showDot = false;
        ballPath[1].position = persPt::get_xyw( hitPt );
    }
    else if( hitPt.x > boxCorner[3].x - ball.Rbound )
    {
        t = ( boxCorner[3].x - ball.Rbound - ball.pos.x )/ball.vel.x;
        hitPt = ball.pos + t*ball.vel;// to right wall
        showDot = false;
        ballPath[1].position = persPt::get_xyw( hitPt );
    }

    // check if it goes through the floor or ceiling 1st
    if( hitPt.y < boxCorner[0].y + ball.Rbound )
    {
        t = ( boxCorner[0].y + ball.Rbound - ball.pos.y )/ball.vel.y;
        hitPt = ball.pos + t*ball.vel;// to floor
        showDot = false;
        ballPath[1].position = persPt::get_xyw( hitPt );
    }
    else if( hitPt.y > boxCorner[1].y - ball.Rbound )//  or ceiling
    {
        t = ( boxCorner[1].y - ball.Rbound - ball.pos.y )/ball.vel.y;
        hitPt = ball.pos + t*ball.vel;// to ceiling
        showDot = false;
        ballPath[1].position = persPt::get_xyw( hitPt );
    }

    if( showDot )
    {
        pathDot.setPosition( ballPath[1].position );
    }
    */
}

void lvl_3dCourt::updatePath()
{
    if( ball.vel.z > 0.0f ) return;
    ballPath[0].position = persPt::get_xyw( ball.pos );
}

bool lvl_3dCourt::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if( rEvent.key.code == sf::Keyboard::Space )
        {
            fireKeyDown = true;
            ball.inUse = false;
            showDot = false;
            numToDraw = 0;// z order once while key down
            for( persBlockGrid& PBG : gridVec )
                if( PBG.doDraw && numToDraw < pPersSorted.size() )
                    pPersSorted[ numToDraw++ ] = &PBG;

            pPersSorted[ numToDraw++ ] = &ball;// last
        }
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if( rEvent.key.code == sf::Keyboard::Space )
        {
            fireKeyDown = false;
            ball.setPosition( paddlePos + ball.Rbound*Zup );
            ball.vel = ballSpeed*Zup;
            ball.inUse = true;
        }
        else if( rEvent.key.code == sf::Keyboard::R )
        {
            if( pGridA )
            {
                for( size_t j = 0; j < pGridA->doDrawCell.size(); ++j )
                    pGridA->doDrawCell[j] = true;

                pGridA->updateGrid();
                Score = 0;
                scoreNumMsg.setString("0");
            }
        }
    }

    return true;
}

void lvl_3dCourt::update( float dt )
{
    if( ball.inUse )
    {
        static vec3f ballPosLast = ball.pos;
        ballPosLast = ball.pos;
        ball.update(dt);
        if( hitBox() )
        {
            assignPath();
        }
        else if( hitPaddle( ballPosLast ) )
        {
            showDot = false;
        }

        // hit test gridA
        vec3f P, vu;
    //    vec3f Vel = ball.vel;
    //    if( gridA.hit( ballPosLast, ball.pos, P, Vel ) )
     //   {
     //       ball.setPosition( P );
      //      ball.vel = Vel;
      //  }

        ballInGrid = false;
        // try taking out cells!

        for( persBlockGrid& PBG : gridVec )
            if( PBG.hitBlock( ballPosLast, doKnockOut ) )
            {
                Score += 100;
                to_SF_string( scoreNumMsg, Score );
                ballInGrid = true;
                break;
            }
    }

    updatePath();
    updatePaddle();

    // z order
    if( fireKeyDown )
    {
    //    numToDraw = 0;
    //    for( persBlockGrid& PBG : gridVec )
     //       if( PBG.doDraw && numToDraw < pPersSorted.size() )
     //           pPersSorted[ numToDraw++ ] = &PBG;
    }
    else
    {
        numToDraw = 0;
        for( persBlockGrid& PBG : gridVec )
            if( PBG.doDraw && numToDraw < pPersSorted.size() )
                pPersSorted[ numToDraw++ ] = &PBG;

        if( !ballInGrid && ball.inUse && ball.doDraw && numToDraw < pPersSorted.size() ) pPersSorted[ numToDraw++ ] = &ball;

        if( numToDraw < pPersSorted.size() + 1 )
            std::sort( pPersSorted.begin(), pPersSorted.begin() + numToDraw, persPt::compare );
    }
}

void lvl_3dCourt::updatePaddle()
{
    // paddle hit color timeout
    if( hitTime > 0 && --hitTime == 0 )
        for( sf::Vertex& V : fanVec ) V.color = paddleColor;

    // aiming with the paddle
    if( fireKeyDown )
    {
        aimPath[0].position = persPt::get_xyw( paddlePos );
        vec3f aimPt, Nu;
        bool foundAim = false;

        for( persBlockGrid& PBG : gridVec )
            if( PBG.lineIntersects( paddlePos, Zup, aimPt, Nu ) )
            {
                aimPath[1].position = persPt::get_xyw( aimPt );
                aimDot.setPosition( aimPath[1].position );
                aimDot.setFillColor( sf::Color::Red );
                foundAim = true;
                break;
            }

        if( !foundAim )
        {
            aimPt = findAimPt( paddlePos, Zup, Nu );
            aimPath[1].position = persPt::get_xyw( aimPt );
            aimDot.setPosition( aimPath[1].position );
            aimDot.setFillColor( sf::Color::Blue );
        }
    }

    // mouse events
    sf::Vector2f dPos = button::msePos() - fanVec[0].position;
    float dPosMag = sqrtf( dPos.x*dPos.x + dPos.y*dPos.y );
    if( !button::pButtMse && ( dPosMag < paddleR ) )// paddle hit
    {
        if( button::clickEvent_Rt() == 1 )
        {
            paddleHeld = !paddleHeld;
            if( !paddleHeld )
            {
                constructAxes( vec3f(0.0f,0.0f,1.0f) );
                graphPaddle();
            }
        }
    }

    // paddle usage
    if( paddleHeld )
    {
        if( button::mseDnLt )// apply yaw and pitch
        {
            // working
            sf::Vector2f dPos = button::msePos() - fanVec[0].position;
            float dPosMag = sqrtf( dPos.x*dPos.x + dPos.y*dPos.y );
            vec3f zup( Kp*dPos.x/paddleR, -Kp*dPos.y/paddleR, 0.0f );
            if( dPosMag < 2.0f*paddleR ) zup.z = sqrtf( 1.0f - dPosMag/(2.0f*paddleR) );
            zup /= zup.mag();
            constructAxes( zup );// from givenZup
        }
        else
        {
            if( button::clickEvent_Lt() == -1 )// release
                constructAxes( vec3f(0.0f,0.0f,1.0f) );

            paddlePos.x = button::mseX - 0.5f*Level::winW;
            paddlePos.y = 0.5f*Level::winH - button::mseY;
        }
        graphPaddle();
    }
    else if( pGridA && button::clickEvent_Lt() == 1 )// click on cell
    {
        unsigned int hitRow = 0, hitCol = 0;
        if( pGridA->hitCellImage( button::msePos(), hitRow, hitCol ) )
        {
            unsigned int idx0 = hitCol + hitRow*pGridA->Cols;
            // check upward through layers for 1st still drawn
            for( unsigned int L = 0; L < pGridA->Layers; ++L )
            {
                unsigned int idx = idx0 + pGridA->Rows*pGridA->Cols*L;
                if( pGridA->doDrawCell[idx] )
                {
                    pGridA->doDrawCell[idx] = false;// no draw
                    pGridA->updateGrid();
                    break;
                }
            }
        }
    }
}

void lvl_3dCourt::draw( sf::RenderTarget& RT ) const
{
    RT.draw( &( boxSideVtxVec[0] ), boxSideVtxVec.size(), sf::Quads );
    RT.draw( &( boxEdgeVtxVec[0] ), boxEdgeVtxVec.size(), sf::Lines );

    if( showDot && ball.vel.z < 0.0f && ball.pos.z > paddlePos.z ) RT.draw( ballPath, 3, sf::LinesStrip );

    // aim point
    if( fireKeyDown )
    {
        for( const persPt* pPers : pPersSorted )
            if( pPers )
            {
                if( pPers == &ball )
                {
                    RT.draw( aimPath, 2, sf::Lines );
                    RT.draw( aimDot );
                    continue;
                }

                pPers->draw(RT);
            }

        drawPaddle(RT);
    }
    else// ball is in play
    {
        if( numToDraw > 0 && numToDraw < pPersSorted.size() + 1 )
            for( unsigned int j = 0; j < numToDraw; ++j )
                if( pPersSorted[j] ) pPersSorted[j]->draw(RT);// may not include the ball

        // z order ball vs paddle
        drawPaddle(RT);

        if( ball.pos.z < paddlePos.z )
            ball.draw(RT);// re draw
        else if( showDot )
            RT.draw( pathDot );
    }

    if( doKnockOut )
    {
        RT.draw( scoreMsg );
        RT.draw( scoreNumMsg );
    }
}

/*
void lvl_3dCourt::draw( sf::RenderTarget& RT ) const
{
    RT.draw( &( boxSideVtxVec[0] ), boxSideVtxVec.size(), sf::Quads );
    RT.draw( &( boxEdgeVtxVec[0] ), boxEdgeVtxVec.size(), sf::Lines );

    if( ball.vel.z < 0.0f && ball.pos.z > paddlePos.z ) RT.draw( ballPath, 3, sf::LinesStrip );

 //   if( ball.pos.z < gridA.pos.z + 0.04f*ballSpeed ) gridA.draw(RT);// gridA before ball - allow some penetration
    if( ball.pos.z < gridA.pos.z + gridA.Layers*gridA.cellD ) gridA.draw(RT);// gridA before ball - allow some penetration
 //   else std::cout << "\n not before";

    for( const persBlockGrid& PBG : gridVec ) PBG.draw(RT);

    if( ball.pos.z > paddlePos.z )
    {
    //    ball.draw(RT);
        if( !gridA.doDrawBall ) ball.draw(RT);
        if( ball.pos.z > gridA.pos.z + gridA.Layers*gridA.cellD ){ gridA.draw(RT); };// std::cout << "\n after"; }// gridA after ball
        drawPaddle(RT);
    }
    else
    {
        gridA.draw(RT);// gridA before paddle
        drawPaddle(RT);
        ball.draw(RT);
    }

    if( showDot ) RT.draw( pathDot );
    if( doKnockOut )
    {
        RT.draw( scoreMsg );
        RT.draw( scoreNumMsg );
    }
}
*/

// paddle functions
bool lvl_3dCourt::initPaddle( std::istream& is )
{
    is >> paddleR >> paddlePos.x >> paddlePos.y;
    paddlePos.z = persPt::Z0;
    is >> numPoints;

    fanVec.resize( numPoints + 2 );
    paddleColor = sf::Color::Magenta;
    paddleColor.a = 100;
    paddleColorHit = sf::Color(0,200,50,100);
    for( sf::Vertex& V : fanVec ) V.color = paddleColor;
    perimVec.resize( numPoints + 1 );
    for( sf::Vertex& V : perimVec ) V.color = sf::Color::Blue;
    ZupLine[0].color = ZupLine[1].color = sf::Color::Black;
    constructAxes( vec3f(0.0f,0.0f,1.0f) );
    graphPaddle();

    return true;
}

void lvl_3dCourt::constructAxes( vec3f Zu_p )// from givenZup
{
    Zup = Zu_p;
    Xup = Zup.cross( persPt::yu );
    Xup /= Xup.mag();
    Yup = Xup.cross( Zup );
}

void lvl_3dCourt::graphPaddle()
{
   float dAngle = 2.0f*vec2f::PI/ numPoints;
    fanVec[0].position = persPt::get_xyw( paddlePos );

    for( unsigned int j = 0; j < numPoints; ++j )
    {
        float angle = j*dAngle;
        vec3f rp = paddleR*( cosf( angle )*Xup + sinf( angle )*Yup );
        perimVec[j].position = fanVec[j+1].position = persPt::get_xyw( paddlePos + rp );
    }

    fanVec[ numPoints + 1 ] = fanVec[1];// closing fan
    perimVec[ numPoints ].position = fanVec[1].position;
    // ZupLine
    ZupLine[0].position = fanVec[0].position;
    ZupLine[1].position = persPt::get_xyw( paddlePos + Zup*ZupLineLength );

}
void lvl_3dCourt::drawPaddle( sf::RenderTarget& RT ) const
{
    RT.draw( ZupLine, 2, sf::Lines );
    RT.draw( &( fanVec[0] ), fanVec.size(), sf::TriangleFan );
    RT.draw( &( perimVec[0] ), perimVec.size(), sf::LinesStrip );
}

bool lvl_3dCourt::hitPaddle( vec3f posLast )
{
    if( ball.vel.z > 0.0f ) return false;// only coming towards front of paddle

    vec3f dPos = ball.pos - paddlePos;
    vec3f dPosLast = posLast - paddlePos;
    if( dPosLast.dot( Zup ) > 0.0f && dPos.dot( Zup ) < 0.0f )// crossed
    {
        // find crossing point
     //   std::cout << "\n crossed!";
        float s = dPos.dot( Zup )/ball.vel.dot( Zup );
        vec3f P = dPos - s*ball.vel;
        // Good now
        if( P.mag() > paddleR ) return false;
        ball.setPosition( P + paddlePos );
        ball.vel -= 2.0f*( ball.vel.dot( Zup ) )*Zup;
        showDot = false;
        hitTime = hitTimeLimit;
        for( sf::Vertex& V : fanVec ) V.color = paddleColorHit;
        return true;
    }

    return false;
}

bool lvl_3dCourt::init_grids( std::istream& is )
{
    if( !is.good() ) return false;

    size_t numGrids; is >> numGrids;
    if( numGrids == 0 ) return true;

    gridVec.reserve( numGrids );
    for( size_t j = 0; j < numGrids && is.good(); ++j )
    {
        gridVec.push_back( persBlockGrid() );
        gridVec.back().init(is);
    }

    return true;
}

bool lvl_3dCourt::init_controls( std::istream& is )
{
    if( !is.good() ) return false;

    sf::Vector2f HBpos, HBsz, sfcOfst;
    is >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;

    sf::Vector2f PosSurf( HBpos + sfcOfst );
    multiCS.init( HBpos, PosSurf, SzSurf, "multi", HBsz.x, HBsz.y );
    button::RegisteredButtVec.push_back( &multiCS );
    multiCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    multiCS.pButtVec.push_back( pMS );

    if( pMS->sdataVec.size() != 7 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // ballSpeed
    pSD->pSetFunc =     [this](float x){ ballSpeed = x; float vMag = ball.vel.mag(); ball.vel *= x/vMag; };
    ballSpeed = pSD->xCurr;
    pStrip->reInit( *pSD );
    // paddle sensitivity
    (++pSD)->pSetFunc = [this](float x){ Kp = x; };
    Kp = pSD->xInit;
    // gridA.Zu pitch
    (++pSD)->pSetFunc = [this](float pitch)
    {
        if( !pGridA ) return;
        gridPitch = pitch*vec2f::PI/180.0f;// degrees to radians
        vec3f v = cosf(gridPitch)*sinf(gridYaw)*persPt::xu + sinf(gridPitch)*persPt::yu
        + cosf(gridPitch)*cosf(gridYaw)*persPt::camDir;
        v /= v.mag();
        pGridA->constructAxes(v);
        pGridA->updateGrid();
    };
    gridPitch = pSD->xInit*vec2f::PI/180.0f;
    // gridA.Zu yaw
    (++pSD)->pSetFunc = [this](float yaw)
    {
        if( !pGridA ) return;
        gridYaw = yaw*vec2f::PI/180.0f;// degrees to radians
        vec3f v = cosf(gridPitch)*sinf(gridYaw)*persPt::xu + sinf(gridPitch)*persPt::yu
        + cosf(gridPitch)*cosf(gridYaw)*persPt::camDir;
        v /= v.mag();
        pGridA->constructAxes(v);
        pGridA->updateGrid();
    };
    gridYaw = pSD->xInit*vec2f::PI/180.0f;
    // gridA.pos.x
    (++pSD)->pSetFunc = [this](float x){ if( pGridA ) pGridA->setPosition( vec3f( x, pGridA->pos.y, pGridA->pos.z ) ); };
    if( pGridA ) pSD->xInit = pGridA->pos.x;
    // gridA.pos.y
    (++pSD)->pSetFunc = [this](float y){ if( pGridA ) pGridA->setPosition( vec3f( pGridA->pos.x, y, pGridA->pos.z ) ); };
    if( pGridA ) pSD->xInit = pGridA->pos.y;
    // gridA.pos.z
    (++pSD)->pSetFunc = [this](float z){ if( pGridA ) pGridA->setPosition( vec3f( pGridA->pos.x, pGridA->pos.y, z ) ); };
    if( pGridA ) pSD->xInit = pGridA->pos.z;


    // a "doKnockOut toggle" button
    sf::Vector2f Pos, Sz; is >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y;
    HBpos += multiCS.bkRect.getPosition();
    buttonRect* pBR = new buttonRect( HBpos.x, HBpos.y, HBsz.x, HBsz.y, nullptr, "KO_cells" );
    pBR->pHitFunc = [this,pBR]() { doKnockOut = pBR->sel; Score = 0; scoreNumMsg.setString("0"); };
    multiCS.pButtVec.push_back( pBR );

    return true;
}
