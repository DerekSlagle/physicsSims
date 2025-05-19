#include "lvl_terrainPerspective.h"

lvl_terrainPerspective::lvl_terrainPerspective()
{
    //ctor
}

lvl_terrainPerspective::~lvl_terrainPerspective()
{
    animatedTransition<float>::numAnimating = 0;
    persFan::shadowVtxArray.clear();// static array must be cleared when level is destroyed
}

bool lvl_terrainPerspective::init()
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
    // reset from last time level played
    animatedTransition<float>::numAnimating = 0;

    std::ifstream fin("include/levels/lvl_terrainPerspective/init_data.txt");
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
    fin >> camAccel >> gravity.y;
    gravity.x = gravity.z = 0.0f;

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

    persPt::updateBackground( trueHorizon, upperPane, nullptr );
 //   updatePane();// assigns remaining positions

    // messages
    msgVec.reserve(6);
    msgNumVec.reserve(6);

    unsigned int fontSz; fin >> fontSz;
    sf::Text msg( "posX", *button::pFont, 14 );
    fin >> rd >> gn >> bu;
    msg.setFillColor( sf::Color(rd,gn,bu) );
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

    msg.setString( "speed" );
    msg.setPosition( posX, posY + 4.0f*offY );
    msgVec.push_back( msg );
    msg.setString( "0.0" );
    msg.setPosition( posX + offX, posY + 4.0f*offY );
    msgNumVec.push_back( msg );
    pSpeedNumMsg = &( msgNumVec.back() );

    msg.setString( "pitch" );
    msg.setPosition( posX, posY + 5.0f*offY );
    msgVec.push_back( msg );
    msg.setString( "0.0" );
    msg.setPosition( posX + offX, posY + 5.0f*offY );
    msgNumVec.push_back( msg );
    pPitchNumMsg = &( msgNumVec.back() );

    std::string fileName;
  //  fin >> fileName;
 //   if( !init_controls( fileName ) ) return false;
    fin >> fileName;
    if( !loadSpriteSheets( fileName ) ) return false;
    fin >> fileName;
    if( !init_terrain( fileName ) ) return false;
    fin >> fileName;
    if( !init_mats( fileName.c_str() ) ) return false;
    fin >> fileName;
    if( !init_boxes( fileName.c_str() ) ) return false;

    rotSpeedVec.resize( 4, 0.0 );// assigned in init multi control
    fin >> fileName;
    if( !init_controls( fileName ) ) return false;

    fin >> fileName;
    persPtRecorder::pFiring = &firing;// static
    persPtRecorder::pRunLevel = &Level::run_update;// static
    if( !recorder.init( fileName, *button::pFont ) ) return false;
 //   fin >> fileName;
 //   if( !init_curves( fileName.c_str() ) ) return false;
 //   if( !init_curveChain( fileName.c_str() ) ) return false;

    fin >> fileName;
    if( !init_fans( fileName.c_str() ) ) return false;

    std::cerr << "\n init() no crash yet";

    // reset all motion adapters
    ma_cone.reset();
    ma_parabola.reset();
    ma_paraChute.reset();
    ma_paraChute2.reset();
//    maxz_sine.reset();

    // the persQuadAni pool
    size_t  SSnum, numQuads; fin >> numQuads >> SSnum;
    quadAniVec.reserve( numQuads );
    if( numQuads > 0 && SSnum < SSvec.size() )
    {
        char rotAmt; fin >> rotAmt;
        quadAniVec.push_back( persQuadAni( fin, SSvec[SSnum] ) );
        persQuadAni& rPQA = quadAniVec.back();
        rPQA.setTxtRect( SSvec[SSnum].getFrRect(0,rPQA.setNum), 'R', rotAmt );
        rPQA.inUse = false;
        rPQA.facingCamera = true;
        for( size_t i = 1; i < numQuads; ++i )
            quadAniVec.push_back( rPQA );
    }

    // the ball and motion adapter pools
    size_t numBalls; fin >> numBalls;
    float Rball; fin >> Rball;
    std::cout << "\nRball = " << Rball;
    fin >> rd >> gn >> bu;
    fin >> dtLaunch;
    persBall aBall;
    aBall.B.setRadius( Rball );
    aBall.B.setOrigin( Rball, Rball );
    aBall.B.setFillColor( sf::Color(rd,gn,bu) );
    aBall.Rbound = Rball;
    aBall.inUse = false;
    ballPool.reserve( numBalls );
 //   ma_xzPool.reserve( numBalls );// motion adapters in same number
//    matAdapter2ndOrder_xz ma_xz = maxz_sine;
//    ma_xz.pRider = nullptr;
    for( size_t j = 0; j < numBalls; ++j )
    {
        ballPool.push_back( aBall );
   //     ma_xzPool.push_back( ma_xz );
    }
    ballPool.back().setPosition( 0.0f, 0.0f, 1000.0f );
    ballPool.back().inUse = true;

    // the shot pool
    fin >> numBalls >> Rball;
    std::cout << "\nshot Rball = " << Rball;
    fin >> rd >> gn >> bu;
    persBallTimed tBall;
    tBall.B.setRadius( Rball );
    tBall.B.setOrigin( Rball, Rball );
    tBall.B.setFillColor( sf::Color(rd,gn,bu) );
    tBall.Rbound = Rball;
    tBall.inUse = false;
    tBall.isMoving = true;
    tBall.tElap = 0.0f;
    shotPool.reserve( numBalls );
    for( size_t j = 0; j < numBalls; ++j )
    {
        shotPool.push_back( tBall );
    }
    // shot related qtys
    firing = false;
    fin >> fireOfst;
    fin >> persBallTimed::tLife;
    persBallTimed::pFdone = [this]( vec3f Pos ){ return getAniQuad( Pos ); };

    // mat surfer
    size_t matIdx; fin >> matIdx >> surfMatPos0.x >> surfMatPos0.z >> surfVX0 >> surfVZ0;
    if( matIdx < persMat_vec.size() )
    {
        pSurfMat = &persMat_vec[ matIdx ];
        surfNu = pSurfMat->getGradient( surfMatPos0.x, surfMatPos0.z );
        surfNu /= surfNu.mag();
        surfMatPos0.y = pSurfMat->pFxz( surfMatPos0.x, surfMatPos0.z );
    }

    // Yu_queue, Zu_queue
    unsigned int qSz; fin >> qSz;
    if( qSz > 2 )
    {
        Yu_total.x = Yu_total.y = Yu_total.z = 0.0f;
        Zu_total.x = Zu_total.y = Zu_total.z = 0.0f;

        // initial fill
        for( unsigned int j = 0; j < qSz; ++j )
        {
            Yu_total += persPt::yu;
            Yu_queue.push( persPt::yu );
            Zu_total += persPt::camDir;
            Zu_queue.push( persPt::camDir );
        }
    }

    // theSun
    float Rsun; fin >> Rsun;
    fin >> rd >> gn >> bu;
    theSun.setRadius( Rsun );
    theSun.setOrigin( Rsun, Rsun );
    theSun.setFillColor( sf::Color(rd,gn,bu) );
    theSun.setOutlineColor( sf::Color::Yellow );
    theSun.setOutlineThickness( 0.2f*Rsun );
    // theMoon
    fin >> Rsun;
    unsigned int SSnumMoon; fin >> SSnumMoon;
    fin >> rd >> gn >> bu;
    fin >> moonDir.x >> moonDir.y >> moonDir.z;
    moonDir /= moonDir.mag();
    theMoon.setRadius( Rsun );
    theMoon.setOrigin( Rsun, Rsun );
    theMoon.setFillColor( sf::Color(rd,gn,bu) );
    theMoon.setOutlineColor( sf::Color::Green );
    theMoon.setOutlineThickness( 0.05f*Rsun );

    update_sun();
    // moon texture
    if( SSnumMoon < SSvec.size() )
    {
        std::cout << "\n init(): moon texture assigned";
        theMoon.setTexture( &( SSvec[ SSnumMoon ].txt ) );
        theMoon.setTextureRect( SSvec[ SSnumMoon ].getFrRect(0,0) );
    }
    else
        std::cout << "\n init(): no moon texture";


    float Amp = 200.0f, kX = 0.001f, kZ = 0.001f;
    fin >> Amp >> kX >> kZ >> groundGrid.inUse;
    groundGrid.init( fin, [Amp,kX,kZ](float x, float z){ return Amp*sinf(kX*x)*sinf(kZ*z); } );

    // help message key H
    Level::init_helpMsg( fin, helpMsg );

    fin.close();

    // TEMP double click on dime
    button::pOnDblClickLeft = [this]()
    {
        if( fanVec[0].hit_image( button::mseX, button::mseY ) )
        {
            if( operCodeVec[0] == 'Y' ) operCodeVec[0] = 'P';
            else if( operCodeVec[0] == 'P' ) operCodeVec[0] = 'R';
            else if( operCodeVec[0] == 'R' ) operCodeVec[0] = 'A';
            else operCodeVec[0] = 'Y';
        }
    };

    button::pOnDblClickRight = [this]()
    {
        if( fanVec[0].hit_image( button::mseX, button::mseY ) )
        {
            if( rotSpeedVec[0] < 3.0f ) rotSpeedVec[0] = 4.0f;
            else rotSpeedVec[0] = 2.0f;
        }
    };

    // single animation
    ani_roll.init( tFinalRoll, 0.0f, 2.0f*vec2f::PI );
    ani_roll.fy = [this]( float u ){ return get_BezSlope( u, roll_u1, roll_u2 ); };
    ani_roll.fUpdateX = [this]( float dX ){ persPt::roll( dX ); return; };

    ani_pitch.init( tFinalPitch, 0.0f, 2.0f*vec2f::PI );
    ani_pitch.fy = [this]( float u ){ return get_BezSlope( u, pitch_u1, pitch_u2 ); };
    ani_pitch.fUpdateX = [this]( float dX ){ persPt::pitch( -dX ); };

    // chain animation
    ani_halfPitch.init( 0.5f*tFinalPitch, 0.0f, vec2f::PI );
    ani_halfPitch.fUpdateX = [this]( float dX ){ persPt::pitch( -dX ); };
    float C = -5.0f;// turn at start
    ani_halfPitch.fy = [this,C]( float u )
    {
        float uSq = u*u;
        float A = 3.0f + C, B = -2.0f*( 1.0f + C );
        return 2.0f*A*u + 3.0f*B*uSq + 4.0f*C*uSq*u;
    };
    ani_halfPitch.pNextAT = &ani_halfRoll;// next animation

    ani_halfRoll.init( 0.5f*tFinalRoll, 0.0f, vec2f::PI );
    ani_halfRoll.fUpdateX = [this]( float dX ){ persPt::roll( dX ); };
    C = 6.0f;// turn at end
    ani_halfRoll.fy = [this,C]( float u )
    {
        float uSq = u*u;
        float A = 3.0f + C, B = -2.0f*( 1.0f + C );
        return 2.0f*A*u + 3.0f*B*uSq + 4.0f*C*uSq*u;
    };
    // vector of pointers for use in update_global()
    pAniTransVec.reserve(4);
    pAniTransVec.push_back( &ani_roll );
    pAniTransVec.push_back( &ani_pitch );
    pAniTransVec.push_back( &ani_halfRoll );
    pAniTransVec.push_back( &ani_halfPitch );


    // clusters
 //   std::vector<persPt*> tempVec;
 //   tempVec.reserve( fanVec.size() );
 //   for( persFan& PF : fanVec ) tempVec.push_back( &PF );
 //   cluster_1.init( tempVec );
    cluster_1.pPersVec.reserve( fanVec.size() );
    cluster_1.pTargetVec.reserve( fanVec.size() );
    for( persFan& PF : fanVec )
    {
        cluster_1.pPersVec.push_back( &PF );
        cluster_1.pTargetVec.push_back( &PF );
    }
    cluster_1.init();
 //   cluster_1.doZ_order = false;

 /*   if( persBox_vec.size() > 2 )
    {
        pCar = &( persBox_vec[2] );
        init_carAnis();
        if( !carAniVec.empty() )
            carAniVec.front().start();
    }   */

    // z ordering
    init_Zorder();

    return true;
}

