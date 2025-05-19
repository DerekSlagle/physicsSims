#include "lvl_analogElectric.h"

void analogDevice::init( std::istream& is )
{
    std::string name; is >> name;
    float posX, posY; is >> posX >> posY;
    sf::Vector2f sz; is >> sz.x >> sz.y;
    bodyRS.setPosition( posX, posY - sz.y/2.0f );
    bodyRS.setSize( sz );
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    bodyRS.setFillColor( sf::Color(rd,gn,bu) );
    // the label
    float offX, offY; is >> offX >> offY;
    unsigned int fontSz; is >> fontSz >> rd >> gn >> bu;
    label.setFont( *button::pFont );
    label.setFillColor( sf::Color(rd,gn,bu) );
    label.setCharacterSize( fontSz );
    label.setString( name.c_str() );
    label.setPosition( posX + offX, posY + offY );
}

void resistor::init( std::istream& is, double& r_I )
{
    analogDevice::init( is );
    is >> R;
    pI = &r_I;
}

void resistor::update()
{
    if( !( pI && pV_Ln && pV_Ld ) ) return;
    *pV_Ld = *pV_Ln - *pI*R;
    if( pNext ) pNext->update();
}

void capacitor::init( std::istream& is, double& r_Q )
{
    analogDevice::init( is );
    is >> C;
    pQ = &r_Q;
}

void capacitor::update()
{
    if( !( pQ && pV_Ln && pV_Ld ) ) return;
    *pV_Ld = *pV_Ln - *pQ/C;
    if( pNext ) pNext->update();
}

void inductor::init( std::istream& is, double& r_dIdt )
{
    analogDevice::init( is );
    is >> L;
    p_dIdt = &r_dIdt;
}

void inductor::update()
{
    if( !( p_dIdt && pV_Ln && pV_Ld ) ) return;
    *p_dIdt = ( *pV_Ln - *pV_Ld )/L;
    if( pNext ) pNext->update();// when would this be used?
}


// lvl_analogElectric member functions
const double lvl_analogElectric::myPI = 3.141592654;

bool lvl_analogElectric::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(120,120,120);
    button::setHoverBoxColor( sf::Color::Black );

    std::ifstream fin("include/levels/lvl_analogElectric/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    // get position for messages below
    float posX, posY; fin >> posX >> posY;

    R1.init( fin, I );
    R1.pV_Ln = &Va;
    R1.pV_Ld = &Vb;
    R1.pNext = &C1;

    C1.init( fin, Q );
    C1.pV_Ln = &Vb;
    C1.pV_Ld = &Vc;
    C1.pNext = &L1;

    L1.init( fin, dIdt );
    L1.pV_Ln = &Vc;
    L1.pV_Ld = &Vd;
    L1.pNext = nullptr;

    osciR.init(fin);
    osciVa = osciI = osciC = osciL = osciR;
    osciI.drawAxes = osciC.drawAxes = osciL.drawAxes = false;
    osciC.setPlotColor( sf::Color::Blue );
    osciL.setPlotColor( sf::Color::Red );
    osciI.setPlotColor( sf::Color::Magenta );
    osciVa.setPlotColor( sf::Color::Green );

    Va_msg.setFont( *button::pFont );
    Va_msg.setCharacterSize( 20 );
    Va_msg.setString( "Vapplied" );
    Va_msg.setPosition( posX, posY );
    Va_msg.setFillColor( osciVa.plotColor );
    I_msg = Vr_msg = Vc_msg = Vl_msg = Va_msg;
    I_msg.setString( "Current" );
    I_msg.setPosition( posX, posY + 25.0f );
    I_msg.setFillColor( osciI.plotColor );
    Vr_msg.setString( "V Resistor" );
    Vr_msg.setPosition( posX, posY + 50.0f );
    Vr_msg.setFillColor( osciR.plotColor );
    Vc_msg.setString( "V capacitor" );
    Vc_msg.setPosition( posX, posY + 75.0f );
    Vc_msg.setFillColor( osciC.plotColor );
    Vl_msg.setString( "V inductor" );
    Vl_msg.setPosition( posX, posY + 100.0f );
    Vl_msg.setFillColor( osciL.plotColor );

    fin >> VaMax >> freq >> numReps;

    float r; fin >> r;// common to all Vx
//    float posX, posY;

    fin >> posX >> posY;
    VaCS.setRadius(r);
    VaCS.setOrigin(r,r);
    VaCS.setPosition( posX, posY );
    VaCS.setFillColor( sf::Color::Black );

    VbCS = VcCS = VdCS = VaCS;
    fin >> posX >> posY; VbCS.setPosition( posX, posY );
    fin >> posX >> posY; VcCS.setPosition( posX, posY );
    fin >> posX >> posY; VdCS.setPosition( posX, posY );

    phaseAngle = 0.0f;
    fin >> Q >> I >> dIdt;

    fin.close();

    Vd = 0.0f;

    if( !init_controls() ) return false;

    return true;
}

