#include "lvl_insidePerspective.h"

// level functions
bool lvl_insidePerspective::init()
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

    std::ifstream fin("include/levels/lvl_insidePerspective/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    persPt::init_stat( fin );

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

    msg.setString( "Head" );
    msg.setPosition( posX, posY + 3.0f*offY );
    msgVec.push_back( msg );
    msg.setString( "" );
    msg.setPosition( posX + offX, posY + 3.0f*offY );
    msgNumVec.push_back( msg );

    fin >> rd >> gn >> bu;
    sf::Color crossColor(rd,gn,bu);// cross
    for( unsigned int n = 0; n < 4; ++n ) viewCross[n].color = crossColor;

    if( !loadSpriteSheets() ){ std::cout << "\n loadSpriteSheets() failed"; return false; }

    unsigned int numRooms; fin >> numRooms;
    if( numRooms > 0 )
    {
        pRoomVec.reserve( numRooms );
        std::string fName;
        std::ifstream finRoom;
        for( unsigned int n = 0; n < numRooms; ++n )
        {
            fin >> fName;
            finRoom.open( fName.c_str() );
            if( !finRoom.good() ){ std::cout << "\n init() could not open " << fName; return false; }
            persRoom* pPR = new persRoom( finRoom, SSvec );
            finRoom.close();
            pPR->restrictYposition = true;
            pRoomVec.push_back( pPR );
        }

    }

    // a test door
    unsigned int SSnum; fin >> SSnum;
    doorA.init( fin, SSvec[SSnum] );

    // 2 objects for a picture draw
    float aDist; fin >> aDist;
    fin >> SSnum;
    persBox_quad a_Box( fin, SSvec[SSnum] );
    float bDist; fin >> bDist;
    fin >> SSnum;
    persBox_quad b_Box( fin, SSvec[SSnum] );

    int test; fin >> test;
    std:: cout << "\n test = " << test;

    fin.close();//****************

    // *** experimental ***
    // adjust orientation (other than square)
    vec3f Tu(1.0f,1.0f,1.0f);
    Tu /= sqrtf(3.0f);
    a_Box.setFrontNu( Tu );
    Tu = vec3f(0.0f,-0.2f,1.0f);// tilt down?
    Tu /= Tu.mag();
    b_Box.setFrontNu( Tu );

    // 3 into aBoxVec
    a_Box.setPosition( doorA.pos + 3.0f*aDist*doorA.Nu + 300.0f*persPt::yHat );
    aBoxVec.push_back( a_Box );
    b_Box.setPosition( doorA.pos + 2.0f*aDist*doorA.Nu - 300.0f*persPt::xHat );
    aBoxVec.push_back( b_Box );
    a_Box.setPosition( doorA.pos + aDist*doorA.Nu );
    aBoxVec.push_back( a_Box );

    // 3 into bBoxVec
    b_Box.setPosition( doorA.pos - 3.0f*bDist*doorA.Nu - 500.0f*persPt::xHat );
    bBoxVec.push_back( b_Box );
    a_Box.setPosition( doorA.pos - 2.0f*bDist*doorA.Nu - 300.0f*persPt::yHat );
    bBoxVec.push_back( a_Box );
    b_Box.setPosition( doorA.pos - bDist*doorA.Nu );
    bBoxVec.push_back( b_Box );

    doorA.updateDrawFill();

    std::function<void( sf::RenderTarget&, std::vector<persBox_quad>& )> pF = []( sf::RenderTarget& RT, std::vector<persBox_quad>& rBoxVec )
    {
        for( persBox_quad& PBQ : rBoxVec )
        {
            PBQ.update(0.0f);
            PBQ.draw(RT);
        }
    };

    doorA.initFill( [this,pF]( sf::RenderTarget& RT ){ pF( RT, aBoxVec ); }, [this,pF]( sf::RenderTarget& RT ){ pF( RT, bBoxVec ); } );
    doorA.clearColor_inFront = sf::Color(0,200,0,160);
    doorA.clearColor_behind = sf::Color(0,0,200);
    // take initial picture
    doorA.fractionOpenCurr = 2.0f*persDoor::fracOpenMin;// fake this value
    doorA.updateDrawFill();// no draw 1st call as static camPos is established
    persPt::camPos += 2.0f*doorA.Nu;// change camera position
    doorA.updateDrawFill();// draw?
    doorA.fractionOpenCurr = 0.0f;// restore


    // the door in the 1st room
    if( !pRoomVec.empty() )
    {
        persRoom& rRoom = *( pRoomVec.front() );
        if( !rRoom.pDoorVec.empty() )
        {
            persDoor& rDoor = *( rRoom.pDoorVec.front() );
         //   rDoor.initFill( [this]( sf::RenderTarget& RT ){ updateDraw_outFromIn(RT); }, []( sf::RenderTarget& RT ){ return; } );
            rDoor.initFill( [this]( sf::RenderTarget& RT ){ updateDraw_outFromIn(RT); }, [&rRoom]( sf::RenderTarget& RT ){ rRoom.updateDrawInFromOut(RT); } );
            rDoor.clearColor_inFront = Level::clearColor;
            rDoor.clearColor_behind = sf::Color(0,0,200);
        }
    }

    // *** end exper ***

    if( !init_controls() ) return false;
    if( !init_outdoors() ) return false;

    std::cout << "\nlvl_insidePerspective::init() numPQ = " << PQvec_out.size();
    size_t totalSize = PQvec_out.size();
    totalSize += pRoomVec.size();// for roomA and roomB
    totalSize += aBoxVec.size();
    totalSize += bBoxVec.size();
    totalSize += 2;// for doorA, pyr1
    pPtSortVec_out.reserve( totalSize );
    for( persQuad& PQ : PQvec_out ) pPtSortVec_out.push_back( &PQ );
    for( persRoom* pPR : pRoomVec ) pPtSortVec_out.push_back( pPR );
    for( persBox_quad& PBQ : aBoxVec ) pPtSortVec_out.push_back( &PBQ );
    for( persBox_quad& PBQ : bBoxVec ) pPtSortVec_out.push_back( &PBQ );
    pPtSortVec_out.push_back( &doorA );
    pPtSortVec_out.push_back( &pyr1 );

    // initial state
    jbCamButt.atRest = false;// fake out so that positions are updated
    update( 0.0f );
    jbCamButt.atRest = true;// reality restored

    return true;
}

