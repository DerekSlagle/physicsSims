#include "lvl_perspective2.h"

bool lvl_perspective2::init()
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

    std::ifstream fin("include/levels/lvl_perspective2/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    persPt::init_stat( fin );
    fin >> persPt::angle >> persPt::pitchAngle;// in degrees
    persPt::angle *= vec2f::PI/180.0f;// to radians
    persPt::pitchAngle *= vec2f::PI/180.0f;
    float compassAngle = persPt::changeCamDir( persPt::angle, persPt::pitchAngle, 0.0f );

    fin >> pipLength;// for view direction cross
    persPt::X0 = Level::winW/2.0f; persPt::Yh = Level::winH/2.0f;
    fin >> camVelXZscale;
    fin >> camSpeed >> yawRate >> pitchRate >> rollRate;

    viewCross[0].position.y = viewCross[1].position.y = persPt::Yh;// horizontal
    viewCross[0].position.x = persPt::X0 - pipLength;
    viewCross[1].position.x = persPt::X0 + pipLength;
    viewCross[2].position.x = viewCross[3].position.x = persPt::X0;// vertical
    viewCross[2].position.y = persPt::Yh - pipLength;
    viewCross[3].position.y = persPt::Yh + pipLength;

    trueHorizon[0].color = trueHorizon[1].color = sf::Color::Blue;
    trueHorizon[0].position.y = trueHorizon[1].position.y = persPt::Yh;
    trueHorizon[0].position.x = 0.0f;
    trueHorizon[1].position.x = 2.0f*persPt::X0;

    // window panes
    unsigned int rd, gn, bu;
    fin >> rd >> gn >> bu;
    Level::clearColor = sf::Color(rd,gn,bu);
    button::setHoverBoxColor( Level::clearColor );
    fin >> rd >> gn >> bu;
    for( size_t i = 0; i < 4; ++i ) upperPane[i].color = sf::Color(rd,gn,bu);
    upperPane[0].position.x = 0.0f;        upperPane[0].position.y = 0.0f;// upper left
    upperPane[1].position.x = Level::winW; upperPane[1].position.y = 0.0f;// upper right
    updatePane();// assigns remaining positions

    // messages
    msgVec.reserve(4);
    msgNumVec.reserve(4);

    sf::Text msg( "posX", *button::pFont, 14 );
    msg.setFillColor( sf::Color::Black );
    float posX, posY, offX, offY;
    fin >> posX >> posY >> offX >> offY;
    msg.setPosition( posX, posY );
    msgVec.push_back( msg );
    msg.setString( "" );
    msg.setPosition( posX + offX, posY );
    msgNumVec.push_back( msg );

    msg.setString( "posY" );
    msg.setPosition( posX, posY + offY );
    msgVec.push_back( msg );
    msg.setString( "" );
    msg.setPosition( posX + offX, posY + offY );
    msgNumVec.push_back( msg );

    msg.setString( "posZ" );
    msg.setPosition( posX, posY + 2.0f*offY );
    msgVec.push_back( msg );
    msg.setString( "" );
    msg.setPosition( posX + offX, posY + 2.0f*offY );
    msgNumVec.push_back( msg );

    msg.setString( "Heading" );
    msg.setPosition( posX, posY + 3.0f*offY );
    msgVec.push_back( msg );
  //  msg.setString( "" );
    to_SF_string( msg, compassAngle );
    msg.setPosition( posX + offX, posY + 3.0f*offY );
    msgNumVec.push_back( msg );

    msg.setString( "Shots:" );
    msg.setPosition( posX + 3.0f*offX, posY );
    msgVec.push_back( msg );
    numShotsMsg = msg;
    numShotsMsg.setString( "0" );
    numShotsMsg.setPosition( posX + 4.0f*offX, posY );

    // aimed firing
    initTargetingMessages( fin );

    fin >> rd >> gn >> bu;
    sf::Color crossColor(rd,gn,bu);// cross
    for( unsigned int n = 0; n < 4; ++n ) viewCross[n].color = crossColor;

    std::string fileName; fin >> fileName;
//    if( !init_controls( fileName ) ) return false;// 2xcout + init_controls() end
//    fin >> fileName;
//    if( !loadSpriteSheets( fileName ) ) return false;// std::cout << "\n#SS = " << SSvec.size();
    if( !spriteSheet::loadSpriteSheets( SSvec, fileName.c_str() ) ) return false;// std::cout << "\n#SS = " << SSvec.size();
    fin >> fileName;
    if( !init_terrain( fileName ) ) return false;// cout: init_terrain_OK
    fin >> fileName;
    if( !init_BasisMotion( fileName ) ) return false;// no cout
    fin >> fileName;
    if( !init_aDrone( fileName ) ) return false;// no cout ??
    fin >> fileName;
    if( !init_Cannons( fileName ) ) return false;// cout: init_aCannon()_done
    aCannon.firingEnabled = true;
    vec3f& cp = aCannon.pos;
    aCannon.AimPt = vec3f( cp.x, cp.y + 2000.0f, cp.z + 4000.0f );
 //   vec3f sep = aCannon.AimPt - cp;
//    aCannon.firePos = cp + sep*( 3.0f*aCannon.Rbound/sep.mag() );

    fin >> fileName;
    if( !init_controls( fileName ) ) return false;// 2xcout + init_controls() end
    fin >> fileName;
 //   if( !init_mats( fileName.c_str() ) ) return false;
 //   if( !mat_1.init( fileName.c_str(), [](float x,float z){ return 200.0f*sinf(x)*sinf(z); } ) ) return false;

    unsigned int numShots; fin >> numShots;
    persBall aBall( fin );
    float outThick; fin >> outThick;
    aBall.isMoving = true;
    aBall.inUse = false;
    aBall.B.setOutlineColor( sf::Color::Black );
    aBall.B.setOutlineThickness( 1.0f );
    shotVec.resize( 100, aBall );

    // persQuadAni
    size_t  SSnum, numQuads; fin >> numQuads >> SSnum;
    quadAniVec.reserve( numQuads );
    if( numQuads > 0 && SSnum < SSvec.size() )
    {
        quadAniVec.push_back( persQuadAni( fin, SSvec[SSnum] ) );
        persQuadAni& rPQA = quadAniVec.back();
        rPQA.setTxtRect( SSvec[SSnum].getFrRect(0,rPQA.setNum), 'R', '0' );
        rPQA.inUse = false;
        rPQA.facingCamera = true;
        for( size_t i = 1; i < numQuads; ++i )
            quadAniVec.push_back( rPQA );
    }

    // fragShotA
    shotBurst::init_stat();
    shotBurst::pFireShot = [this]( vec3f Pos, vec3f Vel, bool MF ) { return fireBall( Pos, Vel, MF ); };
    shotBurst::pGetFireBall = [this]( shotBurst& rSB ) { return getAniQuad( rSB, rSB.pos ); };
    fragShotA.init( fin );

    // persQuadSpin
    PQS_1.initSpin( fin );
    size_t setNum, frIdx; fin >> SSnum >> setNum >> frIdx;
    char chA, chB; fin >> chA >> chB;
    PQS_1.pTxt = &SSvec[SSnum].txt;
    PQS_1.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    fin >> PQS_1.hitStops;
    fin >> PQS_1.dragTorque;

    // iShot
    iShot = shotVec.front();
    iShot.B.setFillColor( sf::Color( 230, 50, 50 ) );
    p_iTarget = &aDrone;

    // help message key H
    Level::init_helpMsg( fin, helpMsg );
 /*   helpMsg.setFont( *button::pFont );
    helpMsg.setFillColor( sf::Color::White );
    unsigned int fontSz; fin >> fontSz;
    helpMsg.setCharacterSize( fontSz );
    fin >> posX >> posY;
    helpMsg.setPosition( posX, posY );

    // the helpMsg

    std::string helpStr, inStr;
    if( getline( fin, inStr) )
    {
        helpStr = inStr + '\n';
        while( getline( fin, inStr) ) helpStr += '\n' + inStr;
        helpMsg.setString( helpStr.c_str() );
    }   */

 //   std::string checkStr; fin >> checkStr;
 //   std::cout << '\n' << checkStr;// got_it!

    fin.close();

    fin.open("include/levels/lvl_perspective2/arcade_data.txt");
    if( !fin ) { std::cout << "\nNo arcade data"; return false; }
    PAT_1.init( fin, SSvec );
    fin.close();
    // target edge data
    fin.open("include/levels/lvl_perspective2/plusTargetEdge_data.txt");
    if( !fin ) { std::cout << "\nNo target edge data"; return false; }
    edgeHit EH;
    while( EH.init( fin ) ) edgeVec.push_back(EH);
    std::cout << "\n edgeVec.size() = " << edgeVec.size();
    fin.close();

    // z ordering
    // all always inUse items
    for( persBox_quad& rPBQ : PBQvec ) pPtAllVec.push_back( &rPBQ );
    for( persQuad& rPQ : nohitPQvec ) pPtAllVec.push_back( &rPQ );

    for( persQuad& rPQ : targetPQvec ) pPtAllVec.push_back( &rPQ );
    for( persBall& rPB : targetBallVec ) pPtAllVec.push_back( &rPB );

    pPtAllVec.push_back( &BMball );
    pPtAllVec.push_back( &BMbase );
    pPtAllVec.push_back( &aDrone );
    pPtAllVec.push_back( &aCannon );
 //   pPtAllVec.push_back( &mat_1 );
 //   pPtAllVec.push_back( &mat_cone );
 //   pPtAllVec.push_back( &mat_parabola );
 //   pPtAllVec.push_back( &mat_hyperbola );
 //   pPtAllVec.push_back( &matBall );

 //   pPtAllVec.push_back( &PQS_1 );
 //   pPtAllVec.push_back( &PAT_1 );

    size_t fullSize = pPtAllVec.size() + shotVec.size() + quadAniVec.size();
    fullSize += 5;// fragShotA, iShot, Homer, PQS_1, PAT_1;
  //  pPtAllVec.push_back( &PAT_1 );
    pPtSortVec.reserve( fullSize );
    for( persPt* pPt : pPtAllVec ) pPtSortVec.push_back( pPt );
    for( persBall& rPB : shotVec ) pPtSortVec.push_back( &rPB );
    for( persQuadAni& rPQA : quadAniVec ) pPtSortVec.push_back( &rPQA );
    pPtSortVec.push_back( &fragShotA );// just to make room
    pPtSortVec.push_back( &iShot );// just to make room
    pPtSortVec.push_back( &Homer );
    pPtSortVec.push_back( &PQS_1 );
    pPtSortVec.push_back( &PAT_1 );

    return true;
}

