#include "lvl_JustDraw.h"

bool lvl_JustDraw::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );


    std::ifstream fin("include/levels/lvl_JustDraw/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    unsigned int rd, gn, bu;
    fin >> rd >> gn >> bu;
    Level::clearColor = sf::Color(rd,gn,bu);
    button::setHoverBoxColor( Level::clearColor );

    sf::Vector2f pos, sz, dPos;
    fin >> pos.x >> pos.y >> sz.x >> sz.y >> dPos.x >> dPos.y;
    size_t numSwatches; fin >> numSwatches;

    swatchVec.resize( 4*numSwatches );
    for( size_t j = 0; j < numSwatches; ++j )
    {
        fin >> rd >> gn >> bu;
        sf::Vertex temp;
        temp.color = sf::Color(rd,gn,bu);
        temp.position = pos;
        swatchVec.push_back( temp );// up left
        temp.position.x += sz.x;// up right
        swatchVec.push_back( temp );
        temp.position.y += sz.y;// dn right
        swatchVec.push_back( temp );
        temp.position.x -= sz.x;// dn left
        swatchVec.push_back( temp );
        pos += dPos;
    }

    //
    selColorMsg.setFont( *button::pFont );
    selColorMsg.setString("COLOR");
    selColorMsg.setCharacterSize(20);
    selColorMsg.setPosition( pos );
    selColor = sf::Color::White;
    selColorMsg.setFillColor( selColor );

    float Rlt = 8.0f;
    hitLight.setRadius(Rlt);
    hitLight.setOrigin(Rlt,Rlt);
    hitLight.setFillColor( sf::Color::Blue );
    sf::Vector2f hitLightPos( 700.0f, 20.0f );
    hitLight.setPosition( hitLightPos );

    bool usingRendTxt; fin >> usingRendTxt;
    bool usingCircle; fin >> usingCircle;

    fin.close();

    if( usingRendTxt )
    {
        fin.open("include/levels/lvl_JustDraw/initRendTxt_data.txt");
        if( !fin ) { std::cout << "\nNo rendTxt_init data"; return false; }
        initRendTxt( fin );
        fin.close();

        fin.open("include/levels/lvl_JustDraw/control_data.txt");
        if( !fin ) { std::cout << "\nNo control_data"; return false; }
        init_viewCS( fin );
        fin.close();
    }

    if( !spriteSheet::loadSpriteSheets( SSvec, "include/levels/lvl_JustDraw/SSfileList.txt" ) )
        { std::cout << "\nNo SS data"; return false; }

    if( usingCircle )
    {
        fin.open("include/levels/lvl_JustDraw/circle_data.txt");
        if( !fin ) { std::cout << "\nNo circle data"; return false; }
        initCircle( fin );
        init_circleControls( fin );
        fin.close();
    }

    return true;
}

bool lvl_JustDraw::initCircle( std::istream& is )
{
    is >> circleR >> circlePos.x >> circlePos.y >> circlePos.z;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    is >> numPoints;
    is >> angPol >> angAz;// in degrees
    angPol *= vec2f::PI/180.0f;
    angAz *= vec2f::PI/180.0f;
    is >> axisLength >> tipL >> tipW;
    is >> rotSpeed >> Kz;

    vtxVec.resize( numPoints + 2 );
//    for( sf::Vertex& V : vtxVec ) V.color = sf::Color::Magenta;
    for( sf::Vertex& V : vtxVec ) V.color = sf::Color(rd,gn,bu);
    perimVec.resize( numPoints + 1 );
    for( sf::Vertex& V : perimVec ) V.color = sf::Color::Blue;
    axesVec.resize(18);
    for( sf::Vertex& V : axesVec ) V.color = sf::Color::Black;
    constructAxes();
    if( !SSvec.empty() ) pSS_dime = &SSvec.front();

    assignTexCoords( Zup.z > 0.0f );
    graphCircle();

    return true;
}

void lvl_JustDraw::constructAxes()// from angPol and angAz
{
    // construct Zup
    vec3f Zu_p;
    Zu_p.z = cosf( angPol );
    Zu_p.x = sinf( angPol )*sinf( angAz );// angle from vertical = Yu
    Zu_p.y = sinf( angPol )*cosf( angAz );
    constructAxes( Zu_p );
}