bool lvl_analogElectric::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_analogElectric::update( float dt )
{
    size_t num_reps = numReps;
    if( sloMoButt.sel )
        num_reps = 1;
    else
        dt /= numReps;

    for( size_t i = 0; i < num_reps; ++i )
    {
        phaseAngle += 2.0*myPI*freq*dt;
        if( phaseAngle > 2.0*myPI ) phaseAngle -= 2.0*myPI;
        Va = VaMax*sinf( phaseAngle );

        osciR.update( R1.getVoltageDrop(), dt );
        osciC.update( C1.getVoltageDrop(), dt );
        osciL.update( L1.getVoltageDrop(), dt );
        osciI.update( I, dt );
        osciVa.update( Va, dt );

//        R1.update();

        I += dIdt*dt;
        Q += I*dt;
        R1.update();
//        dIdt = ( Va - I*R1.R - Q/C1.C )/L1.L;
    }

}

void lvl_analogElectric::draw( sf::RenderTarget& RT ) const
{
    R1.draw(RT);
    C1.draw(RT);
    L1.draw(RT);

    RT.draw( VaCS );
    RT.draw( VbCS );
    RT.draw( VcCS );
    RT.draw( VdCS );

    osciR.draw(RT);
    osciC.draw(RT);
    osciL.draw(RT);
    osciI.draw(RT);
    osciVa.draw(RT);

    RT.draw( Va_msg );
    RT.draw( I_msg );
    RT.draw( Vr_msg );
    RT.draw( Vc_msg );
    RT.draw( Vl_msg );
}


bool lvl_analogElectric::init_controls()
{
    std::ifstream fin("include/levels/lvl_analogElectric/control_data.txt");
    if( !fin ) { std::cout << "\nNo control data"; return false; }

    // paramsCS
    sf::Vector2f hbPos, csPos, csDims;
    fin >> hbPos.x >> hbPos.y >> csPos.x >> csPos.y >> csDims.x >> csDims.y;

    paramsCS.init( hbPos, csPos, csDims, "params" );
    paramsCS.pButtVec.push_back( &paramsMS );
    button::RegisteredButtVec.push_back( &paramsCS );

    sf::Vector2f ofst(10.0f,10.0f);
    paramsMS.init( csPos + ofst, &paramsStrip, fin );

    if( paramsMS.sdataVec.size() != 5 ) { std::cout << "\nbad globalMS data"; return false; }
    stripData* pSD = &paramsMS.sdataVec.front();
    // R
    pSD->pSetFunc =     [this](float x){ R1.R = x; };
    paramsStrip.reInit( *pSD );
    R1.R = pSD->xCurr;
     // L
    (++pSD)->pSetFunc = [this](float x){ L1.L = x; };
    L1.L = pSD->xCurr;
    // C
    (++pSD)->pSetFunc = [this](float x){ C1.C = x; };
    C1.C = pSD->xCurr;
    // VaMax
    (++pSD)->pSetFunc = [this](float x){ VaMax = x; };
    VaMax = pSD->xCurr;

    // freq
    std::function<void(float)> pSetFreq = [this](float x)
                        {
                            freq = x;
                            osciI.set_tSweep( (float)numPeriods/freq );
                            osciR.set_tSweep( (float)numPeriods/freq );
                            osciC.set_tSweep( (float)numPeriods/freq );
                            osciL.set_tSweep( (float)numPeriods/freq );
                            osciVa.set_tSweep( (float)numPeriods/freq );
                            numReps = 2*(size_t)freq;
                            std::cout << "\nnumReps = " << numReps;
                        };

    (++pSD)->pSetFunc = [this,pSetFreq](float x){ pSetFreq(x); };
    freq = pSD->xCurr;

    // the numPeriods selector
    numPeriodsSelector.init( fin, nullptr );
    numPeriodsSelector.title.setFillColor( sf::Color::White );
    numPeriodsSelector.p_setFunc = [this,pSetFreq](unsigned int n){ numPeriods = n; pSetFreq(freq); };
    paramsCS.pButtVec.push_back( &numPeriodsSelector );

    // Testing: adjust freq *= or /= 1.01f while button is held
    static double Freq = 1.0f;
    pb_freqUp.init( sf::Vector2f(10,400), sf::Vector2f(50,24), 'O', "UP", [this,&Freq](){ Freq = freq; freq *= 1.01; }, [this,&Freq](){ freq = Freq; } );
    button::RegisteredButtVec.push_back( &pb_freqUp );
    pb_freqDown.init( sf::Vector2f(10,460), sf::Vector2f(50,24), 'O', "DOWN", [this,&Freq](){ Freq = freq; freq /= 1.01; }, [this,&Freq](){ freq = Freq; } );
    button::RegisteredButtVec.push_back( &pb_freqDown );

    pb_VsumButt.init( sf::Vector2f(10,520), sf::Vector2f(50,24), 'O', "Vsum", [this](){ std::cout << "\nVsum = " << Va - R1.getVoltageDrop() - C1.getVoltageDrop() - L1.getVoltageDrop(); }, nullptr );
    button::RegisteredButtVec.push_back( &pb_VsumButt );

    sf::Text label( "SloMo", *button::pFont, 12 );
    label.setFillColor( sf::Color::Black );
    sloMoButt.init( 10, 580, 50, 24, nullptr, label );
    button::RegisteredButtVec.push_back( &sloMoButt );

    return true;
}
