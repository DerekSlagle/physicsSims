#include "LvlQM.h"

// virtual functions
bool LvlQM::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(200,200,200);
    button::setHoverBoxColor( sf::Color::Black );

    std::ifstream fin("include/levels/LvlQM/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }
    fin >> rootFindIterLimit;
    fin.close();

    if( !init_plots() ) return false;
    if( !init_controls() ) return false;

    return true;
}

bool LvlQM::init_plots()
{
    std::ifstream fin("include/levels/LvlQM/functionPlot_data.txt");
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

    plot_3.init(fin);
    plot_3.axisVtxVec.pop_back();// eliminate x-axis
    plot_3.axisVtxVec.pop_back();

    fin.close();

    return true;
}

void LvlQM::makeWell( const functionPlot& rWave_fp )
{
    sf::Vertex vtx;
    vtx.color = sf::Color::Blue;
    // draw well
    vtx.position.x = rWave_fp.originPos.x + rWave_fp.tMax*rWave_fp.tScale;// right end
    vtx.position.y = rWave_fp.originPos.y;
    wellVtxArray[0] = vtx;
    vtx.position.x = rWave_fp.originPos.x + a*rWave_fp.tScale;// up right corner
    vtx.position.y = rWave_fp.originPos.y;
    wellVtxArray[1] = vtx;
    vtx.position.x = rWave_fp.originPos.x + a*rWave_fp.tScale;// dn right corner
    vtx.position.y = rWave_fp.originPos.y + V0*rWave_fp.yScale;
    wellVtxArray[2] = vtx;
    vtx.position.x = rWave_fp.originPos.x - a*rWave_fp.tScale;// dn left corner
    vtx.position.y = rWave_fp.originPos.y + V0*rWave_fp.yScale;
    wellVtxArray[3] = vtx;
    vtx.position.x = rWave_fp.originPos.x - a*rWave_fp.tScale;// up left corner
    vtx.position.y = rWave_fp.originPos.y;
    wellVtxArray[4] = vtx;
    vtx.position.x = rWave_fp.originPos.x - rWave_fp.tMax;// left end
    vtx.position.y = rWave_fp.originPos.y;
    wellVtxArray[5] = vtx;
}

void LvlQM::makePlot_1()
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

void LvlQM::makePlot_2()
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

void LvlQM::makePlot_3( size_t rootIdx )
{
    plot_3.plotVtxVec.clear();
    makeWell( plot_3 );
    if( rootIdx + 1 > zVec.size() )
    {
        std::cout << "\nroot #" << rootIdx << " does not exist";
        return;
    }
    else std::cout << "\nrootIdx = " << rootIdx;

    // draw wavefunction
    std::ifstream fin("include/levels/LvlQM/oddWave_data.txt");
    if( !fin ) { std::cout << "\nNo oddWave data"; return; }
    double z, L, E, F, C;
    fin >> C;// amplitude of sine wave
    fin.close();

    z = zVec[rootIdx];
    k = sqrt( z0*z0 - z*z )/a;
    L = z/a;
    E = V0*( (z/z0)*(z/z0) - 1.0 );

    float dx = 2.0*static_cast<double>( plot_3.tMax )/subInts;
    float fValue;
    plot_3.pValue = &fValue;
    plot_3.tElap = -plot_3.tMax;

    if( odd_evenSelector.selIdx == 0 )// graph an odd wave
    {
        F = C*sin(z)*exp(k*a);
        for( float x = -plot_3.tMax; x < plot_3.tMax; x += dx )
        {
            if( x < -a ) { fValue = -F*expf(k*x); plot_3.update(dx); }
            else if( x < a ) { fValue = C*sinf(L*x); plot_3.update(dx); }
            else { fValue = F*expf(-k*x); plot_3.update(dx); }// x > a
        }
    }
    else// graph an even wavefunction
    {
        F = C*cos(z)*exp(k*a);
        for( float x = -plot_3.tMax; x < plot_3.tMax; x += dx )
        {
            if( x < -a ) { fValue = F*expf(k*x); plot_3.update(dx); }
            else if( x < a ) { fValue = C*cosf(L*x); plot_3.update(dx); }
            else { fValue = F*expf(-k*x); plot_3.update(dx); }// x > a
        }
    }

    std::cout << "\na*k = " << a*k << " z = " << z << " E/V0 = " << E/V0 << " F/C = " << F/C;
    // TEMP: output boundary values
//    std::cout << "\n f(a-) = " << C*sinf(L*a)  << "  fp(a-) = " << C*L*cosf(L*a);
//    std::cout << "\n f(a+) = " << F*expf(-k*a) << "  fp(a+) = " << -F*k*expf(-k*a);
}