void lvl_JustDraw::constructAxes( vec3f Zu_p )// from givenZup
{
    // construct primed frame
    Zup = Zu_p;
    Zup /= Zup.mag();

    Xup = Yup.cross( Zup );
 //   Xup = Zup.cross( Yup );

    Xup /= Xup.mag();

 //   Yup = Xup.cross( Zup );
    Yup = Zup.cross( Xup );

    // primed axes
    for( sf::Vertex& V : axesVec )
    {
        V.position.x = circlePos.x;// origin ends
        V.position.y = Level::winH - circlePos.y;
    }

    sf::Vector2f xyProj;
    xyProj.x = axisLength*Xup.dot(Xu);
    xyProj.y = -axisLength*Xup.dot(Yu);
    axesVec[0].position -= xyProj;// x prime axis
    axesVec[1].position += xyProj;
    axesVec[2].position = axesVec[3].position = axesVec[4].position = axesVec[5].position = axesVec[1].position;
    vec3f tipOfst = tipW*Yup - tipL*Xup;
    axesVec[3].position.x += tipOfst.dot(Xu);
    axesVec[3].position.y -= tipOfst.dot(Yu);
    tipOfst -= 2.0f*tipW*Yup;
    axesVec[5].position.x += tipOfst.dot(Xu);
    axesVec[5].position.y -= tipOfst.dot(Yu);

    xyProj.x = axisLength*Yup.dot(Xu);
    xyProj.y = -axisLength*Yup.dot(Yu);
    axesVec[6].position -= xyProj;// y prime axis
    axesVec[7].position += xyProj;
    axesVec[8].position = axesVec[9].position = axesVec[10].position = axesVec[11].position = axesVec[6].position;
    tipOfst = tipW*Xup + tipL*Yup;
    axesVec[9].position.x += tipOfst.dot(Xu);
    axesVec[9].position.y -= tipOfst.dot(Yu);
    tipOfst -= 2.0f*tipW*Xup;
    axesVec[11].position.x += tipOfst.dot(Xu);
    axesVec[11].position.y -= tipOfst.dot(Yu);

    xyProj.x = axisLength*Zup.dot(Xu);
    xyProj.y = -axisLength*Zup.dot(Yu);
    axesVec[12].position += xyProj;// z prime axis
 //   axesVec[13].position += xyProj;// extend z axis to -axisLength
    axesVec[14].position = axesVec[15].position = axesVec[16].position = axesVec[17].position = axesVec[12].position;
    tipOfst = tipW*Xup - tipL*Zup;
    axesVec[15].position.x += tipOfst.dot(Xu);
    axesVec[15].position.y -= tipOfst.dot(Yu);
    tipOfst -= 2.0f*tipW*Xup;
    axesVec[17].position.x += tipOfst.dot(Xu);
    axesVec[17].position.y -= tipOfst.dot(Yu);

}

void lvl_JustDraw::graphCircle()
{
    float dAngle = 2.0f*vec2f::PI/ numPoints;
    vtxVec[0].position.x = circlePos.x;
    vtxVec[0].position.y = Level::winH - circlePos.y;
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        float angle = j*dAngle;
        vec3f rp = circleR*( cosf( angle )*Xup + sinf( angle )*Yup );
        vtxVec[j+1].position.x = circlePos.x + rp.dot( Xu );
        vtxVec[j+1].position.y = Level::winH - ( circlePos.y + rp.dot( Yu ) );// y is up
        perimVec[j].position = vtxVec[j+1].position;
    }

    vtxVec[ numPoints + 1 ].position = vtxVec[1].position;// closing fan
    perimVec[ numPoints ].position = vtxVec[1].position;
}

void lvl_JustDraw::assignTexCoords( bool facingZ )
{
    if( !pSS_dime ) return;

    size_t frIdx = ( facingZ ) ? 0 : 1;
    sf::IntRect iRect = pSS_dime->getFrRect( frIdx, 0 );
    sf::Vector2f dimeCenter( iRect.left + 0.5f*iRect.width, iRect.top + 0.5f*iRect.height );
    float txtRadius = 0.5f*iRect.width;

    float dAngle = 2.0f*vec2f::PI/ numPoints;
    vtxVec[0].texCoords = dimeCenter;
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        float angle = j*dAngle;
    //    float angle = facingZ ? j*dAngle : 2.0f*vec2f::PI - j*dAngle;

  //      vtxVec[j+1].texCoords.x = dimeCenter.x + txtRadius*cosf( angle );
        vtxVec[j+1].texCoords.x = dimeCenter.x;
        vtxVec[j+1].texCoords.x += facingZ ? txtRadius*cosf( angle ) : -txtRadius*cosf( angle );


        vtxVec[j+1].texCoords.y = dimeCenter.y - txtRadius*sinf( angle );
    }

    vtxVec[ numPoints + 1 ].texCoords = vtxVec[1].texCoords;// closing fan
}

