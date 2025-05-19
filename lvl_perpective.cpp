#include "lvl_perpective.h"

bool lvl_perpective::useSounds = false;
float lvl_perpective::soundAttenuDistance = 2000.0f;// volume = xxSoundVolume*( soundAttenuDistance/sep.mag() )
float lvl_perpective::soundVolumeMin = 10.0f;

bool lvl_perpective::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color::Black;
    button::setHoverBoxColor( Level::clearColor );
    // static members
    persPt::camPos.x = persPt::camPos.y = persPt::camPos.z = 0.0f;
    persPt::camDir.x = 0.0f; persPt::camDir.y = 0.0f; persPt::camDir.z = 1.0f;
    persPt::yu.x = 0.0f; persPt::yu.y = 1.0f; persPt::yu.z = 0.0f;
    persPt::xu.x = 1.0f; persPt::xu.y = 0.0f; persPt::xu.z = 0.0f;
    persPt::angle = persPt::pitchAngle = persPt::rollAngle = 0.0f;

    std::ifstream fin("include/levels/lvl_perspective/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    fin >> persPt::Z0;
    fin >> persPt::camPos.x >> persPt::camPos.y >> persPt::camPos.z;
    fin >> persPt::camDir.x >> persPt::camDir.y >> persPt::camDir.z;
    persPt::xu = persPt::yHat.cross( persPt::camDir );
    persPt::xu /= persPt::xu.mag();
    persPt::yu = persPt::camDir.cross( persPt::xu );

    fin >> pipLength;// for horizonCross
    persPt::X0 = Level::winW/2.0f; persPt::Yh = Level::winH/2.0f;
    fin >> camVelXZscale;

    fin >> camAccel >> yawRate >> pitchRate >> rollRate;
    fin >> gravity.x >> gravity.y >> gravity.z;
    fin >> tFireDelay >> fireOfst >> fireVel;

    // horizon line
    horizonCross[0].position.x = 0.8f*persPt::X0;// persPt::X0 - Level::winW/2.0f;
    horizonCross[1].position.x = 1.2f*persPt::X0;// persPt::X0 + Level::winW/2.0f;
    horizonCross[0].position.y = horizonCross[1].position.y = persPt::Yh;
    // vertical pip
    horizonCross[2].position.y = persPt::Yh - pipLength/2.0f;
    horizonCross[3].position.y = persPt::Yh + pipLength/2.0f;
    horizonCross[2].position.x = horizonCross[3].position.x = persPt::X0;

    trueHorizon[0].color = trueHorizon[1].color = sf::Color::Blue;
    trueHorizon[0].position.y = trueHorizon[1].position.y = persPt::Yh;
    trueHorizon[0].position.x = 0.0f;
    trueHorizon[1].position.x = 2.0f*persPt::X0;

    // window panes
    unsigned int rd, gn, bu;
    fin >> rd >> gn >> bu;
    for( size_t i = 0; i < 4; ++i ) upperPane[i].color = sf::Color(rd,gn,bu);
    fin >> rd >> gn >> bu;
    for( size_t i = 0; i < 4; ++i ) lowerPane[i].color = sf::Color(rd,gn,bu);
    upperPane[0].position.x = 0.0f;        upperPane[0].position.y = 0.0f;// upper left
    upperPane[1].position.x = Level::winW; upperPane[1].position.y = 0.0f;// upper right
    lowerPane[2].position.x = Level::winW; lowerPane[2].position.y = Level::winH;// lower right
    lowerPane[3].position.x = 0.0f;        lowerPane[3].position.y = Level::winH;// lower left
    updatePane();// assigns remaining positions

    // messages
    float posX, posY, ofstY; fin >> posX >> posY >> ofstY;
    unsigned int charSz; fin >> charSz;
    camPosMsg.setFont( *button::pFont );
    camPosMsg.setCharacterSize( charSz );
    camPosMsg.setFillColor( sf::Color::White );
    camXmsg = camYmsg = camZmsg = angleMsg = angleNumMsg = camPosMsg;
    camPosMsg.setString( "camPos" );
    camPosMsg.setPosition( posX, posY );
    to_SF_string( camXmsg, persPt::camPos.x );
    camXmsg.setPosition( posX, posY + ofstY );
    to_SF_string( camYmsg, persPt::camPos.y );
    camYmsg.setPosition( posX, posY + 2.0f*ofstY );
    to_SF_string( camZmsg, persPt::camPos.z );
    camZmsg.setPosition( posX, posY + 3.0f*ofstY );
    fin >> posX;// angle messages at same elevation as position
    float posXnum; fin >> posXnum;
    angleMsg.setString( "heading" );
    angleMsg.setPosition( posX, posY );
    to_SF_string( angleNumMsg, persPt::angle );
    angleNumMsg.setPosition( posXnum, posY );
    speedMsg = speedNumMsg = camPosMsg;
    speedMsg.setString( "camVel" );
    speedMsg.setPosition( posX, posY + ofstY );
    to_SF_string( speedNumMsg, camVel );
    speedNumMsg.setPosition( posXnum, posY + ofstY );
    pitchMsg = pitchNumMsg = rollMsg = rollNumMsg = camPosMsg;
    pitchMsg.setString( "pitch" );
    pitchMsg.setPosition( posX, posY + 2.0f*ofstY );
    to_SF_string( pitchNumMsg, persPt::camDir.dot( persPt::yHat ) );
    pitchNumMsg.setPosition( posXnum, posY + 2.0f*ofstY );
    rollMsg.setString( "roll" );
    rollMsg.setPosition( posX, posY + 3.0f*ofstY );
    to_SF_string( rollNumMsg, persPt::xu.dot( persPt::yHat ) );
    rollNumMsg.setPosition( posXnum, posY + 3.0f*ofstY );

    hiLoAngleMsg.setString( getHighAngle ? "High" : "Low" );
    hiLoAngleMsg.setPosition( horizonCross[0].position.x, horizonCross[0].position.y - 20.0f );
    hiLoAngleMsg.setFont( *button::pFont );
    hiLoAngleMsg.setCharacterSize( 18 );
    hiLoAngleMsg.setFillColor( sf::Color::Green );
    fireAngleNumMsg = hiLoAngleMsg;
    fireAngleNumMsg.setPosition( horizonCross[0].position.x + 50.0f, horizonCross[0].position.y - 20.0f );
    fireAngleNumMsg.setString( "----" );
    fireAngleNumMsg.setFillColor( sf::Color::Red );
    helpMsg.setString( "Help" );
    helpMsg.setFont( *button::pFont );
    helpMsg.setCharacterSize( 14 );
    helpMsg.setFillColor( sf::Color::Green );
    fin >> posX >> posY;
    helpMsg.setPosition( posX, posY );

    lookAroundMsg = hiLoAngleMsg;
    lookAroundMsg.setFillColor( sf::Color::Green );
    lookAroundMsg.setCharacterSize( 14 );
    lookAroundMsg.setString( "Look Around" );
    lookAroundMsg.setPosition( horizonCross[0].position.x, horizonCross[0].position.y + 4.0f );

    numShotsMsg = numShotsNumMsg = helpMsg;
    float ofstX; fin >> posX >> posY >> ofstX;
    numShotsMsg.setString( "shots:" );
    numShotsMsg.setPosition( posX, posY );
    numShotsNumMsg.setString( "0" );
    numShotsNumMsg.setPosition( posX + ofstX, posY );

    fin >> dtFireMsg;// how long to display firing angle message

    // lines
    lineVec.push_back( persLine( fin ) );
    lineVec.push_back( persLine( fin ) );

    // balls
    size_t numBalls; fin >> numBalls;
    ballVec.reserve( numBalls );
    if( numBalls > 0 ) ballVec.push_back( persBall(fin) );
    persBall& rPB = ballVec.back();
    persBall* pPB = &rPB;
    rPB.B.setOutlineColor( sf::Color::Black );
    float outThick; fin >> outThick;
    rPB.B.setOutlineThickness( outThick );

    for( size_t i = 0; i < numBalls; ++i )
    {
        if( i > 0 ){ ballVec.push_back( rPB ); pPB = &ballVec.back(); }

        pPB->isMoving = true;
        pPB->inUse = false;
    }

    superBall = rPB;
    vec3f Pos; fin >> Pos.x >> Pos.y >> Pos.z;
    float Rsb; fin >> Rsb;
    superBall.Rbound = Rsb;
    superBall.B.setFillColor( sf::Color(100,0,200) );
    superBall.B.setRadius( Rsb );
    superBall.B.setOrigin( Rsb, Rsb );
    superBall.setPosition( Pos );
    superBall.inUse = true;
    fin >> superBall.mass;
    fin >> SBdragCoeff >> superBall.isMoving;
    superBall.vel = vec3f();
    SBpos = superBall.pos;

    calcBall = rPB;
    calcBall.B.setFillColor( sf::Color::Green );
    calcBall.isMoving = false;
    calcBall.inUse = false;
    fin >> tFlightMax >> cbKaccel >> cbSpeed0;

    harmOsciBall = rPB;
    fin >> HOpos.x >> HOpos.y >> HOpos.z;
    harmOsciBall.setPosition( HOpos );
    fin >> rd >> gn >> bu;
    harmOsciBall.B.setFillColor( sf::Color(rd,gn,bu) );
    harmOsciBall.B.setOutlineColor( sf::Color::Black );
    harmOsciBall.B.setOutlineThickness( 1.0f );
    harmOsciBall.isMoving = true;
    harmOsciBall.inUse = true;
    harmOsciBall.vel = vec3f();
    // balls for the box
    fin >> numBalls;
    std::cerr << "\n numBalls = " << numBalls;
    freeBallVec.reserve( numBalls );
    persBall tempBall( fin );// the template
    tempBall.inUse = true;
    tempBall.isMoving = false;
    tempBall.B.setOutlineColor( sf::Color::Black );
    fin >> outThick;
    fin >> tempBall.mass;
    tempBall.B.setOutlineThickness( outThick );
    sf::Color CLR = tempBall.B.getFillColor();
    for( size_t i = 0; i < numBalls; ++i )
    {
        if( CLR.r < 255 - 20 ) CLR.r += 20;
        if( CLR.b > 20 ) CLR.b -= 20;
        tempBall.B.setFillColor( CLR );
        freeBallVec.push_back( tempBall );// assign positions after multiBallBox.init() below
    }
    freeGrav = vec3f();

    // fragShotA
    shotBurst::init_stat();
    shotBurst::pFireShot = [this]( vec3f Pos, vec3f Vel, bool MF )
    {
        bool retVal = fireBall( Pos, Vel, MF );
        return retVal;
    };
    shotBurst::pGetFireBall = [this]( shotBurst& rSB )
    {
        bool retVal = getAniQuad( rSB, rSB.pos );
        if( lvl_perpective::useSounds && retVal )
            playShotSound( fragBoomSoundBuff, fragBoomSoundVolume, rSB.pos );

        return retVal;
    };

    fragShotA.init( fin );
//    fragShotA.fuseMode = 'E';
//    fragShotA.Rdet = 500.0f;

    std::cerr << "\n init_controls() B";
    // persBox_wf
    size_t numBoxes; fin >> numBoxes;
    boxVec.reserve( numBoxes + 27 );
    std::cerr << "\n numBoxes = " << numBoxes;

    for( size_t i = 0; i < numBoxes; ++i ) boxVec.push_back( persBox_wf(fin) );
    // a ring of 12 pb_wf
    float Rring; fin >> Rring;
    fin >> Pos.y;
    vec3f Sz; fin >> Sz.x >> Sz.y >> Sz.z;
    float a = 0.0f;
    for( int i = 0; i < 24; ++i )
    {
        if( i == 12 ){ a = 0.0f; Rring *= 5.0f; Sz.x *= 3.0f; }
        Pos.x = persPt::X0 + Rring*sinf(a);
        Pos.z = persPt::Z0 + Rring*cosf(a);
        boxVec.push_back( persBox_wf(Pos, Sz, sf::Color::Red) );
        persBox_wf& rpbwf = boxVec.back();
        // rotate box to face inward
        rpbwf.rotateAxis( persPt::yHat, a );
        a += myPI/6.0f;
    }

    // north pole
    Pos.x = 0.0f;
 //   Pos.y = 200000.0f;
    fin >> Pos.y;
    Pos.z = persPt::Z0;
    boxVec.push_back( persBox_wf( Pos, 10.0f*Sz, sf::Color::White ) );
    // south pole
    Pos.y *= -1.0f;
    boxVec.push_back( persBox_wf( Pos, 10.0f*Sz, sf::Color::White ) );
    // a cage for harmOsciBall
    fin >> Sz.x; Sz.z = Sz.y = Sz.x;
 //   Sz.x = Sz.y = Sz.z = 8.0f*harmOsciBall.Rbound;
    boxVec.push_back( persBox_wf( HOpos, Sz, sf::Color::White ) );

    // persBox_quad
    if( !loadSpriteSheets() ) return false;
    if( !init_boxes() ) return false;
    persIsland::pBoxTemplateVec = &boxTemplatesVec;// to copy from
    persIsland::pAnyShots_inUse = &anyShots_inUse;
    persIsland::pShotVec = &ballVec;
    persIsland::pAniVec = &quadAniVec;
    persIsland::pGetAniQuad = [this]( persBall& PB, vec3f Pos )
    {
        bool gotAQ = getAniQuad( PB, Pos );
        if( gotAQ ) playShotSound( shotBoomSoundBuff, shotBoomSoundVolume, PB.pos );
        return gotAQ;
    };
    // a dedicated instance - box bounds collision zone for multiple balls
    float V0; fin >> V0;
    multiBallBox.init( fin );
    // assign ball positions within box
    setFreeBallPositions();

    // persQuad
    quadVec.push_back( persQuad() );
    // use this Quad as the target
    pBounceQuad = &quadVec.back();
    size_t SSnum; fin >> SSnum;
    if( SSnum < SSvec.size() )
    {
        pBounceQuad->init( vec3f(-500.0f,600.0f,4200.0f), 160.0f, 160.0f, vec3f(0.0f, 0.0f, 1.0f), sf::Color::White, &(SSvec[SSnum].txt) );
        pBounceQuad->setTxtRect( SSvec[SSnum].getFrRect(0,0), 'R', '0' );
    }

    // persQuadAni
    size_t numQuads; fin >> numQuads;
    quadAniVec.reserve( numQuads );
    if( numQuads > 0 )
    {
        quadAniVec.push_back( persQuadAni( fin, SSvec[2] ) );
        persQuadAni& rPQA = quadAniVec.back();
        rPQA.setTxtRect( SSvec[2].getFrRect(0,rPQA.setNum), 'R', '0' );
        rPQA.inUse = false;
        rPQA.facingCamera = true;
        for( size_t i = 1; i < numQuads; ++i )
            quadAniVec.push_back( rPQA );
    }

    fin >> SSnum;
    if( SSvec.size() > SSnum )
    {
        messagePQA.init( fin, SSvec[ SSnum ] );
        messagePQA.setTxtRect( SSvec[ SSnum ].getFrRect(0,messagePQA.setNum), 'R', '0' );
        messagePQA.inUse = true;
        for( size_t i = 0; i < 4; ++i )
        {
            messagePQA.pt[i] -= messagePQA.pos;
            messagePQA.pt[i] = messagePQA.pt[i].rotate_axis( vec3f(0.0f,0.0f,1.0f), -myPI/2.0f );
            messagePQA.pt[i] += messagePQA.pos;
        }
    }

    // hyper jump
    size_t numPortalPairs; fin >> numPortalPairs;
    hyperVec_1.reserve( numPortalPairs );
    hyperVec_2.reserve( numPortalPairs );
    for( size_t i = 0; i < numPortalPairs; ++i )
    {
        fin >> SSnum;
        if( SSnum >= SSvec.size() ) SSnum = 0;
        hyperVec_1.push_back( persQuad( fin, &(SSvec[SSnum].txt) ) );
        hyperVec_1.back().setTxtRect( SSvec[SSnum].getFrRect(0,0), 'R', '1' );
        fin >> SSnum;
        if( SSnum >= SSvec.size() ) SSnum = 0;
        hyperVec_2.push_back( persQuad( fin, &(SSvec[SSnum].txt) ) );
        hyperVec_2.back().setTxtRect( SSvec[SSnum].getFrRect(0,0), 'R', '1' );
    }

    // persCurve
 //   float A, B, Zmax, fN; fin >> A >> B >> Zmax >> fN;// fN = # of oscillations in z
 //   std::function<void(float,vec3f&)> p_InitFunc = [A,B,Zmax,fN]( float par, vec3f& Pos )
 //   { Pos.x = A*cosf(par); Pos.y = B*sinf(par); Pos.z = Zmax*cosf( fN*par ); };
  //  curve1.init( fin, p_InitFunc );

    size_t frSet, frIdx; fin >> SSnum >> frSet >> frIdx;
    Pend.init( fin );
    Pend.showL = true;
    Pend.bob.B.setOutlineColor( sf::Color::Black );
    Pend.bob.B.setOutlineThickness( 1.0f );
    std::cerr << "\n init() A";
    if( SSnum < SSvec.size() )
    {
        Pend.topQ.pTxt = &( SSvec[SSnum].txt );
        Pend.topQ.setTxtRect( SSvec[SSnum].getFrRect( frIdx, frSet ), 'R', '0' );
    }
    std::cerr << "\n init() B";

    Pend.bob.inUse = true;
    Pend.bob.isMoving = false;

    if( !init_terrain() ) return false;
    if( !init_paths() ) return false;// call after init_boxes

    // NEW vector of islands from list of files
    std::string fName_path; fin >> fName_path;
    std::string fName_file = "fNames.txt";
    fName_file = fName_path + fName_file;
    std::ifstream finList( fName_file.c_str() );
    if( !finList ) { std::cout << "\nNo island file names"; return false; }
    size_t Nislands; finList >> Nislands;
    pIslandVec.reserve( Nislands );
    for( size_t i = 0; i < Nislands; ++i )// each island init data file
    {
        finList >> fName_file;
        fName_file = fName_path + fName_file;
        pIslandVec.push_back( new persIsland( fName_file, SSvec ) );
    }
    finList.close();
    pSpecialIsland = pIslandVec.front();

    // mvAcc
    fin >> fName_path;
    finList.open( fName_path.c_str() );
    if( !finList ) { std::cout << "\nNo move accel file names"; return false; }
    mvAdaptAcc.init( finList, nullptr );
    finList.close();

    std::cerr << "\n init() C";
    if( !init_controls() ) return false;
    std::cerr << "\n init() D";
    if( !init_3fPath() ) return false;// init droneQuad after this call
    if( !init_shadows() ) return false;
    shadow4_PQB.init( testPBQ, sHat, shadowColor );

    lvl_perpective::useSounds = false;
    if( !init_sounds() ) return false;



    init_zOrder();

    // droneQuad
    droneQuad.init( fin, nullptr );
    size_t numEle;// in the queue
    fin >> L3f_adapterDrone.v >> droneElevation >> droneAccFactor;
    L3f_adapterDrone.init( *pCoasterPath, [this](vec3f pos){ droneQuad.setPosition( pos + droneElevation*persPt::yHat ); } );
 //   L3f_adapterDrone.pLeg0 = L3f_adapterDrone.pLegCurr = nullptr;

    fin >> numEle;
    size_t SSnumDrone; fin >> SSnumDrone;
    char Rn; fin >> Rn;
    if( SSnumDrone < SSvec.size() )
    {
        spriteSheet& rSS = SSvec[ SSnumDrone ];
        droneQuad.pTxt = &( rSS.txt );
        droneQuad.setTxtRect( rSS.getFrRect( 0, 0 ), 'R', Rn );
    }

    for( size_t j = 0; j < numEle; ++j )
        droneAccelQueue.push( vec3f() );

    mvAdaptAcc.pSetPosAcc = [this]( vec3f Pos, vec3f Acc )
    {
        droneQuad.setPosition( Pos );
        vec3f Vel = mvAdaptAcc.vel;
        float velMag = Vel.mag();
        Vel = (velMag > 1.0f) ? Vel/velMag : vec3f(0.0f,1.0f,0.0f);

        Acc -= gravity;
        float accMag = Acc.mag();
        if( accMag > 0.1f )
        {
            Acc /= accMag;
            droneQuad.setOrientation( Acc, Vel );
        }
        else
        {
            droneQuad.setOrientation( vec3f(0.0f,1.0f,0.0f), Vel );
        }
    };

    // view while on coaster
    fin >> camDirQ_sz >> trackLookAhead;
    fin >> trackYawRange >> trackPitchRange;

    // check
    float R; fin >> R; std::cout << "\ninit(): R = " << R;

    // the helpMsg
    std::string helpStr, inStr;
    if( getline( fin, inStr) )
    {
        helpStr = inStr + '\n';
        while( getline( fin, inStr) ) helpStr += '\n' + inStr;
        helpMsg.setString( helpStr.c_str() );
    }

    fin.close();

//    for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);
//    pPtVec_4list = p_persPtVec;

    return true;
}// end of init()