//void lvl_insidePerspective::updateDrawFill( float dt )
//{
  //  doorA.updateDrawFill(dt);
//}

void lvl_insidePerspective::updatePane()
{
    upperPane[2].position = trueHorizon[1].position;
    upperPane[3].position = trueHorizon[0].position;
}

bool lvl_insidePerspective::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if( rEvent.key.code == sf::Keyboard::Left ) move_LtRt = -1;
        else if( rEvent.key.code == sf::Keyboard::Right ) move_LtRt = 1;
        else if( rEvent.key.code == sf::Keyboard::Up ) move_UpDown = 1;
        else if( rEvent.key.code == sf::Keyboard::Down ) move_UpDown = -1;
        else if( rEvent.key.code == sf::Keyboard::R ) jbCamButt.dropStick = true;// release the joybutton
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        move_LtRt = move_UpDown = 0;
        jbCamButt.dropStick = false;
    }

    return true;
}

void lvl_insidePerspective::update_global( float dt )
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
}

void lvl_insidePerspective::updateDraw_outFromIn( sf::RenderTarget& RT )
{
    float U = persPt::camDir.dot( persPt::yHat );
    float yHorizon = persPt::Yh + persPt::Z0*U/sqrtf( 1.0f - U*U );

    trueHorizon[0].position.y = trueHorizon[1].position.y = yHorizon;
    updatePane();
    for( persTrail& rPT : trailVec ) rPT.update(0.0f);
    for( persQuad& rPQ : PQvec_out ) rPQ.update(0.0f);

    for( persRoom* pPR : pRoomVec )
    {
        pPR->update_amInsideRoom();
        if( !pPR->amInsideRoom )
        {
            for( persWall* pPW : pPR->pWallVec ) pPW->update(0.0f);
            pPR->update_outdoors(0.0f);
            pPR->update_doDraw();
            if( pPR->doDraw ) for( persQuad& PQ : pPR->PQwallVec ) PQ.update(0.0f);
        }
    }


    unsigned int N = 0;
    for( persQuad& rPQ : PQvec_out )
    { if( rPQ.doDraw && N < pPtSortVec_out.size() ) pPtSortVec_out[ N++ ] = &rPQ; }
    for( persRoom* pPR : pRoomVec )
    { if( pPR->doDraw && !pPR->amInsideRoom && N < pPtSortVec_out.size() ) pPtSortVec_out[ N++ ] = pPR; }
    for( persBox_quad& PBQ : aBoxVec )
    { if( PBQ.doDraw && N < pPtSortVec_out.size() ) pPtSortVec_out[ N++ ] = &PBQ; }
    for( persBox_quad& PBQ : bBoxVec )
    { if( PBQ.doDraw && N < pPtSortVec_out.size() ) pPtSortVec_out[ N++ ] = &PBQ; }
    if( doorA.doDraw && N < pPtSortVec_out.size() ) pPtSortVec_out[ N++ ] = &doorA;

    if( N > 1 ) std::sort( pPtSortVec_out.begin(), pPtSortVec_out.begin() + N, persPt::compare );

    RT.draw( upperPane, 4, sf::Quads );
    RT.draw( trueHorizon, 2, sf::Lines );
    for( const persTrail& rPT : trailVec ) rPT.draw(RT);
    for( unsigned int i = 0; i < N; ++i ) pPtSortVec_out[i]->draw(RT);
}