void lvl_JustDraw::drawCircle( sf::RenderTarget& RT ) const// and axes
{
    const size_t Vsz = vtxVec.size();

    if( Zup.z >= 0.0f ) RT.draw( &axesVec[12], 6, sf::Lines );// z prime axis pointing away

    if( Vsz > 2 )
    {
        if( pSS_dime )RT.draw( &( vtxVec[0] ), Vsz, sf::TriangleFan, &(pSS_dime->txt) );
        else RT.draw( &( vtxVec[0] ), Vsz, sf::TriangleFan );
    }


    RT.draw( &( perimVec[0] ), perimVec.size(), sf::LinesStrip );
    RT.draw( &axesVec[0], 12, sf::Lines );// x, y prime axes
    if( Zup.z < 0.0f ) RT.draw( &axesVec[12], 6, sf::Lines );// z prime axis pointing towards
}

bool lvl_JustDraw::init_circleControls( std::istream& is )
{
    float R, r, posX, posY; is >> R >> r >> posX >> posY;

    circleJoy.init( R, r, posX, posY );
    circleJoy.pFunc_ff = [this](float x, float y)
    {
     //   float Sxy = 0.5f*vec2f::PI/circleJoy.r_ring;
     //   float r = sqrtf( x*x + y*y );
     //   angPol = Sxy*r;
      //  angAz = atan2f( x, -y );

        vec3f zup( x/circleJoy.r_ring, -y/circleJoy.r_ring, 0.0f );
        float xySq = zup.x*zup.x + zup.y*zup.y;
        if( xySq < 1.0f  ) zup.z = sqrtf( 1.0f - xySq );
        zup /= zup.mag();
        constructAxes( zup );
    //    graphCircle();
    };
    button::RegisteredButtVec.push_back( &circleJoy );

    return true;
}

bool lvl_JustDraw::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_JustDraw::update( float dt )
{
    rotAngle += rotSpeed*dt;
    if( rotSpeed > 0.0f && rotAngle > 2.0f*vec2f::PI ) rotAngle -= 2.0f*vec2f::PI;
    if( rotSpeed < 0.0f && rotAngle < -2.0f*vec2f::PI ) rotAngle += 2.0f*vec2f::PI;

    static bool circleJoy_atRestLast = false;
    // rotate about Yp axis
    if( circleJoy.atRest )
    {
        vec3f Zup_last = Zup;
    //    Zup = Zup_last.rotate_axis( Xup, 0.1f*rotSpeed*dt );
        Zup = Zup.rotate_axis( Yu, rotSpeed*dt );

    //    Yup = Yup.rotate_axis( Xup, 0.1f*rotSpeed*dt );
        Yup = Yup.rotate_axis( Yu, rotSpeed*dt );

        constructAxes( Zup );
   //     Xup = Yup.cross( Zup );

        if( Zup_last.z >= 0.0f && Zup.z < 0.0f )
        {
            assignTexCoords( false );
        }
        else if( Zup_last.z <= 0.0f && Zup.z > 0.0f ) assignTexCoords( true );
    }
    else if( circleJoy_atRestLast ) assignTexCoords( true );// following control pickup

    circleJoy_atRestLast = circleJoy.atRest;

    graphCircle();

    if( button::pButtMse ) return;
    sf::Vector2f mp = button::msePos();

    if( hitPict() )
    {
        hitLight.setFillColor( sf::Color::Magenta );

        if( hitPixel() ) hitLight.setFillColor( sf::Color::Green );
     //   else hitLight.setFillColor( sf::Color::Blue );
    }
    else
    {
        hitLight.setFillColor( sf::Color::Blue );
        if( button::clickEvent_Lt() == 1 )
        {

            // hit test color swatches
            for( size_t j = 0; j < swatchVec.size(); j += 4 )
            {
                if( mp.x < swatchVec[j].position.x ) continue;// left of
                if( mp.x > swatchVec[j+1].position.x ) continue;// right of
                if( mp.y < swatchVec[j].position.y ) continue;// above
                if( mp.y > swatchVec[j+2].position.y ) continue;// below
                // it's a hit!
                selColor = swatchVec[j].color;
                selColorMsg.setFillColor( selColor );
                for( sf::Vertex& V : vtxVec ) V.color = selColor;
            //    std::cout << "\nHit!";
                return;
            }
        }

    }// end if missed pict area
}

