#include "LvlQM_FSW.h"

// virtual functions
bool LvlQM_FSW::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(200,200,200);
    button::setHoverBoxColor( sf::Color::Black );

    std::ifstream fin("include/levels/LvlQM_FSW/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }
    fin >> rootFindIterLimit;
    fin >> V0scale;

    LvlQM::init(fin);

    fin.close();

    // messages
    sf::Text Temp("Temp", *button::pFont, 14);
    Temp.setFillColor( energyMsg.getFillColor() );
    float xPos1 = energyMsg.getPosition().x;
    float xPos2 = energyNumMsg.getPosition().x;
    float yPos = energyMsg.getPosition().y;
    float dyPos = energyMsg.getPosition().y - massMsg.getPosition().y;
    aMsg = aNumMsg = Temp;
    aMsg.setPosition( xPos1, yPos + 6.0*dyPos ); aNumMsg.setPosition( xPos2, yPos + 6.0*dyPos );
    aMsg.setString("a = Bohr radius x");
    V0Msg = V0NumMsg = Temp;
    V0Msg.setPosition( xPos1, yPos + 7.0*dyPos ); V0NumMsg.setPosition( xPos2, yPos + 7.0*dyPos );
    V0Msg.setString("V0 in electron volts:");

    if( !init_plots() ) return false;
    if( !init_controls() ) return false;
    rootsButt.pHitFunc();// preload roots for default z0

    xCoordMsg.setPosition( plot_wf.originPos.x + 10.0f, plot_wf.originPos.y + 10.0f );

    return true;
}

bool LvlQM_FSW::init_plots()
{
    std::ifstream fin("include/levels/LvlQM_FSW/functionPlot_data.txt");
    if( !fin ) { std::cout << "\nNo functionPlot data"; return false; }

    // common data
    fin >> subInts;

    plot_1.init(fin);
    pFunc_1_odd = [this]( double x ) { return 1.0/tan(x); };
    pFunc_1_even = [this]( double x ) { return tan(x); };
    pFunc_1 = pFunc_1_odd;
    makePlot_1();

    plot_2.init(fin);
    z0 = static_cast<double> (plot_2.tMax);
    pFunc_2_odd = [this]( double x ) { return -1.0*sqrt( (z0/x)*(z0/x) - 1.0 ); };
    pFunc_2_even = [this]( double x ) { return sqrt( (z0/x)*(z0/x) - 1.0 ); };
    pFunc_2 = pFunc_2_odd;
    makePlot_2();

    plot_wf.init(fin);
    plot_wf.axisVtxVec.pop_back();// eliminate x-axis
    plot_wf.axisVtxVec.pop_back();

    fin.close();

    plot_v = plot_wf;
    plot_v.plotVtxVec.resize(6);

    energyLine[0].color = sf::Color::Red;
    energyLine[1].color = sf::Color::Red;

    return true;
}

void LvlQM_FSW::makePlot_v()
{
    sf::Vertex vtx;
    vtx.color = sf::Color::Blue;
    // draw well
    vtx.position.x = plot_v.originPos.x + plot_v.tMax*plot_v.tScale;// right end
    vtx.position.y = plot_v.originPos.y;
    plot_v.plotVtxVec[0] = vtx;
    vtx.position.x = plot_v.originPos.x + a*plot_v.tScale;// up right corner
    vtx.position.y = plot_v.originPos.y;
    plot_v.plotVtxVec[1] = vtx;
    vtx.position.x = plot_v.originPos.x + a*plot_v.tScale;// dn right corner
    vtx.position.y = plot_v.originPos.y + V0*V0scale;
    plot_v.plotVtxVec[2] = vtx;
    vtx.position.x = plot_v.originPos.x - a*plot_v.tScale;// dn left corner
    vtx.position.y = plot_v.originPos.y + V0*V0scale;
    plot_v.plotVtxVec[3] = vtx;
    vtx.position.x = plot_v.originPos.x - a*plot_v.tScale;// up left corner
    vtx.position.y = plot_v.originPos.y;
    plot_v.plotVtxVec[4] = vtx;
    vtx.position.x = plot_v.originPos.x - plot_v.tMax;// left end
    vtx.position.y = plot_v.originPos.y;
    plot_v.plotVtxVec[5] = vtx;
}

void LvlQM_FSW::makePlot_1()
{
    plot_1.plotVtxVec.clear();
    plot_1.tElap = 0.0;
    double xMax = static_cast<double> (plot_1.tMax);
    double dx = xMax/subInts;
    float fValue;
    plot_1.pValue = &fValue;
    for( double x = 0.0; x < xMax; x += dx )
    {
        fValue = static_cast<float>( pFunc_1(x) );
        plot_1.update( static_cast<float>(dx) );
    }
}