void lvl_perpective::init_zOrder()
{
    size_t totalSize = boxVec.size() + quadVec.size() + quadAniVec.size() + ballVec.size();
 //   totalSize += quadBoxVec.size();
    totalSize += shAdaptVec.size();
    totalSize += terrainPQvec.size() + CarVec.size();
    totalSize += pIslandVec.size();
    totalSize += hyperVec_1.size() + hyperVec_2.size();
    totalSize += freeBallVec.size();
    totalSize += shadowVec.size();
    totalSize += 4;// for messagePQA, shadow4_PQB, fragShotA, droneQuad
    totalSize += 5;// for superBall, calcBall, harmOsciBall, multiBallBox, Pend
    p_persPtVec.reserve( totalSize );
    for( persBox_wf& box : boxVec ) p_persPtVec.push_back( &box );
//    for( persBox_quad& Qbox : quadBoxVec ) p_persPtVec.push_back( &Qbox );
    for( shadowAdapter& rSA : shAdaptVec ) p_persPtVec.push_back( &rSA );
    for( persQuad& PQ : quadVec ) p_persPtVec.push_back( &PQ );

    for( persQuad& PQ : terrainPQvec ) p_persPtVec.push_back( &PQ );
    for( persBox_quad& PQB : CarVec ) p_persPtVec.push_back( &PQB );
    for( persIsland* pPI : pIslandVec ) p_persPtVec.push_back( pPI );
    for( persQuad& PQ : hyperVec_1 ) p_persPtVec.push_back( &PQ );
    for( persQuad& PQ : hyperVec_2 ) p_persPtVec.push_back( &PQ );
    for( persBall& PB : freeBallVec ) p_persPtVec.push_back( &PB );
    for( persShadow& PS : shadowVec ) p_persPtVec.push_back( &PS );
    p_persPtVec.push_back( &messagePQA );
    p_persPtVec.push_back( &superBall );
    p_persPtVec.push_back( &calcBall );
 //   p_persPtVec.push_back( &curve1 );
    p_persPtVec.push_back( &harmOsciBall );
    p_persPtVec.push_back( &multiBallBox );
    p_persPtVec.push_back( &Pend );
    p_persPtVec.push_back( &shadow4_PQB );
    p_persPtVec.push_back( &droneQuad );


    common_pPtVec.reserve( p_persPtVec.size() );// things that are always inUse
    for( persPt* pPt : p_persPtVec ) common_pPtVec.push_back( pPt );// use this shorter list when no shots or anis inUse

    // now add the shot and ani pointers
    for( persQuadAni& PQA : quadAniVec ) p_persPtVec.push_back( &PQA );// fireballs
    for( persBall& PB : ballVec ) p_persPtVec.push_back( &PB );// the shots
    p_persPtVec.push_back( &fragShotA );// a special shot

    for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);
    pPtVec_4list = p_persPtVec;
}

void lvl_perpective::setFreeBallPositions()// constant initial positions. all vel *= 0.0f
{
    float a = 0.0f;
    size_t numBalls = freeBallVec.size();
    float Rb = freeBallVec[0].Rbound;
    float Rring = multiBallBox.sz.x < multiBallBox.sz.y ?  multiBallBox.sz.x : multiBallBox.sz.y;
    Rring = Rring < multiBallBox.sz.z ?  Rring : multiBallBox.sz.z;
    Rring = Rring/2.0f - 2.0f*Rb;
 //   std::cout << "\n Rring = " << Rring;
    vec3f Pos;// Pos.y = multiBallBox.pos.y;
    for( size_t i = 0; i < numBalls; ++i )
    {
        Pos.x = multiBallBox.pos.x + Rring*sinf(a);
        Pos.z = multiBallBox.pos.z + Rring*cosf(a);
        Pos.y = multiBallBox.pos.y + ( i%2 ? Rb : -Rb );
        freeBallVec[i].setPosition( Pos );
   //     freeBallVec[i].vel = vec3f();
        freeBallVec[i].vel.x = freeV0*sinf(a);
        freeBallVec[i].vel.z = freeV0*cosf(a);
        freeBallVec[i].vel.y = 0.5f*( i%2 ? freeV0 : -freeV0 );
        a += 2.0f*myPI/(float)numBalls;
    }
}

bool lvl_perpective::init_boxes()
{
    std::ifstream fin("include/levels/lvl_perspective/box_data.txt");
    if( !fin ) { std::cout << "\nNo box data"; return false; }


    size_t numTemlates; fin >> numTemlates;
    boxTemplatesVec.reserve( numTemlates );// temp1, temp2, etc
    size_t numBoxesTotal; fin >> numBoxesTotal;
    quadBoxVec.reserve( numBoxesTotal );

    size_t SSnum;
    for( size_t i = 0; i < numTemlates; ++i )
    {
        fin >> SSnum;
        boxTemplatesVec.push_back( persBox_quad( fin, SSvec[SSnum] ) );
    }

    // TEMP
    if( boxTemplatesVec.size() > 4 )
    {
        persBox_quad& PQB = boxTemplatesVec[4];
        PQB.leftQ.flip('X');
    }

    // fill the quadBoxVec
    vec3f Pos;
    float rotAngle = 0.0f;// rotate about Top.Nu by rotAngle
    for( size_t i = 0; i < numTemlates; ++i )
    {
        size_t numBoxes; fin >> numBoxes;
        for( size_t j = 0; j < numBoxes; ++j )
        {
            fin >> Pos.x >> Pos.y >> Pos.z >> rotAngle;
            persBox_quad temp( boxTemplatesVec[i] );
            temp.setPosition( Pos );
            if( rotAngle*rotAngle > 1.0e-4 )
            {
                vec3f Tu( sinf( rotAngle ), 0.0f, cosf( rotAngle ) );
                temp.setFrontNu( Tu );
            }
            quadBoxVec.push_back( temp );
        }
    }

    pPBQvec.reserve( numBoxesTotal );
    for( persBox_quad& rPBQ : quadBoxVec ) pPBQvec.push_back( &rPBQ );// all targets

    // an extra for testing a shadowAdapter
    size_t idx; fin >> idx;
    fin >> Pos.x >> Pos.y >> Pos.z;
    testPBQ = boxTemplatesVec[idx];// copy?
    testPBQ.setPosition( Pos );

    return true;
}

void lvl_perpective::closeOtherControls( controlSurface* pCS )// and open new one
{
    if( !pCS ) return;// safety 1st!

    if( pCS->homeButt.sel )// opening control
    {
        // close currently open
        if( pCS != &multiCS && multiCS.sel ) { multiCS.setSel(false); return; }
        if( pCS != &islandCS && islandCS.sel ) { islandCS.setSel(false); return; }
        if( pCS != &HOballCS && HOballCS.sel ) { HOballCS.setSel(false); return; }
        if( pCS != &freeBallCS && freeBallCS.sel ) { freeBallCS.setSel(false); return; }
        if( pCS != &PendCS && PendCS.sel ) { PendCS.setSel(false); return; }
        if( pCS != &shadowControl && shadowControl.sel ) { shadowControl.setSel(false); return; }
        if( pCS != &fragCS && fragCS.sel ) { fragCS.setSel(false); return; }
    }
}


bool lvl_perpective::init_controls()
{
    std::ifstream fin("include/levels/lvl_perspective/control_data.txt");
    if( !fin ) { std::cout << "\nNo control data"; return false; }

    float R, r, posX, posY; fin >> R >> r >> posX >> posY;

    jbCamButt.init( R, r, posX, posY );
    jbCamButt.pFunc_ff = [this](float x, float z)
    {
        float Sxy = camVelXZscale/( 1.0f + 0.0015f*camVel );
        yawRate = Sxy*x*x;
        pitchRate = -Sxy*z*z;

        if( x < 0.0f ) yawRate *= -1.0f;
        if( z < 0.0f ) pitchRate *= -1.0f;
    };
    button::RegisteredButtVec.push_back( &jbCamButt );

    // CS2 control
    std::string fName; fin >> fName;
    std::ifstream finCS2Control( fName.c_str() );
    if( !finCS2Control ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    sf::Vector2f PosHmButt, sfcOfst;
    float W, H;
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_CS2Control( PosHmButt, sf::Vector2f(W,H), sfcOfst, finCS2Control );

   // multi control
    fin >> fName;
    std::ifstream finMultiControl( fName.c_str() );
    if( !finMultiControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_MultiControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finMultiControl );

    // HOball control
    fin >> fName;
    std::ifstream finHOballControl( fName.c_str() );
    if( !finMultiControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_HOballControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finHOballControl );

    // FreeBallall control
    fin >> fName;
    std::ifstream finFreeBallControl( fName.c_str() );
    if( !finFreeBallControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_FreeBallControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finFreeBallControl );

    // island control
    fin >> fName;
    std::ifstream finIslandControl( fName.c_str() );
    if( !finIslandControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_IslandControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finIslandControl );

    std::cerr << "\n init_controls()B";

    // Pend control
    fin >> fName;
    std::ifstream finPendControl( fName.c_str() );
    if( !finPendControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_PendControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finPendControl );

    // shadowControl
    fin >> fName;
    std::ifstream finShadowControl( fName.c_str() );
    if( !finShadowControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_shadowControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finShadowControl );

    // fragControl
    fin >> fName;
    std::ifstream finFragControl( fName.c_str() );
    if( !finFragControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_fragControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finFragControl );

    // soundControl
    fin >> fName;
    std::ifstream finSoundControl( fName.c_str() );
    if( !finSoundControl ){ std::cout << "\n init_controls() could not open " << fName; return false; }
    fin >> PosHmButt.x >> PosHmButt.y >> W >> H >> sfcOfst.x >> sfcOfst.y;
    init_soundControl( PosHmButt, sf::Vector2f(W,H), sfcOfst, finSoundControl );


    // check
    fin >> R; std::cout << "\ninit_controls(): R = " << R;
    return true;
}

bool lvl_perpective::init_CS2Control( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    CS2.init( HBpos, PosSurf, SzSurf, "misc", HBsz.x, HBsz.y );
    CS2.homeButt.pHitFunc = nullptr;//[this](){ closeOtherControls( &CS2 ); };
    button::RegisteredButtVec.push_back( &CS2 );
    CS2.ownButts = true;

 //   float posX, posY; is >> posX >> posY;// as offset from PosSurf
 //   posX += PosSurf.x; posY += PosSurf.y;
    pArButt = new buttonRect();
    pArButt->init( is, nullptr );// autoRollButt
    pArButt->setSel(true);
    CS2.pButtVec.push_back( pArButt );

    pPaneButt = new buttonRect();
    pPaneButt->init( is, nullptr );// paneButt
    pPaneButt->setSel(true);
    CS2.pButtVec.push_back( pPaneButt );

    persPt::angle = 0.0f;

    pushButton* pLevelButt = new pushButton();
    pLevelButt->init( is, 'O', nullptr, nullptr );// pbLevelOut
    CS2.pButtVec.push_back( pLevelButt );
    pLevelButt->pFuncClose = [this]()
    {
        persPt::camDir -= persPt::camDir.y*persPt::yHat;// now it's horizontal
        persPt::camDir /= persPt::camDir.mag();// and unit length
        persPt::yu = persPt::yHat;
        persPt::xu = persPt::yu.cross( persPt::camDir );
        for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);
        for( persLine& PL : lineVec ) PL.update(0.0f);
        to_SF_string( pitchNumMsg, persPt::camDir.dot( persPt::yHat ) );
        to_SF_string( rollNumMsg, persPt::xu.dot( persPt::yHat ) );
    };

    pTriggerButt = new pushButton();
    pTriggerButt->init( is, 'O', nullptr, nullptr );// pbTrigger
    CS2.pButtVec.push_back( pTriggerButt );
    pTriggerButt->pFuncClose = [this]()
    {
        static int N = -1;
        fireBall( vec3f( -500.0f, 600.0f, 200.0f ), vec3f( N*100.0f, 400.0f, 1000.0f ), true );
        if( ++N > 1 ) N = -1;
   };

    pDblFireButt = new buttonRect();
    pDblFireButt->init( is, nullptr );// dblFireButt
    CS2.pButtVec.push_back( pDblFireButt );

    pTargetButt = new buttonRect();
    pTargetButt->init( is, nullptr );// targetButt
    pTargetButt->pHitFunc = [this](){ if( !pTargetButt->sel && pTargetQuad ){ pTargetQuad->setColor( sf::Color::White ); pTargetQuad = nullptr; } };
    CS2.pButtVec.push_back( pTargetButt );

    pHitAllButt = new buttonRect();// hitAllButt
    pHitAllButt->init( is, [this](){ if( !pHitAllButt->sel ) setFreeBallPositions(); for( persBall& PB : freeBallVec ) PB.isMoving = pHitAllButt->sel; } );
    CS2.pButtVec.push_back( pHitAllButt );

    buttonRect* pSBmoveButt = new buttonRect();// SBmoveButt
    pSBmoveButt->init( is, [ this, pSBmoveButt ](){ superBall.isMoving = pSBmoveButt->sel; } );
    CS2.pButtVec.push_back( pSBmoveButt );

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;
    return true;
}// end of init_CS2Control()