void lvl_JustDraw::draw( sf::RenderTarget& RT ) const
{
    RT.draw( &( swatchVec[0] ), swatchVec.size(), sf::Quads );
    RT.draw( selColorMsg );
    if( pRendTxt ) RT.draw( &pictQuad[0], 4, sf::Quads, pPictTxt );
    RT.draw( hitLight );

    drawCircle(RT);

//    const size_t Vsz = vtxVec.size();
//    if( Vsz > 2 ) RT.draw( &( vtxVec[0] ), Vsz, sf::TriangleFan );
//    if( Vsz > 2 ) RT.draw( &( vtxVec[0] ), Vsz, sf::TriangleStrip );


}

void lvl_JustDraw::updateRendTxt()
{
    if( pRendTxt )
    {
        pRendTxt->setView( sf::View( viewPos_curr, viewSz_curr ) );

        pRendTxt->clear( pictClearColor );

        pRendTxt->draw( rectA );
        pRendTxt->draw( inMsg );

        pRendTxt->display();

     //   rendImg = pPictTxt->copyToImage();
    }
}

bool lvl_JustDraw::initRendTxt( std::istream& is )
{
    if( !is ) return false;

    // prepare the canvas
    unsigned int imgW, imgH; is >> imgW >> imgH;
    txtImgSz.x = viewSz_curr.x = (float)imgW;
    txtImgSz.y = viewSz_curr.y = (float)imgH;
    viewPos_curr.x = 0.5f*viewSz_curr.x;
    viewPos_curr.y = 0.5f*viewSz_curr.y;
    pRendTxt = new sf::RenderTexture;
    pRendTxt->create( imgW, imgH );
    rendImg.create( imgW, imgH );

    pPictTxt = &( pRendTxt->getTexture() );
    sf::View defView = pRendTxt->getDefaultView();
    sf::Vector2f vwPos = defView.getCenter(), vwSz = defView.getSize();
    std::cout << "\n vwCtr = " << vwPos.x << ", " << vwPos.y;
    std::cout << "    vwSz = " << vwSz.x << ", " << vwSz.y;

    for( unsigned int n = 0; n < 4; ++n ) pictQuad[n].color = sf::Color::White;
    pictQuad[0].texCoords = sf::Vector2f( 0.0f, 0.0f );// up left
    pictQuad[1].texCoords = sf::Vector2f( imgW, 0.0f );// up right
    pictQuad[2].texCoords = sf::Vector2f( imgW, imgH );// dn right
    pictQuad[3].texCoords = sf::Vector2f( 0.0f, imgH );// dn left

  //  sf::RectangleShape rectA;
    sf::Vector2f rectPos, rectSz; is >> rectPos.x >> rectPos.y >> rectSz.x >> rectSz.y;
    rectA.setPosition( rectPos );
    rectA.setSize( rectSz );
    unsigned int rd, gn, bu, ap;
    is >> rd >> gn >> bu >> ap;
    rectA.setFillColor( sf::Color(rd,gn,bu,ap) );

  //  sf::Text inMsg;
    inMsg.setFont( *button::pFont );
    is >> rectPos.x >> rectPos.y;
    inMsg.setPosition( rectPos );
    unsigned int charSz; is >> charSz;
    inMsg.setCharacterSize( 20 );
    is >> rd >> gn >> bu >> ap;
    inMsg.setFillColor( sf::Color(rd,gn,bu,ap) );

    inMsg.setString("SB\nParking\nZone");

    is >> rd >> gn >> bu >> ap;
    pictClearColor = sf::Color(rd,gn,bu,ap);
//    viewPos_curr.x = viewPos_curr.y = 0.0f;
    pRendTxt->setView( sf::View( viewPos_curr, viewSz_curr ) );

    pRendTxt->clear( pictClearColor );

    pRendTxt->draw( rectA );
    pRendTxt->draw( inMsg );

    pRendTxt->display();

    rendImg = pPictTxt->copyToImage();

    sf::Vector2f pictPos, pictSz; is >> pictPos.x >> pictPos.y >> pictSz.x >> pictSz.y;

 //   sf::View pictView( sf::Vector2f( 0.0f, 0.0f ), pictSz );// center pos, size
 //   pRendTxt->setView( pictView );

    pictQuad[0].position = pictPos;// up left
    pictQuad[1].position = pictPos + sf::Vector2f( pictSz.x, 0.0f );// up right
    pictQuad[2].position = pictPos + pictSz;// dn right
    pictQuad[3].position = pictPos + sf::Vector2f( 0.0f, pictSz.y );// dn right

    pRendTxt->setView( sf::View( viewPos_curr, viewSz_curr ) );

    return true;
}