void lvl_insidePerspective::update( float dt )
{
    if( !jbCamButt.atRest )
    {
        update_global(dt);// camera position and orientation and "live" messages
        amOutside = true;
        for( persRoom* pPR : pRoomVec )
        {
            pPR->update(dt);
            if( pPR->amInsideRoom ) amOutside = false;
        }

        // outdoors: sort items for draw
        if( amOutside )
        {
            update_outdoors(dt);
            pyr1.update(dt);
     //   updateDrawFill(dt);
        //    for( persBox_quad& PBQ : aBoxVec ) PBQ.update(dt);
        //    for( persBox_quad& PBQ : bBoxVec ) PBQ.update(dt);

            numToDrawOut = 0;
            for( persQuad& rPQ : PQvec_out )
            { if( rPQ.doDraw && numToDrawOut < pPtSortVec_out.size() ) pPtSortVec_out[ numToDrawOut++ ] = &rPQ; }

            for( persRoom* pPR : pRoomVec )
            { if( pPR->doDraw && numToDrawOut < pPtSortVec_out.size() ) pPtSortVec_out[ numToDrawOut++ ] = pPR; }

            for( persBox_quad& PBQ : aBoxVec )
            { if( PBQ.doDraw && numToDrawOut < pPtSortVec_out.size() ) pPtSortVec_out[ numToDrawOut++ ] = &PBQ; }
            for( persBox_quad& PBQ : bBoxVec )
            { if( PBQ.doDraw && numToDrawOut < pPtSortVec_out.size() ) pPtSortVec_out[ numToDrawOut++ ] = &PBQ; }

            if( doorA.doDraw && numToDrawOut < pPtSortVec_out.size() ) pPtSortVec_out[ numToDrawOut++ ] = &doorA;
            if( pyr1.doDraw && numToDrawOut < pPtSortVec_out.size() ) pPtSortVec_out[ numToDrawOut++ ] = &pyr1;

            if( numToDrawOut > 1 ) std::sort( pPtSortVec_out.begin(), pPtSortVec_out.begin() + numToDrawOut, persPt::compare );
        }
    }

    // update if outside
    if( amOutside )
    {
        doorA.update(dt);// animation continues
     //   pyr1.update(dt);
     //   updateDrawFill(dt);
        for( persBox_quad& PBQ : aBoxVec ) PBQ.update(dt);
        for( persBox_quad& PBQ : bBoxVec ) PBQ.update(dt);

    }
    // update always
    for( persRoom* pPR : pRoomVec ) pPR->update_always(dt);

}