void lvl_terrainPerspective::init_Zorder()
{
    size_t fullSize = persMat_vec.size() + PBwf_vec.size();// + curveVec.size();
 //   fullSize += persBox_vec.size();

    pPtStillVec.reserve( fullSize );
    for( persBox_wf& rPBwf : PBwf_vec ) pPtStillVec.push_back( &rPBwf );
    for( persMat& rPM : persMat_vec ) pPtStillVec.push_back( &rPM );
 //   for( persBox& rPB : persBox_vec ) pPtStillVec.push_back( &rPB );


    size_t movingSize = persBox_vec.size() + 6;
    pPtMovingVec.reserve( movingSize );
    for( persBox& rPB : persBox_vec ) pPtMovingVec.push_back( &rPB );
    pPtMovingVec.push_back( &coneBall );
    pPtMovingVec.push_back( &parBall );
    pPtMovingVec.push_back( &paraChuteBall );
    pPtMovingVec.push_back( &paraChuteBall2 );
    pPtMovingVec.push_back( &cluster_1 );
    pPtMovingVec.push_back( &pf_wideA );

 //   for( persCurve& rPCv : curveVec ) pPtStillVec.push_back( &rPCv );

 //   fullSize += quadAniVec.size() + ballPool.size() + shotPool.size();
    fullSize += quadAniVec.size() + ballPool.size() + shotPool.size();// + fanVec.size();
 //   fullSize += 5;// for coneBall, parBall, paraChuteBall, paraChuteBall2, cluster_1
    fullSize += movingSize;
    pPtSortVec.reserve( fullSize );
    // just filling with valid pointers. These will be overwritten each frame
    for( persPt* pPt : pPtStillVec ) pPtSortVec.push_back( pPt );
    for( persPt* pPt : pPtMovingVec ) pPtSortVec.push_back( pPt );
    for( persQuadAni& rPQA : quadAniVec ) pPtSortVec.push_back( &rPQA );
    for( persBall& rPB : ballPool ) pPtSortVec.push_back( &rPB );
    for( persBall& rPB : shotPool ) pPtSortVec.push_back( &rPB );

    // temp to tshoot a crash. nullptr in vector?
    std::cout << "\nPBwf_vec.size() = " << PBwf_vec.size();
    std::cout << "\nquadAniVec.size() = " << quadAniVec.size();
    std::cout << "\npPtAllVec = " << pPtStillVec.size();
    for( persPt* pPt : pPtStillVec ) if( !pPt ) std::cout << " null";
    std::cout << "\npPtSortVec = " << pPtSortVec.size();
    for( persPt* pPt : pPtSortVec ) if( !pPt ) std::cout << " null";

 //   std::sort( pPtStillVec.begin(), pPtStillVec.end(), persPt::compare );// crash. ok without persBox_wf
    std::sort( pPtSortVec.begin(), pPtSortVec.end(), persPt::compare );// crash. ok without persBox_wf
    // try hand roll
//    persPt::sortByDistance( pPtSortVec );// OK. works and doesn't crash
}

void lvl_terrainPerspective::init_carAnis()
{
    if( !pCar ) return;
    pCar->isMoving = true;

    carAniVec.reserve(4);
    animatedTransition<float> temp;

    temp.init( carTimeA, 0.0f, 1.0f );
    temp.fy = []( float u ){ return u; };
    temp.fUpdateX = [this]( float dX )
    {
        makeXuLevelLH( pCar->Xu, pCar->Yu, pCar->Zu );
        pCar->vel = carSpeed*pCar->Zu;
    };
    carAniVec.push_back( temp );

    float bankTime = vec2f::PI*carTurnR/carSpeed;
    temp.init( bankTime, 0.0f, vec2f::PI );
    temp.fy = []( float u ){ return u; };
    temp.fUpdateX = [this]( float dt )
    {
        vec3f::bank( -gravity.y, carSpeed, -carSpeed/carTurnR, dt, pCar->Xu, pCar->Yu, pCar->Zu );
        rotateBasisLH( persPt::yHat, carSpeed*dt/carTurnR, pCar->Xu, pCar->Yu, pCar->Zu );
    //    rotateBasisLH( persPt::yHat, dt, pCar->Xu, pCar->Yu, pCar->Zu );
        pCar->vel = carSpeed*pCar->Zu;
    };
    carAniVec.push_back( temp );

    temp.init( carTimeA, 0.0f, 1.0f );
    temp.fy = []( float u ){ return u; };
    temp.fUpdateX = [this]( float dX )
    {
        makeXuLevelLH( pCar->Xu, pCar->Yu, pCar->Zu );
        pCar->vel = carSpeed*pCar->Zu;
    };
    carAniVec.push_back( temp );

    temp.init( bankTime, 0.0f, vec2f::PI );
    temp.fy = []( float u ){ return u; };
    temp.fUpdateX = [this]( float dt )
    {
        vec3f::bank( -gravity.y, carSpeed, -carSpeed/carTurnR, dt, pCar->Xu, pCar->Yu, pCar->Zu );
        rotateBasisLH( persPt::yHat, carSpeed*dt/carTurnR, pCar->Xu, pCar->Yu, pCar->Zu );
    //    rotateBasisLH( persPt::yHat, dt, pCar->Xu, pCar->Yu, pCar->Zu );
        pCar->vel = carSpeed*pCar->Zu;
    };
    carAniVec.push_back( temp );

    for( unsigned int j = 0; j < carAniVec.size(); ++j )
        carAniVec[j].pNextAT = &( carAniVec[ (j+1)%carAniVec.size() ] );
}

bool lvl_terrainPerspective::init_terrain( const std::string& inFileName )
{
    std::ifstream fin( inFileName.c_str() );
    if( !fin ) { std::cout << "\nNo terrain data"; return false; }

    // persBox_wf templates
    size_t num; fin >> num;
    PBwf_toCopy.reserve( num );

    for( size_t j = 0; j < num; ++j )
    {
        PBwf_toCopy.push_back( persBox_wf( fin ) );
    }

    // 2 rings
    size_t idxRing1, numRing1, idxRing2, numRing2;
    float r1, r2;
    fin >> idxRing1 >> r1 >> numRing1 >> idxRing2 >> r2 >> numRing2;
    PBwf_vec.reserve( numRing1 + numRing2 );

    // 1st ring
    vec3f Pos;
    Pos.y = 0.0f;
    float ang = 0.0f;
    float dAng = 2.0f*vec2f::PI/numRing1;
    if( idxRing1 >= PBwf_toCopy.size() ) idxRing1 = 0;
    for( size_t j = 0; j < numRing1; ++j )
    {
        Pos.x = r1*sinf(ang);
        Pos.z = r1*cosf(ang);
        PBwf_vec.push_back( PBwf_toCopy[idxRing1] );
        PBwf_vec.back().setPosition( Pos );
        ang += dAng;
    }
    // 2nd ring
    ang = 0.0f;
    dAng = 2.0f*vec2f::PI/numRing2;
    if( idxRing2 >= PBwf_toCopy.size() ) idxRing2 = 0;
    for( size_t j = 0; j < numRing2; ++j )
    {
        Pos.x = r2*sinf(ang);
        Pos.z = r2*cosf(ang);
        PBwf_vec.push_back( PBwf_toCopy[idxRing2] );
        PBwf_vec.back().setPosition( Pos );
        ang += dAng;
    }

    return true;
}

bool lvl_terrainPerspective::init_boxes( const char* fileName )
{
    std::ifstream fin( fileName );
    if( !fin ) { std::cout << "\nNo box data"; return false; }

    size_t numBoxes; fin >> numBoxes;
    std::cout << "\n***************numBoxes: " << numBoxes;
    persBox_vec.reserve( numBoxes );// temp1, temp2, etc

    size_t SSnum;
    for( size_t i = 0; i < numBoxes; ++i )
    {
        fin >> SSnum;
        persBox_vec.push_back( persBox( fin, &(SSvec[SSnum]) ) );
    }

    return true;
}