bool lvl_JustDraw::init_viewCS( std::istream& is )
{

 //   sf::Text Label;
 //   Label.setFont( *button::pFont );
  //  Label.setCharacterSize(12);
  //  Label.setFillColor( sf::Color::Black );
    sf::Vector2f HBpos, HBsz;
    sf::Vector2f sfcOfst;

    is >> HBpos.x >> HBpos.y >> HBsz.x >> HBsz.y >> sfcOfst.x >> sfcOfst.y;
    std::string inStr; is >> inStr;// label for home button
    sf::Vector2f PosSurf( HBpos + sfcOfst );
    sf::Vector2f SzSurf; is >> SzSurf.x >> SzSurf.y;
    viewCS.init( HBpos, PosSurf, SzSurf, inStr.c_str(), HBsz.x, HBsz.y );
    viewCS.ownButts = true;
    button::RegisteredButtVec.push_back( &viewCS );

    sf::Vector2f Pos; is >> Pos.x >> Pos.y;// as offset from PosSurf
    Pos += PosSurf;
    buttonValOnHit* pStrip = new buttonValOnHit();
    multiSelector* pMS = new multiSelector( Pos, pStrip, is );
    pMS->ownsStrip = true;
    viewCS.pButtVec.push_back( pMS );

    // the lambdas for each button
    if( pMS->sdataVec.size() != 4 ) { std::cout << "\nbad multiSel data"; return false; }
    stripData* pSD = &( pMS->sdataVec.front() );
    // posX
    pSD->pSetFunc =     [this](float x){ viewPos_curr.x = x; updateRendTxt(); };
    viewPos_curr.x = pSD->xCurr;
    pStrip->reInit( *pSD );
    // posY
    (++pSD)->pSetFunc = [this](float x){ viewPos_curr.y = x; updateRendTxt(); };
    viewPos_curr.y = pSD->xInit;// posY
    // SzX
    (++pSD)->pSetFunc = [this](float x){ viewSz_curr.x = x; updateRendTxt(); };
    viewSz_curr.x = pSD->xInit;
    // SzY
    (++pSD)->pSetFunc = [this](float x){ viewSz_curr.y = x; updateRendTxt(); };
    viewSz_curr.y = pSD->xInit;

    std::string checkStr; is >> checkStr;
    std::cout << '\n' << checkStr;

    return true;
}

bool lvl_JustDraw::hitPict()const// true if mouse over pictClearColor
{
    if( !pRendTxt ) return false;

    sf::Vector2f hitPos = button::msePos();
    if( hitPos.x < pictQuad[0].position.x ) return false;
    if( hitPos.y < pictQuad[0].position.y ) return false;
    if( hitPos.x > pictQuad[2].position.x ) return false;
    if( hitPos.y > pictQuad[2].position.y ) return false;

    return true;
}

bool lvl_JustDraw::hitPixel()const// true if mouse over pictClearColor
{
    if( !pRendTxt ) return false;
    // hit rect?
    if( !hitPict() ) return false;
    sf::Vector2f hitPos = button::msePos();
    hitPos -= pictQuad[0].position;


    // from viewSz. Works only if pictQuad[0].texCoords = (0,0) ?
//    hitPos.x *= viewSz_curr.x/( pictQuad[1].position.x - pictQuad[0].position.x );
//    hitPos.y *= viewSz_curr.y/( pictQuad[3].position.y - pictQuad[0].position.y );// now 0 to txtImgSz

    // from viewSz. Works for general texCoords assignment?
    hitPos.x *= ( pictQuad[1].texCoords.x - pictQuad[0].texCoords.x )/( pictQuad[1].position.x - pictQuad[0].position.x );
    hitPos.y *= ( pictQuad[3].texCoords.y - pictQuad[0].texCoords.y )/( pictQuad[3].position.y - pictQuad[0].position.y );// now 0 to txtImgSz

    hitPos.x += viewPos_curr.x - 0.5f*txtImgSz.x;// correct for shift of view center. working
    hitPos.y += viewPos_curr.y - 0.5f*txtImgSz.y;// correct for shift of view center. working

    bool hitPx = true;// if hit missed the texture
    if( hitPos.x > 0.0f && hitPos.x < txtImgSz.x && hitPos.y > 0.0f && hitPos.y < txtImgSz.y )
        hitPx = ( pictClearColor == rendImg.getPixel( (unsigned int)hitPos.x, (unsigned int)hitPos.y ) );

    return hitPx;


    // check values
 //   if( button::clickEvent_Lt() == 1 )
 //   {
  //      std::cerr << "\n pxCoord = " << pxCoord.x << ", " << pxCoord.y;
  //      std::cerr << "  pxColor = " << (unsigned int)pxColor.r << ", " << (unsigned int)pxColor.g << ", " << (unsigned int)pxColor.b;
  //  }
}
