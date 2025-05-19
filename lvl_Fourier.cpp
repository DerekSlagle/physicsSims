#include "lvl_Fourier.h"

bool lvl_Fourier::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(200,200,200);
    button::setHoverBoxColor( Level::clearColor );


    std::ifstream fin("include/levels/lvl_Fourier/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    fin >> xOrigin >> yOrigin >> L;
    fin >> dxVtx;

    float msgPosX, msgPosY; fin >> msgPosX >> msgPosY;
    unsigned int fontSz; fin >> fontSz;

    fin.close();

    if( !getPlotData() ) return false;
    if( !init_controls() ) return false;

    // coeffsMsg
    coeffsMsg.setFont( *button::pFont );
    coeffsMsg.setCharacterSize( fontSz );
    coeffsMsg.setFillColor( sf::Color::Black );
    coeffsMsg.setPosition( msgPosX, msgPosY );
    updateCoeffsMsg();

    return true;
}

void lvl_Fourier::updateCoeffsMsg()
{
    std::string msg( "coeffs" );
    for( size_t n = 0; n < sinVec.size(); ++n )
    {
        std::stringstream ss;
        ss.precision(4);
        ss << sinVec[n];
        std::string tempStr;
        ss >> tempStr;
        msg += '\n';
        msg += tempStr;
    }

    coeffsMsg.setString( msg.c_str() );
}

bool lvl_Fourier::getPlotData()
{
    std::ifstream fin("include/levels/lvl_Fourier/plot_data.txt");
    if( !fin ) { std::cout << "\nNo plot data"; return false; }

    unsigned int rd, gn, bu, ap; fin >> rd >> gn >> bu >> ap;
    plotColor = sf::Color(rd,gn,bu,ap);
    fin >> rd >> gn >> bu;
    funcColor = sf::Color(rd,gn,bu,ap);
    fin >> rd >> gn >> bu;
    sf::Color axesColor(rd,gn,bu);
    fin >> tScale; tScale = 1.0f/tScale;
    // get coeffs
    size_t numCoeffs; fin >> numCoeffs;
    sinVec.clear();
    sinVec.reserve( numCoeffs );
    sinVecA.clear();
    sinVecA.reserve( numCoeffs );
    sinVecB.clear();
    sinVecB.reserve( numCoeffs );

    fin >> Amp;
    char inType; fin >> inType;// 'S', 'T' or 'C'

    if( inType == 'S' )// square wave
    {
        double k = 4.0*Amp/myPId;
        for( size_t i = 1; i <= numCoeffs; ++i )
        {
        //    double C; fin >> C;
            double C = i%2 ? k/i : 0.0;
            sinVec.push_back( C );
        }
    }
    else if( inType == 'T' )// triangle, or saw tooth wave
    {
        int sign = 1;
        for( size_t n = 1; n <= numCoeffs; ++n )
        {
            if( n%2 == 0 )
                sinVec.push_back( 0.0 );// even n
            else
            {
                sinVec.push_back( (double)sign*8.0*Amp/( (n*myPId)*(n*myPId) ) );
                sign *= -1;
            }
        }
    }
    else// read in coeffs
    {
        for( size_t n = 0; n < numCoeffs; ++n )
        {
            double C;
            if( !(fin >> C) ){ std::cout << "\ngetPlotData(): setA only " << n << " of " << numCoeffs << " coefficients given"; numCoeffs = sinVec.size(); break; }
            sinVec.push_back( C );
            sinVecA.push_back( C );
        }

        for( size_t n = 0; n < numCoeffs; ++n )
        {
            double C;
            if( !(fin >> C) ){ std::cout << "\ngetPlotData(): setB only " << n << " of " << numCoeffs << " coefficients given"; numCoeffs = sinVec.size(); break; }
            sinVecB.push_back( C );
        }

        if( sinVecB.size() < numCoeffs ) sinVecB.clear();
    }

    fin.close();

    // axes
    axes[0].color = axes[1].color = axes[2].color = axes[3].color = axesColor;
    axes[0].position = sf::Vector2f( xOrigin - (float)(1.2*L), yOrigin );// x axis
    axes[1].position = sf::Vector2f( xOrigin + (float)(1.2*L), yOrigin );
    axes[2].position = sf::Vector2f( xOrigin, yOrigin + (float)(1.5*Amp) );// y axis
    axes[3].position = sf::Vector2f( xOrigin, yOrigin - (float)(1.5*Amp) );

    makePlot( inType );

    return true;
}

void lvl_Fourier::updatePlot()
{
    size_t numVtx = vtxVec.size();
    double x = 0.0;
    for( size_t n = 1; n + 1 < numVtx; ++n )
    {
        x = n*dxVtx - L;
        double sum = 0.0;
        for( size_t k = 0; k < sinVec.size(); ++k )
            sum += sinVec[k]*sin( (k+1)*myPId*x/L );

        vtxVec[n].position = sf::Vector2f( xOrigin + (float)x, yOrigin - (float)sum );
    }
}