void lvl_perspective2::initTargetingMessages( std::ifstream& is )
{
    angleMsg.setFont( *button::pFont );
    angleMsg.setCharacterSize(14);
    angleMsg.setFillColor( sf::Color::Green );
    targetingMsg = angleMsg;
    targetingMsg.setFillColor( sf::Color::Red );

    float posX, posY, offX, offY;
    is >> posX >> posY >> offX >> offY;
    targetingMsg.setPosition( posX, posY );
    targetingMsg.setString("targeting: OFF");
    angleMsg.setPosition( posX + 3.0f*offX, posY );
    angleMsg.setString("angle: LOW");

    posY += 10.0f;// bump!
    targetMsgVec.reserve( 10 );// *******
    sf::Text msg( angleMsg );
    msg.setFillColor( sf::Color::White );

    msg.setString("range");
    msg.setPosition( posX, posY + offY );
    targetMsgVec.push_back( msg );// 1
    msg.setString("---");// value
    msg.setPosition( posX + offX, posY + offY );
    targetMsgVec.push_back( msg );// 2
    pRangeMsg = &targetMsgVec.back();

    msg.setString("elev");
    msg.setPosition( posX + 2.0f*offX, posY + offY );
    targetMsgVec.push_back( msg );// 3
    msg.setString("---");// value
    msg.setPosition( posX + 3.0f*offX, posY + offY );
    targetMsgVec.push_back( msg );// 4
    pElevationMsg = &targetMsgVec.back();

    msg.setString("dist");
    msg.setPosition( posX + 4.0f*offX, posY + offY );
    targetMsgVec.push_back( msg );// 5
    msg.setString("---");// value
    msg.setPosition( posX + 5.0f*offX, posY + offY );
    targetMsgVec.push_back( msg );// 6
    pDistanceMsg = &targetMsgVec.back();

    msg.setString("angle");
    msg.setPosition( posX, posY + 2.0f*offY );
    targetMsgVec.push_back( msg );// 7
    msg.setString("---");// value
    msg.setPosition( posX + offX, posY + 2.0f*offY );
    targetMsgVec.push_back( msg );// 8
    pAngleMsg = &targetMsgVec.back();

    msg.setString("time");
    msg.setPosition( posX + 2.0f*offX, posY + 2.0f*offY );
    targetMsgVec.push_back( msg );// 9
    msg.setString("---");// value
    msg.setPosition( posX + 3.0f*offX, posY + 2.0f*offY );
    targetMsgVec.push_back( msg );// 10
    pflightTimeMsg = &targetMsgVec.back();
    sf::Vector2f Sz, bkPos;
    is >> Sz.x >> Sz.y >> bkPos.x >> bkPos.y;
    bkPos.x += posX; bkPos.y += posY;
    targetBkRect.setSize( Sz );
    targetBkRect.setPosition( bkPos );
    unsigned int alp; is >> alp;
    targetBkRect.setFillColor( sf::Color(0,0,0,alp) );
    targetBkRect.setOutlineColor( sf::Color(200,0,0,255) );
    targetBkRect.setOutlineThickness( 2.0f );
}

bool lvl_perspective2::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if( rEvent.key.code == sf::Keyboard::Space )
        {
            if( !firing )
            {
                tFire = tFireDelay;// trigger 1st shot immediately
                firing = true;
            }
        }
        else if( rEvent.key.code == sf::Keyboard::C )// aCannon will be firing
        {
            if( amTargeting )
            {
                if( !firingCannon && ( pTargetQuad || pTgtBall ) )
                {
                 //   aCannon.AimPt = aimPt;
                    firingCannon = true;
                    aCannon.startAiming( aimPt );
                }
            }
            else if( !aCannon.isMoving && persPt::camDir.y < -0.01f )// start cannon move
            {
                float s = -persPt::camPos.y/persPt::camDir.y;
                vec3f moveTo = persPt::camPos + s*persPt::camDir + aCannon.Rbound*persPt::yHat;
                aCannon.startMove( moveTo );
             //   std::cout << "\n aCannon move!";
            }
        }
        else if( rEvent.key.code == sf::Keyboard::Left ) move_LtRt = -1;
        else if( rEvent.key.code == sf::Keyboard::Right ) move_LtRt = 1;
        else if( rEvent.key.code == sf::Keyboard::Up ) move_UpDown = 1;
        else if( rEvent.key.code == sf::Keyboard::Down ) move_UpDown = -1;
        else if( rEvent.key.code == sf::Keyboard::R ) jbCamButt.dropStick = true;// release the joybutton
        else if( rEvent.key.code == sf::Keyboard::H ) showHelpMsg = !showHelpMsg;
        else if( rEvent.key.code == sf::Keyboard::V )// zoom in
        {
            if( !isZoomed )
            {
                isZoomed = true;
                camPosSave = persPt::camPos;
                persPt::camPos += persPt::camDir*zoomDistance;
                for( persPt* pPt : pPtAllVec ) pPt->update(0.0f);
            }
        }
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        move_LtRt = move_UpDown = 0;
        jbCamButt.dropStick = false;
        firing = false;
        firingCannon = false;
        if( isZoomed )
        {
            isZoomed = false;
            persPt::camPos = camPosSave;
            persPt::calibrateCameraAxes();
            for( persPt* pPt : pPtAllVec ) pPt->update(0.0f);
        }

        if( rEvent.key.code == sf::Keyboard::B ) BM_isMoving = !BM_isMoving;
        else if( rEvent.key.code == sf::Keyboard::A )
        {
            aimingHigh = !aimingHigh;// toggle high/low angle aim
            angleMsg.setString( aimingHigh ? "angle High" : "angle LOW" );
            aCannon.aimingHigh = aimingHigh;
        }
        else if( rEvent.key.code == sf::Keyboard::T )// toggle targeting mode
        {
            if( pTargetQuad )
            {
                pTargetQuad->setColor( sf::Color::White );
                pTargetQuad = nullptr;
            }
            amTargeting = !amTargeting;
            aimingHigh = false;
            angleMsg.setString( aimingHigh ? "angle: High" : "angle: LOW" );
            targetingMsg.setString( amTargeting ? "targeting: ON" : "targeting: OFF" );
            pflightTimeMsg->setString("---");
            pAngleMsg->setString("---");
            pRangeMsg->setString("---");
            pElevationMsg->setString("---");
            pDistanceMsg->setString("---");
        }
        else if( rEvent.key.code == sf::Keyboard::F )// fire fragShotA
        {
            if( !fragShotA.inUse )
            {
                if( amTargeting && ( pTargetQuad || pTgtBall ) )
                {
                    float fireVelSave = fireVel;
                    fireVel = Homer.Speed0;
                    float tFlight;
                    vec3f vel;
                    if( getFiringVelocity( vel, tFlight ) )
                    {
                        fragShotA.tBoom = tFlight - tFuse;
                        fragShotA.fire( getShotPos0(), vel );
                    }
                    fireVel = fireVelSave;// restore
                }
                else
                    fragShotA.fire( getShotPos0(), getShotVel0() );
            }
        }
        else if( rEvent.key.code == sf::Keyboard::X )// fire Homer
        {
            if( Homer.ball.inUse )
            {
                getAniQuad( Homer.ball, Homer.ball.pos );
            }
            Homer.ball.inUse = true;
            Homer.tFlight = 0.0f;
            Homer.pos = Homer.pos0 = getShotPos0();
            Homer.vel0 = getShotVel0();
            Homer.vel0 *= Homer.Speed0/Homer.vel0.mag();
            Homer.ball.vel = Homer.vel0;
            Homer.ball.setPosition( Homer.pos );

        }
        else if( rEvent.key.code == sf::Keyboard::I )// fire iShot
        {
            get_iShot();
        }
        else if( rEvent.key.code == sf::Keyboard::S )// reset flip target position
        {
            PQS_1.reset();
            PAT_1.resetTargets();
        }
    }

    return true;
}