bool lvl_terrainPerspective::init_fans( const char* fileName )
{
    std::ifstream fin( fileName );
    if( !fin ) return false;

    unsigned int numFans; fin >> numFans;
    fanVec.reserve( numFans );
 //   rotSpeedVec.resize( numFans, 0.0 );// assigned in init multi control
    operCodeVec.reserve( numFans );
//    persFan::shadowVtxArray.reserve( numFans );
    persFan::shadowVtxArray.reserve( numFans + 1 );// + 1 for pf_wideA

 //   if( SSvec.size() < 4 ) return false;
    char operCode = 'N';
    unsigned int SSnum = 0;

    // dime
    fin >> SSnum >> operCode;
    if( SSnum >= SSvec.size() ) SSnum = 0;
    fanVec.push_back( persFan( fin, &( SSvec[SSnum] ) ) );
    operCodeVec.push_back( operCode );
    // flower
    fin >> SSnum >> operCode;
    float Rmin, Rmax, Nrot; fin >> Rmin >> Rmax >> Nrot;
    float Af = 0.5f*( Rmax + Rmin ), Bf = 0.5f*( Rmax - Rmin );
    std::function<float(float)> pfR = [Af,Bf,Nrot]( float angle ){ return Af + Bf*sinf( Nrot*angle ); };
    if( SSnum >= SSvec.size() ) SSnum = 0;
    fanVec.push_back( persFan( fin, pfR, &( SSvec[SSnum] ) ) );
    operCodeVec.push_back( operCode );
    // ellipse
    fin >> operCode;
    fin >> Af >> Bf;
    pfR = [Af,Bf]( float a ){ return Af*Bf/sqrtf( Af*Af*sinf(a)*sinf(a) + Bf*Bf*cosf(a)*cosf(a) ); };
    fanVec.push_back( persFan( fin, pfR, nullptr ) );
    operCodeVec.push_back( operCode );
    // 5 point star
    fin >> SSnum >> operCode >> rotSpeedVec[3];
    fin >> Bf >> Af;// center to tip
//    Af *= Bf;
//    Af = 0.5528f*Bf;
    pfR = [Af,Bf]( float a )
    {
        int N = static_cast<int>( 5.0f*a/vec2f::PI + 0.1f );
        return ( N%2 == 0 ) ? Bf : Af;
    };
    if( SSnum >= SSvec.size() ) SSnum = 0;
    fanVec.push_back( persFan( fin, pfR, &( SSvec[SSnum] ) ) );
    operCodeVec.push_back( operCode );

    // light direction given yaw and pitch
    // moved to control

    // new type persFanWide
    fin >> SSnum;
    if( SSnum >= SSvec.size() ) SSnum = 0;

    int SSnumSide = -1;
    fin >> SSnumSide;
    if( SSnumSide > 0 && SSnumSide < static_cast<int>( SSvec.size() ) )
        pf_wideA.init( fin, &( SSvec[SSnum] ), &( SSvec[SSnumSide] ) );
    else
        pf_wideA.init( fin, &( SSvec[SSnum] ), nullptr );

    fin.close();

    // add a shadow for each
    for( persFan& PF : fanVec ) PF.addShadow();
    pf_wideA.addShadow();
    // add an outline for these 2
    fanVec[1].initPerim( sf::Color::Magenta );
    fanVec[3].initPerim( sf::Color::Black );
    // adding coordinateAxes to the ellipse
    const unsigned int caIdx = 2;
    fanVec[caIdx].initPerim( sf::Color::Red );// and a perimeter
    // bind coordinates to fan
    persFan* pPF = &( fanVec[caIdx] );
    coordAxesA.pOrigin = &( pPF->pos );
    coordAxesA.pDoDraw = &( pPF->doDraw );
    coordAxesA.pXu = &( pPF->Xup );
    coordAxesA.pYu = &( pPF->Yup );
    coordAxesA.pZu = &( pPF->Zup );

    coordAxesA.init( sf::Color::Yellow, fanVec[caIdx].Rbound*1.5f, 20.0f, 6.0f, 12, [pPF](vec3f P){ return pPF->get_xyw(P); }, button::pFont );
    coordAxesA.setLabelColor( sf::Color::Black );

    return true;
}

bool lvl_terrainPerspective::init_mats( const char* fileName )
{
    std::ifstream fin( fileName );
    if( !fin ) return false;

    // Etot messages
    unsigned int fontSz, rd, gn, bu;
    fin >> fontSz >> rd >> gn >> bu;
    coneEtotMsg.setFont( *button::pFont );
    coneEtotMsg.setCharacterSize( fontSz );
    coneEtotMsg.setFillColor( sf::Color(rd,gn,bu) );
    // equate the others
    parEtotMsg = parEtotNumMsg = coneEtotNumMsg = coneEtotMsg;
    parChtEtotMsg = parChtEtotNumMsg = coneEtotMsg;
    parCht2EtotMsg = parCht2EtotNumMsg = coneEtotMsg;

    // init the balls
    float Rmb; fin >> Rmb;
    fin >> rd >> gn >> bu;
    coneBall.init( vec3f(0.0f,0.0f,1000.0f), Rmb, sf::Color(rd,gn,bu) );
    paraChuteBall2 = paraChuteBall = parBall = coneBall;
    paraChuteBall2.B.setFillColor( sf::Color::Blue );
    paraChuteBall2.pos.x += 100.0f;
 //   sineBall = paraChuteBall2;
 //   sineBall.pos.x += 100.0f;

    persMat_vec.reserve(5);// hill, cone, parab, hyper, paraChute
 //   persMat tempMat;

    // #1 a hill
    float slope, K; fin >> slope >> K;
    std::function<float(float,float)> p_Fxz = [slope,K](float x,float z){ return slope*cosf(K*x)*cosf(K*z); };
    persMat_vec.push_back( persMat( fin, p_Fxz ) );
    persMat* p_PM = &persMat_vec.back();
    pSinePM = p_PM;
    p_PM->pFx = [slope,K]( double x, double z ){ return -slope*K*sinf(K*x)*cosf(K*z); };
    p_PM->pFz = [slope,K]( double x, double z ){ return -slope*K*cosf(K*x)*sinf(K*z); };
    // new 2d motion adapter
    double X0, X10, Z0, Z10; fin >>  X0 >> X10 >> Z0 >> Z10;
    sineLaunchPos = p_PM->toWorldPos( X0, Z0 );
 //   maxz_sine.init( X0, X10, Z0, Z10, *p_PM, sineBall );
 //   maxz_sine.setPosition();// of sineBall
 //   std::cout << "\nsineBall.pos = " << sineBall.pos.x << ", " << sineBall.pos.y << ", " << sineBall.pos.z;
 //   sineLaunchPos = sineBall.pos;
 //   maxz_sine.Grav = static_cast<double>( gravity.y );// up since hill is concave down
 //   maxz_sine.pFx = [slope,K]( double x, double z ){ return -slope*K*sinf(K*x)*cosf(K*z); };
 //   maxz_sine.pFxx = [slope,K]( double x, double z ){ return -slope*K*K*cosf(K*x)*cosf(K*z); };
 //   maxz_sine.pFz = [slope,K]( double x, double z ){ return -slope*K*cosf(K*x)*sinf(K*z); };
 //   maxz_sine.pFzz = [slope,K]( double x, double z ){ return -slope*K*K*cosf(K*x)*cosf(K*z); };
    // reset with velocity components given on surface
 //   grad_u = persPt::yHat - maxz_sine.pPM->pFx( X0, Z0 )*persPt::xHat - maxz_sine.pPM->pFz( X0, Z0 )*persPt::zHat;
    grad_u = persPt::yHat - pSinePM->pFx( X0, Z0 )*persPt::xHat - pSinePM->pFz( X0, Z0 )*persPt::zHat;
    grad_u /= grad_u.mag();
    if( grad_u.dot( persPt::yHat ) < 0.9f )// not parallel
    {
        sineVs1 = grad_u.cross( persPt::yHat );// horizontal component in tangent plane
        sineVs1 /= sineVs1.mag();
        sineVs2 = sineVs1.cross( grad_u );// straight uphill
    }
    else
    {
        grad_u = persPt::yHat;
        sineVs1 = persPt::xHat;
        sineVs2 = persPt::zHat;
    }
    // assign initial velocity
 //   float v1, v2;
    fin >> sineV0 >> angV0 >> dAngV0;
 //   if( v1*v1 > 1.0f ) v2 = 0.0f;
 //   else v2 = sqrtf( 1.0f - v1*v1 );
 //   vec3f V = sineV0*( cosf( angV0 )*sineVs1 + sinf( angV0 )*sineVs2 );
 //   maxz_sine.x10 = V.dot( persPt::xHat );
 //   maxz_sine.z10 = V.dot( persPt::zHat );

    // #2 *** a cone ***
    float Rmax = 300.0f;
    fin >> Acone >> Rmax;
    slope = Acone;
    p_Fxz = [slope,Rmax](float x,float z)
    {
        float R = sqrtf( x*x + z*z );
        return R > Rmax ? slope*Rmax : slope*R;// flat
    };
    persMat_vec.push_back( persMat( fin, p_Fxz ) );
    p_PM = &persMat_vec.back();
    p_PM->pFx = [slope,Rmax]( double x, double z ){ double R = sqrtf( x*x + z*z ); return R > Rmax ? 0.0 : slope*x/sqrt( x*x + z*z ); };
    p_PM->pFz = [slope,Rmax]( double x, double z ){ double R = sqrtf( x*x + z*z ); return R > Rmax ? 0.0 : slope*z/sqrt( x*x + z*z ); };
 //   persMat_vec.back().isMoving = true;// animate!
 //   pConeMat = &persMat_vec.back();//
    // animate!
    std::function<double(double,double,double)> F_r2 = [this]( double R, double R1, double Ang1 )
    { return ( R*Ang1*Ang1 + gravity.y*Acone )/( 1.0f + Acone*Acone );  };
    ma_cone.init( fin, persMat_vec.back(), F_r2, coneBall );
    ma_cone.setPosition();
    // Etot message
    float posX, posY, dX; fin >> posX >> posY >> dX;
    coneEtotMsg.setPosition( posX, posY );
    coneEtotMsg.setString( "coneEtot = " );
    coneEtotNumMsg.setPosition( posX + dX, posY );
    coneEtotNumMsg.setString( "0.0" );

    // #3 a parabola
    fin >> Apar >> Rmax;
    slope = Apar;
    p_Fxz = [slope,Rmax](float x,float z)
    {
        float R = sqrtf( x*x + z*z );// slope*( x*x + z*z );
        return R > Rmax ? slope*Rmax*Rmax : slope*( x*x + z*z );// return y
    };
    persMat_vec.push_back( persMat( fin, p_Fxz ) );
    p_PM = &persMat_vec.back();
 //   p_PM->pFx = [slope,Rmax]( double x, double z ){ double R = sqrtf( x*x + z*z ); return R > Rmax ? 0.0 : 2.0*slope*x; };
 //   p_PM->pFz = [slope,Rmax]( double x, double z ){ double R = sqrtf( x*x + z*z ); return R > Rmax ? 0.0 : 2.0*slope*z; };
    p_PM->pFx = [slope,Rmax]( double x, double z ){ return 2.0*slope*x; };
    p_PM->pFz = [slope,Rmax]( double x, double z ){ return 2.0*slope*z; };
 //   persMat_vec.back().isMoving = true;// animate!
 //   pParMat = &persMat_vec.back();
    // for the adapter
    F_r2 = [this]( double R, double R1, double Ang1 )
    {
        float G = gravity.y/( persMat_vec.back().scaleX*persMat_vec.back().scaleZ );
        double r2 = R*Ang1*Ang1 + 2.0f*G*Apar*R - 4.0f*Apar*Apar*R1*R1;
        r2 /= ( 1.0f + 4.0f*Apar*Apar*R*R );
        return r2;
    };
    ma_parabola.init( fin, persMat_vec.back(), F_r2, parBall );
    ma_parabola.setPosition();
    // Etot message
    fin >> posX >> posY >> dX;
    parEtotMsg.setPosition( posX, posY );
    parEtotMsg.setString( " parEtot = " );
    parEtotNumMsg.setPosition( posX + dX, posY );
    parEtotNumMsg.setString( "0.0" );

    // #4 a hyperbola
    double A, B; fin >> A >> B >> Rmax;

    p_Fxz = [A,B,Rmax](float x,float z)
    {
        double Rsq = x*x + z*z, R = sqrt(Rsq);// this is scaled down
     //   return R > Rmax ? A*sqrtf( Rmax*Rmax + B ) : A*sqrtf( Rsq + B );// small Rmax
        return R > Rmax ? sqrtf( A*Rmax*Rmax + B*B ) : sqrt( A*Rsq + B*B );
    };
    persMat_vec.push_back( persMat( fin, p_Fxz ) );
    p_PM = &persMat_vec.back();
 /*   p_PM->pFx = [A,B,Rmax]( double x, double z )
    {
        double Rsq = x*x + z*z, R = sqrt(Rsq);
        return R > Rmax ? 0.0 : A*x/sqrt( A*Rsq + B*B );
    };
    p_PM->pFz = [A,B,Rmax]( double x, double z )
    {
        double Rsq = x*x + z*z, R = sqrt(Rsq);
        return R > Rmax ? 0.0 : A*z/sqrt( A*Rsq + B*B );
    };  */
    p_PM->pFx = p_PM->pFz = nullptr;

    // #5 parabolic chute: y = A*x*x
    fin >> AparaChute >> pcZ0  >> Vz;
    pcZ = pcZ0;
    p_Fxz = [this](float x,float z)
    {
        return AparaChute*x*x;
    };
    persMat_vec.push_back( persMat( fin, p_Fxz ) );
    p_PM = &persMat_vec.back();
    p_PM->pFx = [this]( double x, double z ){ return 2.0*AparaChute*x; };
    p_PM->pFz = []( double x, double z ){ return 0.0; };
    std::function<double(double,double)>
    F_x2 = [this]( double x, double x1 )
    {
        double u = 2.0*AparaChute;
        return u*x*( gravity.y - u*x1*x1 )/( 1.0 + u*u*x*x );
    };
    ma_paraChute.init( fin, *p_PM, F_x2, paraChuteBall, &pcZ );
    ma_paraChute.setPosition();
    // a 2nd ball
    pcZ2 = pcZ; Vz2 = -Vz;
    X0 = ma_paraChute.x0, X10 = ma_paraChute.x10;
    ma_paraChute2.init( X0, X10, *p_PM, paraChuteBall2, [this](double x){ return AparaChute*x*x; },
     [this](double x){ return 2.0*AparaChute*x; }, [this](double x){ return 2.0*AparaChute; }, &pcZ2 );
     ma_paraChute2.Grav = static_cast<double>( gravity.y );
     ma_paraChute2.numReps = ma_paraChute.numReps;
     ma_paraChute2.setPosition();

    fin >> posX >> posY >> dX;
    parChtEtotMsg.setPosition( posX, posY );
    parChtEtotMsg.setString( " par2Etot = " );
    parChtEtotNumMsg.setPosition( posX + dX, posY );
    parChtEtotNumMsg.setString( "0.0" );

    parCht2EtotNumMsg.setPosition( posX + 2.0f*dX, posY );
    parCht2EtotNumMsg.setString( "0.0" );

    // TEMP: testing persMat inBoundBox()
 //   unsigned int ptCnt; fin >> ptCnt;
 //   vec3f testPos;
  //  for( unsigned int j = 0; j < ptCnt; ++j )
  //  {
 //       fin >> testPos.x >> testPos.y >> testPos.z;
  //      testPos += maxz_sine.pPM->pos;
  //      std::cout << "\n( " << testPos.x << ", " << testPos.y << ", " << testPos.z << " )";
  //      std::cout << " " << ( maxz_sine.pPM->inBoundingBox( testPos ) ? "hit" : "miss" );
  //  }

    // TEMP testing persMat hit test
    size_t PMidx; fin >> PMidx;
    std::string fName; fin >> fName;
 //   std::ifstream finTest( fName.c_str() );
 //   if( finTest && PMidx < persMat_vec.size() )
  //  {
  //      persMat_vec[PMidx].hitTest_test( finTest, std::cout );
  //  }

    // test accuracy of Fx and Fz approximation
    fin >> PMidx;
    fin >> fName;
    std::ofstream fout( fName.c_str() );
    if( !persMat_vec[PMidx].getFx_Fy_test( fin, fout ) ) std::cout << "\nBad FxFz test";

    return true;
}

