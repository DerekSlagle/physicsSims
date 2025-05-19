#include "LvlQM_HarmOsci.h"

bool LvlQM_HarmOsci::init()// virtual
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(200,200,200);
    button::setHoverBoxColor( sf::Color::Black );

    std::ifstream fin("include/levels/LvlQM_HarmOsci/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }
    LvlQM::init(fin);
    fin.close();

    if( !init_plots() ) return false;
    xCoordMsg.setPosition( plot_wf.originPos.x + 10.0f, plot_wf.originPos.y + 10.0f );
    if( !init_controls() ) return false;
    makePlot_v();
    makePlot_wf();

    return true;
}

bool LvlQM_HarmOsci::init_plots()
{
    std::ifstream fin("include/levels/LvlQM_HarmOsci/functionPlot_data.txt");
    if( !fin ) { std::cout << "\nNo functionPlot data"; return false; }

    // common data
    fin >> subInts >> Escale;

    plot_v.init( fin, sf::Color::Blue );
    plot_v.tScale = 1.0;
    plot_v.axisVtxVec[2].position.x -= plot_v.tMax;

    plot_wf.init(fin);
    plot_wf.tScale = 1.0;
    plot_wf.axisVtxVec.pop_back();// eliminate x-axis
    plot_wf.axisVtxVec.pop_back();

    fin.close();

    energyLine[0].color = sf::Color::Red;
    energyLine[1].color = sf::Color::Red;
    energyLine[0].position.x =  plot_v.originPos.x - plot_v.tMax*plot_v.tScale;
    energyLine[1].position.x =  plot_v.originPos.x + plot_v.tMax*plot_v.tScale;

    return true;
}

void LvlQM_HarmOsci::makePlot_v()// virtual
{
    plot_v.plotVtxVec.clear();
    double a = sqrt( 2.0*plot_v.yMax*plot_v.yScale/k );
    double dx = 2.0*plot_v.tMax/subInts;
    float fValue;
    plot_v.pValue = &fValue;
    plot_v.tElap = -a;
    for( double x = -a; x <= a; x += dx )
    {
        fValue = 0.5*k*x*x;
        plot_v.update(dx);
    }
}

// used in wave functions
double wf_coeff( unsigned int n )// returns 1/sqrt( 2^n*n! )
{
    static unsigned int nLast = 0;
    static double B = 1.0;
    if( n != nLast )// only when n has changed since last call
    {
        B = 1.0;
        for( unsigned int q = 1; q <= n; ++q ) B *= 2.0*q;// build up B = (2^n)*n!
        nLast = n;
    }
    return 1.0/sqrt(B);
}
// virtual
double LvlQM_HarmOsci::wf( double x, unsigned int n )// the wave function
{
    double z = sf*x;
    return wf_coeff( n )*HermitePoly( z, n )*exp( -z*z/2.0 );
}
// virtual
double LvlQM_HarmOsci::d_wf_dx( double x, unsigned int n )// 1st derivative wrt x
{
    double z = sf*x;
    if( n == 0 ) return -z*exp( -z*z/2.0 )*sf;
    return sf*wf_coeff( n )*( 2.0*n*HermitePoly(z,n-1) - z*HermitePoly(z,n) )*exp( -z*z/2.0 );
}
// virtual
double LvlQM_HarmOsci::d2_wf_dx2( double x, unsigned int n )// 2nd derivative wrt x
{
    double z = sf*x;
    if( n == 0 ) return sf*sf*(z*z-1.0)*exp( -z*z/2.0 );
    if( n == 1 ) return sf*sf*sqrt(2.0)*z*( z*z - 3.0 )*exp( -z*z/2.0 );
    return sf*sf*wf_coeff( n )*( (z*z-1.0)*HermitePoly(z,n) - 4.0*n*z*HermitePoly(z,n-1) + 4.0*n*(n-1.0)*HermitePoly(z,n-2) )*exp( -z*z/2.0 );
}