void lvl_perspective2::update( float dt )
{
    if( !jbCamButt.atRest )
    {
        update_global(dt);// camera position and orientation and "live" messages
        for( persPt* pPt : pPtAllVec ) pPt->update(dt);
    }

    // update moving and/or animating objects
    updateBasisMotion(dt);
    updateTargeting();// aimPt assigned
    fragShotA.update( gravity, dt );
    if( fragShotA.inUse ) hitAll( fragShotA, dt );
    Homer.update(dt);
    aCannon.update(dt);
    aCannon.update_turret(dt);
    aDroneAdapter.update(dt);
    PAT_1.update(dt);
  //  update_mats(dt);

    // spin target
    // reset position with left click on image
    if( PQS_1.persPt::hit_image( button::mseX, button::mseY ) )
    {
        if( button::clickEvent_Lt() == 1 )
        {
            PQS_1.reset();
            to_SF_string( PQSangleNumMsg, PQS_1.angle*180.0f/vec2f::PI );
        }
        else if( !PQS_1.isSpinning && button::didScroll )// rotate
        {
            float dAng = 0.2f*button::scrollAmount;
            PQS_1.setRotation( PQS_1.angle + dAng );
            to_SF_string( PQSangleNumMsg, PQS_1.angle*180.0f/vec2f::PI );
        //    PQS_1.setOrientation( PQS_1.Nu.rotate_axis( rotAxis,  ), rotAxis );
        //    PQS_1.setOrientation( PQS_1.Nu, rotAxis );
        }
    }

    if( PQSgravityOn ) PQS_1.update( PQSgravity, dt );
    else PQS_1.update(dt);

    if( p_iTarget && iShot.inUse )
    {
        iShot.update(dt);
        tiFlight += dt;
        vec3f N( persPt::camPos - iShot.pos );
        vec3f P(iShot.pos + iShot.Rbound*N/N.mag() ), vu;

        if( tiFlight > tiFlightMax )
        {
            getAniQuad( iShot, P );
            std::cout << "\n iShot destruct!";
        }
        else if( iShot.pos.y < 0.0f )
        {
            getAniQuad( iShot, P );
            std::cout << "\n iShot hit the ground";
        }
        else if( p_iTarget->hit( iShot.pos - iShot.vel*dt, iShot.pos, P, vu ) )
        {
            N = ( persPt::camPos - p_iTarget->pos );
            N /= N.mag();
            getAniQuad( iShot, P + p_iTarget->Rbound*N );
            std::cout << "\n aDrone was hit!";
        }
    }

//    if( firingCannon && amTargeting && pTargetQuad )
 //       aCannon.AimPt = aimPt;
    //    aCannon.AimPt = persPt::camPos + persPt::camDir*targetDistance;

    if( anyShots_inUse )
    {
        anyShots_inUse = false;
        unsigned int numInUse = 0;
        for( persBall& PB : shotVec )
        {
            PB.update( gravity, dt );
            if( PB.inUse ){ anyShots_inUse = true; ++numInUse; }
        }

        to_SF_string( numShotsMsg, numInUse );

        // collision test
        if( anyShots_inUse )
            for( persBall& PB : shotVec )
                hitAll( PB, dt );
    }

    // fireball animations
    if( anyAnis_inUse )
    {
        anyAnis_inUse = false;
        for( persQuadAni& PQA : quadAniVec )
        {
            PQA.update(dt);
            if( PQA.inUse ) anyAnis_inUse = true;
        }
    }

    // firing
    vec3f shotPos0 = getShotPos0();
    if( firing && (tFire+=dt) > tFireDelay )
    {
        vec3f velShot = getShotVel0();
        float tFlight = 0.0f;
        if( amTargeting && ( pTargetQuad || pTgtBall ) )
            getFiringVelocity( velShot, tFlight );

        fireBall( shotPos0, velShot, false );
        tFire -= tFireDelay;
    }

    // z ordering
    numToDraw = 0;
    for( persPt* pPt : pPtAllVec )
        if( pPt->doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = pPt;

    if( anyShots_inUse )
        for( persBall& PB : shotVec )
            if( PB.inUse && PB.doDraw && numToDraw < pPtSortVec.size() )
                pPtSortVec[ numToDraw++ ] = &PB;

    if( anyAnis_inUse )
        for( persQuadAni& rPQA : quadAniVec )
            if( rPQA.inUse && rPQA.doDraw && numToDraw < pPtSortVec.size() )
                pPtSortVec[ numToDraw++ ] = &rPQA;

    if( fragShotA.inUse && fragShotA.doDraw && numToDraw < pPtSortVec.size() )
        pPtSortVec[ numToDraw++ ] = &fragShotA;
    if( Homer.doDraw && numToDraw < pPtSortVec.size() )
        pPtSortVec[ numToDraw++ ] = &Homer;
    if( iShot.inUse && iShot.doDraw && numToDraw < pPtSortVec.size() )
        pPtSortVec[ numToDraw++ ] = &iShot;
    if( PQS_1.doDraw && numToDraw < pPtSortVec.size() )
        pPtSortVec[ numToDraw++ ] = &PQS_1;
    if( PAT_1.doDraw && numToDraw < pPtSortVec.size() )
        pPtSortVec[ numToDraw++ ] = &PAT_1;


    if( numToDraw > 0 )
    //    std::sort( pPtSortVec.begin(), pPtSortVec.begin() + numToDraw, [this]( persPt* pA, persPt* pB ){ return pA->getDistance() < pB->getDistance(); } );
        std::sort( pPtSortVec.begin(), pPtSortVec.begin() + numToDraw, persPt::compare );
}

void lvl_perspective2::hitAll( persBall& PB, float dt )
{
    if( !PB.inUse ) return;


    if( PB.pos.y < 0.0f )// target = the ground
    {
     //   PB.doDraw = PB.inUse = false;
        getAniQuad( PB, PB.pos + 10.0f*persPt::yHat );
     //   anyAnis_inUse = persQuadAni::getOne( quadAniVec, PB.pos + 10.0f*persPt::yHat );
        return;
    }

    vec3f P, vu;
    // check vs the buildings
    for( persBox_quad& PBQ : PBQvec )
    {
        if( PBQ.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
        {
            vec3f N = ( persPt::camPos - P );
            N /= N.mag();
         //   PB.doDraw = PB.inUse = false;
            getAniQuad( PB, P + 10.0f*N );
            return;
        }
    }

    // check vs the target quads
    for( persQuad& PQ : targetPQvec )
    {
        if( PQ.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
        {
            if( hitPixel( 6, sf::Color(255,255,255,0), PQ, P ) )
            {
             //   vec3f N = ( persPt::camPos - P );
             //   N /= N.mag();
              //  getAniQuad( PB, P + PQ.Rbound*N );

                PB.setPosition( P + PB.Rbound*vu );// a bit in front
                PB.vel = PB.vel.mag()*vu;// bounce off
                return;
            }
            // check for edge hits
            if( edgeVec.size() > 0 )
            {
                for( const edgeHit& EH : edgeVec )// each edge
                {
                    vec3f xu = PQ.pt[2] - PQ.pt[1], yu = PQ.pt[1] - PQ.pt[0];
                    xu /= xu.mag(); yu /= yu.mag();
                    float x = ( P - PQ.pos ).dot( xu );
                    float y = ( P - PQ.pos ).dot( yu );
                    if( EH.hit( x, y, PB.Rbound, 1.0f, xu, yu, PB.vel ) ) break;
                }
            }

            return;
        }
    }

    for( persBall& tgtPB : targetBallVec )
    {
     //   std:: cout << " tgtBall";
        if( tgtPB.hitFree( PB, 1.0f, dt ) )
        {
        //    std:: cout << "\n tgtBall hit!";
        //    vec3f N = ( persPt::camPos - tgtPB.pos );
        //    N /= N.mag();
        //    getAniQuad( PB, tgtPB.pos + tgtPB.Rbound*N );
            return;
        }
    }

    // vs aDrone
    if( aDrone.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        vec3f N = ( persPt::camPos - P );
        N /= N.mag();
        getAniQuad( PB, P + aDrone.Rbound*N );
        return;
    }

    // vs PQS_1
    if( PQS_1.hitSpin( PB, dt, 1.0f, P, vu ) )
    {
        PB.setPosition(P);
        PB.vel = vu*PB.vel.mag();
        return;
    }

    // vs shooting gallery
    if( PAT_1.hit( PB, dt, 0.7f, P, vu ) ) return;

    // vs mat_1
/*    if( mat_1.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
   //     if( ( P - mat_1.pos ).dot( persPt::yHat ) < 0.0f ) return;// exclude hits to bottom half of sphere
        PB.setPosition(P);
        PB.vel = vu*PB.vel.mag();
        return;
    }

    if( mat_parabola.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        PB.setPosition(P);
        PB.vel = vu*PB.vel.mag();
        std::cout << "Hit parab ";
        return;
    }       */

}

// assumes that PQ is hit at hitPt

bool lvl_perspective2::hitPixel( unsigned int ssNum, sf::Color xprtColor, persQuad& PQ, vec3f hitPt )const// true if hit over transparency Color
{
    if( PQ.pTxt == nullptr ) return true;// any hit counts?
    if( !( pVtxTCmin && pVtxTCx && pVtxTCy ) ) return true;// vertex order not assigned. any hit counts?

    sf::Vector2f hitPos = persPt::get_xyw( hitPt );

 //   hitPos -= pVtxTCmin->position;// TC min
    hitPos -= PQ.vtx[0].position;

    hitPos.y *= -1.0f;
 //   if( PQ.Nu.dot( hitPt - persPt::camPos ) > 0.0f ) hitPos.x *= -1.0f;
 //   hitPos.x *= -1.0f;
    sf::Vector2u txtSz = PQ.pTxt->getSize();

    std::cout << "\n bs: hitPos = " << hitPos.x << ", " << hitPos.y;

    // scale from texCoords
 /*   float dPosX = pVtxTCx->position.x - pVtxTCmin->position.x;
    if( dPosX*dPosX > 1.0f )
    {
        hitPos.x *= ( pVtxTCx->texCoords.x - pVtxTCmin->texCoords.x )/( pVtxTCx->position.x - pVtxTCmin->position.x );
        hitPos.y *= ( pVtxTCy->texCoords.y - pVtxTCmin->texCoords.y )/( pVtxTCy->position.y - pVtxTCmin->position.y );
    }
    else
    {
        hitPos.x *= ( pVtxTCx->texCoords.x - pVtxTCmin->texCoords.x )/( pVtxTCx->position.y - pVtxTCmin->position.y );
        hitPos.y *= ( pVtxTCy->texCoords.y - pVtxTCmin->texCoords.y )/( pVtxTCy->position.x - pVtxTCmin->position.x );
    }

    hitPos += pVtxTCmin->texCoords; */

    // scale from texCoords
    hitPos.x *= ( PQ.vtx[3].texCoords.x - PQ.vtx[0].texCoords.x )/( PQ.vtx[3].position.x - PQ.vtx[0].position.x );
    hitPos.y *= ( PQ.vtx[0].texCoords.y - PQ.vtx[1].texCoords.y )/( PQ.vtx[0].position.y - PQ.vtx[1].position.y );// now 0 to txtImgSz
    hitPos += PQ.vtx[1].texCoords;

    std::cout << "\n as: PQ.vtx[0].texCoords = " << PQ.vtx[0].texCoords.x << ", " << PQ.vtx[0].texCoords.y;
    std::cout << "\n as: hitPos = " << hitPos.x << ", " << hitPos.y;

    if( ( hitPos.x > 0.0f && hitPos.x < (float)txtSz.x ) && ( hitPos.y > 0.0f && hitPos.y < (float)txtSz.y ) )
    {
        std::cout << "\n\n hitTarget";
        sf::Color hitColor = SSvec[ssNum].img.getPixel( (unsigned int)hitPos.x, (unsigned int)hitPos.y );
        std::cout << "  hitColor: " << (unsigned int)hitColor.r << ", " << (unsigned int)hitColor.b << ", " << (unsigned int)hitColor.g << ", " << (unsigned int)hitColor.a;
        std::cout << "\n  xptColor: " << (unsigned int)xprtColor.r << ", " << (unsigned int)xprtColor.b << ", " << (unsigned int)xprtColor.g << ", " << (unsigned int)xprtColor.a;
        bool match = ( xprtColor == hitColor );
        std::cout << "  match: " << match;
        return !match;// true if not matching colors
    }

    return false;
}

/*
bool lvl_perspective2::hitPixel( unsigned int ssNum, sf::Color xprtColor, persQuad& PQ, vec3f hitPt )const// true if hit over transparency Color
{
    if( PQ.pTxt == nullptr ) return false;// any hit counts?

    sf::Vector2f hitPos = persPt::get_xyw( hitPt );
    hitPos -= PQ.vtx[0].position;// up left
    hitPos.y *= -1.0f;
 //   if( PQ.Nu.dot( hitPt - persPt::camPos ) > 0.0f ) hitPos.x *= -1.0f;
    hitPos.x *= -1.0f;
    sf::Vector2u txtSz = PQ.pTxt->getSize();

    std::cout << "\n bs: hitPos = " << hitPos.x << ", " << hitPos.y;
    // scale from texCoords
    hitPos.x *= ( PQ.vtx[3].texCoords.x - PQ.vtx[0].texCoords.x )/( PQ.vtx[0].position.x - PQ.vtx[3].position.x );
    hitPos.y *= ( PQ.vtx[0].texCoords.y - PQ.vtx[1].texCoords.y )/( PQ.vtx[0].position.y - PQ.vtx[1].position.y );// now 0 to txtImgSz
    hitPos += PQ.vtx[1].texCoords;
    std::cout << "\n as: PQ.vtx[0].texCoords = " << PQ.vtx[0].texCoords.x << ", " << PQ.vtx[0].texCoords.y;
    std::cout << "\n as: hitPos = " << hitPos.x << ", " << hitPos.y;

    if( ( hitPos.x > 0.0f && hitPos.x < (float)txtSz.x ) && ( hitPos.y > 0.0f && hitPos.y < (float)txtSz.y ) )
    {
        std::cout << "\n\n hitTarget";
        sf::Color hitColor = SSvec[ssNum].img.getPixel( (unsigned int)hitPos.x, (unsigned int)hitPos.y );
        std::cout << "  hitColor: " << (unsigned int)hitColor.r << ", " << (unsigned int)hitColor.b << ", " << (unsigned int)hitColor.g << ", " << (unsigned int)hitColor.a;
        std::cout << "\n  xptColor: " << (unsigned int)xprtColor.r << ", " << (unsigned int)xprtColor.b << ", " << (unsigned int)xprtColor.g << ", " << (unsigned int)xprtColor.a;
        bool match = ( xprtColor == hitColor );
        std::cout << "  match: " << match;
        return !match;// true if not matching colors
    }

    return false;
}
*/

void lvl_perspective2::update_global( float dt )
{
    vec3f hu = persPt::camDir - persPt::camDir.dot( persPt::yHat )*persPt::yHat;
    hu /= hu.mag();
    if( button::mseDnLt ){ persPt::camPos += hu*(camSpeed*dt); }
    else if( button::mseDnRt ) { persPt::camPos -= hu*(camSpeed*dt); }

    float dAr = 0.0f;
    if( button::didScroll ) dAr = rollRate*button::scrollAmount;

    float compassAngle = persPt::changeCamDir( yawRate*dt, pitchRate*dt, dAr );
    to_SF_string( msgNumVec[3], compassAngle );

    persPt::calibrateCameraAxes();
    // translational motion via arrow keys
    if( move_LtRt != 0 ) persPt::camPos += move_LtRt*move_xyVel*persPt::xu*dt;
    if( move_UpDown != 0 ) persPt::camPos += move_UpDown*move_xyVel*persPt::yu*dt;
    to_SF_string( msgNumVec[0], persPt::camPos.x );
    to_SF_string( msgNumVec[1], persPt::camPos.y );
    to_SF_string( msgNumVec[2], persPt::camPos.z );
    // keep xu level
    persPt::xu = persPt::yHat.cross( persPt::camDir );
    persPt::xu /= persPt::xu.mag();
    persPt::yu = persPt::camDir.cross( persPt::xu );

    float U = persPt::camDir.dot( persPt::yHat );
    float yHorizon = persPt::Yh + persPt::Z0*U/sqrtf( 1.0f - U*U );

    trueHorizon[0].position.y = trueHorizon[1].position.y = yHorizon;
    updatePane();

    if( amTargeting && pTargetQuad )
    {
        getFiringVelocity();
    }
}


float lvl_perspective2::launchHellfire()// returns tFlight to hit
{
 //   vec3f hf_Pos0;// at xf from target
 //   vec3f hf_PosTarget;
 //   vec3f hf_PosShot;
 //   vec3f hf_Hu;// unit vector in x,z plane = line of path
 //   float hf_xf, hf_xc, hf_yMax, hf_yf;
 //   float Hboom;// distance over target: hf_yf = hf_PosTarget.y + hBoom;

    vec3f sep = hf_PosTarget - hf_PosShot;// both given
    // find hf_Hu
    hf_Hu = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;
    hf_Hu /- hf_Hu.mag();
 //   float K = ( hf_yMax - hf_yf )/( hf_yMax - hf_PosShot.y );

    return 5.0f;
}

void lvl_perspective2::updateHellfire( float ft )
{

}

void lvl_perspective2::draw( sf::RenderTarget& RT ) const
{
    RT.draw( upperPane, 4, sf::Quads );
    RT.draw( trueHorizon, 2, sf::Lines );

 //   for( const persPt* pPt : pPtSortVec ) pPt->draw(RT);
    for( unsigned int j = 0; j < numToDraw; ++j ) pPtSortVec[j]->draw(RT);
    if( BMball.doDraw && BMbase.doDraw ) RT.draw( BMrod, 2, sf::Lines );

    RT.draw( targetingMsg );
    if( amTargeting )
    {
        RT.draw( targetBkRect );
        RT.draw( angleMsg );
        for( const sf::Text& txt : targetMsgVec ) RT.draw( txt );
    }

 //   RT.draw( aDrone.droneQuad.vtx, 4, sf::Quads );

    RT.draw( viewCross, 4, sf::Lines );
    // always over
    for( const sf::Text& txt : msgVec ) RT.draw( txt );
    for( const sf::Text& txt : msgNumVec ) RT.draw( txt );
    RT.draw( numShotsMsg );
 //   RT.draw( mb_EtotNumMsg );
    if( showHelpMsg ) RT.draw( helpMsg );
}

void lvl_perspective2::updatePane()
{
    upperPane[2].position = trueHorizon[1].position;
    upperPane[3].position = trueHorizon[0].position;
}

bool lvl_perspective2::fireBall( vec3f Pos, vec3f Vel, bool addMuzzleFlash )
{
    for( persBall& PB : shotVec )
    {
        if( !PB.inUse )
        {
            if( addMuzzleFlash ) getAniQuad( PB, Pos );// muzzle flash
            PB.reset( Pos, Vel );
            anyShots_inUse = true;
            return true;;
        }
    }

    return false;
}

bool lvl_perspective2::getAniQuad( persBall& rPB, vec3f Pos )// trigger animation at ball position and take ball out of use
{
    rPB.inUse = false;
    rPB.doDraw = false;// to prevent z ordering
    return getAniQuad( Pos );
}

bool lvl_perspective2::getAniQuad( vec3f Pos )// trigger animation at ball Pos
{
    if( persQuadAni::getOne( quadAniVec, Pos ) )
    {
        anyAnis_inUse = true;
        return true;
    }

    return false;// all Ani are inUse
}

bool lvl_perspective2::getFiringVelocity()// calls below
{
    vec3f vel; float tFlight;
    return getFiringVelocity( vel, tFlight );
}

bool lvl_perspective2::getFiringVelocity( vec3f& vel, float& tFlight )// false if h too low or R too great for given fireVel. writes vel
{
 //   vec3f sep = persPt::camDir*targetDistance;
    vec3f sep = aimPt - persPt::camPos;
    sep += fireOfst*persPt::yu;// correct for cannon offset from view point
    float h = sep.dot( persPt::yHat );// - fireOfst*persPt::yu.dot( persPt::yHat );// vertical component
    vec3f rxz = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;
    float R = rxz.mag();// horizontal
    rxz /= R;// now unit length
    float V0 = fireVel;// + camVel; it's not that simple
    float k = -gravity.y*R*R/(2.0f*fireVel*fireVel);
    float deter = R*R - 4.0f*k*( h + k );
    if( deter < 0.0f )
    {
     //   std::cout << "\nOut of range";
        vel = getShotVel0();
        tFlight = 0.0f;// temp cheat
        pflightTimeMsg->setString("---");
        pAngleMsg->setString("OOR");
        pRangeMsg->setString("---");
        pElevationMsg->setString("---");
        pDistanceMsg->setString("---");
        if( pTargetQuad ) pTargetQuad->setColor( sf::Color::Red );
        return false;
    }

    float tanAngle = aimingHigh ? R + sqrtf( deter ) : R - sqrtf( deter );
    tanAngle /= 2.0f*k;
    float sA = tanAngle/sqrtf( 1.0f + tanAngle*tanAngle ), cA = sqrtf( 1.0f - sA*sA );
    vel = V0*( cA*rxz + sA*persPt::yHat );
    tFlight = R/( V0*cA );

    to_SF_string( *pAngleMsg, atanf( tanAngle )*180.0f/myPI );
    to_SF_string( *pflightTimeMsg, tFlight );
    to_SF_string( *pRangeMsg, R );
    to_SF_string( *pElevationMsg, h );
    to_SF_string( *pDistanceMsg, sep.mag() );
    if( pTargetQuad ) pTargetQuad->setColor( sf::Color::Green );

    return true;
}

float lvl_perspective2::get_iShot()
{
    if( !p_iTarget ) return 0.0f;
    iShot.inUse = true;
    iShot.setPosition( getShotPos0() );
    vec3f Rct = p_iTarget->pos - iShot.pos;
    vec3f& Vt = p_iTarget->vel;
    float St = Vt.mag(), Ss = fireVel;
 //   float tf = ( Vt.dot( Rct ) - Ss )/( Ss*Ss - St*St );// Low flight time
    float tf = ( Vt.dot( Rct ) + Ss*Rct.mag() )/( Ss*Ss - St*St );// high flight time
    iShot.vel = Rct/tf + Vt;
    float vMag = iShot.vel.mag();
    std::cout << "\n iShot.vel = " << vMag;
    tiFlight = 0.0f;
 //   if( vMag > 5000.0f ) iShot.vel *= 5000.0f/vMag;
    return tf;
}

void lvl_perspective2::updateTargeting()
{
    if( !amTargeting ) return;

    // vs target from last frame
    // NO! because if move off of existing one onto a closer overlapping one, the switch to the closer one won't occur
 //   if( pTargetQuad && pTargetQuad->isSighted( targetDistance )  ) return;

    float distMin = 1.0e6f;
    persQuad* pTargetMin = nullptr;
    vec3f P;
    float targetDistance = 0.0f;

    // hit a box?
    persQuad* pPQ = nullptr;

    for( persBox_quad& PBQ : PBQvec )
    {
        if( PBQ.isSighted( targetDistance, pPQ ) && targetDistance < distMin )
        {
            float dist = ( P - persPt::camPos ).mag();
            if( dist < distMin )
            {
                distMin = targetDistance;
                pTargetMin = pPQ;
            }
        }
    }

    // hit a targetQuad?
    for( persQuad& PQ : targetPQvec )
    {
        if( PQ.isSighted( targetDistance ) && targetDistance < distMin )
        {
            distMin = targetDistance;
            pTargetMin = &PQ;
            //    break;// can't. targets may be layered. Must sight to nearest
        }
    }

    pTgtBall = nullptr;
    // hit a target ball?
    for( persBall& PB : targetBallVec )
    {
        vec3f P;
        if( PB.isSighted( P ) )// assign aimPt instead?
        {
         //   targetDistance = ( P - persPt::camPos ).mag();
            targetDistance = ( PB.pos - persPt::camPos ).mag();
            if( targetDistance < distMin )
            {
                distMin = targetDistance;
                pTgtBall = &PB;
            }
        }
    }

    if( pTgtBall )// found closer than any target quad
    {
        pTargetMin = nullptr;
        aimPt = persPt::camPos + persPt::camDir*distMin;
        getFiringVelocity();
     //   std:: cout << "\n tgtBall sighted!";
    }

    if( pTargetMin )// a hit
    {
        if( pTargetMin != pTargetQuad )
        {
            if( pTargetQuad ) pTargetQuad->setColor( sf::Color::White );
       //     pTargetMin->setColor( sf::Color::Green );
            pTargetQuad = pTargetMin;
            getFiringVelocity();
        }
     //   pTargetQuad = pTargetMin;
     //   targetDistance = distMin;
        aimPt = persPt::camPos + persPt::camDir*distMin;
    }
    else if( pTargetQuad )
    {
        pTargetQuad->setColor( sf::Color::White );
        pTargetQuad = nullptr;
        pflightTimeMsg->setString("---");
        pAngleMsg->setString("---");
        pRangeMsg->setString("---");
        pElevationMsg->setString("---");
        pDistanceMsg->setString("---");
    }
}

bool lvl_perspective2::init_controls( const std::string& fileName )
{
 //   std::ifstream fin("include/levels/lvl_perspective2/control_data.txt");
    std::ifstream fin( fileName.c_str() );
    if( !fin ) { std::cout << "\nNo control data"; return false; }

    float R, r, posX, posY; fin >> R >> r >> posX >> posY;

    jbCamButt.init( R, r, posX, posY );
    jbCamButt.pFunc_ff = [this](float x, float z)
    {
        float Sxy = camVelXZscale;
        yawRate = Sxy*x*x;
        pitchRate = -Sxy*z*z;

        if( x < 0.0f ) yawRate *= -1.0f;
        if( z < 0.0f ) pitchRate *= -1.0f;
    };
    button::RegisteredButtVec.push_back( &jbCamButt );

    sf::Text Label;
    Label.setFont( *button::pFont );
    Label.setCharacterSize(12);
    Label.setFillColor( sf::Color::Black );
    sf::Vector2f HBpos, HBsz;
    std::string fName;

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y;
    Label.setString( fName.c_str() );
    controlList.init( HBpos.x, HBpos.y, HBsz.x, HBsz.y, Label );
    controlList.persist = 3;

     multiSelector* pMS = nullptr;
     buttonValOnHit* pVOH = nullptr;
    std::ifstream finControl;
    sf::Vector2f sfcOfst;

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( multiCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
    init_MultiControl( pMS, pVOH, finControl );
    std::cout << "\n initMC done";
    finControl.close();
    controlList.pButtVec.push_back( &multiCS );

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( fragCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
  //  init_fragControl( pMS, pVOH, finControl );
    init_fragControl( fragCS, pMS, pVOH, finControl, fragShotA );
    finControl.close();
    controlList.pButtVec.push_back( &fragCS );

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( basisCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
 //   init_basisControl( pMS, pVOH, finControl );
    finControl.close();
    init_basisControl( pMS, pVOH, Rbm, thAngle, phiAngle_1 );
    controlList.pButtVec.push_back( &basisCS );

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( droneCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
    init_droneControl( pMS, pVOH, finControl );
    finControl.close();
    controlList.pButtVec.push_back( &droneCS );

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( cannonCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
  //  init_CannonControl( pMS, pVOH, finControl );
    init_CannonControl( cannonCS, pMS, pVOH, finControl, aCannon );
    finControl.close();
    controlList.pButtVec.push_back( &cannonCS );

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( homerCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
 //   init_homerControl( pMS, pVOH, finControl );
    init_homerControl( pMS, pVOH, finControl, Homer );
    finControl.close();
    controlList.pButtVec.push_back( &homerCS );

    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( spinTargetCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
    init_spinTargetControl( pMS, pVOH, finControl );
    finControl.close();
    controlList.pButtVec.push_back( &spinTargetCS );

    button::RegisteredButtVec.push_back( &controlList );

    std::string checkStr; fin >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

multiSelector* lvl_perspective2::init_controlMS( controlSurface& rCS, buttonValOnHit*& pStrip, sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    std::string inStr; is >> inStr;// label for home button
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    rCS.init( HBpos, PosSurf, SzSurf, inStr.c_str(), HBsz.x, HBsz.y );
    rCS.ownButts = true;

    sf::Vector2f Pos; is >> Pos.x >> Pos.y;// as offset from PosSurf
    Pos += PosSurf;
    pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( Pos, pStrip, is );
    pMS->ownsStrip = true;
    rCS.pButtVec.push_back( pMS );

    is >> inStr;
    std::cout << '\n' << inStr;
    return pMS;
}

// KEEP too many floats to pass
bool lvl_perspective2::init_MultiControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is )
{
    if( pMS->sdataVec.size() != 9 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // camSpeed
    pSD->pSetFunc =     [this](float x){ camSpeed = x; };
    camSpeed = pSD->xCurr;
    pStrip->reInit( *pSD );
    //  move_xyVel
    (++pSD)->pSetFunc = [this](float x){ move_xyVel = x; };
    move_xyVel = pSD->xInit;
    // fireVel
    (++pSD)->pSetFunc = [this](float x){ fireVel = aCannon.shotSpeed = x; };
    fireVel = aCannon.shotSpeed = pSD->xInit;
    // firing rate
    (++pSD)->pSetFunc = [this](float x){ tFireDelay = 1.0f/x; };
    tFireDelay = 1.0f/pSD->xInit;
    // gravity
    (++pSD)->pSetFunc = [this](float x){ gravity.y = aCannon.gravY = -x; };
    gravity.y = aCannon.gravY = -pSD->xInit;
    // camVelXZscale = joystick sensitivity
    (++pSD)->pSetFunc = [this](float x){ camVelXZscale = x; };
 //   pSD->xCurr = camVelXZscale;
    camVelXZscale = pSD->xCurr;
    // zoomDistance
    (++pSD)->pSetFunc = [this](float x){ zoomDistance = x; };
    zoomDistance = pSD->xInit;
    // aCannon.moveSpeed
    (++pSD)->pSetFunc = [this](float x){ aCannon.moveSpeed = x; };
    aCannon.moveSpeed = pSD->xInit;
    // tFuse
    (++pSD)->pSetFunc = [this](float x){ tFuse = x; };
    tFuse = pSD->xInit;

    // a "reset all" button
    sf::Vector2f Pos, Sz; is >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    Pos += multiCS.bkRect.getPosition();
    pushButton* pPB = new pushButton( Pos, Sz, 'O', "Reset All", nullptr, nullptr );
    pPB->pFuncClose = [pMS]() { pMS->resetAll(); };
    multiCS.pButtVec.push_back( pPB );

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

// KEEP. Too many objects to pass
bool lvl_perspective2::init_droneControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is )
{
    if( pMS->sdataVec.size() != 2 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // aDroneAdapter.v
    pSD->pSetFunc =     [this](float x){ aDroneAdapter.v = x; };
    aDroneAdapter.v = pSD->xCurr;
    pStrip->reInit( *pSD );
    // aDrone.accelFactor
    (++pSD)->pSetFunc = [this](float x){ aDrone.accelFactor = x; };
    aDrone.accelFactor = pSD->xInit;

    // a "reset path position" button
    sf::Vector2f Pos, Sz; is >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    Pos += multiCS.bkRect.getPosition();
    pushButton* pPB = new pushButton( Pos, Sz, 'O', "Reset", nullptr, nullptr );
    pPB->pFuncClose = [this]()
    {
        aDroneAdapter.s = 0.0f;
        aDroneAdapter.pLegCurr = aDroneAdapter.pLeg0;
        aDroneAdapter.update(0.0f);
    };
    droneCS.pButtVec.push_back( pPB );

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

// Keep. Too much stuff to pass
bool lvl_perspective2::init_spinTargetControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is )
{
    if( pMS->sdataVec.size() != 5 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // Mass
    pSD->pSetFunc =     [this](float x){ PQS_1.M = x; };
    PQS_1.M = pSD->xCurr;
    pStrip->reInit( *pSD );
    // moment of inertia
    (++pSD)->pSetFunc = [this](float x){ PQS_1.I = x; };// scale is in degrees
    PQS_1.I = pSD->xInit;
    // PQSgravity
    (++pSD)->pSetFunc = [this](float x){ PQSgravity.y = -1.0f*x; };
    PQSgravity.y = -1.0f*pSD->xInit;
    // dragTorque x 1000
    (++pSD)->pSetFunc = [this](float x){ PQS_1.dragTorque = 1000.0f*x; };
    PQS_1.dragTorque = 1000.0f*pSD->xInit;
    // Rcm as fraction of w
    (++pSD)->pSetFunc = [this](float x){ PQS_1.Rcm = x*PQS_1.h; };
    PQS_1.Rcm = pSD->xInit*PQS_1.h;

    // toggle gravity
    sf::Vector2f Pos, Sz;
    is >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    Pos += spinTargetCS.bkRect.getPosition();
    buttonRect* pBR = new buttonRect( Pos.x, Pos.y, Sz.x, Sz.y, nullptr , "Gravity" );
    pBR->setSel( true );
    pBR->pHitFunc = [this,pBR](){ PQSgravityOn = pBR->sel; if( PQSgravityOn ) PQS_1.isSpinning = true; };
    spinTargetCS.pButtVec.push_back( pBR );

    // extras on spinTargetCS
    unsigned int fontSz; is >> fontSz;
    is >> Pos.x >> Pos.y;
    Pos += spinTargetCS.bkRect.getPosition();
    PQSangleMsg.setFont( *button::pFont );
    PQSangleMsg.setCharacterSize( fontSz );
    PQSangleMsg.setFillColor( sf::Color::White );
    PQSangleMsg.setString("angle");
    PQSangleMsg.setPosition( Pos );
    PQSangVelMsg = PQSangVelNumMsg = PQSangleNumMsg = PQSangleMsg;

    PQSangleNumMsg.setString("0.0");
    is >> Pos.x;
    Pos.x += spinTargetCS.bkRect.getPosition().x;
    PQSangleNumMsg.setPosition( Pos );

    PQSangVelMsg.setString("angVel");
    is >> Pos.x >> Pos.y;
    Pos += spinTargetCS.bkRect.getPosition();
    PQSangVelMsg.setPosition( Pos );

    PQSangVelNumMsg.setString("0.0");
    is >> Pos.x;
    Pos.x += spinTargetCS.bkRect.getPosition().x;
    PQSangVelNumMsg.setPosition( Pos );

    // spinLight
    float Rlt; is >> Rlt;
    spinLight.setRadius( Rlt );
    spinLight.setFillColor( sf::Color::Red );
    is >> Pos.x >> Pos.y;
    Pos += spinTargetCS.bkRect.getPosition();
    spinLight.setPosition( Pos );
    spinLight.setOrigin( Rlt, Rlt );


    spinTargetCS.updateAll = [this]()
    {

        if( PQS_1.isSpinning )
        {
            to_SF_string( PQSangVelNumMsg, PQS_1.angVel/(2.0f*vec2f::PI) );
            to_SF_string( PQSangleNumMsg, PQS_1.angle*180.0f/vec2f::PI );
        }

        static bool wasSpinning = false;
        if( wasSpinning != PQS_1.isSpinning )
        {
            spinLight.setFillColor( PQS_1.isSpinning ? sf::Color::Green : sf::Color::Red );
            if( !PQS_1.isSpinning ) PQSangVelNumMsg.setString("0.0");// just stopped
        }
        // for next time
        wasSpinning = PQS_1.isSpinning;
    };

    spinTargetCS.drawAll = [this]( sf::RenderTarget& RT )
    {
        RT.draw( PQSangleMsg );
        RT.draw( PQSangleNumMsg );
        RT.draw( PQSangVelMsg );
        RT.draw( PQSangVelNumMsg );
        RT.draw( spinLight );
    };

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_perspective2::init_terrain( const std::string& inFileName )
{
    std::ifstream fin( inFileName.c_str() );
    if( !fin ) { std::cout << "\nNo terrain data"; return false; }

    // 2 persBox_quad as template
    size_t SSnum; fin >> SSnum;
    persBox_quad a_Box( fin, SSvec[SSnum] );
    PBQ_toCopy.push_back( a_Box );
    fin >> SSnum;
    persBox_quad b_Box( fin, SSvec[SSnum] );
    PBQ_toCopy.push_back( b_Box );

    // nohitPQ
    char chA, chB;
    size_t setNum, frIdx; fin >> SSnum >> setNum >> frIdx >> chA >> chB;
    if( SSnum < SSvec.size() )
    {
        tree_toCopy.init( fin, &( SSvec[SSnum].txt ) );
        tree_toCopy.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    }

    tree_toCopy.facingCamera = true;

    fin >> SSnum >> setNum >> frIdx >> chA >> chB;
    if( SSnum < SSvec.size() )
    {
        treeBase_toCopy.init( fin, &( SSvec[SSnum].txt ) );
        treeBase_toCopy.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    }

    // hitPQ
    fin >> SSnum >> setNum >> frIdx >> chA >> chB;
    if( SSnum < SSvec.size() )
    {
        target_toCopy.init( fin, &( SSvec[SSnum].txt ) );
        target_toCopy.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
        persQuad& PQ = target_toCopy;
        PQ.setPosition( persPt::camPos + persPt::camDir*persPt::Z0 );
        PQ.update(0.0f);// to get vtx[].positions assigned

        for( unsigned int j = 0; j < 4; ++j )
        {
            std::cout << "\n pos = " << PQ.vtx[j].position.x << ", " << PQ.vtx[j].position.y;
            std::cout << "  tex = " << PQ.vtx[j].texCoords.x << ", " << PQ.vtx[j].texCoords.y;
        }

        // To support hitPixel() find the vtx with:
        unsigned int vtxTCmin = 0;// lowest texCoords
        for( unsigned int j = 1; j < 4; ++j )
        {
            if( PQ.vtx[j].texCoords.x + 1.0f < PQ.vtx[vtxTCmin].texCoords.x ) vtxTCmin = j;// lower in x
            if( PQ.vtx[j].texCoords.y + 1.0f < PQ.vtx[vtxTCmin].texCoords.y ) vtxTCmin = j;// lower in y
        }

        unsigned int vtxTCx = 0;// lowest y greater x
        unsigned int vtxTCy = 0;// lowest x greater y
        if( PQ.vtx[ (vtxTCmin+1)%4 ].texCoords.x > PQ.vtx[vtxTCmin].texCoords.x + 1.0f )// greater x
        {
            vtxTCx = (vtxTCmin+1)%4;// > x
            vtxTCy = (vtxTCmin+3)%4;// > y
        }
        else// other way
        {
            vtxTCx = (vtxTCmin+3)%4;
            vtxTCy = (vtxTCmin+1)%4;
        }

        // save as vtx*
        pVtxTCmin = &PQ.vtx[vtxTCmin];
        pVtxTCx = &PQ.vtx[vtxTCx];
        pVtxTCy = &PQ.vtx[vtxTCy];
        // check result
        std::cout << "\n TCmin = " << pVtxTCmin->texCoords.x << ", " << pVtxTCmin->texCoords.y;
        std::cout << "\n TCx   = " << pVtxTCx->texCoords.x << ", " << pVtxTCx->texCoords.y;
        std::cout << "\n TCy   = " << pVtxTCy->texCoords.x << ", " << pVtxTCy->texCoords.y;
    }

    //*** USAGE ***

    // PBQ
    size_t num; fin >> num;
    for( size_t j = 0; j < num; ++j )
    {
        vec3f Pos; fin >> Pos.x >> Pos.y >> Pos.z;
        Pos.y += 0.5f*a_Box.sz.y;
        a_Box.setPosition( Pos );
        PBQvec.push_back( a_Box );
    }
    fin >> num;
    for( size_t j = 0; j < num; ++j )
    {
        vec3f Pos; fin >> Pos.x >> Pos.y >> Pos.z;
        Pos.y += 0.5f*b_Box.sz.y;
        b_Box.setPosition( Pos );
        PBQvec.push_back( b_Box );
    }

    // trees
    fin >> num;
    for( size_t j = 0; j < num; ++j )
    {
        vec3f Pos; fin >> Pos.x >> Pos.y >> Pos.z;
        treeBase_toCopy.setPosition( Pos );
        nohitPQvec.push_back( treeBase_toCopy );

        Pos.y += 0.5f*tree_toCopy.h;
        tree_toCopy.setPosition( Pos );
        nohitPQvec.push_back( tree_toCopy );
    }

    // targets
    fin >> num;
    for( size_t j = 0; j < num; ++j )
    {
        vec3f Pos, Nu; fin >> Pos.x >> Pos.y >> Pos.z >> Nu.x >> Nu.y >> Nu.z;
        Pos.y += 0.5f*target_toCopy.h;
        target_toCopy.setPosition( Pos );
        target_toCopy.setNu( Nu );
        targetPQvec.push_back( target_toCopy );
    }

    // balls
    fin >> num;
    targetBallVec.reserve( num );
    for( size_t j = 0; j < num; ++j )
    {
        vec3f Pos; fin >> Pos.x >> Pos.y >> Pos.z;
        float R0; fin >> R0;
        unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
        targetBallVec.push_back( persBall( Pos, R0, sf::Color(rd,gn,bu) ) );
    }

    std::string checkStr; fin >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_perspective2::init_BasisMotion( const std::string& fileName )
{
    std::ifstream fin( fileName.c_str() );
    if( !fin ) { std::cout << "\nno " << fileName; return false; }

    BMbase.init( fin );
    BMbase.facingCamera = true;

    fin >> Rbm >> Rbm_1 >> thAngle >> thAngle_1 >>  phiAngle >>  phiAngle_1;

    fin >> cartBasis.t[0].x >> cartBasis.t[0].y >> cartBasis.t[0].z;
    fin >> cartBasis.t[1].x >> cartBasis.t[1].y >> cartBasis.t[1].z;
    fin >> cartBasis.t[2].x >> cartBasis.t[2].y >> cartBasis.t[2].z;

    float Rball; fin >> Rball;
    unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
    vec3f Pos = BMbase.pos + ( Rbm*cartBasis.sphPol_rHat( thAngle, phiAngle ) ).to_vec3f();
    BMball.init( Pos, Rball, sf::Color(rd,gn,bu) );

    tElapBM = 0.0f;
    fin >> rd >> gn >> bu;
    BMrod[0].color = BMrod[1].color = sf::Color(rd,gn,bu);
    BMrod[0].position = persPt::get_xyw( BMbase.pos );
    BMrod[1].position = BMball.B.getPosition();

//    BM_isMoving = true;

    return true;
}

void lvl_perspective2::updateBasisMotion( float dt )
{
    if( BM_isMoving )
    {
     //   tElapBM += dt;
     //   if( tElapBM*phiAngle_1 > 10.0f || tElapBM*phiAngle_1 < -10.0f ) tElapBM -= 10.0f/phiAngle_1;// limit range
        phiAngle += phiAngle_1*dt;
        BMball.pos = BMbase.pos + ( Rbm*cartBasis.sphPol_rHat( thAngle, phiAngle ) ).to_vec3f();
        vec3d thHat = cartBasis.sphPol_thHat( thAngle, phiAngle );
        vec3d phiHat = cartBasis.sphPol_phiHat( phiAngle );
        BMball.vel = ( Rbm*( thAngle_1*thHat + sin(thAngle)*phiAngle_1*phiHat ) ).to_vec3f();
    }

    BMball.update(dt);
    BMbase.update(dt);
    BMrod[0].position = persPt::get_xyw( BMbase.pos );
    BMrod[1].position = BMball.B.getPosition();
}

bool lvl_perspective2::init_aDrone( const std::string& fileName )
{
    std::ifstream fin( fileName.c_str() );
    if( !fin ) { std::cout << "\n init_aDrone() no file: " << fileName; return false; }

    size_t SSnum; fin >> SSnum;
    if( SSnum >= SSvec.size() ){ std::cout << "\n init_aDrone() SSnum too high: " << SSnum; return false; }

    aDrone.init( fin, &( SSvec[SSnum] ) );
    fin >> aDroneAdapter.v;

    // init Homer
    Homer.init(fin);
    Homer.pTarget = &aDrone;
    Homer.pGetAni = [this]( persBall& PB, vec3f Pos ){ return getAniQuad( PB, Pos ); };

    // init the adapter
    std::string fName; fin >> fName;
    if( !initDronePath( pDronePath, fName ) ) return false;


    //aDroneAdapter.init( *pDronePath, [this](vec3f Pos){ aDrone.droneQuad.pos = Pos; aDrone.updateGrav( gravity, 0.0f ); } );
    aDroneAdapter.init( *pDronePath, [this](vec3f Pos){ aDrone.pos = Pos; aDrone.updateGrav( gravity, 0.0f ); } );

    aDroneAdapter.pTu = &aDrone.Tu;
    aDroneAdapter.pVel = &aDrone.vel;
    aDroneAdapter.pAccel = &aDrone.accel;

    std::string checkStr; fin >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_perspective2::initDronePath( Leg3f*& pLeg0, const std::string& inFileName )
{
    std::ifstream fin( inFileName.c_str() );
    if( !fin ) { std::cout << "\n initDronePath() no file: " << inFileName; return false; }

    char endType; fin >> endType;// 'O' = open, 'C' = closed
    vec3f pos0;// start position
    fin >> pos0.x >> pos0.y >> pos0.z;

    unsigned int numLegs = 0; fin >> numLegs;
    if( numLegs == 0 ) return true;// done

    unsigned int numLinLegs = 0;
    unsigned int numCirLegs = 0;
    std::vector<linLeg3f*> pLLvec;
    std::vector<cirLeg3f*> pCLvec;
    Leg3f* pPrev = nullptr;

    for( unsigned int n = 0; n < numLegs; ++n )
    {
        char legType; fin >> legType;// 'L' = linLeg3f  'C' = cirLeg3f
        vec3f t0;

        switch( legType )
        {
            case 'L' :
            {
                linLeg3f* pLL = nullptr;
                float L;
                if( !pPrev )
                {
                    fin >> t0.x >> t0.y >> t0.z >> L;
                    t0 /= t0.mag();
                    pPrev = pLeg0 = pLL =  new linLeg3f( pos0, pos0 + L*t0 );// get 1st leg data
                }
                else
                {
                    bool newT0; fin >> newT0;

                    if( newT0 )
                    {
                        fin >> t0.x >> t0.y >> t0.z >> L;
                        t0 /= t0.mag();
                        pLL =  new linLeg3f( pPrev->posF, pPrev->posF + L*t0 );
                        pLL->prev = pPrev;
                        pPrev->next = pLL;
                        pPrev = pLL;
                    }
                    else
                    {
                        fin >> L;
                        vec3f posF = pPrev->posF + pPrev->T( pPrev->len )*L;
                        pPrev = pLL = new linLeg3f( *pPrev, posF );// remaining legs
                    }

                }

                ++numLinLegs;
                pLLvec.push_back( pLL );
                pos0 = pLL->posF;
                break;
            }

            case 'C' :
            {
                cirLeg3f* pCL = nullptr;
                vec3f nu;
                char Dir;
                float r, Angle;
                if( !pPrev )
                {
                    fin >> t0.x >> t0.y >> t0.z;
                    fin >> Dir;
                    fin >> r >> Angle;
                    pPrev = pLeg0 = pCL =  new cirLeg3f( pos0, t0, Dir, r, Angle );// get 1st leg data
                }
                else
                {
                    fin >> Dir;
                    fin >> r >> Angle;
                    pPrev = pCL = new cirLeg3f( *pPrev, Dir, r, Angle );// remaining legs

                }
                pos0 = pCL->posF;
                ++numCirLegs;
                pCLvec.push_back( pCL );
                break;
            }

            default : break;
        }// end switch
    }// end for

    std::cout << "\n init_3fPath(): numLinLegs = " << numLinLegs;

    // assign path as closed or open
    pPrev->next = endType == 'O' ? nullptr : pLeg0;
    if( pPrev->next ) { pPrev->next->prev = pPrev; }
//    L3f_adapter.init( *pLeg0, [this](vec3f pos){ persPt::camPos = pos; } );

    return true;
}

bool lvl_perspective2::init_Cannons( const std::string& fileName )
{
    std::ifstream fin( fileName.c_str() );
    if( !fin ) { std::cout << "\n init_Cannons() no file: " << fileName; return false; }
    // just 1 for starters
    aCannon.init( fin, firingCannon, [this]( vec3f Pos, vec3f Vel, bool MF ) { return fireBall( Pos, Vel, MF ); } );
    aCannon.pGetFireball = [this]( vec3f Pos ) { return getAniQuad( Pos ); };
    aCannon.pFireFragShot = [this]( vec3f Pos, vec3f Vel, float tFuse )
    {
        if( fragShotA.inUse ) return false;
        fragShotA.reset( Pos, Vel );
        fragShotA.tBoom = tFuse;
        return true;
    };

    std::string checkStr; fin >> checkStr;
    std::cout << '\n' << checkStr;
    return true;
}