bool lvl_perpective::init_PendControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    // Pend control
 //   Label.setString("Pend");
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    PendCS.init( HBpos, PosSurf, SzSurf, "Pend", HBsz.x, HBsz.y );
    PendCS.homeButt.pHitFunc = [this](){ closeOtherControls( &PendCS ); };
    button::RegisteredButtVec.push_back( &PendCS );
    PendCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    PendCS.pButtVec.push_back( pMS );
    if( pMS->sdataVec.size() != 9 ) { std::cout << "\nbad PendSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // theta
    pSD[0].pSetFunc =     [this](float x){ Pend.th = x; Pend.re_init(); };
    Pend.th = pSD[0].xInit;
    pStrip->reInit( *pSD );
    // theta dot
    pSD[1].pSetFunc =     [this](float x){ Pend.th_1 = x; Pend.updateEtot(); };
    Pend.th_1 = pSD[1].xInit;
    // phi
    pSD[2].pSetFunc =     [this](float x){ Pend.phi = x; Pend.re_init(); };
    Pend.phi = pSD[2].xInit;
    // phi dot
    pSD[3].pSetFunc =     [this](float x){ Pend.phi_1 = x; Pend.updateLz(); };
    Pend.phi_1 = pSD[3].xInit;

    // mass
    pSD[4].pSetFunc =     [this](float x){ Pend.bob.mass = x; Pend.updateLz(); Pend.updateEtot(); };
    Pend.bob.mass = pSD[4].xInit;
    // rod Length
    pSD[5].pSetFunc =     [this](float x){ Pend.L = x; Pend.re_init(); };
    Pend.L = pSD[5].xInit;
    // GravY
    pSD[6].pSetFunc =     [this](float x){ Pend.GravY = x; Pend.re_init(); };
    Pend.GravY = pSD[6].xInit;
    // thBump
    pSD[7].pSetFunc =     [this](float x){ return; };// just change xCurr
    // phiBump
    pSD[8].pSetFunc =     [this](float x){ return; };// just change xCurr

    float W; is >> W >> posX >> posY;// new button width and offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    pPendResetButt = new pushButton();// dm. used in handleEvent() where key 'M' release toggles motion
    pPendResetButt->init( sf::Vector2f(posX,posY), sf::Vector2f(W,HBsz.y), 'O', "RESET", nullptr, nullptr );
    PendCS.pButtVec.push_back( pPendResetButt );
    // reset to initial conditions
    pPendResetButt->pFuncClose = [ pMS, this ]()
    {
        if( pMS->sdataVec.size() < 7 ) { std::cout << "\nbad data"; return; }
        stripData* pSD = &( pMS->sdataVec.front() );
        Pend.th = pSD[0].xCurr;
        Pend.th_1 = pSD[1].xCurr;
        Pend.phi = pSD[2].xCurr;
        Pend.phi_1 = pSD[3].xCurr;
        Pend.bob.mass = pSD[4].xCurr;
        Pend.L = pSD[5].xCurr;
        Pend.GravY = pSD[6].xCurr;
        Pend.re_init();
        if( PendCS.updateAll )
        {
            bool moving = Pend.bob.isMoving;
            Pend.bob.isMoving = true;
            PendCS.updateAll();
            Pend.bob.isMoving = moving;
        }
    };
    // a run/stop button
    is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonRect* pRunButt = new buttonRect();
    pRunButt->init( posX, posY, W, HBsz.y, [ this, pRunButt ](){ Pend.bob.isMoving = pRunButt->sel; }, "RUN" );
    PendCS.pButtVec.push_back( pRunButt );
    // take a step while stopped
    is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    pushButton* pStepButt = new pushButton();
    pStepButt->init( sf::Vector2f(posX,posY), sf::Vector2f(W,HBsz.y), 'O', "STEP", nullptr, nullptr );
    PendCS.pButtVec.push_back( pStepButt );
    pStepButt->pFuncClose = [this]()
    {
        if( Pend.bob.isMoving ) return;
        Pend.bob.isMoving = true;
        Pend.update(0.01);// a 10ms step
        if( PendCS.updateAll ) PendCS.updateAll();
        Pend.bob.isMoving = false;

    };

    // conserve Lz during motion
    is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonRect* pConsButt = new buttonRect();
    pConsButt->init( posX, posY, W, HBsz.y, [ this, pConsButt ](){ Pend.useConservedLz = pConsButt->sel; }, "consL" );
 //   pConsButt->setSel( true );
    PendCS.pButtVec.push_back( pConsButt );

    // conserve Etot during motion
    is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    pConsButt = new buttonRect();
    pConsButt->init( posX, posY, W, HBsz.y, [ this, pConsButt ](){ Pend.useConservedEtot = pConsButt->sel; }, "consE" );
//    pConsButt->setSel( true );
    PendCS.pButtVec.push_back( pConsButt );

    // apply thBump
    pushButton* pThBumpButt = new pushButton();
    float H; is >> W >> H >> posX >> posY;
    posX += PosSurf.x; posY += PosSurf.y;
    pThBumpButt->init( sf::Vector2f(posX,posY), sf::Vector2f(W,H), 'O', "thBump", nullptr, nullptr );
    PendCS.pButtVec.push_back( pThBumpButt );
    pThBumpButt->pFuncClose = [ this, pMS ]()
    {
        if( !Pend.bob.isMoving ) return;
        if( pMS->sdataVec.size() < 8 ) { std::cout << "\nbad data"; return; }
        stripData* pSD = &( pMS->sdataVec.front() );
        Pend.th_1 += pSD[7].xCurr;
        pSD[1].xCurr = Pend.th_1;

     //   double thSave = Pend.th; Pend.th = pSD[0].xCurr;
     //   double phi_1Save = Pend.phi_1; Pend.phi_1 = pSD[3].xCurr;
        Pend.updateEtot();
     //   Pend.th = thSave;
     //   Pend.phi_1 = phi_1Save;
    };
    // apply phiBump
    pushButton* pPhiBumpButt = new pushButton();
    is >> posY; posY += PosSurf.y;
    pPhiBumpButt->init( sf::Vector2f(posX,posY), sf::Vector2f(W,H), 'O', "phiBump", nullptr, nullptr );
    PendCS.pButtVec.push_back( pPhiBumpButt );
    pThBumpButt->pFuncClose = [ this, pMS ]()
    {
        if( !Pend.bob.isMoving ) return;
        if( pMS->sdataVec.size() < 9 ) { std::cout << "\nbad data"; return; }
        stripData* pSD = &( pMS->sdataVec.front() );
        Pend.phi_1 += pSD[8].xCurr;
        pSD[3].xCurr = Pend.phi_1;

    //    double thSave = Pend.th; Pend.th = pSD[0].xCurr;
    //    double th_1Save = Pend.th_1; Pend.th_1 = pSD[1].xCurr;
        Pend.updateLz();
        Pend.updateEtot();
     //   Pend.th = thSave;
     //   Pend.th_1 = th_1Save;
    };

    // extras for PendCS to update and display
    PendCS.drawAll = [this]( sf::RenderTarget& RT )
    {
        for( const sf::Text& txt : PendMsgVec ) RT.draw( txt );
        for( const sf::Text& txt : PendNumMsgVec ) RT.draw( txt );
    };

    const size_t numMsgPairs = 7;// Lz, E, vx, vy, vz, th, phi
    PendMsgVec.reserve( numMsgPairs );
    sf::Text Msg( "Lz", *button::pFont, 14 );
    Msg.setFillColor( sf::Color::Green );
    is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    Msg.setPosition( posX, posY );// Lz
    PendMsgVec.push_back( Msg );
    PendNumMsgVec.reserve( numMsgPairs );
    float offX, offY; is >> offX >> offY;// offsets from LzMsg
    Msg.setPosition( posX + offX, posY );
    PendNumMsgVec.push_back( Msg );// LzNum
    Msg.setString( "Etot" );
    Msg.setPosition( posX, posY + offY );
    PendMsgVec.push_back( Msg );
    Msg.setPosition( posX + offX, posY + offY );
    PendNumMsgVec.push_back( Msg );// EtotNum
    // bob.vel
    is >> posX >> offX; posX += PosSurf.x;// offX is to vel numMsg
    Msg.setString( "vx" );
    Msg.setPosition( posX, posY );
    PendMsgVec.push_back( Msg );
    Msg.setPosition( posX + offX, posY );
    PendNumMsgVec.push_back( Msg );// velxNum
    Msg.setString( "vy" );
    Msg.setPosition( posX, posY + offY );
    PendMsgVec.push_back( Msg );
    Msg.setPosition( posX + offX, posY + offY );
    PendNumMsgVec.push_back( Msg );// velyNum
    Msg.setString( "vz" );
    Msg.setPosition( posX, posY + 2.0*offY );
    PendMsgVec.push_back( Msg );
    Msg.setPosition( posX + offX, posY + 2.0*offY );
    PendNumMsgVec.push_back( Msg );// velyNum
    // th and phi
    is >> posX >> offX; posX += PosSurf.x;// offX is to angle numMsg
    Msg.setString( "th" );
    Msg.setPosition( posX, posY );
    PendMsgVec.push_back( Msg );
    Msg.setPosition( posX + offX, posY );
    PendNumMsgVec.push_back( Msg );// thNum
    Msg.setString( "phi" );
    Msg.setPosition( posX, posY + offY );
    PendMsgVec.push_back( Msg );
    Msg.setPosition( posX + offX, posY + offY );
    PendNumMsgVec.push_back( Msg );// phiNum

    Pend.re_init();// initial state of Pend established

    PendCS.updateAll = [this]()
    {
        if( Pend.bob.isMoving )
        {
            to_SF_string( PendNumMsgVec[0], Pend.calcLz() );
            to_SF_string( PendNumMsgVec[1], Pend.calcEtot() );
            to_SF_string( PendNumMsgVec[2], Pend.bob.vel.x );
            to_SF_string( PendNumMsgVec[3], Pend.bob.vel.y );
            to_SF_string( PendNumMsgVec[4], Pend.bob.vel.z );
            to_SF_string( PendNumMsgVec[5], Pend.th*180.0/3.1415927 );
            to_SF_string( PendNumMsgVec[6], Pend.phi*180.0/3.1415927 );
        }
    };
    PendCS.updateAll();

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;
    return true;
}// end of init_PendControlinit()