//double lvl_terrainPerspective::getEtot()const// for cone (0) and parabola (1)
double lvl_terrainPerspective::getEtot( const persMat* p_PM, unsigned int idx )const// assigned at init
{
//    if( matIdx >= persMat_vec.size() ) return 0.0f;
    if( !p_PM ) return 0.0f;


    double rSq = 1.0f;

 //   if( matIdx == 2 )// for parabola
    if( p_PM == ma_parabola.pPM )// for parabola
    {
    //    float G = gravity.y/( persMat_vec[1].scaleX*persMat_vec[1].scaleZ );
        float G = gravity.y/( p_PM->scaleX*p_PM->scaleZ );
        const double &r = ma_parabola.r, &r1 = ma_parabola.r1, &ang1 = ma_parabola.ang1;
        rSq = r*r;
        return 0.5f*( r1*r1*( 1.0f + 4.0f*Apar*Apar*rSq ) + rSq*ang1*ang1 ) - G*Apar*rSq;
    }
 //   else if( matIdx == 1 )// cone
    else if( p_PM == ma_cone.pPM )// cone
    {
        const double &r = ma_cone.r, &r1 = ma_cone.r1, &ang1 = ma_cone.ang1;
        return 0.5f*r1*r1*( 1.0f + Acone*Acone ) + 0.5f*r*r*ang1*ang1 - gravity.y*r*Acone;
    }

    else if( p_PM == ma_paraChute.pPM )// paraChute
    {
        const double &x = idx == 1 ? ma_paraChute.x : ma_paraChute2.x;
        const double &x1 = idx == 1 ? ma_paraChute.x1 : ma_paraChute2.x1;
        const double grav = idx == 1 ? static_cast<double>( gravity.y ) : ma_paraChute2.Grav;
        const double &rVz = idx == 1 ? Vz : Vz2;
        double y = AparaChute*x*x;
        return 0.5f*( x1*x1*( 1.0 + 4.0*AparaChute*y ) + rVz*rVz ) - grav*y;
    }

    return 123.45f;
}