void lvl_Fourier::makePlot( char inType )
{
    // produce plot
    size_t numVtx = 2.0*L/dxVtx + 2;// ? for terminal vtx ?
    vtxVec.reserve( numVtx );
    sf::Vertex tempVtx;
    tempVtx.color = plotColor;

    tempVtx.position = sf::Vector2f( xOrigin - (float)L, yOrigin );
    vtxVec.push_back( tempVtx );// add left end point
    double x = 0.0;
    for( size_t n = 1; n < numVtx; ++n )
    {
        x = n*dxVtx - L;
        double sum = 0.0;
        for( size_t k = 0; k < sinVec.size(); ++k )
            sum += sinVec[k]*sin( (k+1)*myPId*x/L );

        tempVtx.position = sf::Vector2f( xOrigin + (float)x, yOrigin - (float)sum );
        vtxVec.push_back( tempVtx );
    }

    if( x < L )// add right end point
    {
        tempVtx.position = sf::Vector2f( xOrigin + (float)L, yOrigin );
        vtxVec.push_back( tempVtx );
        std::cout << "\n right end added";
    }

    std::cout << "\nnumVtx = " << numVtx << "  vtxVec.size() = " << vtxVec.size();

    // function plot
    tempVtx.color = funcColor;
    if( inType == 'S' )// square wave
    {
        vtxVec_func.reserve( 6 );
        tempVtx.position = sf::Vector2f( xOrigin - (float)L, yOrigin );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin - (float)L, yOrigin + (float)Amp );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin, yOrigin + (float)Amp );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin, yOrigin - (float)Amp );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin + (float)L, yOrigin - (float)Amp );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin + (float)L, yOrigin );
        vtxVec_func.push_back( tempVtx );
    }
    else if( inType == 'T' )// triangle wave
    {
        vtxVec_func.reserve( 4 );
        tempVtx.position = sf::Vector2f( xOrigin - (float)L, yOrigin );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin - (float)(L/2.0), yOrigin + (float)Amp );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin + (float)(L/2.0), yOrigin - (float)Amp );
        vtxVec_func.push_back( tempVtx );
        tempVtx.position = sf::Vector2f( xOrigin + (float)L, yOrigin );
        vtxVec_func.push_back( tempVtx );
    }
}

bool lvl_Fourier::init_controls()
{
    std::ifstream fin("include/levels/lvl_Fourier/control_data.txt");
    if( !fin ) { std::cout << "\nNo plot data"; return false; }

    rePlotButt.init( fin, [this](){ sinVec.clear(); vtxVec.clear(); vtxVec_func.clear(); getPlotData(); updateCoeffsMsg(); } );
    rePlotButt.mode = 'I';
    button::RegisteredButtVec.push_back( &rePlotButt );

    drawFuncButt.init( fin, nullptr );
    drawFuncButt.setSel( true );
    button::RegisteredButtVec.push_back( &drawFuncButt );
    drawAxesButt.init( fin, nullptr );
    drawAxesButt.setSel( true );
    button::RegisteredButtVec.push_back( &drawAxesButt );

    std::function<void( std::vector<double>& )> pF = [this]( std::vector<double>& cVec ) { for( size_t n = 0; n < sinVec.size() && n < cVec.size(); ++n ) sinVec[n] = cVec[n];  updatePlot(); };
 //   drawDistA_Butt.init( fin, [this,pF](){ if( drawDistA_Butt.sel )( pF( sinVecA ); intFactor = 1.0f; drawDistB_Butt.setSel(false); ) } );// draw series A
    drawDistA_Butt.init( fin, [this,pF](){ if( drawDistA_Butt.sel ){ pF( sinVecA ); intFactor = 1.0f; drawDistB_Butt.setSel(false); } } );// draw series A
    button::RegisteredButtVec.push_back( &drawDistA_Butt );
 //   drawDistB_Butt.init( fin, [this,pF](){ if( drawDistB_Butt.sel )( pF( sinVecB ); intFactor = 0.0f; drawDistA_Butt.setSel(false); ) } );// draw series B
    drawDistB_Butt.init( fin, [this,pF](){ if( drawDistB_Butt.sel ){ pF( sinVecB ); intFactor = 0.0f; drawDistA_Butt.setSel(false); } } );// draw series B
    button::RegisteredButtVec.push_back( &drawDistB_Butt );
    animateButt.init( fin, [this](){ animating = animateButt.sel; } );// animate between A and B
    button::RegisteredButtVec.push_back( &animateButt );

    tScaleStrip.init( fin, [this](float x){ tScale = 1.0f/x; } );
    button::RegisteredButtVec.push_back( &tScaleStrip );

    return true;
}

bool lvl_Fourier::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_Fourier::update( float dt )
{
    if( !animating ) return;

    if( sinVecA.empty() || sinVecB.empty() ) return;
    if( ( sinVec.size() != sinVecA.size() ) || ( sinVec.size() != sinVecB.size() ) ) return;

//    if( tElap < tUpdate ){ tElap += dt; return; }

    static int dir = 1;
    intFactor += dir*tScale*dt;

    if( intFactor > 1.0f )
    {
        dir = -1;
        intFactor = 1.0f;
    }
    else if( intFactor < 0.0f )
    {
        dir = 1;
        intFactor = 0.0f;
    }

//    std::cout << "\nintFactor = " << intFactor;

    for( size_t i = 0; i < sinVec.size(); ++i )
    {
        sinVec[i] = intFactor*sinVecA[i] + ( 1.0 - intFactor )*sinVecB[i];
    }

    updatePlot();
}

void lvl_Fourier::draw( sf::RenderTarget& RT ) const
{
    if( drawAxesButt.sel ) RT.draw( axes, 4, sf::Lines );
    if( drawFuncButt.sel ) RT.draw( &(vtxVec_func[0]), vtxVec_func.size(), sf::LinesStrip );
    RT.draw( &(vtxVec[0]), vtxVec.size(), sf::LinesStrip );
    RT.draw( coeffsMsg );
}