void LvlQM_HarmOsci::makePlot_wf()// virtual
{
    plot_wf.plotVtxVec.clear();
    const int& n = energySelector.countValue;
    float fValue;
    plot_wf.pValue = &fValue;
    plot_wf.tElap = -plot_wf.tMax;
    double dx = 2.0*plot_v.tMax/subInts;
    double E = ( 0.5 + static_cast<float>( n ) )*sqrt(k/m);
    to_SF_string( energyNumMsg, E*h_bar*EVperJoule );// energy in electron volts
    energyLine[0].position.y = energyLine[1].position.y = plot_v.originPos.y - E*Escale;
    double Area = 0.0;// under wf^2
    double xSqExpect = 0.0;
    double pSqExpect = 0.0;

    for( double x = -plot_v.tMax; x < plot_v.tMax; x += dx )
    {
        fValue = static_cast<float>( Awf*wf( x, n ) );
        double fVSq = fValue*fValue;
        Area += fVSq*dx;
        if( wf_wfSqSelector.selIdx == 1 ) fValue = fVSq/4.0;// graphing wf^2, with reduced amplitude
        plot_wf.update(dx);
        xSqExpect += x*x*fVSq*dx;
        pSqExpect += fValue*d2_wf_dx2(x,n)*dx;
    }

    xSqExpect /= Area;
    to_SF_string( xSqExpectNumMsg, xSqExpect );
    pSqExpect *= -h_bar*h_bar/Area;
 //   if( pSqExpect < 0.0 ) pSqExpect *= -1.0;
    to_SF_string( pSqExpectNumMsg, pSqExpect );
    to_SF_string( uncertNumMsg, sqrt(xSqExpect*pSqExpect) );

    for( sf::Vertex& v : plot_wf.plotVtxVec ) v.position.y -= E*Escale;
    Area /= 4.0;// to compensate for reduced amplitude
    to_SF_string( areaNumMsg, Area );
}

bool LvlQM_HarmOsci::makePlot_super()
{
    std::ifstream fin("include/levels/LvlQM_HarmOsci/super_data.txt");
    if( !fin ) { std::cout << "\nNo super_data.txt file"; return false; }
    double coeff = 0.0;
    superVec.clear();
    while( fin >> coeff ) superVec.push_back( coeff );
    fin.close();

    if( superVec.empty() ) { std::cout << "\nNo makePlotSuper() data"; return false; }
    // normailze
    double normFactor = 0.0;
    for( double c : superVec ) normFactor += c*c;
    normFactor = sqrt( normFactor );

    plot_wf.plotVtxVec.clear();
    float fValue;
    plot_wf.pValue = &fValue;
    plot_wf.tElap = -plot_wf.tMax;
    double dx = 2.0*plot_v.tMax/subInts;
    double Eexpect = 0.0;
    for( size_t n = 0; n < superVec.size(); ++n )
        Eexpect += ( 0.5 + static_cast<double>( n ) )*superVec[n];

    Eexpect *= sqrt(k/m)/normFactor;
    double Area = 0.0;// under wf^2
    double xExpect = 0.0, xSqExpect = 0.0;
    double pExpect = 0.0, pSqExpect = 0.0;

    for( double x = -plot_v.tMax; x < plot_v.tMax; x += dx )
    {
        double sum = 0.0, sum_1 = 0.0, sum_2 = 0;
        // sum over the wave functions
        for( size_t n = 0; n < superVec.size(); ++n )
        {
            double B = superVec[n]*Awf/normFactor;
            sum += B*wf( x, n );// wave function
            sum_1 += B*d_wf_dx(x,n);// 1st derivative
            sum_2 += B*d2_wf_dx2(x,n);// 2nd derivative
        }

        pExpect += sum*sum_1;
        pSqExpect += sum*sum_2;
        double sumSq = sum*sum;
        Area += sumSq;
        xExpect += x*sumSq;
        xSqExpect += x*x*sumSq;
        if( wf_wfSqSelector.selIdx == 1 ) fValue = sumSq/4.0;// graphing wf^2, with reduced amplitude
        else fValue = static_cast<float>( sum );
        plot_wf.update(dx);
    }

    xExpect /= Area;
    xSqExpect /= Area;
    to_SF_string( xSqExpectNumMsg, xSqExpect );
    pExpect *= h_bar/Area;
    pSqExpect *= -h_bar*h_bar/Area;
    to_SF_string( pSqExpectNumMsg, pSqExpect );
    to_SF_string( uncertNumMsg, sqrt(xSqExpect*pSqExpect) );
    to_SF_string( energyNumMsg, Eexpect*h_bar*EVperJoule );// energy in electron volts
    energyLine[0].position.y = energyLine[1].position.y = plot_v.originPos.y - Eexpect*Escale;
    for( sf::Vertex& v : plot_wf.plotVtxVec ) v.position.y -= Eexpect*Escale;
    Area *= dx/4.0;// to compensate for reduced amplitude
    to_SF_string( areaNumMsg, Area );

    return true;
}