void lvl_insidePerspective::update_outdoors( float dt )
{
    float U = persPt::camDir.dot( persPt::yHat );
    float yHorizon = persPt::Yh + persPt::Z0*U/sqrtf( 1.0f - U*U );

    trueHorizon[0].position.y = trueHorizon[1].position.y = yHorizon;
    updatePane();
    for( persTrail& rPT : trailVec ) rPT.update(dt);
    for( persQuad& rPQ : PQvec_out ) rPQ.update(dt);

    for( persRoom* pPR : pRoomVec ) pPR->update_outdoors(dt);
}

void lvl_insidePerspective::draw( sf::RenderTarget& RT ) const
{
    if( amOutside )
    {
        RT.draw( upperPane, 4, sf::Quads );
        RT.draw( trueHorizon, 2, sf::Lines );
        for( const persTrail& rPT : trailVec ) rPT.draw(RT);
        for( unsigned int i = 0; i < numToDrawOut; ++i ) pPtSortVec_out[i]->draw(RT);

        RT.draw( viewCross, 4, sf::Lines );
    }
    else
    {
        for( const persRoom* pPR : pRoomVec )
            if( pPR->amInsideRoom ) pPR->draw(RT);
    }

    // always over
    for( const sf::Text& txt : msgVec ) RT.draw( txt );
    for( const sf::Text& txt : msgNumVec ) RT.draw( txt );

    return;
}

bool lvl_insidePerspective::init_outdoors()
{
    std::ifstream fin("include/levels/lvl_insidePerspective/outdoors_data.txt");
    if( !fin ) { std::cout << "\nNo outdoors data"; return false; }

    unsigned int numPQ; fin >> numPQ;
    PQvec_out.reserve( numPQ );
    for( unsigned int j = 0; j < numPQ; ++j )
    {
        float W, H;
        vec3f Pos, Nu; fin >> Pos.x >> Pos.y >> Pos.z >> W >> H >> Nu.x >> Nu.y >> Nu.z;
        size_t SSnum, SetNum, FrIdx; fin >> SSnum >> SetNum >> FrIdx;
        char chA, chB; fin >> chA >> chB;
        bool faceCam; fin >> faceCam;
        PQvec_out.push_back( persQuad( Pos, W, H, Nu, sf::Color::White, &( SSvec[SSnum].txt ) ) );
        PQvec_out.back().setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), chA, chB );
        PQvec_out.back().facingCamera = faceCam;
    }

    // trails
    unsigned int numTrails; fin >> numTrails;
    trailVec.reserve( numTrails );
    unsigned int rd, gn, bu;
    vec3f StartPos, EndPos;
    float Width;

    for( unsigned int j = 0; j < numTrails; ++j )
    {
        fin >> StartPos.x >> StartPos.y >> StartPos.z >> EndPos.x >> EndPos.y >> EndPos.z;
        fin >> Width;
        fin >> rd >> gn >> bu;// edge
        sf::Color edgeColor(rd,gn,bu);
        fin >> rd >> gn >> bu;// surface
        sf::Color surfaceColor(rd,gn,bu);
        trailVec.push_back( persTrail( StartPos, EndPos, Width, edgeColor, surfaceColor ) );
    }

    pyr1.init( fin );

    std::string checkStr; fin >> checkStr;
    std::cerr << '\n' << checkStr;

    return true;
}