bool lvl_terrainPerspective::init_controls( const std::string& fileName )
{
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


    // control surfaces
    sf::Vector2f HBpos, HBsz;
    std::string fName;
    multiSelector* pMS = nullptr;
    buttonValOnHit* pVOH = nullptr;
    std::ifstream finControl;
    sf::Vector2f sfcOfst;
    // multi control
    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( multiCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
    init_MultiControl( pMS, pVOH, finControl );
    std::cout << "\n initMC done";
    finControl.close();
    button::RegisteredButtVec.push_back( &multiCS );

    // controlList
    sf::Text Label;
    Label.setFont( *button::pFont );
    Label.setCharacterSize(12);
    Label.setFillColor( sf::Color::Black );
    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y;
    Label.setString( fName.c_str() );
    controlList.init( HBpos.x, HBpos.y, HBsz.x, HBsz.y, Label );
    controlList.persist = 3;

    // Bezier control
    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( BezierCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
    init_BezierControl( pMS, pVOH, finControl );
    std::cout << "\n initMC done";
    finControl.close();
    //button::RegisteredButtVec.push_back( &BezierCS );
    controlList.pButtVec.push_back( &BezierCS );

    // lighting control
    fin >> fName >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    pMS = init_controlMS( lightingCS, pVOH, HBpos, HBsz, sfcOfst, finControl );
    init_lightingControl( pMS, pVOH, finControl );
    std::cout << "\n initMC done";
    finControl.close();
    //button::RegisteredButtVec.push_back( &BezierCS );
    controlList.pButtVec.push_back( &lightingCS );

    fin >> fName;
    finControl.open( fName.c_str() );
    if( !finControl ) { std::cout << "\n init_controls() could not open " << fName; return false; }
    init_matAdapterControls( finControl );
    finControl.close();

    controlList.pButtVec.push_back( &coneCS );
    controlList.pButtVec.push_back( &paraCS );

    button::RegisteredButtVec.push_back( &controlList );

    return true;
}

// returns 1st found in line of sight
const persPt* lvl_terrainPerspective::findSighted()const
{
 //   const persPt* pSighted = nullptr;
    for( const persMat& PM : persMat_vec  )
        if( PM.isSighted() ) return &PM;

    for( const persFan& PF : fanVec )
        if( PF.isSighted() ) return &PF;

    if( pf_wideA.isSighted() ) return &pf_wideA;

    std::cout << "\n none sighted";
    return nullptr;
}

bool lvl_terrainPerspective::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        // List: Space Left, Right, Up, Down
        //       J, Z, Lshift, Rshift, H, A

        if( rEvent.key.code == sf::Keyboard::Space )
        {
            if( !firing )
            {
                tFire = tFireDelay;// trigger 1st shot immediately
                firing = true;
            }
        }
        else if( rEvent.key.code == sf::Keyboard::Left )
        {
            if( move_LtRt == 0 && ( LShiftKeyDown || RShiftKeyDown ) )// find poiR
            {
                const persPt* pSighted = findSighted();
                if( pSighted ) poiR = ( pSighted->pos - persPt::camPos ).mag();
                else poiR = 3.0f*persPt::Z0;
            }
            move_LtRt = -1;
        }
        else if( rEvent.key.code == sf::Keyboard::Right )
        {
            if( move_LtRt == 0 && ( LShiftKeyDown || RShiftKeyDown ) )// find poiR
            {
                const persPt* pSighted = findSighted();
                if( pSighted ) poiR = ( pSighted->pos - persPt::camPos ).mag();
                else poiR = 3.0f*persPt::Z0;
            }
            move_LtRt = 1;
        }
        else if( rEvent.key.code == sf::Keyboard::Up ) move_UpDown = 1;
        else if( rEvent.key.code == sf::Keyboard::Down ) move_UpDown = -1;
        else if( rEvent.key.code == sf::Keyboard::J ) jbCamButt.dropStick = true;// release the joybutton
        else if( rEvent.key.code == sf::Keyboard::Z ){ camSpeed = 0.0f; to_SF_string( msgNumVec[4], camSpeed ); }
        else if( rEvent.key.code == sf::Keyboard::LShift ) LShiftKeyDown = true;
        else if( rEvent.key.code == sf::Keyboard::RShift ) RShiftKeyDown = true;
     //   else if( rEvent.key.code == sf::Keyboard::H ) showHelpMsg = !showHelpMsg;
        else if( rEvent.key.code == sf::Keyboard::A ) analyze_flag = true;
    //    else if( rEvent.key.code == sf::Keyboard::Y ) Y_KeyDown = true;// curve chain yaw
    //    else if( rEvent.key.code == sf::Keyboard::P ) P_KeyDown = true;// pitch
    //    else if( rEvent.key.code == sf::Keyboard::R ) R_KeyDown = true;// roll
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        move_LtRt = move_UpDown = 0;
        jbCamButt.dropStick = false;

        // List: Lshift, Rshift, B, Space, Num1, C, P, Q, Num2, Num3
        if( rEvent.key.code == sf::Keyboard::LShift ) LShiftKeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::RShift ) RShiftKeyDown = false;
    //    else if( rEvent.key.code == sf::Keyboard::Y ) Y_KeyDown = false;// curve chain yaw
    //    else if( rEvent.key.code == sf::Keyboard::P ) P_KeyDown = false;// pitch
     //   else if( rEvent.key.code == sf::Keyboard::R ) R_KeyDown = false;// roll


        else if( rEvent.key.code == sf::Keyboard::B ) ani_roll.start();// tRoll = 2.0f*vec2f::PI/poiAngVel;// start barrel roll
        else if( rEvent.key.code == sf::Keyboard::Space ) firing = false;
        else if( rEvent.key.code == sf::Keyboard::Num1 )// call back all shots
        {
            for( persBallTimed& rPBT : shotPool ) { rPBT.inUse = rPBT.doDraw = false; }
            for( persBall& rPB : ballPool ) { rPB.inUse = rPB.doDraw = false; }
        }
        else if( rEvent.key.code == sf::Keyboard::C )// start chain animation
        {
            if( LShiftKeyDown || RShiftKeyDown )
            {
                cluster_1.showBound = !cluster_1.showBound;
                cluster_1.showCenter = !cluster_1.showCenter;
            }
            else
                ani_halfPitch.start();
        }
        else if( rEvent.key.code == sf::Keyboard::P )// loop over or (+shift) place or return surfer
        {
            if( LShiftKeyDown || RShiftKeyDown )// place or return surfer
            {
                if( pMatSurfer && pMatBouncer )// return to ballPool
                {
                    pMatSurfer->inUse = pMatSurfer->doDraw = pMatSurfer->isMoving = false;
                    *pMatBouncer = *pMatSurfer;
                    pMatSurfer = pMatBouncer = nullptr;
                    std::cout << "\nSTOP surfing";
                }
                else// getSurfer
                {
                    pMatSurfer = getPoolBall();
                    pMatBouncer = getPoolBall();
                    if( pMatSurfer && pMatBouncer )// ball obtained
                    {
                        surfMatPos = surfMatPos0;
                        pMatSurfer->vel.x = surfVX0;
                        pMatSurfer->vel.y = 0.0f;
                        pMatSurfer->vel.z = surfVZ0;
                        std::cout << "\nStart surfing";
                        if( pSurfMat )
                        {
                            vec3f iPos = pSurfMat->toWorldPos( surfMatPos.x, surfMatPos.z );
                            vec3f Nu = pSurfMat->getGradient( surfMatPos.x, surfMatPos.z );
                        //    iPos += Nu*( pMatSurfer->Rbound/Nu.mag() );
                            iPos += Nu/Nu.mag();
                            pMatSurfer->setPosition( iPos );
                            pMatSurfer->isMoving = true;
                            *pMatBouncer = *pMatSurfer;
                            pMatBouncer->B.setFillColor( sf::Color::Red );
                        }

                    }
                }
            }
            else// start vertical loop
            {
                ani_pitch.start();
            }
        }
        else if( rEvent.key.code == sf::Keyboard::Q )// start/stop bouncer
        {
            if( pMatBouncer )
            {
                pMatBouncer->inUse = pMatBouncer->doDraw = false;
                pMatBouncer = nullptr;
                std::cout << "\nSTOP bouncing";
            }
            else
            {
                pMatBouncer = getPoolBall();
                if( pMatBouncer )// ball obtained
                {
              //      surfMatPos = surfMatPos0;
                    pMatBouncer->vel.x = surfVX0;
                    pMatBouncer->vel.y = 0.0f;
                    pMatBouncer->vel.z = surfVZ0;
                    std::cout << "\nStart bouncing";
                    if( pSurfMat )
                    {
                        vec3f iPos = pSurfMat->toWorldPos( surfMatPos0.x, surfMatPos0.z );
                    //    vec3f Nu = pSurfMat->getGradient( surfMatPos.x, surfMatPos.z );
                    //    iPos += Nu*( pMatSurfer->Rbound/Nu.mag() );
                        iPos += 100.0f*persPt::yHat;
                        pMatBouncer->setPosition( iPos );
                        pMatBouncer->isMoving = true;
                    }

                }
            }
        }
        else if( rEvent.key.code == sf::Keyboard::Num2 )// start/stop recording
        {
        /*    if( recorderMode != 'R' )
            {
                recorderMode = 'R';// record
                buffIsFull = false;// start at beginning
                endIdx = 0;
            }
            else recorderMode = 'O';// off  */
            if( recorder.mode != 'R' )
            {
                recorder.startNewRecording();
             //   recorder.mode = 'R';// record
             //   recorder.buffIsFull = false;// start at beginning
             //   recorder.endIdx = 0;
            }
        //    else recorder.mode = 'O';// off
            else recorder.stop();// off
        }
        else if( rEvent.key.code == sf::Keyboard::Num3 )// start/stop playback
        {
            if( recorder.mode != 'P' )
            {
                recorder.play();
            //    recorder.mode = 'P';// record
            //    if( recorder.buffIsFull ) recorder.playIdx = ( recorder.endIdx + 1 )%recorder.numFrames;
            //    else recorder.playIdx = 0;
            }
       //     else recorder.mode = 'O';// off
            else recorder.stop();// off
        }
    }

    return true;
}

void lvl_terrainPerspective::update_global( float dt )
{
    float zcDotY = persPt::camDir.dot( persPt::yHat );
    vec3f hu = persPt::camDir - zcDotY*persPt::yHat;
    float huMag = hu.mag();
    if( huMag < 0.01f ) hu = persPt::zHat;
    else hu /= huMag;

    if( button::mseDnLt )
    {
        if( doCoast ) camSpeed += camAccel*dt;
        else persPt::camPos += hu*(camSpeed*dt);

     //   if( button::clickEvent_Rt() == 1 ) camSpeed = 0.0f;// both down
        to_SF_string( msgNumVec[4], camSpeed );

    }
    else if( button::mseDnRt )
    {
        if( doCoast ) camSpeed -= camAccel*dt;
        else persPt::camPos -= hu*(camSpeed*dt);

     //   if( button::clickEvent_Lt() == 1 ) camSpeed = 0.0f;// stop
        to_SF_string( msgNumVec[4], camSpeed );
    }

 //   if( doCoast ) persPt::camPos += hu*(camSpeed*dt);
    if( doCoast )
    {
        persPt::camPos += persPt::camDir*(camSpeed*dt);
        if( animatedTransition<float>::numAnimating == 0 )
            vec3f::bank( -gravity.y, camSpeed, yawRate, dt, persPt::xu, persPt::yu, persPt::camDir );
    }

    float dAr = 0.0f;
    if( button::didScroll ) dAr = rollRate*button::scrollAmount;

    float compassAngle = persPt::changeCamDir( yawRate*dt, pitchRate*dt, dAr );
    to_SF_string( msgNumVec[3], compassAngle );

    persPt::calibrateCameraAxes();
    // translational motion via arrow keys
    if( move_LtRt != 0 )
    {
        if( LShiftKeyDown || RShiftKeyDown )
        {
            if( keepLevel ) persPt::updatePOI( poiR, move_LtRt*poiAngVel, dt );
            else persPt::roll( move_LtRt*poiAngVel*dt );
        }
   //     else if( Y_KeyDown || P_KeyDown || R_KeyDown )// rotate curve chain
   //     {
   //         if( Y_KeyDown ) rotate_curveChain( 'Y', move_LtRt*rotSpeedChain*dt );
    //        else if( P_KeyDown ) rotate_curveChain( 'P', move_LtRt*rotSpeedChain*dt );
    //        else rotate_curveChain( 'R', move_LtRt*rotSpeedChain*dt );
    //    }
        else persPt::camPos += move_LtRt*move_xyVel*persPt::xu*dt;// just translate
    }

    if( move_UpDown != 0 )
    {
        if( LShiftKeyDown || RShiftKeyDown ) persPt::pitch( move_UpDown*poiAngVel*dt );
        else persPt::camPos += move_UpDown*move_xyVel*persPt::yHat*dt;
    }

    to_SF_string( msgNumVec[0], persPt::camPos.x );
    to_SF_string( msgNumVec[1], persPt::camPos.y );
    to_SF_string( msgNumVec[2], persPt::camPos.z );

    if( pPitchNumMsg )
    {
        float angle = asinf( persPt::camDir.dot( persPt::yHat ) )*( 180.0f/vec2f::PI );
        to_SF_string( *pPitchNumMsg, angle );
    }

    // new animatedTransition
    if( animatedTransition<float>::numAnimating > 0 )
 //   if( animatedTransition<float>::numAnimating > 1 )// due to car always animating
    {
     //   ani_roll.updateRate(dt);
     //   ani_pitch.updateRate(dt);
     //   ani_halfPitch.updateRate(dt);
     //   ani_halfRoll.updateRate(dt);

        for( animatedTransition<float>* pAT : pAniTransVec )
            pAT->updateRate(dt);
    }
    else if( keepLevel )
        persPt::keepXuLevel();// keep xu level
    else if( false )//( doCoast )
    {
        if( persPt::yu.y > 0.1f )
        {
            vec3f::bank( -gravity.y, camSpeed, yawRate, dt, persPt::xu, persPt::yu, persPt::camDir );
        }

         //   if( persPt::yu.y > 0.7f )
         //   {
                Yu_total -= Yu_queue.front();// this total
                Yu_queue.pop();
                Yu_queue.push( persPt::yu );
                Yu_total += persPt::yu;//       should remain balanced
                persPt::yu = Yu_total*( 1.0f/Yu_queue.size() );
        //    }

            persPt::camDir /= persPt::camDir.mag();
       //     persPt::yu = Yu_total*( 1.0f/Yu_queue.size() );
            persPt::yu /= persPt::yu.mag();
            // form xu
            persPt::xu = persPt::yu.cross( persPt::camDir );
            persPt::yu = persPt::camDir.cross( persPt::xu );// assure orthogonality


    }

  //  updatePane();
    persPt::updateBackground( trueHorizon, upperPane, nullptr );
}

// friend
float get_BezSlope( float u, float u1, float u2 )
{
    if( u > 1.0f ) u = 1.0f;
    float uSq = u*u;
    float A = 1.0f + u1 + u2, C = u1*u2, B = A + C - 1.0f;
    float K = 0.2f - 0.25*A + B/3.0f - 0.5f*C;// magnitude
    return ( uSq*uSq - A*uSq*u + B*uSq - C*u )/K;// dy/du. caller mult by du/dt to find speed = dy/dt
}

