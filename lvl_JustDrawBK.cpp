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
    bool usingBezier; fin >> usingBezier;
    bool usingTriStrip; fin >> usingTriStrip;

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

    if( usingBezier )
    {
        fin.open("include/levels/lvl_JustDraw/Bezier_data.txt");
        if( !fin ) { std::cout << "\nNo circle data"; return false; }
        init_Bezier( fin );
     //   init_circleControls( fin );
        fin.close();
    }

    if( usingTriStrip )
    {
        fin.open("include/levels/lvl_JustDraw/triStrip_data.txt");
        if( !fin ) { std::cout << "\nNo triStrip data"; return false; }
        init_triStrip( fin );
        fin.close();
    }

    return true;
}

void lvl_JustDraw::init_Bezier( std::istream& is )
{
    is >> bezOrigin.x >> bezOrigin.y;
    is >> bezScaleX >> bezScaleY;
    is >> bezK1 >> bezK2 >> bezK3 >> bezK4;
    is >> numPoints;
    vtxVec.resize( numPoints );
    bezVtxVec2.resize( numPoints );
    for( sf::Vertex& V : vtxVec ) V.color = sf::Color::Black;
    for( sf::Vertex& V : bezVtxVec2 ) V.color = sf::Color::Red;

    // axes
    bezAxes[0].color = bezAxes[1].color = bezAxes[2].color = bezAxes[3].color = sf::Color::Black;
    bezAxes[4].color = bezAxes[5].color = bezAxes[6].color = bezAxes[7].color = sf::Color(0,0,0,80);

    bezK1 = 0.0f;
    bezK2 = 3.0f + bezK4;
    bezK3 = -2.0f*( 1.0f + bezK4 );
    plotBezCurve();
    plotBezAxes();
}

void lvl_JustDraw::plotBezCurve()
{
    float du = 1.0f/( numPoints - 1 );
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        float u = j*du, uSq = u*u;
        float y = bezK1*u + bezK2*uSq + bezK3*uSq*u + bezK4*uSq*uSq;
        float yp = bezK1 + 2.0f*bezK2*u + 3.0f*bezK3*uSq + 4.0f*bezK4*uSq*u;

        if( j < vtxVec.size() )
        {
            bezVtxVec2[ j ].position.x = vtxVec[ j ].position.x = bezOrigin.x + bezScaleX*u;
            vtxVec[ j ].position.y = bezOrigin.y - bezScaleY*y;
            bezVtxVec2[ j ].position.y = bezOrigin.y - bezScaleY*yp;
        }
    }
}

void lvl_JustDraw::plotBezAxes()
{
    bezAxes[0].position.x = bezOrigin.x - 0.2f*bezScaleX;// x axis
    bezAxes[1].position.x = bezOrigin.x + 1.4f*bezScaleX;
    bezAxes[0].position.y = bezAxes[1].position.y = bezOrigin.y;
    //
    bezAxes[4].position.x = bezOrigin.x - 0.1f*bezScaleX;// x axis
    bezAxes[5].position.x = bezOrigin.x + 1.0f*bezScaleX;
    bezAxes[4].position.y = bezAxes[5].position.y = bezOrigin.y - 1.0f*bezScaleY;

    bezAxes[2].position.y = bezOrigin.y + 0.2f*bezScaleY;// y axis
    bezAxes[3].position.y = bezOrigin.y - 1.4f*bezScaleY;
    bezAxes[2].position.x = bezAxes[3].position.x = bezOrigin.x;
    //
    bezAxes[6].position.y = bezOrigin.y + 0.1f*bezScaleY;// y axis
    bezAxes[7].position.y = bezOrigin.y - 1.0f*bezScaleY;
    bezAxes[6].position.x = bezAxes[7].position.x = bezOrigin.x + 1.0f*bezScaleX;
}

bool lvl_JustDraw::init_triStrip( std::istream& is )
{
    unsigned int SSidx, SetNum, FrIdx;
    is >> SSidx >> SetNum >> FrIdx;
    if( SSidx >= SSvec.size() || SetNum >= SSvec[SSidx].numSets() ) return false;
    if( FrIdx >= SSvec[SSidx].getFrCount(SetNum) ) return false;
    pTriStripTxt = &( SSvec[SSidx].txt );

    sf::IntRect frRect = SSvec[ SSidx ].getFrRect( FrIdx, SetNum, false );
    unsigned int numCells; is >> numCells;
    sf::Vector2f pos0, Sz;
    is >> pos0.x >> pos0.y >> Sz.x >> Sz.y;

    sf::Vertex vtx;
    vtx.color = sf::Color::White;
    // define texture corners
    sf::Vector2f tcUpLt( frRect.left, frRect.top );
    sf::Vector2f tcDnLt( frRect.left, frRect.top + frRect.height );

    sf::Vector2f tcUpRt( frRect.left + frRect.width, frRect.top );
    sf::Vector2f tcDnRt( frRect.left + frRect.width, frRect.top + frRect.height );

    // 1st cell
    vtx.position = pos0;
    vtx.texCoords = tcUpLt;
    vtxVec2.push_back(vtx);

    vtx.position = pos0;
    vtx.position.y += Sz.y;
    vtx.texCoords = tcDnLt;
    vtxVec2.push_back(vtx);

    vtx.position = pos0;
    vtx.position.x += Sz.x;
    vtx.texCoords = tcUpRt;
    vtxVec2.push_back(vtx);

    vtx.position = pos0;
    vtx.position += Sz;
    vtx.texCoords = tcDnRt;
    vtxVec2.push_back(vtx);

    // then back and forth across the texture
    for( unsigned int j = 1; j < numCells; ++j )
    {
        pos0.x += Sz.x;// to next cell

        if( j%2 )// odd j = texCoords to left
        {
            vtx.position = pos0;
            vtx.position.x += Sz.x;
            vtx.texCoords = tcUpLt;
            vtxVec2.push_back(vtx);

            vtx.position = pos0;
            vtx.position += Sz;
            vtx.texCoords = tcDnLt;
            vtxVec2.push_back(vtx);
        }
        else// texCoords to right
        {
            vtx.position = pos0;
            vtx.position.x += Sz.x;
            vtx.texCoords = tcUpRt;
            vtxVec2.push_back(vtx);

            vtx.position = pos0;
            vtx.position += Sz;
            vtx.texCoords = tcDnRt;
            vtxVec2.push_back(vtx);
        }
    }

    return true;
}

bool lvl_JustDraw::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_JustDraw::update( float dt )
{

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

    if( pTriStripTxt )
        RT.draw( &(vtxVec2[0]), vtxVec2.size(), sf::TrianglesStrip, pTriStripTxt );

    if( vtxVec.size() > 1 ) RT.draw( &( vtxVec[0] ), vtxVec.size(), sf::LinesStrip );
    if( bezVtxVec2.size() > 1 ) RT.draw( &( bezVtxVec2[0] ), bezVtxVec2.size(), sf::LinesStrip );
    RT.draw( bezAxes, 8, sf::Lines );
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