bool lvl_perpective::init_HOballControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    HOballCS.init( HBpos, PosSurf, SzSurf, "HOball", HBsz.x, HBsz.y );
    HOballCS.homeButt.pHitFunc = [this](){ closeOtherControls( &HOballCS ); };
    button::RegisteredButtVec.push_back( &HOballCS );
    HOballCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    HOballCS.pButtVec.push_back( pMS );

    if( pMS->sdataVec.size() != 4 ) { std::cout << "\nbad HOball multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // HOspring
    pSD[0].pSetFunc =     [this](float x){ HOspring = x; };
    HOspring = pSD[0].xCurr;
    pStrip->reInit( *pSD );
    // HOdamp
    pSD[1].pSetFunc =     [this](float x){ HOdamp = x; };
    HOdamp = pSD[1].xCurr;
    // harmOsciBall.mass
    pSD[2].pSetFunc =     [this](float x){ harmOsciBall.mass = x; };
    harmOsciBall.mass = pSD[2].xCurr;
    // harmOsciBall.B radius
    pSD[3].pSetFunc =     [this](float x){ harmOsciBall.Rbound = x; harmOsciBall.B.setRadius(x); harmOsciBall.B.setOrigin(x,x); harmOsciBall.update(0.0f); };
    pSD[3].pSetFunc( pSD[3].xCurr );
    std::cerr << "\nharmOsciBall.Rbound = " << harmOsciBall.Rbound;

    is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    float W, H; is >> W >> H;
    pushButton* pPB = new pushButton( sf::Vector2f(posX,posY), sf::Vector2f(W,H), 'O', "STOP", [this](){ harmOsciBall.vel = vec3f(); harmOsciBall.setPosition( HOpos ); harmOsciBall.update(0.0f); }, nullptr );
 //   stopButt.init( sf::Vector2f(posX,posY), sf::Vector2f(W,H), 'O', "STOP", [this](){ harmOsciBall.vel = vec3f(); harmOsciBall.setPosition( HOpos ); harmOsciBall.update(0.0f); }, nullptr );
    HOballCS.pButtVec.push_back( pPB );

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_perpective::init_IslandControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    return false;// this call is causing IslandB to vanish

    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    islandCS.init( HBpos, PosSurf, SzSurf, "Island", HBsz.x, HBsz.y );
    islandCS.homeButt.pHitFunc = [this](){ closeOtherControls( &islandCS ); };
    button::RegisteredButtVec.push_back( &islandCS );
    islandCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    islandCS.pButtVec.push_back( pMS );

    if( pMS->sdataVec.size() != 4 ) { std::cout << "\nbad island multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // pSpecialIsland->viewAlignMin
    pSD[0].pSetFunc = [this](float x){ pSpecialIsland->viewAlignMin = cosf( myPI*x/180.0f ); };
    pSpecialIsland->viewAlignMin = cosf( myPI*pSD->xCurr/180.0f );
    pStrip->reInit( *pSD );
    // islandB.Rcam
    pSD[1].pSetFunc = [this](float x){ pSpecialIsland->Rcam = x; pSpecialIsland->update(0.0f); };
    pSpecialIsland->Rcam = pSD->xCurr;
    // islandB.Rview
    pSD[2].pSetFunc = [this](float x){ pSpecialIsland->Rview = x; pSpecialIsland->update(0.0f); };
    pSpecialIsland->Rview = pSD->xCurr;
    // islandB.R1
    pSD[3].pSetFunc = [this](float x){ pSpecialIsland->R1 = x; pSpecialIsland->update(0.0f); };
    pSpecialIsland->R1 = pSD->xCurr;

    sf::Text Label("island", *button::pFont, 12);
    Label.setFillColor( sf::Color::Black );
    float W, H; is >> posX >> posY >> W >> H;
    posX += PosSurf.x; posY += PosSurf.y;
    radioButton* pRB = new radioButton();
    pRB->init( posX, posY, W, H, pIslandVec.size(), Label, 0 );
    pRB->pFuncIdx = [ this, pSD, pStrip, pMS ](size_t idx)
    {
        if( idx < pIslandVec.size() )
        {
            pSpecialIsland = pIslandVec[idx];
            pSD[0].xCurr = pSpecialIsland->viewAlignMin;
            pSD[1].xCurr = pSpecialIsland->Rcam;
            pSD[2].xCurr = pSpecialIsland->Rview;
            pSD[3].xCurr = pSpecialIsland->R1;
            pStrip->reInit( pSD[ pMS->idx_curr ] );
        }

    };
    islandCS.pButtVec.push_back( pRB );

     std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;
    return true;
}

bool lvl_perpective::init_fragControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    fragCS.init( HBpos, PosSurf, SzSurf, "FRAG", HBsz.x, HBsz.y );
    fragCS.homeButt.pHitFunc = [this](){ closeOtherControls( &fragCS ); };
    button::RegisteredButtVec.push_back( &fragCS );
    fragCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    fragCS.pButtVec.push_back( pMS );

    std::cerr << "\ninit_FragControl() A";

    if( pMS->sdataVec.size() != 4 ) { std::cout << "\nbad fragShotA multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // tBoom
    pSD[0].pSetFunc =     [this](float x){ fragShotA.tBoom = x; fragShotA.tElap = 0.0f; };
    fragShotA.tBoom = pSD[0].xInit;
    pStrip->reInit( *pSD );
    // Vfire
    pSD[1].pSetFunc =     [this](float x){ fragShotA.Vfire = x; };
    fragShotA.Vfire = pSD[1].xInit;
    // Vfrags
    pSD[2].pSetFunc =     [this](float x){ fragShotA.Vfrags = x; };
    fragShotA.Vfrags = pSD[2].xInit;
    // Rdet
    pSD[3].pSetFunc =     [this](float x){ fragShotA.Rdet = x; };
    fragShotA.Rdet = pSD[3].xInit;

    // select a frag pattern
    sf::Text Label("pattern", *button::pFont, 12);
    Label.setFillColor( sf::Color::White );
    float W, H; is >> posX >> posY >> W >> H;
    posX += PosSurf.x; posY += PosSurf.y;
    radioButton* pRB = new radioButton();
    pRB->init( posX, posY, W, H, shotBurst::numPatterns, Label, 0 );
    pRB->pFuncIdx = [this](size_t idx)
    {
        if( idx < shotBurst::numPatterns )
            fragShotA.velPattern = idx;
    };
    fragCS.pButtVec.push_back( pRB );

    // fuseMode
    Label.setString("fuseMode");
    is >> posX >> posY >> W >> H;
    posX += PosSurf.x; posY += PosSurf.y;
    pRB = new radioButton();
    pRB->init( posX, posY, W, H, 3, Label, 0 );
    pRB->pFuncIdx = [this](size_t idx)
    {
        if( idx == 0 ) fragShotA.fuseMode = 'T';// timed
        else if( idx == 1 ) fragShotA.fuseMode = 'E';// elevation
        else if( idx == 2 ) fragShotA.fuseMode = 'P';// proximity
    };
    fragCS.pButtVec.push_back( pRB );

     std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;
    return true;
}

bool lvl_perpective::init_FreeBallControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    freeBallCS.init( HBpos, PosSurf, SzSurf, "Free", HBsz.x, HBsz.y );
    freeBallCS.homeButt.pHitFunc = [this](){ closeOtherControls( &freeBallCS ); };
    button::RegisteredButtVec.push_back( &freeBallCS );
    freeBallCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    freeBallCS.pButtVec.push_back( pMS );

    std::cerr << "\ninit_FreeBallControl() A";

    if( pMS->sdataVec.size() != 7 ) { std::cout << "\nbad freeBall multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // freeV0
    pSD[0].pSetFunc =     [this](float x){ freeV0 = x; };
    freeV0 = pSD[0].xCurr;
    pStrip->reInit( *pSD );
    // mass
    pSD[1].pSetFunc =     [this](float x){ for( persBall& PB : freeBallVec ) PB.mass = x; };
//    pSD[1].pSetFunc( pSD[1].xCurr );
    // Rbound
    pSD[2].pSetFunc =     [this](float x){ for( persBall& PB : freeBallVec ){ PB.Rbound = x; PB.update(0.0f); } };
 //   pSD[2].pSetFunc( pSD[2].xCurr );
    // freeGrav.y
    pSD[3].pSetFunc =     [this](float x){ freeGrav.y = -x; };
    freeGrav.y = -pSD[3].xCurr;
    // sz.x
    pSD[4].pSetFunc =     [this](float x){ multiBallBox.sz.x = x; multiBallBox.setSize( multiBallBox.sz ); };
    pSD[4].xCurr = pSD[4].xInit = multiBallBox.sz.x;
    // sz.y
    pSD[5].pSetFunc =     [this](float y){ multiBallBox.sz.y = y; multiBallBox.setSize( multiBallBox.sz ); };
    pSD[5].xCurr = pSD[5].xInit = multiBallBox.sz.y;
    // sz.z
    pSD[6].pSetFunc =     [this](float z){ multiBallBox.sz.z = z; multiBallBox.setSize( multiBallBox.sz ); };
    pSD[6].xCurr = pSD[6].xInit = multiBallBox.sz.z;

    std::cerr << "\ninit_FreeBallControl() B";

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;
 //   std::cout << "\n **************lvl_perpective::init_MultiControl() has been called***************";
    return true;
}

bool lvl_perpective::init_MultiControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    multiCS.init( HBpos, PosSurf, SzSurf, "multi", HBsz.x, HBsz.y );
    multiCS.homeButt.pHitFunc = [this](){ closeOtherControls( &multiCS ); };
    button::RegisteredButtVec.push_back( &multiCS );
    multiCS.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    multiCS.pButtVec.push_back( pMS );

    if( pMS->sdataVec.size() != 12 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // fireVel
    pSD->pSetFunc =     [this](float x){ fireVel = x; };
    fireVel = pSD->xCurr;
    pStrip->reInit( *pSD );
    // tFireDelay
    (++pSD)->pSetFunc = [this](float x){ tFireDelay = 1.0f/x; };
    tFireDelay = 1.0f/pSD->xCurr;
    // g
    (++pSD)->pSetFunc = [this](float x){ gravity.y = -x; };
    gravity.y = -pSD->xCurr;
    // camAccel
    (++pSD)->pSetFunc = [this](float x){ camAccel = x; };
    camAccel = pSD->xCurr;
    // camVelXZscale = joystick sensitivity
    (++pSD)->pSetFunc = [this](float x){ camVelXZscale = x; };
    pSD->xCurr = camVelXZscale;
    // coeffRest shots vs superBall
    (++pSD)->pSetFunc = [this](float x){ coeffRest = x; };
    coeffRest = pSD->xCurr;
    // zoomFactor
    (++pSD)->pSetFunc = [this](float x){ zoomFactor = 1.0f/x; };
    zoomFactor = 1.0f/pSD->xCurr;
    // autoRollSensitivity
    (++pSD)->pSetFunc = [this](float x){ autoRollSens = x; };
    autoRollSens = pSD->xCurr;
    //  move_xyVel
    (++pSD)->pSetFunc = [this](float x){ move_xyVel = x; };
    move_xyVel = pSD->xCurr;
    //  camStep
    (++pSD)->pSetFunc = [this](float x){ camStep = x; };
    camStep = pSD->xInit;
    //  persPt::Z0
    (++pSD)->pSetFunc = [this](float x){ persPt::Z0 = x; for( persPt* pPt : p_persPtVec ) pPt->update(0.0f); };
    persPt::Z0 = pSD->xCurr;
    //  sb_vReturn
    (++pSD)->pSetFunc = [this](float x)
    {
        if( sbReturning )
        {
            superBall.vel *= (x/sb_vReturn);
            float tRemain = sb_tTrip - sb_tElap;
            sb_tTrip -= tRemain;
            tRemain /= (x/sb_vReturn);
            sb_tTrip += tRemain;
        }
        sb_vReturn = x;
    };
    sb_vReturn = pSD->xInit;


    // a "reset all" button
    sf::Vector2f Pos; is >> Pos.x >> Pos.y >> SzSurf.x >> SzSurf.y;
    Pos += PosSurf;
    pushButton* pPB = new pushButton( Pos, SzSurf, 'O', "Reset All", nullptr, nullptr );
    pPB->pFuncClose = [ pMS, pStrip ]()
    {
        if( pMS->sdataVec.size() != pMS->selButtVec.size() ) return;
        for( size_t k = 0; k < pMS->sdataVec.size(); ++k )
        {
            pMS->sdataVec[k].xCurr = pMS->sdataVec[k].xInit;
            if( pMS->selButtVec[k].sel ) pStrip->reInit( pMS->sdataVec[k] );
            else pMS->sdataVec[k].pSetFunc( pMS->sdataVec[k].xInit );
        }
    };

    multiCS.pButtVec.push_back( pPB );

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;
 //   std::cout << "\n **************lvl_perpective::init_MultiControl() has been called***************";
    return true;
}// end of init_MultiControl()

bool lvl_perpective::init_shadowControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    shadowControl.init( HBpos, PosSurf, SzSurf, "shade", HBsz.x, HBsz.y );
    shadowControl.homeButt.pHitFunc = [this](){ closeOtherControls( &shadowControl ); };
    button::RegisteredButtVec.push_back( &shadowControl );
    shadowControl.ownButts = true;

    // slideBar for theta
    sf::Text Title; Title.setFont( *button::pFont );
    Title.setCharacterSize( 14 );
    Title.setFillColor( sf::Color::Green );
    Title.setString( "azimuth" );
    float x, y, W, H, Wbutt, val_min, val_max, init_val;
    is >> x >> y >> W >> H >> Wbutt >> val_min >> val_max >> init_val;
    slideBar* pSBth = new slideBar();
    x += PosSurf.x; y += PosSurf.y;
    pSBth->init( x, y, W, H, Title, val_min, val_max, init_val, nullptr, Wbutt );
    shadowControl.pButtVec.push_back( pSBth );

    // slideBar for phi
    Title.setString( "polar" );
    is >> x >> y >> val_min >> val_max >> init_val;
    slideBar* pSBphi = new slideBar();
    x += PosSurf.x; y += PosSurf.y;
    pSBphi->init( x, y, W, H, Title, val_min, val_max, init_val, nullptr, Wbutt );
    shadowControl.pButtVec.push_back( pSBphi );

    std::function<void(void)> pSet_sHat = [this]()
    {
        for( persShadow& rPS : shadowVec ){ rPS.sHat = sHat; rPS.update(0.0f); }
        for( shadowAdapter& rSA : shAdaptVec ){ rSA.update( sHat, 0.0f ); }
        shadow4_PQB.update( sHat, 0.0f );
    };

    // set sHat given theta
    pSBth->pSetFunc = [ this, pSBphi, pSet_sHat ]( float th )
    {
        float phi = pSBphi->fVal;
        sHat.y = -cosf( th );
        sHat.x = sinf( th )*cosf( phi );
        sHat.z = sinf( th )*sinf( phi );
        sHat /= sHat.mag();// shouldn't be necc.
        pSet_sHat();
    };
    // set sHat given phi
    pSBphi->pSetFunc = [ this, pSBth, pSet_sHat ]( float phi )
    {
        float th = pSBth->fVal;
        sHat.y = -cosf( th );
        sHat.x = sinf( th )*cosf( phi );
        sHat.z = sinf( th )*sinf( phi );
        sHat /= sHat.mag();
        pSet_sHat();
    };

    // toggle shadows on/off
    is >> x >> y >> W >> H;
    x += PosSurf.x; y += PosSurf.y;
    buttonRect* pRB = new buttonRect( x, y, W, H, nullptr , "shade" );
    pRB->pHitFunc = [this, pRB](){ persShadow::shadowsOn = pRB->sel; };
    pRB->setSel( true );
    shadowControl.pButtVec.push_back( pRB );

    // init
    pSBth->pSetFunc( pSBth->fVal );

    return true;
}

bool lvl_perpective::init_soundControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
{
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    soundControl.init( HBpos, PosSurf, SzSurf, "sound", HBsz.x, HBsz.y );
    soundControl.homeButt.pHitFunc = [this](){ closeOtherControls( &soundControl ); };
    button::RegisteredButtVec.push_back( &soundControl );
    soundControl.ownButts = true;

    float posX, posY; is >> posX >> posY;// as offset from PosSurf
    posX += PosSurf.x; posY += PosSurf.y;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( sf::Vector2f(posX,posY), pStrip, is );
    pMS->ownsStrip = true;
    soundControl.pButtVec.push_back( pMS );

    if( pMS->sdataVec.size() != 6 ) { std::cout << "\nbad multiSel data"; return false; }
 //   stripData* pSD = &( pMS->sdataVec.front() );
    // shotFire
  //  pSD->pSetFunc =     [this](float x){ shotFireSoundVolume = x; };
 //   shotFireSoundVolume = pSD->xCurr;
 //   pStrip->reInit( *pSD );
    // shotBoom
  //  (++pSD)->pSetFunc = [this](float x){ shotFireSoundVolume = x; };
 //   shotFireSoundVolume = pSD->xCurr;

    std::vector<float*> soundVolVec;
    soundVolVec.reserve(6);
    soundVolVec.push_back( &shotFireSoundVolume );
    soundVolVec.push_back( &shotBoomSoundVolume );
    soundVolVec.push_back( &fragFireSoundVolume );
    soundVolVec.push_back( &fragBoomSoundVolume );
    soundVolVec.push_back( &shotBounceSoundVolume );
    soundVolVec.push_back( &ballVsWallSoundVolume );
    pMS->setLambdas( soundVolVec );
    pStrip->reInit( pMS->sdataVec.front() );
    return true;
}

bool lvl_perpective::init_sounds()
{
    std::ifstream fin( "include/levels/lvl_perspective/sound_data.txt" );
    if( !fin ) { std::cout << "\nopening sound_data failed"; return false; }

    // use sounds?
    unsigned int numSounds; fin >> numSounds;
    lvl_perpective::useSounds = ( numSounds > 0 );
    fin >> lvl_perpective::soundAttenuDistance;
    fin >> lvl_perpective::soundVolumeMin;
    if( !lvl_perpective::useSounds ) return true;// before loading sounds

    std::string fName;
 //   float volume = 50.0f;
    fin >> fName;// fire shot
    if ( !shotFireSoundBuff.loadFromFile( fName.c_str() ) ) { std::cout << "\n no shot fire sound"; return false; }
    fin >> fName;// Boom
    if ( !shotBoomSoundBuff.loadFromFile( fName.c_str() ) ) { std::cout << "\n no shot boom sound"; return false; }
    sf::Sound tempSound;
    tempSound.setBuffer( shotFireSoundBuff ); tempSound.setVolume( shotFireSoundVolume );// default
    shotSoundVec.reserve( numSounds );
    for( unsigned int n = 0; n < numSounds; ++n )
        shotSoundVec.push_back( tempSound );

    // fragShot
    fin >> fName;
    if ( !fragFireSoundBuff.loadFromFile( fName.c_str() ) ) { std::cout << "\n no frag fire sound"; return false; }
    fin >> fName;
    if ( !fragBoomSoundBuff.loadFromFile( fName.c_str() ) ) { std::cout << "\n no frag boom sound"; return false; }
    // shot bounce
    fin >> fName;
    if ( !shotBounceSoundBuff.loadFromFile( fName.c_str() ) ) { std::cout << "\n no shot bounce sound"; return false; }
    // ball vs wall
    fin >> fName;
    if ( !ballVsWallSoundBuff.loadFromFile( fName.c_str() ) ) { std::cout << "\n no ballVsWall sound"; return false; }

    fin.close();

 //   fragBoomSound.play();
 //   playShotSound( ballVsWallSoundBuff, ballVsWallSoundVolume );

    return true;
}

bool lvl_perpective::init_terrain()
{
    std::ifstream fin("include/levels/lvl_perspective/terrain_data.txt");
    if( !fin ) { std::cout << "\nNo terrain data"; return false; }

//    persLS.init( fin );

    // persQuad island
    size_t rows, cols; fin >> rows >> cols;

    vec3f pos0; fin >> pos0.x >> pos0.y >> pos0.z;// center of bottom left persQuad
    float W, H; fin >> W >> H;// dimensions of each persQuad
    vec3f nu( 0.0f, 1.0f, 0.0f );// for starters
    size_t SSnum, SetNum, FrIdx; fin >> SSnum >> SetNum >> FrIdx;
    if( SSnum >= SSvec.size() ){ std::cout << "\n init_terrain(): island SSnum =  " << SSnum << " too high"; return false; }
    if( SetNum > SSvec[SSnum].numSets() ){ std::cout << "\n init_terrain(): island SetNum =  " << SetNum << " too high"; return false; }
    if( FrIdx > SSvec[SSnum].getFrCount( SetNum ) ){ std::cout << "\n init_terrain(): island FrIdx =  " << FrIdx << " too high"; return false; }

    size_t numPQ, numTrees; fin >> numPQ >> numTrees;// totals
    terrainPQvec.reserve( numPQ + numTrees );
    // island surface
    for( size_t r = 0; r < rows; ++r )
        for( size_t c = 0; c < cols; ++c )
        {
            vec3f Pos = pos0;
            Pos.x += r*H;
            Pos.z += c*W;
            terrainPQvec.push_back( persQuad( Pos, W, H, nu, sf::Color::White, &(SSvec[SSnum].txt) ) );
            terrainPQvec.back().setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), 'R', '0' );
        }

    // trees on the island
    fin >> numTrees;
    fin >> W >> H;
    fin >> SSnum >> SetNum >> FrIdx;
    if( SSnum >= SSvec.size() ){ std::cout << "\n init_terrain(): trees SSnum =  " << SSnum << " too high"; return false; }
    if( SetNum > SSvec[SSnum].numSets() ){ std::cout << "\n init_terrain(): trees SetNum =  " << SetNum << " too high"; return false; }
    if( FrIdx > SSvec[SSnum].getFrCount( SetNum ) ){ std::cout << "\n init_terrain(): trees FrIdx =  " << FrIdx << " too high"; return false; }
    vec3f ofst;
    ofst.y = H/2.0f;
    for( size_t i = 0; i < numTrees; ++i )
    {
        fin >> ofst.x >> ofst.z;
        terrainPQvec.push_back( persQuad( pos0 + ofst, W, H, vec3f(0.0f,0.0f,1.0f), sf::Color::White, &(SSvec[SSnum].txt) ) );
        terrainPQvec.back().setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), 'R', '1' );
        terrainPQvec.back().facingCamera = true;
    }

    // other trees
    fin >> numTrees;
    if( SSnum >= SSvec.size() ){ std::cout << "\n init_terrain(): trees SSnum =  " << SSnum << " too high"; return false; }
    if( SetNum > SSvec[SSnum].numSets() ){ std::cout << "\n init_terrain(): trees SetNum =  " << SetNum << " too high"; return false; }
    if( FrIdx > SSvec[SSnum].getFrCount( SetNum ) ){ std::cout << "\n init_terrain(): trees FrIdx =  " << FrIdx << " too high"; return false; }
    vec3f Pos;
    Pos.y = H/2.0f;
    for( size_t i = 0; i < numTrees; ++i )
    {
        fin >> Pos.x >> Pos.y >> Pos.z;
        fin >> W >> H >> SSnum >> SetNum >> FrIdx;
        terrainPQvec.push_back( persQuad( Pos, W, H, vec3f(0.0f,0.0f,1.0f), sf::Color::White, &(SSvec[SSnum].txt) ) );
        terrainPQvec.back().setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), 'R', '1' );
        terrainPQvec.back().facingCamera = true;
    }

    // other persQuad
    fin >> numPQ;
    for( size_t i = 0; i < numPQ; ++i )
    {
        fin >> Pos.x >> Pos.y >> Pos.z >> W >> H;
        fin >> SSnum >> SetNum >> FrIdx;
        terrainPQvec.push_back( persQuad( Pos, W, H, nu, sf::Color::White, &(SSvec[SSnum].txt) ) );
        terrainPQvec.back().setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), 'R', '0' );
    }

    fin >> numPQ;
    std::cerr << "\n numPQ = " << numPQ;


    return true;
}