void LvlQM_FSW::makePlot_2()
{
    plot_2.plotVtxVec.clear();
    double dx = z0/subInts;
    float fValue;
    plot_2.pValue = &fValue;
    plot_2.tElap = static_cast<float>(dx);
    for( double x = dx; x < z0; x += dx )
    {
        fValue = static_cast<float>( pFunc_2(x) );
        plot_2.update( static_cast<float>(dx) );
    }
}

void LvlQM_FSW::makePlot_wf()
{
    plot_wf.plotVtxVec.clear();
    // find V0
    double y = z0*h_bar/( a*r_Bohr );
    V0 = EVperJoule*y*y/( 2.0*m*Mp );
    to_SF_string( V0NumMsg, V0 );
    to_SF_string( aNumMsg, a );

    const int& rootIdx = energySelector.countValue;

    if( rootIdx + 1 > (int)zVec.size() )
    {
        std::cout << "\nroot #" << rootIdx << " does not exist";
        return;
    }
    else std::cout << "\nrootIdx = " << rootIdx;

    // draw wavefunction
    double z, E;
    z = zVec[rootIdx];
    k = sqrt( z0*z0 - z*z )/a;
    L = z/a;
    E = V0*( (z/z0)*(z/z0) - 1.0 );
    to_SF_string( energyNumMsg, E );
    makePlot_v();
    float dy = E*V0scale;
    energyLine[0].position.x = plot_wf.originPos.x - plot_wf.tMax*plot_wf.tScale;
    energyLine[0].position.y = plot_wf.originPos.y - dy;
    energyLine[1].position.x = plot_wf.originPos.x + plot_wf.tMax*plot_wf.tScale;
    energyLine[1].position.y = plot_wf.originPos.y - dy;

    float dx = 2.0*static_cast<double>( plot_wf.tMax )/subInts;
    float fValue;
    plot_wf.pValue = &fValue;
    plot_wf.tElap = -plot_wf.tMax;

    double Area = 0.0;// under wf^2
    double xSqExpect = 0.0;
    double pSqExpect = 0.0;

    // NEW
    if( odd_evenSelector.selIdx == 0 )
        F = sin(z)*exp(k*a);// odd wave function
    else
        F = cos(z)*exp(k*a);

    for( float x = -plot_wf.tMax; x < plot_wf.tMax; x += dx )
    {
        double fV = Awf*wf(x,0);// 2nd arg value doesn't matter. Dependence is in F
        fValue = static_cast<float>( fV );
        Area += fV*fV*dx;
        xSqExpect += x*x*fV*fV*dx;
        pSqExpect += fV*Awf*d2_wf_dx2(x,0)*dx;
        if( wf_wfSqSelector.selIdx == 1 ) fValue *= 3.0f*fValue/Awf;// amplitude reduced for display
        plot_wf.update(dx);
    }

    for( sf::Vertex& v : plot_wf.plotVtxVec )
        v.position.y -= dy;

    xSqExpect /= Area;
    to_SF_string( xSqExpectNumMsg, xSqExpect );
    pSqExpect *= -h_bar*h_bar/Area;
    to_SF_string( pSqExpectNumMsg, pSqExpect );
    to_SF_string( uncertNumMsg, sqrt(xSqExpect*pSqExpect) );
    Area *= 3.0/Awf;// to compensate for reduced amplitude
    to_SF_string( areaNumMsg, Area );

    // Experiment: integrate out the solution and commpare graphically
    if( integrateButt.sel )
    {
        plot_4Vec.clear();
        double y, yp, ypp;
        // initial conditions for the 2nd order ODE
        if( odd_evenSelector.selIdx == 0 ) { y = 0.0; yp = L*Awf; }// odd function
        else { y = Awf; yp = 0.0; }

        std::vector<sf::Vertex> tempVec;// temp storage for reverse integration from x = 0 to x = -plot_wf.tMax*plot_wf.tScale
        sf::Vertex vtx;
        vtx.color = sf::Color::Green;
        double Lsq = L*L;
        for( double x = 0.0; x > -a*plot_wf.tScale; x -= dx )
        {
            ypp = -Lsq*y;
            yp -= ypp*dx;
            y -= yp*dx;
            vtx.position.x = plot_wf.originPos.x + x*plot_wf.tScale;
            vtx.position.y = plot_wf.originPos.y - y*plot_wf.yScale - dy;
            tempVec.push_back( vtx );
        }
        double Ksq = k*k;
        double yLast = 0.0;// to stop y from crossing x-axis
        double ypLast = 0.0;// to stop y from curving away from x-axis
        for( double x = -a*plot_wf.tScale; x > -plot_wf.tMax*plot_wf.tScale; x -= dx )
        {
            yLast = y;
            ypLast = yp;
            ypp = Ksq*y;
            yp -= ypp*dx;
            y -= yp*dx;
            if( y*yLast < 0.0 || ypLast*yp < 0.0 ) break;
            vtx.position.x = plot_wf.originPos.x + x*plot_wf.tScale;
            vtx.position.y = plot_wf.originPos.y - y*plot_wf.yScale - dy;
            tempVec.push_back( vtx );
        }

        // copy into plot_4Vec in reverse order
        for( std::vector<sf::Vertex>::reverse_iterator rIt = tempVec.rbegin(); rIt != tempVec.rend(); ++rIt )
            plot_4Vec.push_back( *rIt );
        // now integrate forward
        // reset ICs
        if( odd_evenSelector.selIdx == 0 ) { y = 0.0; yp = L*Awf; }// odd function
        else { y = Awf; yp = 0.0; }

        for( double x = 0.0; x < a*plot_wf.tScale; x += dx )
        {
            ypp = -Lsq*y;
            yp += ypp*dx;
            y += yp*dx;
            vtx.position.x = plot_wf.originPos.x + x*plot_wf.tScale;
            vtx.position.y = plot_wf.originPos.y - y*plot_wf.yScale - dy;
            plot_4Vec.push_back( vtx );
        }

        for( double x = a*plot_wf.tScale; x < plot_wf.tMax*plot_wf.tScale; x += dx )
        {
            yLast = y;
            ypLast = yp;
            ypp = Ksq*y;
            yp += ypp*dx;
            y += yp*dx;
            if( y*yLast < 0.0 || ypLast*yp < 0.0 ) break;
            vtx.position.x = plot_wf.originPos.x + x*plot_wf.tScale;
            vtx.position.y = plot_wf.originPos.y - y*plot_wf.yScale - dy;
            plot_4Vec.push_back( vtx );
        }

    }// end if integrateButt.sel

}// end of makePlot_wf()