void lvl_terrainPerspective::update_mats( float dt )
{
   for( persMat& rPM : persMat_vec ) rPM.update(dt);
}

void lvl_terrainPerspective::update_sun()
{
    // vary moon position
 //   moonDir += 0.01f*persPt::xHat;
 //   moonDir /= moonDir.mag();

    const float Rhb[] = { theSun.getRadius(), theMoon.getRadius() };// heavenly body radius
    const vec3f Dir[] = { lightDir, moonDir };
    bool *p_doDraw[] = { &doDrawSun, &doDrawMoon };
    sf::CircleShape *pCS[] = { &theSun, &theMoon };
    doDrawSun = doDrawMoon = false;

    // sun and moon
    for( unsigned j = 0; j < 2; ++j )
        if( persPt::camDir.dot( Dir[j] ) < 0.0f )// facing towards
        {
            sf::Vector2f Pos = persPt::get_xyw( persPt::camPos - Dir[j]*persPt::Z0 );
            if( Pos.x + Rhb[j] < 0.0f ) ;// left of window
            else if( Pos.x - Rhb[j] > Level::winW ) ;// right of window
            else if( Pos.y + Rhb[j] < 0.0f ) ;// above window
            else if( Pos.y - Rhb[j] > Level::winH ) ;// below window
            else
            {
                *( p_doDraw[j] ) = true;
                pCS[j]->setPosition( Pos );
            }
        }
}

void lvl_terrainPerspective::update( float dt )
{
    if( !jbCamButt.atRest )
    {
        recorder.update();
        update_global(dt);// camera position and orientation and "live" messages
        for( persPt* pPt : pPtStillVec ) pPt->update(dt);

        update_sun();
        groundGrid.update();

    //    vec3f::roll( rotSpeedVec[0]*dt, persBox_vec[0].Xu, persBox_vec[0].Yu, persBox_vec[0].Zu );// speed of dime
    //    vec3f::pitch( rotSpeedVec[1]*dt, persBox_vec[1].Xu, persBox_vec[1].Yu, persBox_vec[1].Zu );// speed of fan
    //    vec3f::yaw( rotSpeedVec[2]*dt, persBox_vec[2].Xu, persBox_vec[2].Yu, persBox_vec[2].Zu );// speed of ellipse

    }

 //   for( persPt* pPt : pPtMovingVec ) pPt->update(dt);
    vec3f::roll( rotSpeedVec[0]*dt, persBox_vec[0].Xu, persBox_vec[0].Yu, persBox_vec[0].Zu );// speed of dime
    vec3f::pitch( rotSpeedVec[1]*dt, persBox_vec[1].Xu, persBox_vec[1].Yu, persBox_vec[1].Zu );// speed of fan
 //   vec3f::yaw( rotSpeedVec[2]*dt, persBox_vec[2].Xu, persBox_vec[2].Yu, persBox_vec[2].Zu );// speed of ellipse

    for( persBox& PB : persBox_vec )
    {
        PB.update(dt);
        PB.updateShading( lightDir );
    }

    // new type persFanWide
    vec3f::roll( rotSpeedVec[0]*dt, pf_wideA.Xup, pf_wideA.Yup, pf_wideA.Zup );// speed of dime
    pf_wideA.update(dt);
    pf_wideA.updateShading( lightDir );
    pf_wideA.updateShadow( lightDir, persPt::yHat, 0.0f );

 //   for( animatedTransition<float>& rAT : carAniVec )
 //   {
  //      rAT.updateRate(dt);
     //   if( rAT.updateRate(dt) )
     //       std::cout << "...car running";
 //   }


    // animations
    if( numAnis_inUse > 0 )
    {
        numAnis_inUse = 0;
        for( persQuadAni& PQA : quadAniVec )
        {
            PQA.update(dt);
            if( PQA.inUse ) ++numAnis_inUse;
        }
    }

    for( unsigned int n = 0; n < fanVec.size(); ++n )
    {
        float dAngle = rotSpeedVec[n]*dt;
        if( operCodeVec[n] == 'Y' ) fanVec[n].yaw( dAngle );
        else if( operCodeVec[n] == 'P' ) fanVec[n].pitch( dAngle );
        else if( operCodeVec[n] == 'R' ) fanVec[n].roll( dAngle );
        else if( operCodeVec[n] == 'A' )
        {
            vec3f tRot = persPt::yu;// rotation axis
            rotateBasisLH( tRot, dAngle, fanVec[n].Xup, fanVec[n].Yup, fanVec[n].Zup );
        }

        fanVec[n].updateShading( lightDir );
        fanVec[n].updateShadow( lightDir, persPt::yHat, 0.0f );
    }

    cluster_1.update(dt);
    coordAxesA.update();

    //coneBall and parBall
    ma_cone.update(dt);
    if( ma_cone.pPM && ma_cone.doAnimate ) to_SF_string( coneEtotNumMsg, getEtot( ma_cone.pPM ) );
    ma_parabola.update(dt);
    if( ma_parabola.pPM && ma_parabola.doAnimate ) to_SF_string( parEtotNumMsg, getEtot( ma_parabola.pPM ) );
    ma_paraChute.update(dt);
    if( ma_paraChute.pPM && ma_paraChute.doAnimate )
    {
        pcZ += Vz*dt;
     //   std::cout << "\n pcZ = " << pcZ << " Vz = " << Vz;
        if( ( pcZ > 0.5*ma_paraChute.pPM->szZ ) || ( pcZ < -0.5*ma_paraChute.pPM->szZ ) ){  Vz *= -1.0; pcZ += Vz*dt; }// std::cout << "\nVz = " << Vz; }
        to_SF_string( parChtEtotNumMsg, getEtot( ma_paraChute.pPM ) );
    }

    ma_paraChute2.update(dt);
    if( ma_paraChute2.pPM && ma_paraChute2.doAnimate )
    {
        pcZ2 += Vz2*dt;
        if( ( pcZ2 > 0.5*ma_paraChute2.pPM->szZ ) || ( pcZ2 < -0.5*ma_paraChute2.pPM->szZ ) ){  Vz2 *= -1.0; pcZ2 += Vz2*dt; }// std::cout << "\nVz = " << Vz; }
    //    float Etot2 =
        to_SF_string( parCht2EtotNumMsg, getEtot( ma_paraChute2.pPM, 2 ) );
    }

    // rain balls from sine hill
    if( runSineBalls )
    {
        static float tElap = 0.0f;
        tElap += dt;
        if( tElap > dtLaunch )
        {
            tElap -= dtLaunch;
            persBall* pPB = getPoolBall();
            if( pPB )
            {
                angV0 += dAngV0;// new launch angle
                pPB->vel = sineV0*( cosf( angV0 )*sineVs1 + sinf( angV0 )*sineVs2 );
                pPB->setPosition( sineLaunchPos );
            //    std::cout << "\nLaunch!";
            }
        }
    }

    if( numBalls_inUse > 0 )
    {
        numBalls_inUse = 0;
        for( persBall& rPB : ballPool )
        {
            rPB.update( gravity, dt );
            if( rPB.inUse ){ ++numBalls_inUse; };// std::cout << " up"; }
        }
    }

    // surfer
    if( pSurfMat && pMatSurfer ) pSurfMat->rideMat( *pMatSurfer, surfMatPos, surfNu, gravity, true, dt );

    // left mouse click to play/pause motion
    if( !button::pButtMse )
    {
        if( button::clickEvent_Lt() == 1 )
        {
            if( ma_cone.pPM && ma_cone.pPM->hit_image( button::mseX, button::mseY ) ) ma_cone.doAnimate = !ma_cone.doAnimate;
            else if( ma_parabola.pPM && ma_parabola.pPM->hit_image( button::mseX, button::mseY ) ) ma_parabola.doAnimate = !ma_parabola.doAnimate;
            else if( ma_paraChute.pPM && ma_paraChute.pPM->hit_image( button::mseX, button::mseY ) )
            { ma_paraChute2.doAnimate = ma_paraChute.doAnimate = !ma_paraChute.doAnimate; std::cout << "\n paraChute doAnimate = " << ma_paraChute.doAnimate; }
            else if( pSinePM && pSinePM->hit_image( button::mseX, button::mseY ) )
            { runSineBalls = !runSineBalls; }
        }
        else if( button::clickEvent_Rt() == 1 )// rt click to reset motion
        {
            if( ma_cone.pPM && ma_cone.pPM->hit_image( button::mseX, button::mseY ) ) ma_cone.reset();
            else if( ma_parabola.pPM && ma_parabola.pPM->hit_image( button::mseX, button::mseY ) ) ma_parabola.reset();
            else if( ma_paraChute.pPM && ma_paraChute.pPM->hit_image( button::mseX, button::mseY ) )
            { ma_paraChute.reset( pcZ0 ); ma_paraChute2.reset( pcZ0 ); }
        }
    }

    if( numShots_inUse > 0 )
    {
        numShots_inUse = 0;
        for( persBallTimed& PBT : shotPool )
        {
            PBT.update( dt );
            if( PBT.inUse ){ ++numShots_inUse; }// std::cout << "\n during updates: numInUse = " << numShots_inUse; }
        }

        // collision test
        if( numShots_inUse > 0 )
        {
            for( persBall& PB : shotPool )
                hitAll( PB, dt );
        }
    }

    // collision test
    if( numBalls_inUse > 0 )
    {
        for( persBall& rPB : ballPool )
            hitAll( rPB, dt );
    }

    // firing
    if( firing && (tFire+=dt) > tFireDelay )
    {
        if( getShot( getShotPos0(), getShotVel0() ) != nullptr )
        {
            tFire -= tFireDelay;
       //     std::cout << "\n Fire!";
        }
    }

    // z ordering
    update_Zorder();
}