bool lvl_perpective::init_paths()
{
    std::ifstream fin("include/leg_types/paths/ovala.txt");
    if( !fin ) { std::cout << "no ovala data\n"; return false; }
//    pPathVec.reserve(5);
    pPathVec.push_back( nullptr );
    Leg*& rpLeg = pPathVec.back();
    makePath( fin, rpLeg );
 //   fin.close();

    // more boxes = cars
    size_t idx, numCars; fin >> idx >> numCars;
    std::cout << "\nidx = " << idx << "  numCars = " << numCars;

 //   const unsigned int numCars = 6;
    CarVec.reserve( numCars );// persPt*
    carLAvec.reserve( numCars );// legAdapter
    float ds = pathLength( *rpLeg )/( numCars ), s0 = 0.0f, vCar = 100.0f;
    fin >> vCar;
    fin.close();
    Leg *iter = rpLeg, *pleg0 = rpLeg;
    for( unsigned int i = 0; i < numCars; ++i )
    {
        CarVec.push_back( persBox_quad( boxTemplatesVec[idx] ) );
        persBox_quad& PQB = CarVec.back();
        carLAvec.push_back( legAdapter() );
        legAdapter& rLA = carLAvec.back();
        rLA.init( pleg0, nullptr, vCar );
        rLA.pSetPosition = [this,&PQB,&rLA](float x, float y)
        {
            PQB.setPosition( vec3f(x, PQB.frontQ.h/2.0f, y) );
            vec3f fNu(1.0f,0.0f,0.0f);
            fNu.y = 0.0f;
            rLA.pLeg->T( rLA.s, fNu.x, fNu.z );
            PQB.setFrontNu( fNu );
        };
        while( iter && s0 > iter->len ) { s0 -= iter->len; iter = iter->next; }
        rLA.s = s0;
        rLA.pLeg = iter;
        rLA.update(0.0f);
    //    updateLAbox( rLA, rCar, 0.0f );
        s0 += ds;
    }

    std::cout << "  CarVec.size() = " << CarVec.size();

    // map out corners
/*    if( !boxTemplatesVec.empty() )
    {
        persBox_quad& PBQ = boxTemplatesVec.front();
        vec3f Tu = PBQ.frontQ.Nu, Th = PBQ.topQ.Nu, Tw = PBQ.rightQ.Nu;
        float w = (PBQ.rightQ.pt[0] - PBQ.leftQ.pt[0]).dot(Tw); w /= 2.0f;
        float h = (PBQ.topQ.pt[0] - PBQ.bottomQ.pt[0]).dot(Th); h /= 2.0f;
        float d = (PBQ.frontQ.pt[0] - PBQ.backQ.pt[0]).dot(Tu); d /= 2.0f;
        vec3f pt[8];
        pt[0] =  PBQ.pos + d*Tu + w*Tw - h*Th;// A
        pt[1] =  PBQ.pos + d*Tu + w*Tw + h*Th;// B
        pt[2] =  PBQ.pos + d*Tu - w*Tw + h*Th;// C
        pt[3] =  PBQ.pos + d*Tu - w*Tw - h*Th;// D
        pt[4] =  PBQ.pos - d*Tu - w*Tw + h*Th;// E
        pt[5] =  PBQ.pos - d*Tu + w*Tw + h*Th;// F
        pt[6] =  PBQ.pos - d*Tu - w*Tw - h*Th;// G
        pt[7] =  PBQ.pos - d*Tu + w*Tw - h*Th;// H
        vec3f* pQpt[] = { PBQ.frontQ.pt, PBQ.backQ.pt, PBQ.leftQ.pt, PBQ.rightQ.pt, PBQ.topQ.pt, PBQ.bottomQ.pt };
        std::string name[] = { "\nfrontQ", "\nbackQ", "\nleftQ", "\nrightQ", "\ntopQ", "\nbottomQ" };

        for( size_t s = 0; s < 6; ++s )// for each side
        {
            for( size_t p = 0; p < 4; ++p )// for each pt on side s pQpt
            {
                for( size_t c = 0; c < 8; ++c )// for each corner A to H
                {
                    vec3f v = pQpt[s][p] - pt[c];
                 //   std::cout << "\n s: " << s << " p: " << p  << " c: " << c << " v.mag() =  " << v.mag();
                    if( v.dot(v) < 1.0f )
                        std::cout << name[s] << ".pt[" << p << "] at " << (char)('A' + c);
                }
            }
        }
    }
    else std::cout << "\ninit_paths(): corner map failed";
*/

    return true;
}


bool lvl_perpective::init_3fPath()
{
    std::ifstream fin("include/leg_types/Leg3f_types/paths3f/pathA.txt");
    if( !fin ) { std::cout << "no pathA data\n"; return false; }

    unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
    sf::Color railColor( rd, gn, bu );
    fin >> rd >> gn >> bu;
    sf::Color ballColor( rd, gn, bu );
    float railOfstX, railOfstY; fin >> railOfstX >> railOfstY;
    float Rball, AngMax; fin >> Rball >> AngMax;// in degrees. for spacing of balls along curves
    AngMax *= 0.0174533f;// to radians

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
                        pPrev = pCoasterPath = pLL =  new linLeg3f( pos0, pos0 + L*t0 );// get 1st leg data
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
                //    std::cout << "\n pLL->len = " << pLL->len << " pLL->posF = " << pLL->posF.x << ' ' << pLL->posF.y << ' ' << pLL->posF.z;
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
                   //     fin >> nu.x >> nu.y >> nu.z;
                        fin >> Dir;
                        fin >> r >> Angle;
                    //    std::cout << " 1st leg r = " << r;
                //        pPrev = pCoasterPath = pCL =  new cirLeg3f( pos0, t0, nu, r, Angle );// get 1st leg data
                        pPrev = pCoasterPath = pCL =  new cirLeg3f( pos0, t0, Dir, r, Angle );// get 1st leg data
                    }
                    else
                    {
                   //     fin >> nu.x >> nu.y >> nu.z;
                        fin >> Dir;
                        fin >> r >> Angle;
                    //    std::cout << " r = " << r;
                   //     pPrev = pCL = new cirLeg3f( *pPrev, nu, r, Angle );// remaining legs
                        pPrev = pCL = new cirLeg3f( *pPrev, Dir, r, Angle );// remaining legs

                    }
                    pos0 = pCL->posF;
                 //   std::cout << "\n pCL->len = " << pCL->len << " pCL->posF = " << pCL->posF.x << ' ' << pCL->posF.y << ' ' << pCL->posF.z;
                    ++numCirLegs;
                    pCLvec.push_back( pCL );
                    break;
                }

            default : break;
        }
    }
    std::cout << "\n init_3fPath(): numLinLegs = " << numLinLegs;

    // assign path as closed or open
    pPrev->next = endType == 'O' ? nullptr : pCoasterPath;
    if( pPrev->next ) { pPrev->next->prev = pPrev; }
    L3f_adapter.init( *pCoasterPath, [this](vec3f pos){ persPt::camPos = pos; } );

    // side rails *** adapt
    coasterRailVec.reserve( 2*numLinLegs );// 2 per leg
    for( linLeg3f* pLL : pLLvec )
    {
     //   std::cout << "\nlinLeg len = " << pLL->len;
        vec3f Tz = pLL->T( 0.0f );// along leg
        vec3f Tx = persPt::yHat.cross( Tz ); Tx /= Tx.mag();
        vec3f Ty = Tz.cross( Tx );
        vec3f stPos  = pLL->pos0 + Tx*railOfstX + Ty*railOfstY;
        vec3f endPos = pLL->posF + Tx*railOfstX + Ty*railOfstY;
        coasterRailVec.push_back( persLine( stPos, endPos, railColor ) );
        stPos  = pLL->pos0 - Tx*railOfstX + Ty*railOfstY;
        endPos = pLL->posF - Tx*railOfstX + Ty*railOfstY;
        coasterRailVec.push_back( persLine( stPos, endPos, railColor ) );

     //   vec3f dPos = pLL->posF - pLL->pos0;
     //   std::cout << " L = " <<  sqrtf( dPos.dot( dPos ) );
    }

    for( cirLeg3f* pCL : pCLvec )
    {
   //     std::cout << "\ncirLeg len = " << pCL->len;
        float maxSpc = pCL->R*AngMax;
        int Nsegs = static_cast<int>( pCL->len/maxSpc ) + 1;
        float ds = pCL->len/Nsegs;
        // 1st
        vec3f iPos = pCL->pos0 + persPt::yHat*railOfstY;
        vec3f ru = pCL->rHat( 0.0f );
        float ruDotYh = ru.dot( persPt::yHat );
        if( ruDotYh*ruDotYh > 0.2f ) ru = pCL->Nu;// for vertical curves
     //   coasterBallVec.push_back( persBall( pCL->pos0 + persPt::yHat*railOfstY, 5.0f, sf::Color::Green ) );
        coasterBallVec.push_back( persBall( iPos + ru*railOfstX, Rball, ballColor ) );
        coasterBallVec.push_back( persBall( iPos - ru*railOfstX, Rball, ballColor ) );

        for( int n = 1; n < Nsegs; ++n )// Nsegs - 1 between
        {
            iPos = pCL->getPos( n*ds ) + persPt::yHat*railOfstY;
            ru = pCL->rHat( n*ds );
            ruDotYh = ru.dot( persPt::yHat );
            if( ruDotYh*ruDotYh > 0.2f ) ru = pCL->Nu;// for vertical curves
            coasterBallVec.push_back(  persBall( iPos + ru*railOfstX, Rball, ballColor ) );
            coasterBallVec.push_back(  persBall( iPos - ru*railOfstX, Rball, ballColor ) );
        }

        // last
        iPos = pCL->posF + persPt::yHat*railOfstY;
        ru = pCL->rHat( pCL->len );
        ruDotYh = ru.dot( persPt::yHat );
        if( ruDotYh*ruDotYh > 0.2f ) ru = pCL->Nu;// for vertical curves
        coasterBallVec.push_back( persBall( iPos + ru*railOfstX, Rball, ballColor ) );
        coasterBallVec.push_back( persBall( iPos - ru*railOfstX, Rball, ballColor ) );
    }

    // some trees
    size_t SSnum, SetNum, FrIdx; fin >> SSnum >> SetNum >> FrIdx;
    persQuad tree( fin, &(SSvec[SSnum].txt) );
    tree.setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), 'R', '1' );
    tree.facingCamera = true;

    fin >> SSnum >> SetNum >> FrIdx;
    persQuad base( fin, &(SSvec[SSnum].txt) );
    base.setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), 'R', '0' );
 //   base.facingCamera = true;

    // make copies of base and tree
    vec3f iPos( 0.0f, railOfstY, 0.0f );
    unsigned int numTrees; fin >> numTrees;
    coasterQuadVec.reserve( 2*numTrees );
    for( unsigned int n = 0; n < numTrees; ++n )
    {
        fin >> iPos.x >> iPos.z;
        base.setPosition( iPos + pos0 );
        coasterQuadVec.push_back( base );
        tree.setPosition( iPos + pos0 + 0.5f*tree.h*persPt::yHat );
        coasterQuadVec.push_back( tree );
    }

    int chkVal; fin >> chkVal;
    std::cout << "\n chkVal = " << chkVal;

    return true;
}

/*
bool lvl_perpective::init_3fPath()
{
    std::ifstream fin("include/leg_types/Leg3f_types/paths3f/pathA.txt");
    if( !fin ) { std::cout << "no pathA data\n"; return false; }

    unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
    sf::Color railColor( rd, gn, bu );
    float railOfstX, railOfstY; fin >> railOfstX >> railOfstY;

    char endType; fin >> endType;// 'O' = open, 'C' = closed
    size_t numLegs = 0;
    // 1st Leg3f
    vec3f pos0;
    if( fin >> pos0.x >> pos0.y >> pos0.z )
    {
        vec3f ofst;
        if( fin >> ofst.x >> ofst.y >> ofst.z )// got 1st leg data
        {
            vec3f posF = pos0 + ofst;
            pCoasterPath = new linLeg3f( pos0, posF );
            Leg3f* pPrev = pCoasterPath;
            numLegs = 1;
             while( fin >> ofst.x >> ofst.y >> ofst.z )// remaining legs
             {
                 posF += ofst;
                 pPrev = new linLeg3f( *pPrev, posF );
                 ++numLegs;
             }

            pPrev->next = endType == 'O' ? nullptr : pCoasterPath;
            L3f_adapter.init( *pCoasterPath, [this](vec3f pos){ persPt::camPos = pos; } );

            // side rails
            coasterRailVec.reserve( 2*numLegs );// 2 per leg
            Leg3f* it = pCoasterPath;
            while( it )
            {
                vec3f Tu = it->T( 0.0f );// along leg
                vec3f Tx = persPt::yHat.cross( Tu ); Tx /= Tx.mag();
                vec3f Ty = Tu.cross( Tx );
                vec3f stPos  = it->pos0 + Tx*railOfstX + Ty*railOfstY;
                vec3f endPos = it->getPos( it->len ) + Tx*railOfstX + Ty*railOfstY;
                coasterRailVec.push_back( persLine( stPos, endPos, railColor ) );
                stPos  = it->pos0 - Tx*railOfstX + Ty*railOfstY;
                endPos = it->getPos( it->len ) - Tx*railOfstX + Ty*railOfstY;
                coasterRailVec.push_back( persLine( stPos, endPos, railColor ) );
                it = it->next;
            }

            return true;
        }
    }

    return false;
}   */


void lvl_perpective::rotateCross()
{
    sf::Vector2f xC( persPt::X0, persPt::Yh );
    horizonCross[0].position.y = persPt::Yh + persPt::X0*tanf( persPt::rollAngle );
    horizonCross[1].position.y = persPt::Yh - persPt::X0*tanf( persPt::rollAngle );
    // pip
    horizonCross[2].position = xC - ( pipLength/2.0f )*sf::Vector2f( sinf( persPt::rollAngle ), cosf( persPt::rollAngle ) );
    horizonCross[3].position = xC + ( pipLength/2.0f )*sf::Vector2f( sinf( persPt::rollAngle ), cosf( persPt::rollAngle ) );
}

bool lvl_perpective::getAniQuad( persBall& rPB, vec3f Pos )// trigger animation at ball position and take ball out of use
{
    rPB.inUse = false;
    rPB.doDraw = false;// to prevent z ordering
    for( persQuadAni& PQA : quadAniVec )
    {
        if( !PQA.inUse )
        {
            PQA.reset();
            PQA.setPosition( Pos );
            anyAnis_inUse = true;
            return true;
        }
    }
    return false;// all Ani are inUse
}

