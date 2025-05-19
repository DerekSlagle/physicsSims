#include "LvlQM.h"

void LvlQM::init( std::istream& is )
{
    // messages
    unsigned int fontSz; is >> fontSz;
    unsigned int rd, gn, bu;
    is >> rd >> gn >> bu;
    sf::Color sfcColor(rd,gn,bu,255);
    is >> rd >> gn >> bu;
    sf::Color txtColor(rd,gn,bu,255);
    float xPos1, xPos2, yPos, dyPos;
    is >> xPos1 >> xPos2 >> yPos >> dyPos;
    float sfcWidth; is >> sfcWidth;

    sf::Text Temp("Temp", *button::pFont, fontSz);
    Temp.setFillColor( txtColor );
    massMsg = massNumMsg = Temp;
    massMsg.setPosition( xPos1, yPos ); massNumMsg.setPosition( xPos2, yPos );
    massMsg.setString("m = proton mass x");
    energyMsg = energyNumMsg = Temp;
    energyMsg.setPosition( xPos1, yPos + dyPos ); energyNumMsg.setPosition( xPos2, yPos + dyPos );
    energyMsg.setString("E in electron volts:");

    xSqExpectMsg = xSqExpectNumMsg = Temp;
    xSqExpectMsg.setPosition( xPos1, yPos + 2.0*dyPos ); xSqExpectNumMsg.setPosition( xPos2, yPos + 2.0*dyPos );
    xSqExpectMsg.setString("x^2 expected :");
    pSqExpectMsg = pSqExpectNumMsg = Temp;
    pSqExpectMsg.setPosition( xPos1, yPos + 3.0*dyPos ); pSqExpectNumMsg.setPosition( xPos2, yPos + 3.0*dyPos );
    pSqExpectMsg.setString("p^2 expected :");
    uncertMsg = uncertNumMsg = Temp;
    uncertMsg.setPosition( xPos1, yPos + 4.0*dyPos ); uncertNumMsg.setPosition( xPos2, yPos + 4.0*dyPos );
    uncertMsg.setString("uncertainty :");
    areaMsg = areaNumMsg = Temp;
    areaMsg.setPosition( xPos1, yPos + 5.0*dyPos ); areaNumMsg.setPosition( xPos2, yPos + 5.0*dyPos );
    areaMsg.setString("Area under wf^2 :");
    shadeAreaMsg = shadeAreaNumMsg = Temp;
    shadeAreaMsg.setPosition( xPos1, yPos + 6.0*dyPos ); shadeAreaNumMsg.setPosition( xPos2, yPos + 6.0*dyPos );
    shadeAreaMsg.setString("Shaded area :");

    xCoordMsg = Temp;
    xCoordMsg.setFillColor( sf::Color::Blue );

    // surface
    msgSurface[0].position.x = xPos1 - 10.0f;// up left
    msgSurface[0].position.y = yPos - 10.0f;
    msgSurface[1].position.x = msgSurface[0].position.x + sfcWidth;// up right
    msgSurface[1].position.y = msgSurface[0].position.y;
    msgSurface[2].position.x = msgSurface[1].position.x;// down right
    msgSurface[2].position.y = msgSurface[0].position.y + 9.0*dyPos + 20.0f;
    msgSurface[3].position.x = msgSurface[0].position.x;// down left
    msgSurface[3].position.y = msgSurface[2].position.y;
    for( size_t i = 0; i < 4; ++i ) msgSurface[i].color = sfcColor;
}

void LvlQM::update( float dt )
{
    if(  button::pButtMse ) return;// mouse is over a button

    showCoord = false;
    float delY = button::mseY - energyLine[0].position.y;
    if( delY < 5.0f && delY > -5.0f )// show coord
    {
        showCoord = true;
        float delX = button::mseX - plot_wf.originPos.x;
        delX /= plot_wf.tScale;
        to_SF_string( xCoordMsg, delX );
    }

    return;
}