// Temporary versions
double LvlQM_FSW::wf( double x, unsigned int n )// the wave function
{
    // graph an odd wave function
    if( odd_evenSelector.selIdx == 0 )
    {
        if( x < -a ) return -F*expf(k*x);
        else if( x < a ) return sinf(L*x);
        return F*expf(-k*x);// x > a
    }

    // even wave function
    if( x < -a ) return F*expf(k*x);
    else if( x < a ) return cosf(L*x);
    return F*expf(-k*x);// x > a
}

double LvlQM_FSW::d_wf_dx( double x, unsigned int n )// 1st derivative wrt x
{
    if( odd_evenSelector.selIdx == 0 )
    {
        if( x < -a ) return -F*k*expf(k*x);
        else if( x < a ) return L*cosf(L*x);
        return -F*k*expf(-k*x);// x > a
    }

    // even wave function
    if( x < -a ) return F*k*expf(k*x);
    else if( x < a ) return -L*sinf(L*x);
    return -F*k*expf(-k*x);// x > a
}

double LvlQM_FSW::d2_wf_dx2( double x, unsigned int n )// 2nd derivative wrt x
{
    if( odd_evenSelector.selIdx == 0 )
    {
        if( x < -a ) return -F*k*k*expf(k*x);
        else if( x < a ) return -L*L*sinf(L*x);
        return F*k*k*expf(-k*x);// x > a
    }

    // even wave function
    if( x < -a ) return F*k*k*expf(k*x);
    else if( x < a ) return -L*L*cosf(L*x);
    return F*k*k*expf(-k*x);// x > a
}

bool LvlQM_FSW::handleEvent( sf::Event& rEvent )
{
    if( !button::pButtMse )// the mouse is not over a button
    {
        if( button::clickEvent_Lt() == 1 )// left mouse button was pressed
        {
            // find a point where plot_1 and plot_2 intersect
            float xVal = -1.0f, yVal = -1.0f;
            if( plot12Control.homeButt.sel && plot_2.getDataPoint( button::mseX, button::mseY, xVal, yVal ) )// click is in plot_2 domain
            {
                std::cout << "\nClicked at: xVal = " << xVal << "  yVal = " << yVal;// graphed values
                double xInt, yInt;
                unsigned int iterCount = findPlotIntersect( static_cast<double>(xVal), 0.001, xInt, yInt, rootFindIterLimit );
                std::cout << "\nAfter " << iterCount << " iterations";
                std::cout << "\nPlots intersect at: " << xInt << ", " << yInt;
                std::cout << "\nCheck: y1 = " << pFunc_1(xInt) << " y2 = " << pFunc_2(xInt) << '\n';
                if( addRootButt.sel && iterCount < rootFindIterLimit )
                {
                    zVec.push_back( xInt );
                    std::cout << "\nroot added. roots found =  " << zVec.size();
                }
            }

        }
    }

    return true;
}