bool lvl_perpective::fireBall( vec3f Pos, vec3f Vel, bool addMuzzleFlash )
{
    for( persBall& PB : ballVec )
    {
        if( !PB.inUse )
        {
            if( addMuzzleFlash ) getAniQuad( PB, Pos );// muzzle flash
            PB.reset( Pos, Vel );
            // new
       /*     if( lvl_perpective::useSounds )
            {
                for( sf::Sound& rSnd : shotSoundVec )
                {
                    if( rSnd.getStatus() != sf::Sound::Playing )
                    {
                        rSnd.play();
                        std::cout << "\n shot sound play!";
                        break;
                    }
                }
            }   */
            // end new
            anyShots_inUse = true;
            return true;
        }
    }

    return false;
}

sf::Sound* lvl_perpective::playShotSound( const sf::SoundBuffer& rSB, float Volume, vec3f soundPos )
{
    if( !lvl_perpective::useSounds ) return nullptr;

    float soundDist = ( soundPos - persPt::camPos ).mag();
    float attenVolume = Volume*soundAttenuDistance/soundDist;
    if( attenVolume > Volume ) attenVolume = Volume;
    else if( attenVolume < soundVolumeMin*Volume ){ std::cout << "\n no sound too quiet"; return nullptr; }// too quiet. Don't bother

    return playShotSound( rSB, attenVolume );
}

sf::Sound* lvl_perpective::playShotSound( const sf::SoundBuffer& rSB, float Volume )
{
    if( !lvl_perpective::useSounds ) return nullptr;

    for( sf::Sound& rSnd : shotSoundVec )
    {
     //   if( rSnd.getStatus() != sf::Sound::Playing )
        if( rSnd.getStatus() == sf::Sound::Stopped )
        {
            rSnd.setBuffer( rSB );

            rSnd.setVolume( Volume );
            rSnd.play();
        //    std::cout << "\n shot sound play!";
            return &rSnd;
        }
    }

    return nullptr;
}

bool lvl_perpective::handleEvent( sf::Event& rEvent )
{
    // for the zoom in function of key 'V'
    static vec3f camPosSave;
 //   static vec3f camDirSave, xuSave, yuSave;

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
        else if( rEvent.key.code == sf::Keyboard::Z )// stop
        {
            camVel = 0.0f;
            to_SF_string( speedNumMsg , 0.0f );
        }
        else if( rEvent.key.code == sf::Keyboard::C )// fire calcBall
        {
            tFlight = 0.0f;
            cbVel0 = ( camVel + cbSpeed0 )*persPt::camDir + 0.05f*cbSpeed0*persPt::yu;// normal value
            cbPos0 = persPt::camPos - fireOfst*persPt::yu;
            if( pTargetQuad )
            {
                if( getFiringVelocity( cbVel0 ) )
                {
                    pTargetQuad->setColor( sf::Color::Blue );
                }
                else pTargetQuad->setColor( sf::Color::Green );
            }
            calcBall.reset( cbPos0, cbVel0 );
        }
        else if( rEvent.key.code == sf::Keyboard::Left ) move_LtRt = -1;
        else if( rEvent.key.code == sf::Keyboard::Right ) move_LtRt = 1;
        else if( rEvent.key.code == sf::Keyboard::Up ) move_UpDown = 1;
        else if( rEvent.key.code == sf::Keyboard::Down ) move_UpDown = -1;
        else if( rEvent.key.code == sf::Keyboard::LShift ) LShiftKeyDown = true;
        else if( rEvent.key.code == sf::Keyboard::RShift ) RShiftKeyDown = true;
        else if( rEvent.key.code == sf::Keyboard::R ) jbCamButt.dropStick = true;// release the joybutton
        else if( rEvent.key.code == sf::Keyboard::B )
        {
            if( LShiftKeyDown || RShiftKeyDown )// trigger superBall position return
            {
                if( !sbReturning )
                {
                    sbReturning = true;
                    vec3f sep = SBpos - superBall.pos;
                    float sepMag = sep.mag();// restore superBall position;
                    sb_tTrip = sepMag/sb_vReturn;
                    sb_tElap = 0.0f;
                    superBall.vel = sep*( sb_vReturn/sepMag );
                }
            }
            else// trigger a barrel roll
            {
                roll_tElap = 0.0f;
            }
        }
        else if( rEvent.key.code == sf::Keyboard::A )// toggle high/low firing angle
        {
            getHighAngle = !getHighAngle;// toggle
            hiLoAngleMsg.setString( getHighAngle ? "High" : "Low" );
        }
        else if( rEvent.key.code == sf::Keyboard::D )// fire timed high and low shots
        {
            if( pTargetButt && pTargetButt->sel )// targeting active
            {
                if( !firingDouble && getHiLoFiringAngles() )
                {
                    vec3f shotPos0 = persPt::camPos - fireOfst*persPt::yu;
                    fireBall( shotPos0, hiFireVel, false );// firing high shot
                    playShotSound( shotFireSoundBuff, shotFireSoundVolume );
                    tDblFire = dtDoubleFire;// delay for firing low shot
                    firingDouble = true;// triggers count down
                    to_SF_string( fireAngleNumMsg, asinf( hiFireVel.y/loFireVel.mag() )*180.0f/myPI );
                    tFireMsg = dtFireMsg;
                    getHighAngle = true;
                    hiLoAngleMsg.setString( "High" );
                    if( pTargetQuad ) pTargetQuad->setColor( sf::Color::Blue );
                }
            }
            else// toggle single/double cannon
            {
                std::cout << "\nTargeting is off";
                if( pDblFireButt ) pDblFireButt->setSel( !pDblFireButt->sel );
            }
        }
        else if( rEvent.key.code == sf::Keyboard::V )// zoom the view in
        {
            if( !isZoomed && pTargetQuad )
            {
                isZoomed = true;
                camPosSave = persPt::camPos;
            //    camDirSave = persPt::camDir;
            //    xuSave = persPt::xu;
            //    yuSave; = persPt::yu;
                vec3f sep = persPt::camPos - pTargetQuad->pos;
                persPt::camPos = pTargetQuad->pos + sep*zoomFactor;
            //    persPt::calibrateCameraAxes();
                for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);
            }
        }

    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        move_LtRt = move_UpDown = 0;
        firing = false;
        jbCamButt.dropStick = false;
        if( isZoomed )
        {
            isZoomed = false;
            persPt::camPos = camPosSave;
            persPt::calibrateCameraAxes();
            for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);
        }

        if( rEvent.key.code == sf::Keyboard::L )// toggle freeView
        {
            freeView = !freeView;

            if( freeView )
            {
                freeCamDir = persPt::camDir;
                freeCamXu = persPt::xu;
                freeCamYu = persPt::yu;
            }
            else
            {
                // immediate return
           //     persPt::camDir = freeCamDir;
           //     persPt::xu = freeCamXu;
           //     persPt::yu - freeCamYu;
                // animated return
                float ang = acosf( persPt::camDir.dot( freeCamDir ) );
                LA_tFinal = ang/LA_rotSpeed;
                if( LA_tFinal < 0.7f ) LA_tFinal = 0.7f;
            //    LA_tFinal = 1.0f/LA_rotSpeed;
                std::cout << "\nLA return: angle = " << ang << " tFinal = " << LA_tFinal;
                LA_tElap = 0.0f;
                delCamDir = freeCamDir - persPt::camDir;
                delXu = freeCamXu - persPt::xu;
                delYu = freeCamYu - persPt::yu;
            }
        }
        // avoids key repeat issue if done on release!
        else if( rEvent.key.code == sf::Keyboard::H ) showHelp = !showHelp;// toggle showSelp
        else if( rEvent.key.code == sf::Keyboard::LShift ) LShiftKeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::RShift ) RShiftKeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::P )
        {
            if( pCoasterPath && L3f_adapter.pSetPosition )
            {
                camOnCoaster = !camOnCoaster;// toggle
                if( camOnCoaster )
                {
                     L3f_adapter.reset();
                     camVel = 0.0f;
                     for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);

                     if( L3f_adapter.pLegCurr )
                     {
                         vec3f Tu = L3f_adapter.pLegCurr->T( L3f_adapter.s );
                         while( camDirQ.size() > 0 ) camDirQ.pop();// until empty
                         camDirSum = Tu*static_cast<float>( camDirQ_sz );
                         for( size_t j = 0; j < camDirQ_sz; ++j )
                            camDirQ.push( Tu );
                     }
                }
            }
        }
        else if( rEvent.key.code == sf::Keyboard::S )// take a camStep
        {
            persPt::camPos += persPt::camDir*camStep;
            if( jbCamButt.atRest ) for( persPt* pPt : p_persPtVec ) pPt->update(0.0f);
        }
        else if( rEvent.key.code == sf::Keyboard::T )// toggle targeting mode
        {
            if( pTargetButt )
            {
                if( pTargetQuad && pTargetButt->sel ){ pTargetQuad->setColor( sf::Color::White ); pTargetQuad = nullptr; }
                pTargetButt->setSel( !pTargetButt->sel );
            }
        }
//        else if( rEvent.key.code == sf::Keyboard::M ) Pend.bob.isMoving = !Pend.bob.isMoving;
        else if( rEvent.key.code == sf::Keyboard::M )
        {
            if( pPendResetButt )
            {
                pPendResetButt->setSel( !pPendResetButt->sel );
                Pend.bob.isMoving = pPendResetButt->sel;// why isn't setSel() doing this?
            }

        //    PendRunButt.setSel( !PendRunButt.sel );
        //    Pend.bob.isMoving = PendRunButt.sel;// why isn't setSel() doing this?
        }
        else if( rEvent.key.code == sf::Keyboard::F )// fire fragShotA
        {
            if( !fragShotA.inUse )
            {
                fragShotA.fire( getShotPos0(), getShotVel0() );
                if( lvl_perpective::useSounds ) playShotSound( fragFireSoundBuff, fragFireSoundVolume );
            }
        }
    }

    return true;
}

//void lvl_perpective::updatePane()
//{
 //   upperPane[2].position = lowerPane[1].position = trueHorizon[1].position;
 //   upperPane[3].position = lowerPane[0].position = trueHorizon[0].position;
//}

void lvl_perpective::updatePane()
{
    // direction toward horizon
    float zcDotY = persPt::camDir.dot( persPt::yHat );
    vec3f hu = persPt::camDir - zcDotY*persPt::yHat;
    float huMag = hu.mag();
    if( huMag < 0.01f ) hu = persPt::zHat;
    else hu /= huMag;

    vec3f hu2 = persPt::yHat.cross( hu );// along horizon
    // horizon line
    sf::Vector2f &hzLn0 = trueHorizon[0].position, &hzLn1 = trueHorizon[1].position;
    hzLn0 = persPt::get_xyw( persPt::camPos + persPt::Z0*( hu - hu2 ) );// left end
    hzLn1 = persPt::get_xyw( persPt::camPos + persPt::Z0*( hu + hu2 ) );// right end

    sf::Vector2f hPerp( hzLn1.y - hzLn0.y, hzLn0.x - hzLn1.x );
    upperPane[0].position = hzLn0 + hPerp;
    upperPane[1].position = hzLn1 + hPerp;

    upperPane[2].position = lowerPane[1].position = hzLn1;
    upperPane[3].position = lowerPane[0].position = hzLn0;

    lowerPane[2].position = hzLn1 - hPerp;
    lowerPane[3].position = hzLn0 - hPerp;
}