void lvl_terrainPerspective::update_Zorder()
{
    cluster_1.numToDraw = 0;

    numToDraw = 0;
    for( persPt* pPt : pPtStillVec )
        if( pPt->doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = pPt;

    for( persPt* pPt : pPtMovingVec )
        if( pPt->doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = pPt;

    persCluster& C1 = cluster_1;

    for( persQuadAni& rPQA : quadAniVec )
    {
     //   if( rPQA.inUse && rPQA.doDraw  && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &rPQA;

        if( rPQA.inUse && rPQA.doDraw )
        {
            if( C1.inBounds( rPQA.pos ) )
            {
                if( C1.numToDraw < C1.pSortedVec.size() ) C1.pSortedVec[ C1.numToDraw ] = &rPQA;
                else C1.pSortedVec.push_back( &rPQA );
                ++C1.numToDraw;
            }
            else
            {
                if( numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw ] = &rPQA;
                else pPtSortVec.push_back( &rPQA );
                ++numToDraw;
            }
        }
    }

    for( persBall& rPB : ballPool )
        if( rPB.inUse && rPB.doDraw  && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &rPB;

    for( persBallTimed& rPB : shotPool )
    {
        if( rPB.inUse && rPB.doDraw )
        {
            if( C1.inBounds( rPB.pos ) )
            {
                if( C1.numToDraw < C1.pSortedVec.size() ) C1.pSortedVec[ C1.numToDraw ] = &rPB;
                else C1.pSortedVec.push_back( &rPB );
                ++C1.numToDraw;
            }
            else
            {
                if( numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw ] = &rPB;
                else pPtSortVec.push_back( &rPB );
                ++numToDraw;
            }
        }
    }

 //   for( persFan& rPF : fanVec )
  //      if( rPF.inUse && rPF.doDraw  && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &rPF;

 //   if( coneBall.doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &coneBall;
 //   if( parBall.doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &parBall;
 //   if( paraChuteBall.doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &paraChuteBall;
 //   if( paraChuteBall2.doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &paraChuteBall2;
 //   if( cluster_1.doDraw && numToDraw < pPtSortVec.size() ) pPtSortVec[ numToDraw++ ] = &cluster_1;

    // CRASH!!  segfault on sort
    if( numToDraw > 0 )// sort!
    {
        persPt::sortByDistance( pPtSortVec, numToDraw );// no crash
        //       std::sort( pPtSortVec.begin(), pPtSortVec.begin() + numToDraw, persPt::compare );
        if( analyze_flag )
        {
            analyze_flag = false;
            analyze_Zorder();
        }
    }

 //       std::sort( pPtSortVec.begin(), pPtSortVec.begin() + numToDraw, persPt::compare );
 //       std::sort( pPtSortVec.begin(), pPtSortVec.begin() + numToDraw );
    cluster_1.z_order();
}

void lvl_terrainPerspective::analyze_Zorder()const
{
    const persPt *pShot = &( shotPool[0] );
    const persPt *pBox = &( persBox_vec[1] );// big brown box
    const persPt *pFan = &( fanVec[0] );// dime

    std::cout << "\n analyze: order is:";
    for( const persPt* pPt : pPtSortVec )
    {
        if(  pPt == pShot ) std::cout << " shot";
        else if(  pPt == pBox ) std::cout << " box";
        else if(  pPt == pFan ) std::cout << " dime";
    }

    // distances from camera
    if( pBox->doDraw )
    {
        std::cout << "\n distance to box : " << pBox->getDistance();
        if( pShot->inUse && pShot->doDraw )
        {
            std::cout << "\n distance to shot: " << pShot->getDistance();
            std::cout << "\n shot < box ? " << persPt::compare( pShot, pBox );
        }

        if( pFan->doDraw )
        {
            std::cout << "\n distance to dime : " << pFan->getDistance();
            std::cout << "\n dime < box ? " << persPt::compare( pFan, pBox );
        }

    }
}

void lvl_terrainPerspective::draw( sf::RenderTarget& RT ) const
{
    RT.draw( upperPane, 4, sf::Quads );
    RT.draw( trueHorizon, 2, sf::Lines );

    if( doDrawSun ) RT.draw( theSun );
    if( doDrawMoon ) RT.draw( theMoon );

    groundGrid.draw(RT);

    for( unsigned int j = 0; j < numToDraw; ++j ) pPtSortVec[j]->draw(RT);
    coordAxesA.draw(RT);

    // new: shadows for type persFan
    if( !persFan::shadowVtxArray.empty() )
    {
        for( const persFan& PF : fanVec )
        {
            if( PF.inUse && PF.doDrawShadow && PF.pShadowVA )
                RT.draw( *(PF.pShadowVA) );
        }

        // for pf_wideA
        if( pf_wideA.inUse && pf_wideA.doDrawShadow && pf_wideA.pShadowVA )
            RT.draw( *(pf_wideA.pShadowVA) );
    }

    RT.draw( viewCross, 4, sf::Lines );
    // always over
    for( const sf::Text& txt : msgVec ) RT.draw( txt );
    for( const sf::Text& txt : msgNumVec ) RT.draw( txt );
    RT.draw( coneEtotMsg );
    RT.draw( coneEtotNumMsg );
    RT.draw( parEtotMsg );
    RT.draw( parEtotNumMsg );
    RT.draw( parChtEtotMsg );
    RT.draw( parChtEtotNumMsg );
    RT.draw( parCht2EtotNumMsg );

    recorder.draw(RT);
    if( showHelpMsg ) RT.draw( helpMsg );
}

multiSelector* lvl_terrainPerspective::init_controlMS( controlSurface& rCS, buttonValOnHit*& pStrip, sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is )
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

bool lvl_terrainPerspective::init_MultiControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is )
{
    if( pMS->sdataVec.size() != 12 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // camSpeed
    pSD->pSetFunc =     [this](float x){ camSpeed = x; if( pSpeedNumMsg ) to_SF_string( *pSpeedNumMsg, x ); };
    camSpeed = pSD->xCurr;
    pStrip->reInit( *pSD );
    // camAccel
    (++pSD)->pSetFunc = [this](float x){ camAccel = x; };
    camAccel = pSD->xCurr;
    //  move_xyVel = translation speed via arrow keys ( scroll )
    (++pSD)->pSetFunc = [this](float x){ move_xyVel = x; };
    move_xyVel = pSD->xCurr;
    // gravity
    (++pSD)->pSetFunc = [this](float x){ gravity.y = ma_paraChute.Grav = ma_paraChute2.Grav = -1.0f*x; };
    gravity.y = ma_paraChute.Grav = ma_paraChute2.Grav = -1.0f*pSD->xCurr;
    // fireVel
    (++pSD)->pSetFunc = [this](float x){ fireVel = x; };
    fireVel = pSD->xCurr;
    // tFireDelay = 1/firing rate
    (++pSD)->pSetFunc = [this](float x){ tFireDelay = 1.0f/x; };
    tFireDelay = 1.0f/pSD->xCurr;
    // persBallTimed::tLife
    (++pSD)->pSetFunc = [this](float x) { persBallTimed::tLife = x; };
    persBallTimed::tLife = pSD->xCurr;
    // persMat.Cr
    (++pSD)->pSetFunc = [this](float x) { for( persMat& rPM : persMat_vec ) rPM.Cr = x; };
    for( persMat& rPM : persMat_vec ) rPM.Cr = pSD->xCurr;
    // tFinalRoll
    (++pSD)->pSetFunc = [this](float x) { tFinalRoll = x; };
    tFinalRoll = pSD->xCurr;
    // rotation speeds
    for( unsigned int n = 0; n < 3; ++n )
    {
        (++pSD)->pSetFunc = [this,n](float x) { rotSpeedVec[n] = x; };
        rotSpeedVec[n] = pSD->xCurr;
    }

    // a "coasting on" button
    sf::Vector2f Pos, Sz; is >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    Pos += multiCS.bkRect.getPosition();
    buttonRect* pBR = new buttonRect( Pos.x, Pos.y, Sz.x, Sz.y, nullptr, "COAST" );
    pBR->setSel(true);// on by default
    pBR->pHitFunc = [this,pBR,pMS]()
    {
        doCoast = pBR->sel;
        if( doCoast ) camSpeed = 0.0f;
        else camSpeed = pMS->sdataVec[0].xCurr;
        if( pSpeedNumMsg ) to_SF_string( *pSpeedNumMsg, camSpeed );
    };
    pBR->pHitFunc();
    multiCS.pButtVec.push_back( pBR );

    // a "keep level" button
    is >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    Pos += multiCS.bkRect.getPosition();
    pBR = new buttonRect( Pos.x, Pos.y, Sz.x, Sz.y, nullptr, "LEVEL" );
//    pBR->setSel(true);// on by default
    pBR->pHitFunc = [this,pBR]() { keepLevel = pBR->sel; };
    pBR->pHitFunc();
    multiCS.pButtVec.push_back( pBR );

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_terrainPerspective::init_BezierControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is )
{
    if( pMS->sdataVec.size() != 6 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // tFinalRoll
    pSD->pSetFunc =     [this](float x){ tFinalRoll = tElapRoll = x; };
    tFinalRoll = tElapRoll = pSD->xCurr;
    pStrip->reInit( *pSD );
    // roll_u1
    (++pSD)->pSetFunc = [this](float x){ roll_u1 = x; };
    roll_u1 = pSD->xCurr;
    // roll_u2
    (++pSD)->pSetFunc = [this](float x){ roll_u2 = x; };
    roll_u2 = pSD->xCurr;

    // tFinalPitch
    (++pSD)->pSetFunc =     [this](float x){ tFinalPitch = tElapPitch = x; };
    tFinalPitch = tElapPitch = pSD->xCurr;
    // pitch_u1
    (++pSD)->pSetFunc = [this](float x){ pitch_u1 = x; };
    pitch_u1 = pSD->xCurr;
    // pitch_u2
    (++pSD)->pSetFunc = [this](float x){ pitch_u2 = x; };
    pitch_u2 = pSD->xCurr;

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_terrainPerspective::init_lightingControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is )
{
    if( pMS->sdataVec.size() != 2 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // pitchAngle
    pSD->pSetFunc =     [this]( float pa )
    {
        pa *= vec2f::PI/180.0f;
        light_pitch = pa;
        lightDir = sinf( pa )*( persPt::xHat*sinf( light_yaw ) + persPt::zHat*cosf( light_yaw ) ) - persPt::yHat*cosf( pa );
    };

    light_pitch = pSD->xCurr*vec2f::PI/180.0f;
    pStrip->reInit( *pSD );

    // yawAngle
    (++pSD)->pSetFunc = [this]( float ya )
    {
        ya *= vec2f::PI/180.0f;
        light_yaw = ya;
        lightDir = sinf( light_pitch )*( persPt::xHat*sinf( ya ) + persPt::zHat*cosf( ya ) ) - persPt::yHat*cosf( light_pitch );
    };
 //   light_yaw = pSD->xCurr*vec2f::PI/180.0f;
    // get lightDir and light_yaw initialized
    pSD->pSetFunc( pSD->xCurr );// argument in degrees

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

// all 3
bool lvl_terrainPerspective::init_matAdapterControls( std::istream& is )
{
    // control surfaces
    sf::Vector2f HBpos, HBsz;
    sf::Vector2f sfcOfst;
    std::string inStr;

    controlSurface* pCS[] = { &coneCS, &paraCS };
    mattAdapterPolar* pMAP[] = { &ma_cone, &ma_parabola };

    for( unsigned int j = 0; j < 2; ++j )
    {
        is >> inStr;// label for home button
        std::cout << "\n inStr:  " << inStr;
        is >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
        sf::Vector2f PosSurf( HBpos + sfcOfst );
        sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;

        pCS[j]->init( HBpos, PosSurf, SzSurf, inStr.c_str(), HBsz.x, HBsz.y );
        pCS[j]->ownButts = true;

        sf::Vector2f Pos; is >> Pos.x >> Pos.y;// as offset from PosSurf
        Pos += PosSurf;
        buttonValOnHit* pVOH = new buttonValOnHit();
        multiSelector* pMS = new multiSelector( Pos, pVOH, is );
        pMS->ownsStrip = true;
        pCS[j]->pButtVec.push_back( pMS );
    //    button::RegisteredButtVec.push_back( pCS[j] );

        // coneCS

        if( pMS->sdataVec.size() != 5 )
        {
            std::cout << "\nbad sdataVec.size = " << pMS->sdataVec.size() << " idx: " << j;
         //   is >> inStr; std::cout << " next: " << inStr;
            return false;
        }
        stripData* pSD = &( pMS->sdataVec.front() );
        mattAdapterPolar* p_map = pMAP[j];
        // r0
        pSD->pSetFunc =     [this, p_map](float x){ p_map->r0 = static_cast<float>(x); };
        p_map->r0 = static_cast<float>( pSD->xCurr );
        pVOH->reInit( *pSD );
        // r10
        (++pSD)->pSetFunc = [this,p_map](float x){ p_map->r10 = static_cast<float>(x); };
        p_map->r1 = static_cast<float>( pSD->xCurr );
        // ang0
        (++pSD)->pSetFunc = [this,p_map](float x){ p_map->ang0 = static_cast<float>(x); };
        p_map->ang0 = static_cast<float>( pSD->xCurr );
        // ang10
        (++pSD)->pSetFunc = [this,p_map](float x){ p_map->ang10 = static_cast<float>(x); };
        p_map->ang10 = static_cast<float>( pSD->xCurr );
        // numReps
        (++pSD)->pSetFunc = [this,p_map](float x){ p_map->numReps = static_cast<unsigned int>(x); };
        p_map->numReps = static_cast<unsigned int>( pSD->xCurr );
    }

    return true;
}

vec3f lvl_terrainPerspective::getShotVel0()const
{
    vec3f hu = persPt::camDir - persPt::camDir.dot( persPt::yHat )*persPt::yHat;
    hu /= hu.mag();
    vec3f retVal = fireVel*persPt::camDir;
    if( doCoast || ( button::mseDnLt && !jbCamButt.atRest ) ) retVal += ( camSpeed*persPt::camDir.dot(hu) )*hu;
    // translational motion via arrow keys
    if( move_LtRt != 0 ) retVal += move_LtRt*move_xyVel*persPt::xu;
    //   if( move_UpDown != 0 ) persPt::camPos += move_UpDown*move_xyVel*persPt::yu*dt;
    if( move_UpDown != 0 ) retVal += move_UpDown*move_xyVel*persPt::yHat;

    return retVal;
}

bool lvl_terrainPerspective::getAniQuad( persBall& rPB, vec3f Pos )// trigger animation at ball position and take ball out of use
{
    rPB.inUse = false;
    rPB.doDraw = false;// to prevent z ordering
 //   --numShots_inUse;
    return getAniQuad( Pos );
}

bool lvl_terrainPerspective::getAniQuad( vec3f Pos )// trigger animation at ball Pos
{
    if( persQuadAni::getOne( quadAniVec, Pos ) )
    {
        ++numAnis_inUse;
        return true;
    }

    return false;// all Ani are inUse
}

persBallTimed* lvl_terrainPerspective::getShot( vec3f Pos, vec3f Vel )
{
    for( persBallTimed& rPB : shotPool )
        if( !rPB.inUse )
        {
            rPB.setPosition( Pos );
            rPB.vel = Vel;
            rPB.inUse = true;
            rPB.tElap = 0.0f;
            ++numShots_inUse;
            return &rPB;
        }

    return nullptr;
}

persBall* lvl_terrainPerspective::getPoolBall()
{
    for( persBall& rPB : ballPool )
    {
        if( !rPB.inUse )// available
        {
            rPB.inUse = true;// checked out
            rPB.isMoving = true;
            ++numBalls_inUse;
            return &rPB;
        }
    }

    return nullptr;
}

void lvl_terrainPerspective::hitAll( persBall& PB, float dt )
{
    if( !PB.inUse ) return;

    if( PB.pos.y < 0.0f && PB.vel.y < 0.0f )// target = the ground
    {
      //  PB.doDraw = PB.inUse = false;
        getAniQuad( PB, PB.pos + 10.0f*persPt::yHat );
        return;
    }

    // vs persMat
    vec3f P, vu;
  //  static int hitCnt = 0;
    for( persMat& rPM : persMat_vec )
        if( rPM.hit( PB.pos - PB.vel*dt, PB.pos, P, PB.vel ) )
        {
      //      PB.setPosition( P + PB.Rbound*vu );
            PB.setPosition( P );
        //    PB.vel = vu*PB.vel.mag();
        //    getAniQuad( PB, PB.pos );
        //    std::cout << "\n hitCnt = " << ++hitCnt;
            return;
        }

    for( persBox& rPB : persBox_vec )
        if( rPB.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
        {

         //   std::cout << "\n ding!";
            vec3f sep = persPt::camPos - rPB.pos;
            sep /= sep.mag();
            PB.setPosition( P + PB.Rbound*sep );
            PB.vel = vu*PB.vel.mag();
         //   getAniQuad( PB, P + PB.Rbound*sep );
            return;
        }

    if( cluster_1.hitAll( PB, dt ) ) return;
}

// utility
bool lvl_terrainPerspective::loadSpriteSheets( const std::string& inFileName )
{
    std::ifstream finList( inFileName.c_str() );
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

// friends
// WORKING!!!
void rotateBasisLH( vec3f tRot, float angle, vec3f& Xu, vec3f& Yu, vec3f& Zu )
{
    float snA = sinf( angle ), csA = cosf( angle );

    tRot /= tRot.mag();
    float Zur = Zu.dot( tRot );
//    vec3f t1 = ( Zur*Zur < 0.99f ) ? Zu.cross( tRot ) : tRot.cross( Yu );

    vec3f t10 = Zu.cross( tRot );
    t10 /= t10.mag();
    vec3f t20 = t10.cross( tRot );

    float Zu1 = Zu.dot(t10), Zu2 = Zu.dot(t20);
    float Yur = Yu.dot( tRot ), Yu1 = Yu.dot(t10), Yu2 = Yu.dot(t20);
    vec3f t1 = csA*t10 + snA*t20;
    vec3f t2 = csA*t20 - snA*t10;

    Zu = Zur*tRot + Zu1*t1 + Zu2*t2;
    Yu = Yur*tRot + Yu1*t1 + Yu2*t2;
    Zu /= Zu.mag();// assure normalization
    Yu /= Yu.mag();
    Xu = Yu.cross(Zu);

    Zu = Xu.cross(Yu);// assure orthogonality
}

void makeXuLevelLH( vec3f& Xu, vec3f& Yu, vec3f& Zu )
{
    Xu = persPt::yHat.cross( Zu );
    float XuMag = Xu.mag();
    if( XuMag > 0.1f ) Xu /= XuMag;
    else Xu = persPt::xHat;

    Yu = Zu.cross( Xu );
}

/*
bool lvl_terrainPerspective::init_curves( const char* fileName )
{
    std::ifstream fin( fileName );
    if( !fin ) return false;

    unsigned int N; fin >> N;
    if( N < 1 ) return false;

    curveVec.reserve(N);
    std::function<void(float,vec3f&)> getPos = nullptr;

    for( unsigned int n = 0; n < N; ++n )
    {
        float Rb; fin >> Rb;
        getPos = [Rb]( float par, vec3f& pos )
        {
            pos.x = Rb*cosf( par );
            pos.y = Rb*sinf( par );
            pos.z = 0.0f;
            return;
        };

        curveVec.push_back( persCurve( fin, getPos ) );
    }

    return true;
}

bool lvl_terrainPerspective::init_curveChain( const char* fileName )
{
    std::ifstream fin( fileName );
    if( !fin ) return false;

    unsigned int Ncurves; fin >> Ncurves;
    if( Ncurves < 1 ) return false;

    curveVec.reserve(Ncurves);
    float Rb; fin >> Rb;
    std::function<void(float,vec3f&)> getPos = [Rb]( float par, vec3f& pos )
    {
        pos.x = Rb*cosf( par );
    //    pos.y = Rb*sinf( par );
    //    pos.z = 0.0f;
        pos.z = Rb*sinf( par );
        pos.y = 0.0f;
        return;
    };
    curveVec.push_back( persCurve( fin, getPos ) );// 1st one

    sf::Color color;
    unsigned int Npts, rd, gn, bu;
    float parMin, parMax;
    for( unsigned int n = 1; n < Ncurves; ++n )
    {
        fin >> Rb >> parMin >> parMax >> Npts;
        fin >> rd >> gn >> bu;
        getPos = [Rb]( float par, vec3f& pos )
        {
            pos.x = Rb*cosf( par );
            //    pos.y = Rb*sinf( par );
            //    pos.z = 0.0f;
            pos.z = Rb*sinf( par );
            pos.y = 0.0f;
            return;
        };

        curveVec.push_back( persCurve( curveVec.back(), sf::Color(rd,gn,bu), parMin, parMax, Npts, getPos ) );
    }

    return true;
}

void lvl_terrainPerspective::rotate_curveChain( char oper, float dAngle )
{
    if( curveVec.empty() ) return;

    persCurve& PC0 = curveVec[0];

    switch( oper )
    {
    case 'Y' :
        vec3f::yaw( dAngle, PC0.Xup, PC0.Yup, PC0.Zup );
        for( unsigned int j = 1; j < curveVec.size(); ++j )
        {
            vec3f dPos = curveVec[j].pos - PC0.pos;
            dPos = dPos.rotate_axis( PC0.Yup, dAngle );
            curveVec[j].pos = PC0.pos + dPos;
        }
        break;
    case 'P' :
        vec3f::pitch( dAngle, PC0.Xup, PC0.Yup, PC0.Zup );
        for( unsigned int j = 1; j < curveVec.size(); ++j )
        {
            vec3f dPos = curveVec[j].pos - PC0.pos;
            dPos = dPos.rotate_axis( PC0.Xup, dAngle );
            curveVec[j].pos = PC0.pos + dPos;
        }
        break;
    case 'R' :
        vec3f::roll( dAngle, PC0.Xup, PC0.Yup, PC0.Zup );
        for( unsigned int j = 1; j < curveVec.size(); ++j )
        {
            vec3f dPos = curveVec[j].pos - PC0.pos;
            dPos = dPos.rotate_axis( PC0.Zup, -1.0f*dAngle );
            curveVec[j].pos = PC0.pos + dPos;
        }
        break;
    }

    // all have equal basis
    PC0.update(0.0f);
    for( unsigned int j = 1; j < curveVec.size(); ++j )
    {
        curveVec[j].Xup = PC0.Xup;
        curveVec[j].Yup = PC0.Yup;
        curveVec[j].Zup = PC0.Zup;
        curveVec[j].update(0.0f);
    }
}
*/