void LvlQM_FSW::update( float dt )
{
    if( plot3Control.homeButt.sel )
        LvlQM::update(dt);

    return;// no animations to update
}

void LvlQM_FSW::draw( sf::RenderTarget& RT ) const
{
    if( plot12Control.homeButt.sel )
    {
        plot_1.draw(RT);
        plot_2.draw(RT);
    }
    else if( plot3Control.homeButt.sel )
    {
   //     RT.draw( wellVtxArray, 6, sf::LinesStrip );
        LvlQM::draw(RT);
        RT.draw( aMsg ); RT.draw( aNumMsg );
        RT.draw( V0Msg ); RT.draw( V0NumMsg );

        // Experimental
        if( integrateButt.sel ) RT.draw( &(plot_4Vec[0]), plot_4Vec.size(), sf::LinesStrip );
    }

    return;
}

// regular functions
bool LvlQM_FSW::init_controls()
{
    sf::Text Label("plots", *button::pFont, 12);
    Label.setFillColor( sf::Color::Black );

    std::ifstream fin("include/levels/LvlQM_FSW/control_data.txt");
    if( !fin ) { std::cout << "\nNo control data"; return false; }

    float xPos, yPos, W, H;
    fin >> xPos >> yPos >> W >> H;
    plotList.init( xPos, yPos, W, H, Label );
    plotList.persist = 3;
    button::RegisteredButtVec.push_back( &plotList );

    // plot12Control
    sf::Vector2f PosHmButt, PosSurf, SzSurf;
    fin >> PosHmButt.x >> PosHmButt.y >> PosSurf.x >> PosSurf.y >> SzSurf.x >> SzSurf.y >> W >> H;
    plot12Control.init( PosHmButt, PosSurf, SzSurf, "plot12", W, H );
    plot12Control.homeButt.pHitFunc = [this](){ if( plot12Control.homeButt.sel ) rootsButt.status = addRootButt.status = button::Status::active; };
    plotList.pButtVec.push_back( &plot12Control );
    // for plot_2 parameter z0
    Label.setString( "z0" );
    fin >> xPos >> yPos >> W >> H;
    z0Strip.init( xPos, yPos, W, H, Label, 0.1f, plot_2.tMax, plot_2.tMax/2.0, [this]( float x ) { z0 = x; zVec.clear(); makePlot_2(); } );
    plot12Control.pButtVec.push_back( &z0Strip );

    // for plot_wf
    fin >> PosHmButt.x >> PosHmButt.y >> PosSurf.x >> PosSurf.y >> SzSurf.x >> SzSurf.y >> W >> H;
    plot3Control.init( PosHmButt, PosSurf, SzSurf, "plot3", W, H );
    plot3Control.onSurfaceOnly = false;
    plot3Control.homeButt.pHitFunc = [this]()
    {
        if( plot3Control.homeButt.sel )
        {
            rootsButt.status = addRootButt.status = button::Status::hidden;
            addRootButt.setSel(false);
            if( zVec.size() > 1 )
            {
                energySelector.status = button::Status::active;
                energySelector.setCountLimit( zVec.size() - 1 );
            }
            else energySelector.status = button::Status::hidden;

            makePlot_wf();
        }
    };
    plotList.pButtVec.push_back( &plot3Control );

    fin >> xPos >> yPos;
    msControl.init( sf::Vector2f(xPos,yPos), &msStrip, fin );
    plot3Control.pButtVec.push_back( &msControl );
    if( msControl.sdataVec.size() != 3 ) { std::cout << "\nbad msControl data"; return false; }
    stripData* pSD = &msControl.sdataVec.front();
    // a
    pSD->pSetFunc =     [this](float x){ a = x; makePlot_wf(); };
    msStrip.reInit( *pSD );
    a = pSD->xCurr;
    // m
    (++pSD)->pSetFunc = [this](float x){ m = x; to_SF_string( massNumMsg, x ); makePlot_wf(); };
    m = pSD->xCurr; to_SF_string( massNumMsg, m );
    // Awf
    (++pSD)->pSetFunc = [this](float x){ Awf = x; makePlot_wf(); };
    Awf = pSD->xCurr;

    initShadeControl(fin);
    plot3Control.pButtVec.push_back( &shadeSurf );

    // set energy level and grapf wf or wf^2
    initSelectorControls(fin);
    plot3Control.pButtVec.push_back( &energySelector );
    plot3Control.pButtVec.push_back( &wf_wfSqSelector );

    // find roots
    fin >> xPos >> yPos >> W >> H;
    Label.setString("find roots");
    Label.setFillColor( sf::Color::Black );
    rootsButt.init( xPos, yPos, W, H, nullptr, Label );
    rootsButt.mode = 'I';
    rootsButt.status = button::Status::hidden;
    button::RegisteredButtVec.push_back( &rootsButt );
    rootsButt.pHitFunc = [this]()
    {
        zVec.clear();
        unsigned int rootCount = 0, rootsFound = 0;
//        double xBegin = odd_evenSelector.selIdx == 0 ? 3.0*PI/4.0 : PI/4.0;
        double db = 0.05;
        double xBegin = odd_evenSelector.selIdx == 0 ? (1.0-db)*PI : (0.5-db)*PI;
        for( double x = xBegin; x < z0; x += PI )
        {
            ++rootCount;
            double xInt, yInt;
            unsigned int iterCount = findPlotIntersect( x, 0.001, xInt, yInt, rootFindIterLimit );
            if( iterCount < rootFindIterLimit && xInt > 0.0 )
            {
                ++rootsFound;
                std::cout << "\nAfter " << iterCount << " iterations";
                std::cout << "\nPlots intersect at: " << xInt << ", " << yInt;
                std::cout << "\nCheck: y1 = " << pFunc_1(xInt) << " y2 = " << pFunc_2(xInt) << '\n';
                zVec.push_back( xInt );
            }
            else std::cout << "\n iter limit exceeded";
        }

        std::cout << '\n' << rootsFound << " roots found";
        std::cout << '\n' << rootCount - rootsFound << " roots missed \n";
    };

    // add a root
    fin >> xPos >> yPos >> W >> H;
    Label.setString("add roots");
    addRootButt.init( xPos, yPos, W, H, nullptr, Label );
    addRootButt.status = button::Status::hidden;
    button::RegisteredButtVec.push_back( &addRootButt );

    // select odd or even functions
    fin >> xPos >> yPos >> W >> H;
    Label.setString("solution type");
    odd_evenSelector.init( xPos, yPos, W, H, 2, Label, 0 );// initially odd
    float offX, offY; fin >> offX >> offY;
    std::vector<std::string> labelVec;
//    fin >> offX >> offY;
//    labelVec.clear();
    labelVec.push_back("odd"); labelVec.push_back("even");
    odd_evenSelector.setButtLabels( offX, offY, labelVec );
    odd_evenSelector.pFuncIdx = [this]( size_t idx )
    {
        if( idx == 0 ) { pFunc_1 = pFunc_1_odd; pFunc_2 = pFunc_2_odd; }
        else { pFunc_1 = pFunc_1_even; pFunc_2 = pFunc_2_even; }
        makePlot_1();
        makePlot_2();
        zVec.clear();
    };
    button::RegisteredButtVec.push_back( &odd_evenSelector );



    // extra for experiment
    fin >> xPos >> yPos >> W >> H;
    Label.setString("integrate");
    Label.setFillColor( sf::Color::Black );
    integrateButt.init( xPos, yPos, W, H, nullptr, Label );
    plot3Control.pButtVec.push_back( &integrateButt );

    fin.close();

    return true;
}

unsigned int LvlQM_FSW::findPlotIntersect( double x0, double dxMin, double& xInt, double& yInt, unsigned int iterLimit )
{
    double y1, y2, y1p, y2p;
    unsigned int iterCount = 0;
    // Newtons method?
    while( ++iterCount < iterLimit )
    {
        y1 = pFunc_1(x0);
        y2 = pFunc_2(x0);
        y1p = ( pFunc_1(x0+dxMin) - y1 )/dxMin;
        y2p = ( pFunc_2(x0+dxMin) - y2 )/dxMin;
        xInt = x0 + ( y2 - y1 )/( y1p - y2p );
        if( (x0-xInt)*(x0-xInt) < dxMin*dxMin ) break;
        x0 = xInt;// for next iteration
    };

    yInt = pFunc_1(xInt);
    return iterCount;
}