void lvl_perpective::update( float dt )
{
    if( !jbCamButt.atRest )
    {
        if( button::mseDnLt ){ camVel += camAccel*dt; to_SF_string( speedNumMsg, camVel ); }
        else if( button::mseDnRt ) { camVel -= camAccel*dt; to_SF_string( speedNumMsg, camVel ); }

        float dAr = 0.0f;
        if( button::didScroll )
        {
            dAr = rollRate*button::scrollAmount;
     //       if( dAr*dAr < rollRate*rollRate ) dAr = 0.0f;// assure ability to stop rolling
        }

        float compassAngle = persPt::changeCamDir( yawRate*dt, pitchRate*dt, dAr );
        to_SF_string( angleNumMsg, compassAngle );
        float U = persPt::camDir.dot( persPt::yHat );

        if( camOnCoaster )
        {
            if( pCoasterPath )
            {
                L3f_adapter.v = camVel;
                L3f_adapter.update( dt );
            //    if( !L3f_adapter.pLegCurr ) camOnCoaster = false;// path end reached
                // keep xu level
            //    persPt::xu = persPt::yHat.cross( persPt::camDir );
            //    persPt::xu /= persPt::xu.mag();
            //    persPt::yu = persPt::camDir.cross( persPt::xu );

                // center view on track
                if( L3f_adapter.pLegCurr ) updateCoasterView();
                else camOnCoaster = false;// path end reached

                if( !camOnCoaster )// test
                {
                    std::cout << "\n leaving coaster at: " << persPt::camPos.x << ", " << persPt::camPos.y << ", " << persPt::camPos.z;
                }
            }
        }
        else
        {
            if( freeView )
            {
                persPt::camPos += camVel*freeCamDir*dt;
                // keep xu level
                persPt::xu = persPt::yHat.cross( persPt::camDir );
                persPt::xu /= persPt::xu.mag();
                persPt::yu = persPt::camDir.cross( persPt::xu );
            }
            else// normal navigation
            {
                persPt::camPos += camVel*dt*persPt::camDir;// the change in camera position

                if( roll_tElap < roll_tFinal )// barrel roll
                {
                    updateBarrelRoll(dt);
                }
                else if( LA_tElap < LA_tFinal )
                {
                    LA_tElap += dt;
                    float u = LA_tElap/LA_tFinal;
                    if( u > 1.0f ) u = 1.0f;
                    // easing
                    float A = 3.0f + LA_k4, B = -2.0f*( 1.0f + LA_k4 );
                    float uSq = u*u, w = A*u*u + B*u*uSq + LA_k4*uSq*uSq;
                   // interpolate each vector
                    float q = 1.0f - w;//u;// dist from end
                    persPt::camDir = freeCamDir - delCamDir*q;
                    persPt::xu = freeCamXu - delXu*q;
                    persPt::yu = freeCamYu - delYu*q;
                }
                else if( pArButt && pArButt->sel && U*U < 0.9f )// apply roll with limited pitch
                {
                    float Ar = -atanf( autoRollSens*camVel*yawRate*persPt::yu.dot( persPt::yHat )/gravity.y );
                    vec3f hu = persPt::yHat.cross( persPt::camDir );
                    hu /= hu.mag();
                    persPt::xu = cosf( Ar )*hu - sinf( Ar )*persPt::yHat;
                    persPt::yu = persPt::camDir.cross( persPt::xu );
                }
            }
        }

        persPt::calibrateCameraAxes();
        // translational motion via arrow keys
        if( move_LtRt != 0 ) persPt::camPos += move_LtRt*move_xyVel*persPt::xu*dt;
        if( move_UpDown != 0 ) persPt::camPos += move_UpDown*move_xyVel*persPt::yu*dt;
        to_SF_string( camXmsg, persPt::camPos.x );
        to_SF_string( camYmsg, persPt::camPos.y );
        to_SF_string( camZmsg, persPt::camPos.z );

//        to_SF_string( pitchNumMsg, U );
        to_SF_string( pitchNumMsg, asinf( U )*180.0f/myPI );
        float B = persPt::xu.dot( persPt::yHat );
        if( B*B < 1.0e-8f ) B = 0.0f;
        to_SF_string( rollNumMsg, B );

    //    float flipFactor = persPt::yu.dot( persPt::yHat ) > 0.0f ? 1.0f : -1.0f;
    //    trueHorizon[0].position.y = trueHorizon[1].position.y = persPt::Yh + flipFactor*persPt::Z0*tanf( asinf( U ) );
    //    trueHorizon[0].position.y -= flipFactor*persPt::X0*tanf( asinf( persPt::xu.dot( persPt::yHat  ) ) );
    //    trueHorizon[1].position.y += flipFactor*persPt::X0*tanf( asinf( persPt::xu.dot( persPt::yHat  ) ) );

    //    if( pPaneButt && pPaneButt->sel ) updatePane();
        if( pPaneButt && pPaneButt->sel ) persPt::updateBackground( trueHorizon, upperPane, lowerPane );

        for( persLine& PL : lineVec ) PL.update(dt);
        if( pCoasterPath )
        {
            for( persLine& PL : coasterRailVec ) PL.update(dt);
            for( persBall& PB : coasterBallVec ) PB.update(dt);
            for( persQuad& PQ : coasterQuadVec ) PQ.update(dt);
        }

  //      persLS.update(dt);
        for( persBox_wf& box : boxVec ) box.update(dt);
    //    for( persBox_quad& Qbox : quadBoxVec ) Qbox.update(dt);
        for( persQuad& PQ : quadVec ) PQ.update(dt);
        for( persQuad& PQ : terrainPQvec ) PQ.update(dt);
        for( persShadow& PS : shadowVec ) PS.update(dt);
        for( shadowAdapter& rSA : shAdaptVec ) rSA.update(dt);

        for( persIsland* pPI : pIslandVec ) pPI->update(dt);
        for( persQuad& PQ : hyperVec_1 ) PQ.update(dt);
        for( persQuad& PQ : hyperVec_2 ) PQ.update(dt);
    //    curve1.update(dt);
        handleHyperJump(dt);
        shadow4_PQB.update(dt);

        // update targeting
        updateTargeting();
    }// end of  if( !jbCamButt.atRest )

    // update moving and/or animating objects
    if( anyShots_inUse )
    {
        anyShots_inUse = false;
        unsigned int numInUse = 0;
        for( persBall& PB : ballVec )
        {
            PB.update( PB.pos.y > 0.0f ? gravity : -gravity, dt );// falls upward under ground
            if( PB.inUse ){ anyShots_inUse = true; ++numInUse; }
        }

        to_SF_string( numShotsNumMsg, numInUse );
    }

    fragShotA.update( (fragShotA.pos.y > 0.0f) ? gravity : -gravity, dt );// falls upward under ground
    if( fragShotA.inUse ) hitAll( fragShotA, dt );

    // droneQuad flies above the coaster
    updateDrones(dt);

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

    // on paths
    for( legAdapter& rLA : carLAvec ) rLA.update(dt);
    // messagePQA
    messagePQA.update(dt);
    // superBall
    superBall.update(dt);
    if( superBall.isMoving && !sbReturning )// apply drag unless SB is returning to SBpos
    {
        // bounce off of the ground ?
        if( superBall.pos.y < 0.0f && superBall.vel.y < 0.0f ) superBall.vel.y *= -1.0f;

        float VsbMag = superBall.vel.mag();
        if( VsbMag > SBdragCoeff*dt ) superBall.vel -= superBall.vel*SBdragCoeff*dt/VsbMag;// apply constant drag
        else superBall.vel *= 0.0f;// full stop
    }
    // animated SB return via key B
    if( sbReturning && ( sb_tElap += dt ) > sb_tTrip )
    {
        superBall.setPosition( SBpos );
        superBall.vel *= 0.0f;
        sbReturning = false;
    }
    // harmOsciBall
    harmOsciBall.update_doDraw();
    if( harmOsciBall.doDraw )
    {
        harmOsciBall.update(dt);
        float K = dt/harmOsciBall.mass;
        harmOsciBall.vel -= harmOsciBall.vel*HOdamp*K;// apply damping force
        harmOsciBall.vel -= ( harmOsciBall.pos - HOpos )*HOspring*K;// apply spring force
    }

    if( calcBall.inUse )
    {
        tFlight += dt;
        if( tFlight > tFlightMax )
        {
            vec3f N = ( persPt::camPos - calcBall.pos );
            N /= N.mag();
            getAniQuad( calcBall, calcBall.pos + 10.0f*N );
            calcBall.inUse = false;
        }
        else
        {
        //    calcBall.pos.x = cbPos0.x + cbVel0.x*tFlight;
        //    calcBall.pos.z = cbPos0.z + cbVel0.z*tFlight;
        //    calcBall.pos.y = cbPos0.y + cbVel0.y*tFlight + gravity.y*tFlight*tFlight/2.0f;
        //    calcBall.vel = cbVel0 + gravity*tFlight;
            // accelerate towards droneQuad
            float vMag = calcBall.vel.mag();
            vec3f vu = calcBall.vel*( 1.0f/vMag );
            vec3f sep = droneQuad.pos - calcBall.pos;
            vec3f sepU = sep*( 1.0f/sep.mag() );
            vec3f acc = cbKaccel*vMag*( vu.cross( sepU ) ).cross( vu );
            calcBall.pos += calcBall.vel*dt + acc*(0.5f*dt*dt);
            calcBall.vel += acc*dt;

            calcBall.update(dt);
            hitAll( calcBall, dt );
        }
    }

    bool doDrawLast = multiBallBox.doDraw;
    multiBallBox.update_doDraw();
    if( multiBallBox.doDraw )// process motion
    {
        multiBallBox.update(dt);
        updateFreeBalls(dt);
    }
    else if( doDrawLast )// just became false. Assign ball.doDraw = false
        for( persBall& PB : freeBallVec ) PB.doDraw = false;

    Pend.update(dt);

    // firing
 //   vec3f shotPos0 = persPt::camPos - fireOfst*persPt::yu;
    vec3f shotPos0 = getShotPos0();
    if( firing && (tFire+=dt) > tFireDelay )
    {
   //     vec3f velShot = ( camVel + fireVel )*persPt::camDir + 0.05f*fireVel*persPt::yu;// normal value
        vec3f velShot = getShotVel0();

        if( pTargetQuad )
        {

            if( getFiringVelocity( velShot, getHighAngle ) )
            {
                float fireAngle = asinf( velShot.y/velShot.mag() );
                to_SF_string( fireAngleNumMsg, fireAngle*180.0f/myPI );
                tFireMsg = dtFireMsg;
                pTargetQuad->setColor( sf::Color::Blue );
            }
            else
                pTargetQuad->setColor( sf::Color::Green );
        }

        if( pDblFireButt && pDblFireButt->sel )
        {
            bool fireA = fireBall( shotPos0 - fireOfst*persPt::xu, velShot, false );
            if( fireBall( shotPos0 + fireOfst*persPt::xu, velShot, false ) || fireA )
                playShotSound( shotFireSoundBuff, shotFireSoundVolume );
        }
        else
        {
            if( fireBall( shotPos0, velShot, false ) )
                playShotSound( shotFireSoundBuff, shotFireSoundVolume );
        }


        tFire -= tFireDelay;
    }

    // firing the timed 2nd shot
    if( firingDouble && (tDblFire-=dt) <= 0.0f )
    {
        fireBall( shotPos0, loFireVel, false );
        playShotSound( shotFireSoundBuff, shotFireSoundVolume );
        to_SF_string( fireAngleNumMsg, asinf( loFireVel.y/loFireVel.mag() )*180.0f/myPI );
        tFireMsg = dtFireMsg;
        firingDouble = false;
        getHighAngle = false;
        hiLoAngleMsg.setString( "Low" );
    }

    // timed firing angle display
    if( tFireMsg > 0.0f ) tFireMsg -= dt;


//    if( doSort ) std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare );

    // translational motion via arrow keys
//    if( move_LtRt != 0 ) persPt::camPos += move_LtRt*move_xyVel*persPt::xu*dt;
//    if( move_UpDown != 0 ) persPt::camPos += move_UpDown*move_xyVel*persPt::yu*dt;

    // collision test balls against targets
    if( anyShots_inUse )
    {
        for( persBall& PB : ballVec )
        {
            hitAll( PB, dt );
        }
    }

    // establish z order for drawing
    update_zOrder();

 //   std::sort( pPtVec_4list.begin(), pPtVec_4list.begin() + numToDraw, [this]( persPt* pA, persPt* pB ){ return pB->getDistance() < pA->getDistance(); } );

    return;
}// end of update()

void lvl_perpective::update_zOrder()
{
    numToDraw = 0;
    size_t sz = pPtVec_4list.size();
    for( persPt* pPt : common_pPtVec )
        if( pPt->doDraw )
        {
            if( numToDraw >= sz ){ std::cerr << " sz exceeded "; break; }
            pPtVec_4list[ numToDraw++ ] = pPt;
        }

    if( anyShots_inUse )
        for( persBall& PB : ballVec )
            if( PB.inUse && PB.doDraw )
            {
                if( numToDraw >= sz ){ std::cerr << " sz exceeded "; break; }
                pPtVec_4list[ numToDraw++ ] = &PB;
            }

    if( anyAnis_inUse )
        for( persQuadAni& PQA : quadAniVec )
            if( PQA.inUse && PQA.doDraw )
            {
                if( numToDraw >= sz ){ std::cerr << " sz exceeded "; break; }
                pPtVec_4list[ numToDraw++ ] = &PQA;
            }

    if( fragShotA.inUse && fragShotA.doDraw )
    {
        if( numToDraw >= sz ){ std::cerr << " sz exceeded "; }
        pPtVec_4list[ numToDraw++ ] = &fragShotA;
    }

    if( numToDraw >= sz ) numToDraw = sz - 1;
    std::sort( pPtVec_4list.begin(), pPtVec_4list.begin() + numToDraw, [this]( const persPt* pA, const persPt* pB ){ return pB->getDistance() < pA->getDistance(); } );
}

void lvl_perpective::updateBarrelRoll( float dt )
{
    if( roll_tElap < roll_tFinal )// barrel roll
    {
        roll_tElap += dt;
        float u = roll_tElap/roll_tFinal;// u: 0 -> 1
        float angSpeed = 0.0f;

        if( u > 1.0f ) u = 1.0f;
        float uSq = u*u;

        {
            float u1 = 0.15;// u2 = 1.0f - u1;
            float A = 2.0f, C = u1*( 1.0f - u1 ), B = 1.0f + C;
            float K = 30.0f/( 1.0f - 5.0f*C );// 1/magnitude
            angSpeed = K*( uSq*uSq - A*uSq*u + B*uSq - C*u );
        }

        float Vavg = 2.0f*vec2f::PI/roll_tFinal;
        angSpeed *= Vavg;
        persPt::roll( angSpeed*dt );
    }
}

// helper function for buffered acceleration
vec3f getAvgAccel( std::queue<vec3f>& accelBuff, vec3f newAccel )// return running average over size of queue
{
    static vec3f accelSum = vec3f();// will build from zero over the 1st droneAccelQueue.size() frames
    accelSum += newAccel;
    accelBuff.push( newAccel );
    accelSum -= accelBuff.front();
    accelBuff.pop();
    if( accelBuff.size() > 1 )
        return accelSum*( 1.0f/accelBuff.size() );

    return vec3f();
}

void lvl_perpective::updateDrones( float dt )
{
    if( L3f_adapterDrone.pLeg0 )
    {
   //     const Leg3f* pLegi = L3f_adapterDrone.pLegCurr;
        L3f_adapterDrone.update( dt );
        float S = L3f_adapterDrone.s;
        vec3f Tu = L3f_adapterDrone.pLegCurr->T( S );
   //     const Leg3f* pLegf = L3f_adapterDrone.pLegCurr;

        // update orientation
        vec3f accel = L3f_adapterDrone.accel();
        accel = getAvgAccel( droneAccelQueue, accel );

        vec3f accTotal = droneAccFactor*accel - gravity;
        accTotal /= accTotal.mag();// unit vector in direction of
        droneQuad.setOrientation( accTotal, Tu );
    }
    else if( mvAdaptAcc.pSetPosAcc && !mvAdaptAcc.accelQueue.empty() )
    {
        mvAdaptAcc.update(dt);
    }

    droneQuad.update(dt);
}

void lvl_perpective::updateCoasterView()
{
    // center view on track
    const Leg3f* pLeg = L3f_adapter.pLegCurr;
    if( !pLeg ) return;

    vec3f T1 = pLeg->T( L3f_adapter.s );
//    float lookAhead = 300.0f;
    float sAhead = L3f_adapter.s + trackLookAhead;
    vec3f T2;
    if( sAhead < pLeg->len )// same Leg
    {
        T2 = pLeg->T( sAhead );
    }
    else// next Leg
    {
        sAhead -= pLeg->len;
        if( pLeg->next )
        {
            T2 = pLeg->next->T( sAhead );
        }
        else T2 = T1;
    }

//    persPt::camDir = L3f_adapter.pLegCurr->T( L3f_adapter.s );
    persPt::camDir = ( T1 + T2 )*0.5f;
    persPt::xu = persPt::yHat.cross( persPt::camDir );
    persPt::xu /= persPt::xu.mag();
    persPt::yu = persPt::camDir.cross( persPt::xu );
    // yaw
//     float yawAngle = yawRate*vec2f::PI/( camVelXZscale*jbCamButt.r_ring*jbCamButt.r_ring );
    sf::Vector2f dPos = jbCamButt.Bpos - jbCamButt.pos;
//                    float yawAngle = 0.8f*vec2f::PI*dPos.x/jbCamButt.r_ring;
    float rx = dPos.x/jbCamButt.r_ring;
    float yawAngle = trackYawRange*vec2f::PI*rx*rx;
    if( dPos.x < 0.0f ) yawAngle *= -1.0f;
    persPt::camDir = persPt::camDir.rotate_axis( persPt::yu, yawAngle );// yaw
    persPt::xu = persPt::yu.cross( persPt::camDir );// new xu
    persPt::xu /= persPt::xu.mag();
//     float pitchAngle = pitchRate*0.5f*vec2f::PI/( camVelXZscale*jbCamButt.r_ring*jbCamButt.r_ring );
    float ry = dPos.y/jbCamButt.r_ring;
    float pitchAngle = trackPitchRange*vec2f::PI*ry*ry;
    if( dPos.y < 0.0f ) pitchAngle *= -1.0f;
    persPt::camDir = persPt::camDir.rotate_axis( persPt::xu, pitchAngle );// pitch
    // get average value
    if( camDirQ.size() > 0 )
    {
        camDirSum += persPt::camDir - camDirQ.front();
        camDirQ.pop();// oldest value
        camDirQ.push( persPt::camDir );// newest value
        persPt::camDir = camDirSum*( 1.0f/static_cast<float>(camDirQ_sz) );
        persPt::xu = persPt::yu.cross( persPt::camDir );// new xu
        persPt::xu /= persPt::xu.mag();
    }

    persPt::yu = persPt::camDir.cross( persPt::xu );// new yu
}

void lvl_perpective::updateFreeBalls( float dt )
{
    if( freeGrav.dot( freeGrav ) > 1.0f ) { for( persBall& PB : freeBallVec ) PB.update( freeGrav, dt ); }
    else { for( persBall& PB : freeBallVec ) PB.update(dt); }


    // collision test
    if( pHitAllButt && pHitAllButt->sel )
    {
        unsigned int hitCnt = persBall::hitAll( freeBallVec, coeffRest, dt );
        if( hitCnt > 0 ) playShotSound( shotBounceSoundBuff, shotBounceSoundVolume, multiBallBox.pos );
        hitCnt = multiBallBox.hitAll_inside( freeBallVec );
        if( hitCnt > 0 )
        {
        //    std::cout << "\nhitCnt = " << hitCnt;
            playShotSound( ballVsWallSoundBuff, ballVsWallSoundVolume, multiBallBox.pos );
        }
    }
}