void LvlQM::draw( sf::RenderTarget& RT ) const
{
    RT.draw( energyLine, 2, sf::Lines );
    plot_v.draw(RT);
    plot_wf.draw(RT);
    if( showCoord ) RT.draw( xCoordMsg );
    RT.draw( msgSurface, 4, sf::Quads );
    RT.draw( massMsg ); RT.draw( massNumMsg );
    RT.draw( energyMsg ); RT.draw( energyNumMsg );
    RT.draw( areaMsg ); RT.draw( areaNumMsg );
    RT.draw( xSqExpectMsg ); RT.draw( xSqExpectNumMsg );
    RT.draw( pSqExpectMsg ); RT.draw( pSqExpectNumMsg );
    RT.draw( uncertMsg ); RT.draw( uncertNumMsg );
    if( shadeSurf.homeButt.sel ) { RT.draw( shadeAreaMsg ); RT.draw( shadeAreaNumMsg ); }
}

void LvlQM::makeShade()
{
    shadeVec.clear();
    float y0g = ( plot_wf.originPos.y - energyLine[0].position.y )*plot_wf.tScale;
    float area = plot_wf.makeShadedArea( shadeCenter - shadeWidth/2.0, shadeCenter + shadeWidth/2.0, shadeVec, sf::Color(200,0,0,100), y0g );
    to_SF_string( shadeAreaNumMsg, area );
}

bool LvlQM::initShadeControl( std::istream& is )
{
    shadeSurf.init( "shade", is );
    shadeSurf.drawAll = [this](sf::RenderTarget& RT){ if( shadeSurf.homeButt.sel ) RT.draw( &(shadeVec[0]), shadeVec.size(), sf::Quads ); };
    shadeMs.init( &shadeStrip, is );
    shadeSurf.pButtVec.push_back( &shadeMs );
    if( shadeMs.sdataVec.size() != 2 ) { std::cout << "\nbad shadeMs data"; return false; }
    shadeSurf.homeButt.pHitFunc = [this]() { makeShade(); };// draw on opening
    stripData* pSD = &shadeMs.sdataVec.front();
    // shadeCenter
    pSD->xMin = -plot_wf.tMax;
    pSD->xMax = plot_wf.tMax;
    pSD->pSetFunc = [this](float x)
    {
        shadeCenter = x;
        makeShade();
    };
    shadeStrip.reInit( *pSD );
    shadeCenter = pSD->xCurr;
    // shadeWidth
    (++pSD)->xMin = 1.0;
    pSD->xMax = plot_wf.tMax;
    pSD->pSetFunc = [this](float x)
    {
        shadeWidth = x;
        makeShade();
    };
    shadeWidth = pSD->xCurr;

    return true;
}

void LvlQM::initSelectorControls( std::istream& is )
{
    // set energy level
    float xPos, yPos, W, H;
    is >> xPos >> yPos >> W >> H;
    unsigned int nMax = 5; is >> nMax;
    sf::Text Label("energy", *button::pFont, 12);
    Label.setFillColor( sf::Color::White );
    Label.setString("energy");
    energySelector.init( xPos, yPos, W, H, 0, nMax, 0, [this](){ makePlot_wf(); if( shadeSurf.homeButt.sel ) makeShade(); }, Label );
    energySelector.title.setFillColor( sf::Color::White );

    // grapf wf or wf^2
    is >> xPos >> yPos >> W >> H;
    Label.setString("      graph");
    wf_wfSqSelector.init( xPos, yPos, W, H, 2, Label, 0 );// initially odd
    float offX, offY; is >> offX >> offY;
    std::vector<std::string> labelVec;
    labelVec.push_back("wf"); labelVec.push_back("wf^2");
    wf_wfSqSelector.setButtLabels( offX, offY, labelVec, 12, sf::Color::White );
    wf_wfSqSelector.pFuncIdx = [this]( size_t idx ) { makePlot_wf(); if( shadeSurf.homeButt.sel ) makeShade(); };
}