bool LvlQM::handleEvent( sf::Event& rEvent )
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

void LvlQM::update( float dt )
{
    return;// no animations to update
}

void LvlQM::draw( sf::RenderTarget& RT ) const
{
    if( plot12Control.homeButt.sel )
    {
        plot_1.draw(RT);
        plot_2.draw(RT);
    }
    else if( plot3Control.homeButt.sel )
    {
        RT.draw( wellVtxArray, 6, sf::LinesStrip );
        plot_3.draw(RT);
    }

    return;
}

// regular functions
bool LvlQM::init_controls()
{
    sf::Text Label("plots", *button::pFont, 12);
    Label.setFillColor( sf::Color::Black );

    std::ifstream fin("include/levels/LvlQM/control_data.txt");
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
    z0Strip.init( xPos, yPos, W, H, Label, 0.1f, plot_2.tMax, plot_2.tMax/2.0, [this]( float x ) { z0 = x; makePlot_2(); } );
    plot12Control.pButtVec.push_back( &z0Strip );

    // for plot_3
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
                rootSelector.status = button::Status::active;
                rootSelector.setCountLimit( zVec.size() - 1 );
            }
            else rootSelector.status = button::Status::hidden;

            makePlot_3();
        }
    };
    plotList.pButtVec.push_back( &plot3Control );

    fin >> xPos >> yPos;
    msControl.init( sf::Vector2f(xPos,yPos), &msStrip, fin );
    plot3Control.pButtVec.push_back( &msControl );
    if( msControl.sdataVec.size() != 3 ) { std::cout << "\nbad msControl data"; return false; }
    stripData* pSD = &msControl.sdataVec.front();
    // a
    pSD->pSetFunc =     [this](float x){ a = x; makePlot_3( rootSelector.hoverValue ); };
    msStrip.reInit( *pSD );
    a = pSD->xCurr;
     // V0
    (++pSD)->pSetFunc = [this](float x){ V0 = x; makePlot_3( rootSelector.countValue ); };
    V0 = pSD->xCurr;
    // m
    (++pSD)->pSetFunc = [this](float x){ m = x; };// makePlot_3( rootSelector.countValue ); };
    m = pSD->xCurr;

    fin >> xPos >> yPos >> W >> H;
    Label.setString("rootIdx");
    rootSelector.init( xPos, yPos, W, H, 0, 10, 0, [this](){ makePlot_3( rootSelector.hoverValue ); }, Label );
    plot3Control.pButtVec.push_back( &rootSelector );

    // find roots
    fin >> xPos >> yPos >> W >> H;
    Label.setString("find roots");
    rootsButt.init( xPos, yPos, W, H, nullptr, Label );
    rootsButt.mode = 'I';
    rootsButt.status = button::Status::hidden;
    button::RegisteredButtVec.push_back( &rootsButt );
    rootsButt.pHitFunc = [this]()
    {
        zVec.clear();
        unsigned int rootCount = 0, rootsFound = 0, iterLimit = 10;
        double xBegin = odd_evenSelector.selIdx == 0 ? 3.0*PI/4.0 : PI/4.0;
        for( double x = xBegin; x < z0; x += PI )
        {
            ++rootCount;
            double xInt, yInt;
            unsigned int iterCount = findPlotIntersect( x, 0.001, xInt, yInt, iterLimit );
            if( iterCount < iterLimit && xInt > 0.0 )
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
    labelVec.push_back("odd"); labelVec.push_back("even");
    odd_evenSelector.setButtLabels( offX, offY, labelVec );
    odd_evenSelector.pFuncIdx = [this]( size_t idx )
    {
        if( idx == 0 ) { pFunc_1 = pFunc_1_odd; pFunc_2 = pFunc_2_odd; }
        else { pFunc_1 = pFunc_1_even; pFunc_2 = pFunc_2_even; }
        makePlot_1();
        makePlot_2();
    };
    button::RegisteredButtVec.push_back( &odd_evenSelector );

    fin.close();

    return true;
}

unsigned int LvlQM::findPlotIntersect( double x0, double dxMin, double& xInt, double& yInt, unsigned int iterLimit )
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