void lvl_perpective::hitAll( persBall& PB, float dt )
{
    if( !PB.inUse ) return;

    vec3f P, vu;
    if( PB.pos.y < 0.0f && PB.pos.y - PB.vel.y*dt >= 0.0f )// target = the ground
    {
    //    getAniQuad( PB, PB.pos + 10.0f*persPt::yHat );
        if( getAniQuad( PB, PB.pos + 10.0f*persPt::yHat ) )
            playShotSound( shotBoomSoundBuff, shotBoomSoundVolume, PB.pos );
     //       playShotSound( shotBounceSoundBuff, shotBounceSoundVolume, PB.pos );
        return;
    }
    if( pBounceQuad->hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        PB.setPosition( P + 5.0f*vu );// a bit in front
        PB.vel = coeffRest*PB.vel.mag()*vu;// bounce off
        playShotSound( shotBounceSoundBuff, shotBounceSoundVolume );
        return;
    }
 //   else if( superBall.hitFixed( PB.pos - PB.vel*dt, PB.Rbound, PB.pos, P, vu ) )
    if( superBall.hitFree( PB, coeffRest, dt ) )
    {
        playShotSound( shotBounceSoundBuff, shotBounceSoundVolume );
        return;
    }
    if( harmOsciBall.hitFree( PB, coeffRest, dt ) )
    {
        playShotSound( shotBounceSoundBuff, shotBounceSoundVolume );
        return;// it's all in the function call
    }
    if( Pend.topQ.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
   //     getAniQuad( PB, P + 5.0f*vu );
        if( getAniQuad( PB, P + 5.0f*vu ) )
            playShotSound( shotBoomSoundBuff, shotBoomSoundVolume, PB.pos );
        return;
    }
    if( Pend.hit( PB, coeffRest, dt ) )
    {
        playShotSound( shotBounceSoundBuff, shotBounceSoundVolume );
        return;
    }
    // check vs the buildings
    for( persBox_quad* pPBQ : pPBQvec )
    {
        if( pPBQ->hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
        {
            vec3f N = ( persPt::camPos - pPBQ->pos );
            N /= N.mag();
         //   getAniQuad( PB, P + 10.0f*N );
            if( getAniQuad( PB, P + 10.0f*N ) )
            playShotSound( shotBoomSoundBuff, shotBoomSoundVolume, PB.pos );
            return;
        }
    }

    // check vs droneQuad
    vec3f vRel = PB.vel - L3f_adapterDrone.velocity();
 //   if( droneQuad.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
//    if( droneQuad.hit( PB.pos - vRel*dt, PB.pos, P, vu ) )
    if( droneQuad.persPt::hit( PB.pos - vRel*dt, PB.pos, P, vu ) )
    {
        vec3f N = ( persPt::camPos - droneQuad.pos );
        N /= N.mag();
        if( getAniQuad( PB, P + 10.0f*N ) )
            playShotSound( shotBoomSoundBuff, shotBoomSoundVolume, PB.pos );
        std::cout << "\n drone HIT!!";
        return;
    }

    if( pHitAllButt && pHitAllButt->sel )// check vs the freeBalls
    {
        for( persBall& freePB : freeBallVec )
            if( PB.hitFree( freePB, coeffRest, dt ) )
            {
                playShotSound( shotBounceSoundBuff, shotBounceSoundVolume );
                return;
            }
    }
}

void lvl_perpective::handleHyperJump( float dt )
{
    persQuad *pPQin = nullptr, *pPQout = nullptr;
    vec3f P, vu;

    for(  size_t i = 0; i < hyperVec_1.size() && i < hyperVec_2.size(); ++i )
    {
        if( hyperVec_1[i].hit( persPt::camPos, persPt::camPos + persPt::camDir*hyperVec_1[i].Rbound*2.0f, P, vu ) )
        {
            if( persPt::camDir.dot( hyperVec_1[i].Nu ) < 0.0f )// entering
            {
                std::cout << "\n hit hyperVec_1[" << i << ']';
                pPQin = &( hyperVec_1[i] );
                pPQout = &( hyperVec_2[i] );
                break;
            }
        }
        else if( hyperVec_2[i].hit( persPt::camPos, persPt::camPos + persPt::camDir*hyperVec_2[i].Rbound*2.0f, P, vu ) )
        {
            if( persPt::camDir.dot( hyperVec_2[i].Nu ) < 0.0f )// entering
            {
                std::cout << "\n hit hyperVec_2[" << i << ']';
                pPQin = &( hyperVec_2[i] );
                pPQout = &( hyperVec_1[i] );
                break;
            }
        }
    }

    if( pPQin && pPQout )// handle crossing
    {
        vec3f xu = pPQin->pt[0] - pPQin->pt[3]; xu /= xu.mag();
        vec3f yu = pPQin->pt[1] - pPQin->pt[0]; yu /= yu.mag();
        vec3f Cdir( persPt::camDir.dot( xu ), persPt::camDir.dot( yu ), persPt::camDir.dot( -pPQin->Nu ) );
        xu = pPQout->pt[3] - pPQout->pt[0]; xu /= xu.mag();
        yu = pPQout->pt[1] - pPQout->pt[0]; yu /= yu.mag();
        persPt::camDir = Cdir.x*xu + Cdir.y*yu + Cdir.z*pPQout->Nu;
        persPt::camPos = pPQout->pos + persPt::camDir*camVel*dt;
        std::cout << "\n hyperJump";
        // recalibrate camera basis
        persPt::camDir /= persPt::camDir.mag();
        persPt::xu = persPt::yu.cross( persPt::camDir );
        persPt::xu /= persPt::xu.mag();
        persPt::yu = persPt::xu.cross( persPt::camDir );
//        if( pPQout == &hyperPQb ) islandB.takePicture( islandB.pos - persPt::camPos, 0.0f );// take picture from emergent perspective
    }
}

void lvl_perpective::updateTargeting()
{
    if( pTargetButt && !pTargetButt->sel ) return;

    // still on same target?
//    if( pTargetQuad && pTargetQuad->isSighted( targetDistance ) ) return;// distance updated

    float distMin = 1.0e6f;
    persQuad* pTargetMin = nullptr;

    bool hit = false;

    // vs targets at an island
    for( persIsland* pPIs : pIslandVec )
    {
        if( !pPIs->isInsideR1 ) continue;
        persQuad* pPQ = nullptr;
        for( persBox_quad* pPBQ : pPIs->pPBQ_targetVec )
        {
            if( pPBQ && pPBQ->isSighted( targetDistance, pPQ ) )
            {
                hit = true;
                if( targetDistance < distMin )
                {
                    distMin = targetDistance;
                    pTargetMin = pPQ;
                }
            }
        }
    }

    // vs designated target
    if( !hit && pBounceQuad->isSighted( targetDistance ) )
    {
        hit = true;
        if( targetDistance < distMin )
        {
            distMin = targetDistance;
            pTargetMin = pBounceQuad;
        }
    }

    if( !hit )// vs the buildings
    {
        persQuad* pPQ = nullptr;
        for( persBox_quad& PBQ : quadBoxVec )
        {
            if( PBQ.isSighted( targetDistance, pPQ ) )
            {
                hit = true;
                if( targetDistance < distMin )
                {
                    distMin = targetDistance;
                    pTargetMin = pPQ;
                }
            //    break;
            }
        }
    }

    if( pTargetMin )// a hit
    {
        if( pTargetMin != pTargetQuad )
        {
            if( pTargetQuad ) pTargetQuad->setColor( sf::Color::White );
            pTargetMin->setColor( sf::Color::Red );
        }
        pTargetQuad = pTargetMin;
        targetDistance = distMin;
    }

    if( !hit && pTargetQuad )
    {
        pTargetQuad->setColor( sf::Color::White );
        pTargetQuad = nullptr;
    }
}

bool lvl_perpective::getFiringVelocity( vec3f& vel, bool getHigh )// false if h too low or R too great for given fireVel. writes vel
{
    vec3f sep = persPt::camDir*targetDistance;
    sep += fireOfst*persPt::yu;// correct for cannon offset from view point
    float h = sep.dot( persPt::yHat );// - fireOfst*persPt::yu.dot( persPt::yHat );// vertical component
    vec3f rxz = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;
    float R = rxz.mag();// horizontal
    rxz /= R;// now unit length
    float V0 = fireVel;// + camVel; it's not that simple
    float k = -gravity.y*R*R/(2.0f*fireVel*fireVel);
    float deter = R*R - 4.0f*k*( h + k );
    if( deter < 0.0f ){ std::cout << "\nOut of range"; return false; }
    float tanAngle = getHigh ? R + sqrtf( deter ) : R - sqrtf( deter );
    tanAngle /= 2.0f*k;
    float sA = tanAngle/sqrtf( 1.0f + tanAngle*tanAngle ), cA = sqrtf( 1.0f - sA*sA );
    vel = V0*( cA*rxz + sA*persPt::yHat );
    return true;
}

bool lvl_perpective::getHiLoFiringAngles()// also writes difference in flight time
{
    vec3f sep = persPt::camDir*targetDistance;
//    std::cout << "\ntargetDistance = " << targetDistance;
    sep += fireOfst*persPt::yu;// correct for cannon offset from view point
    float h = sep.dot( persPt::yHat );// - fireOfst*persPt::yu.dot( persPt::yHat );// vertical component

    vec3f rxz = sep - ( sep.dot( persPt::yHat ) )*persPt::yHat;
    float R = rxz.mag();// horizontal

    rxz /= R;// now unit length
    float V0 = fireVel;// + camVel; it's not that simple
 //   std::cout << "  h = " << h << " R = " << R;
    float k = -gravity.y*R*R/(2.0f*fireVel*fireVel);
    float deter = R*R - 4.0f*k*( h + k );
    if( deter < 0.0f ){ std::cout << "\nOut of range"; return false; }

    float tanAngle = ( R - sqrtf( deter ) )/( 2.0f*k );
    float sA = tanAngle/sqrtf( 1.0f + tanAngle*tanAngle ), cA = sqrtf( 1.0f - sA*sA );
    loFireVel = V0*( cA*rxz + sA*persPt::yHat );
//    std::cout << " Low angle = " << atanf( tanAngle )*180.0f/myPI;
    float tFlightLow = R/( V0*cA );

    tanAngle = ( R + sqrtf( deter ) )/( 2.0f*k );
    sA = tanAngle/sqrtf( 1.0f + tanAngle*tanAngle ); cA = sqrtf( 1.0f - sA*sA );
    hiFireVel = V0*( cA*rxz + sA*persPt::yHat );
 //   std::cout << " High angle = " << atanf( tanAngle )*180.0f/myPI;
    float tFlightHigh = R/( V0*cA );

    dtDoubleFire = tFlightHigh - tFlightLow;
    return true;
}

void lvl_perpective::draw( sf::RenderTarget& RT ) const
{
    if( pPaneButt && pPaneButt->sel )
    {
        RT.draw( upperPane, 4, sf::Quads );
        RT.draw( lowerPane, 4, sf::Quads );
        RT.draw( trueHorizon, 2, sf::Lines );
    }

    for( const persLine& PL : lineVec ) PL.draw(RT);
    if( pCoasterPath )
    {
        for( const persLine& PL : coasterRailVec ) PL.draw(RT);
        for( const persBall& PB : coasterBallVec ) PB.draw(RT);
        for( const persQuad& PQ : coasterQuadVec ) PQ.draw(RT);
    }
//    for( const persPt* pPt : p_persPtVec ) pPt->draw(RT);
    for( size_t n = 0; n < numToDraw; ++n ) pPtVec_4list[n]->draw(RT);
    RT.draw( horizonCross, 4, sf::Lines );
    if( pTargetButt && pTargetButt->sel )
    {
        RT.draw( hiLoAngleMsg );
        if( tFireMsg > 0.0f ) RT.draw( fireAngleNumMsg );
    }

    // messages on top
    RT.draw( camPosMsg );
    RT.draw( camXmsg );
    RT.draw( camYmsg );
    RT.draw( camZmsg );
    RT.draw( angleMsg );
    RT.draw( angleNumMsg );
    RT.draw( speedMsg );
    RT.draw( speedNumMsg );
    RT.draw( pitchMsg );
    RT.draw( pitchNumMsg );
    RT.draw( rollMsg );
    RT.draw( rollNumMsg );
    RT.draw( numShotsMsg );
    RT.draw( numShotsNumMsg );
    if( showHelp ) RT.draw( helpMsg );
    if( freeView ) RT.draw( lookAroundMsg );
}

// utility
bool lvl_perpective::loadSpriteSheets()
{
    std::ifstream finList("include/levels/lvl_perspective/SSfileList.txt");
    if( !finList ) { std::cout << "\nno ssFileList"; return false; }

    int numSS = 0; finList >> numSS;
    if( numSS < 1 ) return false;
    SSvec.reserve( numSS );
    std::string fileName;

    while( numSS-- > 0 && finList >> fileName )
    {
        fileName = "include/levels/lvl_perspective/images/" + fileName;
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

void lvl_perpective::cleanup()
{
    size_t numDel = 0;
    for( Leg*& pPath : pPathVec ) destroyPath( pPath );
    for( persPt* pPt : pCarVec ) delete pPt;
    for( persIsland* pPI : pIslandVec ){ delete pPI; ++numDel; }
    Leg3f::destroy3fPath( pCoasterPath );
    std::cout << '\n' << numDel << " islands deleted";
 //   Leg3f::destroy3fPath( pCoasterPath );
    if( !shotSoundVec.empty() )
    {
        for( sf::Sound& rSnd : shotSoundVec )
            if( rSnd.getStatus() == sf::Sound::Playing ) rSnd.stop();
    }
}

void lvl_perpective::makeRectSolid( vec3f pos, vec3f sz, sf::Color color, std::vector<vec3f>& r_posVec, std::vector<sf::Vertex>& r_vtxVec )
{
    std::vector<vec3f> ptVec;
    // make 12 sf::Lines
    // front
    ptVec.push_back( vec3f( 0.0f, 0.0f, 0.0f ) );
    ptVec.push_back( vec3f( 0.0f, sz.y, 0.0f ) );// front left
    ptVec.push_back( vec3f( 0.0f, sz.y, 0.0f ) );
    ptVec.push_back( vec3f( sz.x, sz.y, 0.0f ) );// front top
    ptVec.push_back( vec3f( sz.x, sz.y, 0.0f ) );
    ptVec.push_back( vec3f( sz.x, 0.0f, 0.0f ) );// front right
    ptVec.push_back( vec3f( sz.x, 0.0f, 0.0f ) );
    ptVec.push_back( vec3f( 0.0f, 0.0f, 0.0f ) );// front bottom// front
    // back
    ptVec.push_back( vec3f( 0.0f, 0.0f, sz.z ) );
    ptVec.push_back( vec3f( 0.0f, sz.y, sz.z ) );// back left
    ptVec.push_back( vec3f( 0.0f, sz.y, sz.z ) );
    ptVec.push_back( vec3f( sz.x, sz.y, sz.z ) );// back top
    ptVec.push_back( vec3f( sz.x, sz.y, sz.z ) );
    ptVec.push_back( vec3f( sz.x, 0.0f, sz.z ) );// back right
    ptVec.push_back( vec3f( sz.x, 0.0f, sz.z ) );
    ptVec.push_back( vec3f( 0.0f, 0.0f, sz.z ) );// back bottom// front
    // front to back
    ptVec.push_back( vec3f( 0.0f, 0.0f, 0.0f ) );
    ptVec.push_back( vec3f( 0.0f, 0.0f, sz.z ) );// bottom left
    ptVec.push_back( vec3f( sz.x, 0.0f, 0.0f ) );
    ptVec.push_back( vec3f( sz.x, 0.0f, sz.z ) );// bottom right
    ptVec.push_back( vec3f( 0.0f, sz.y, 0.0f ) );
    ptVec.push_back( vec3f( 0.0f, sz.y, sz.z ) );// top left
    ptVec.push_back( vec3f( sz.x, sz.y, 0.0f ) );
    ptVec.push_back( vec3f( sz.x, sz.y, sz.z ) );// top right

    for( vec3f ptPos : ptVec )
    {
        sf::Color clr = ptPos.z > sz.z/2.0f ? sf::Color::Green : sf::Color::Red;
        ptPos += pos;
        r_posVec.push_back( ptPos );
        sf::Vertex vtx; vtx.color = clr;
        vtx.position = persPt::get_xyw( ptPos );
        r_vtxVec.push_back( vtx );
    }

}

bool lvl_perpective::init_shadows()
{
    std::ifstream fin("include/levels/lvl_perspective/initShadow_data.txt");
    if( !fin ) { std::cout << "\nNo initShadow_data.txt"; return false; }

    size_t numShadows; fin >> numShadows;
    PqVec_4shadow.reserve( numShadows );
    shadowVec.reserve( numShadows );
    vec3f Pos, Nu;
    float W, H;
    size_t SSnum, SetNum, FrIdx;
    char chA, chB;
    unsigned int rd, gn, bu, alpha;

    fin >> SSnum >> SetNum >> FrIdx >> chA >> chB >> sHat.x >> sHat.y >> sHat.z;
    fin >> rd >> gn >> bu >> alpha;
    shadowColor = sf::Color(rd,gn,bu,alpha);
    sHat /= sHat.mag();
 //   std::cout << "\n\n sHat: mag = " << sHat.mag() << " pos: " << sHat.x << ' ' << sHat.y << ' ' << sHat.z;
 //   std::cout << "\n\nCOLOR = " << rd << " " << gn << ' ' << bu << ' ' << alpha;

    for( size_t j = 0; j < numShadows; ++j )
    {
        fin >> Pos.x >> Pos.y >> Pos.z >> W >> H >> Nu.x >> Nu.y >> Nu.z;
        PqVec_4shadow.push_back( persQuad( Pos, W, H, Nu, sf::Color::White, &( SSvec[SSnum].txt ) )  );
        persQuad& rPQ = PqVec_4shadow.back();
        rPQ.setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), chA, chB );

        shadowVec.push_back( persShadow( rPQ, sHat, shadowColor ) );
    }

    for( persShadow& rPS : shadowVec ) rPS.update(0.0f);

    // A shadowAdapter for each PBQ
    if( !quadBoxVec.empty() )
    {
        shAdaptVec.reserve( quadBoxVec.size() );
        for( persBox_quad& rPBQ : quadBoxVec )
        {
            shAdaptVec.push_back( shadowAdapter( rPBQ, sHat, shadowColor ) );
            shAdaptVec.back().update(0.0f);
        }
    }



    return true;
}