bool LvlQM_HarmOsci::init_controls()
{
    std::ifstream fin("include/levels/LvlQM_HarmOsci/control_data.txt");
    if( !fin ) { std::cout << "\nNo control data"; return false; }

    // for plot_wf
//    fin >> PosHmButt.x >> PosHmButt.y >> PosSurf.x >> PosSurf.y >> SzSurf.x >> SzSurf.y >> W >> H;
    plot3Control.init( "plot_wf", fin );
    plot3Control.onSurfaceOnly = false;
    plot3Control.homeButt.setSel(true);
    button::RegisteredButtVec.push_back( &plot3Control );

//    fin >> xPos >> yPos;
    msControl.init( &msStrip, fin );
    plot3Control.pButtVec.push_back( &msControl );
    if( msControl.sdataVec.size() != 4 ) { std::cout << "\nbad msControl data"; return false; }
    stripData* pSD = &msControl.sdataVec.front();
    // k
    pSD->pSetFunc =     [this](float x){ k = x; makePlot_v(); makePlot_wf(); };
    msStrip.reInit( *pSD );
    k = pSD->xCurr;
    // m
    (++pSD)->pSetFunc = [this](float x){ m = x; to_SF_string( massNumMsg, x ); makePlot_wf(); };
    m = pSD->xCurr; to_SF_string( massNumMsg, m );
    // Awf
    (++pSD)->pSetFunc = [this](float x){ Awf = x*pow( m*k, 0.25 ); makePlot_wf(); };
    Awf = pSD->xCurr*pow( m*k, 0.25 );
    // sf
    (++pSD)->pSetFunc = [this](float x){ sf = x; makePlot_wf(); };
    sf = pSD->xCurr;

    initShadeControl(fin);
    plot3Control.pButtVec.push_back( &shadeSurf );

    // set energy level and grapf wf or wf^2
    initSelectorControls(fin);
    plot3Control.pButtVec.push_back( &energySelector );
    plot3Control.pButtVec.push_back( &wf_wfSqSelector );


    // New
    sf::Text Label("super", *button::pFont, 12);
    Label.setFillColor( sf::Color::Black );
    float xPos, yPos, W, H;
    fin >> xPos >> yPos >> W >> H;
    superButt.init( xPos, yPos, W, H, nullptr, Label );
    superButt.mode = 'I';
    plot3Control.pButtVec.push_back( &superButt );
    superButt.pHitFunc = [this]()
    {
        makePlot_super();
    };

    fin.close();

    return true;
}

bool LvlQM_HarmOsci::handleEvent( sf::Event& rEvent )// virtual
{
    return true;
}

void LvlQM_HarmOsci::draw( sf::RenderTarget& RT ) const// virtual
{
    LvlQM::draw(RT);
    return;
}

double LvlQM_HarmOsci::HermitePoly( double x, unsigned int n )
{
    double xSq = x*x;

    switch( n )
    {
        case 0 :
            return 1.0;
        case 1 :
            return 2.0*x;
        case 2 :
            return 4.0*xSq - 2.0;
        case 3 :
            return ( 8.0*xSq - 12.0 )*x;
        case 4 :
            return 16.0*xSq*xSq - 48.0*xSq + 12.0;
        case 5 :
            return ( 32.0*xSq*xSq - 160.0*xSq + 120.0 )*x;
        default:
            if( n < 2 ) break;
            return 2.0*x*HermitePoly( x, n-1 ) - 2.0*( n - 1.0 )*HermitePoly( x, n-2 );
    }

    return 0.0;
}