bool lvl_insidePerspective::init_controls()
{
    std::ifstream fin("include/levels/lvl_insidePerspective/control_data.txt");
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

    sf::Vector2f HBpos, HBsz, sfcOfst;
    fin >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    sf::Vector2f SzSurf; fin >> SzSurf.x >> SzSurf.y;

    sf::Vector2f PosSurf( HBpos + sfcOfst );
    multiCS.init( HBpos, PosSurf, SzSurf, "multi", HBsz.x, HBsz.y );
    button::RegisteredButtVec.push_back( &multiCS );
    multiCS.ownButts = true;


    fin >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, fin );
    pMS->ownsStrip = true;
    multiCS.pButtVec.push_back( pMS );

    if( pMS->sdataVec.size() != 6 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // camSpeed
    pSD->pSetFunc =     [this](float x){ camSpeed = x; };
    camSpeed = pSD->xCurr;
    pStrip->reInit( *pSD );
    //  move_xyVel
    (++pSD)->pSetFunc = [this](float x){ move_xyVel = x; };
    move_xyVel = pSD->xInit;
    //  doorA.startMotion( x )
    (++pSD)->pSetFunc = [this](float x){ doorA.startMotion( x ); };
    doorA.fractionOpen = pSD->xInit;
    //  doorA.moveSpeed
    (++pSD)->pSetFunc = [this](float x){ doorA.moveSpeed = x; };
    doorA.moveSpeed = pSD->xInit;
    //  doorB.startMotion( x )
    persDoor& rPD = *( pRoomVec.front()->pDoorVec.front() );
    (++pSD)->pSetFunc = [&rPD](float x){ rPD.startMotion( x ); };
    rPD.fractionOpen = pSD->xInit;
    //  doorA.moveSpeed
    (++pSD)->pSetFunc = [&rPD](float x){ rPD.moveSpeed = x; };
    rPD.moveSpeed = pSD->xInit;

    // a button to toggle fill view
    fin >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonRect* pButt = new buttonRect( posX, posY, 50.0f, 30.0f, nullptr, "FILL" );
    pButt->pHitFunc = [this, pButt](){ doorA.set_doDrawFill( pButt->sel ); };
    pButt->setSel( true );
    multiCS.pButtVec.push_back( pButt );

    std::string checkStr; fin >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

// utility
bool lvl_insidePerspective::loadSpriteSheets()
{
    std::ifstream finList("include/levels/lvl_insidePerspective/SSfileList.txt");
    if( !finList ) { std::cout << "\nno ssFileList"; return false; }

    std::string rootName;// directory path from file
    finList >> rootName;
    int numSS = 0; finList >> numSS;
    if( numSS < 1 ) return false;
    SSvec.reserve( numSS );

    std::string fileName;
    while( numSS-- > 0 && finList >> fileName )
    {
  //      fileName = "include/levels/lvl_perspective/images/" + fileName;
        fileName = rootName + fileName;
        std::ifstream f_in( fileName.c_str() );
        if( !f_in ) { std::cout << "\nno " << fileName; return false; }
        SSvec.push_back( spriteSheet( f_in ) );
        f_in.close();
    }

    // apply transparency masks?
    size_t numToMask = 0; finList >> numToMask;
    if( numToMask > 0 )
    {
        for( size_t i = 0; i < numToMask; ++i )
        {
            size_t idx = 0; finList >> idx;
            unsigned int rd, gn, bu; finList >> rd >> gn >> bu;
            if( idx < SSvec.size() )
            {
                SSvec[idx].img.createMaskFromColor( sf::Color(rd,gn,bu) );
                SSvec[idx].txt.loadFromImage( SSvec[idx].img );
            }
        }
    }

    finList.close();
    std::cout << "\n#SS = " << SSvec.size();
    return true;
}
