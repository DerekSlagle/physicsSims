#include "lvl_electricControls.h"

bool lvl_electricControls::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(200,200,200);
    button::setHoverBoxColor( Level::clearColor );
    button::moValtxt.setFillColor( sf::Color::Black );

    sf_terminal::doDrawWires = false;

    std::function<void(void)> pFuncClose = [this]()
    {
        // start the next motor
        sf_terminal* pTerm = nullptr;
        if( con2LsVec[0].state && !con2LsVec[0].com.state ) pTerm = &con2LsVec[0].com;
        else if( con2LsVec[1].state && !con2LsVec[1].com.state ) pTerm = &con2LsVec[1].com;
        else if( con2LsVec[2].state && !con2LsVec[2].com.state ) pTerm = &con2LsVec[2].com;

        if( pTerm )
        {
            pTerm->setState(true);
            pTerm->setState(false);
        }
    };
    //                                                                                  pFuncClose                       pFuncOpen
    runButt.init( sf::Vector2f(960,300), sf::Vector2f(80,40), 'O', "RUN", [pFuncClose](){ pFuncClose(); }, nullptr );
    button::RegisteredButtVec.push_back( &runButt );
    sf::Text Title( "speed", *button::pFont, 14 );
    Title.setFillColor( sf::Color::Black );
    speedStrip.init( 900, 240, 160.0f, 20.0f, Title, 10.0f, 300.0f, 80.0f, &Vld );
    speedStrip.setLabelColors( sf::Color::Black );
    button::RegisteredButtVec.push_back( &speedStrip );

    // startStopControl
    const unsigned int numControls = 3;
    ssControlVec.reserve( numControls );
    const std::string fName[numControls] = {
        "include/levels/lvlElectric/init_data_ssControl.txt",
        "include/levels/lvlElectric/init_data_ssControl2.txt",
        "include/levels/lvlElectric/init_data_ssControl3.txt"
    };

    for( unsigned int j = 0; j < numControls; ++j )
    {
        startStopControl temp;

        ssControlVec.push_back( temp );
        startStopControl& rSSC = ssControlVec.back();
        if( !rSSC.init( nullptr, nullptr, nullptr, fName[j].c_str() ) )
        { std::cout << "\n no " << fName[j].c_str() << " file"; return false; }

        button::RegisteredButtVec.push_back( &rSSC.pbStart.pbButt );
        button::RegisteredButtVec.push_back( &rSSC.pbStop.pbButt );
    }
    for( unsigned int j = 0; j + 1 < numControls; ++j )
        ssControlVec[j].busA.pNext = &(ssControlVec[j+1].busA);



    std::ifstream fin("include/levels/lvlElectric/init_dataA.txt");
    if( !fin ) { std::cout << "\nNo init_dataA"; return false; }

    bool* pSel = &cPowerButt.sel;
    cPowerButt.init( fin, [this,pSel](){ t_Ln1.setState(*pSel); } );// control power
    button::RegisteredButtVec.push_back( &cPowerButt );
    LdPowerButt.init( fin, [this](){ bool st = LdPowerButt.sel; t_Power1.setState(st); ssControlVec[1].conP.com.setState(st); ssControlVec[2].conP.com.setState(st); } );//Load power
    button::RegisteredButtVec.push_back( &LdPowerButt );
    // TEMP test
    testButt.init( fin, nullptr );
    button::RegisteredButtVec.push_back( &testButt );

    t_Ln1.init( &(ssControlVec[0].busA), fin );// control power
    t_Power1.init( &(ssControlVec[0].conP.com), fin );// Load power


    const unsigned int numLoadTerms = 6;
    loadTermVec.reserve( numLoadTerms );
    sf::CircleShape* pCS = &loadCS;
    std::function<void(bool)> pFunc = [pCS](bool st){ if(st) pCS->setFillColor(sf::Color::Green); else pCS->setFillColor(sf::Color::Blue); };
    for( unsigned int j = 0; j < numLoadTerms; ++j )
    {
        loadTermVec.push_back( sf_loadTerminal() );
        loadTermVec.back().init( pFunc, nullptr, fin );// loadTerminal
    }

//    t_LdLt = loadTermVec[0];
//    t_LdRt = loadTermVec[1];
 //   t_LdUp = loadTermVec[2];
 //   t_LdLt_rev = loadTermVec[3];
 //   t_LdRt_rev = loadTermVec[4];
 //   t_LdUp_rev = loadTermVec[5];

 //   t_LdLt.init( [this](bool st){ if(st) loadCS.setFillColor(sf::Color::Green); else loadCS.setFillColor(sf::Color::Red); }, nullptr, fin );// loadTerminal
 //   t_LdLt_rev.init( [this](bool st){ if(st) loadCS.setFillColor(sf::Color::Green); else loadCS.setFillColor(sf::Color::Red); }, nullptr, fin );// loadTerminal
 //   t_LdRt.init( [this](bool st){ if(st) loadCS.setFillColor(sf::Color::Blue); else loadCS.setFillColor(sf::Color::Red); }, nullptr, fin );// loadTerminal
 //   loadTermVec[4].init( [this](bool st){ if(st) loadCS.setFillColor(sf::Color::Blue); else loadCS.setFillColor(sf::Color::Red); }, nullptr, fin );// loadTerminal
 //   loadTermVec[2].init( [this](bool st){ if(st) loadCS.setFillColor(sf::Color::White); else loadCS.setFillColor(sf::Color::Red); }, nullptr, fin );// loadTerminal
  //  t_LdUp_rev.init( [this](bool st){ if(st) loadCS.setFillColor(sf::Color::White); else loadCS.setFillColor(sf::Color::Red); }, nullptr, fin );// loadTerminal

    float R; fin >> R;
    loadCS.setRadius(R);
    loadCS.setOrigin(R,R);
    float posX, posY; fin >> posX >> posY;// in file but no longer used

    // limitSwitch
    LsVec.reserve( numControls );
    con2LsVec.reserve( numControls );
    for( unsigned int j = 0; j < numControls; ++j )
    {
        con2LsVec.push_back( sf_contact() );
        sf_contact& rCon = con2LsVec.back();

        LsVec.push_back( sf_limitSwitch() );
        sf_limitSwitch& rLS = LsVec.back();
        rLS.init( fin, nullptr, nullptr );
        // wire limit switch in series with the stop button
        ssControlVec[j].pbStop.Ld.pNext = &( rLS.com );
        rLS.nc.pNext = &( ssControlVec[j].conC );
        rLS.pNext = &rCon;// add 2nd contact
    }

    std::string msg; fin >> msg;
    std::cout << '\n' << msg;

    LsPowerButt.init( fin, [this](){ bool st = LsPowerButt.sel; con2LsVec[0].com.setState(st); con2LsVec[1].com.setState(st); con2LsVec[2].com.setState(st); } );// power to LS 2nd contacts
    button::RegisteredButtVec.push_back( &LsPowerButt );
    revSelectButt.init( fin, [this](){ bool st = revSelectButt.sel; for( unsigned int i = 0; i < 3; ++i ){ revConMC[i].setState(st); revConLD[i].setState(st); } } );// power to LS 2nd contacts
    button::RegisteredButtVec.push_back( &revSelectButt );

    const unsigned int idx[numControls] = { 1, 2, 0 };
    for( unsigned int j = 0; j < numControls; ++j )
    {
        con2LsVec[j].init( &( ssControlVec[ idx[j] ].pbStart.Ld ), nullptr, fin, nullptr );
    }

    float W, H; fin >> msg >> posX >> posY >> W >> H;
 //   stopAllButt.init( sf::Vector2f(posX,posY), sf::Vector2f(W,H), 'C', msg.c_str(),
 //       [this](){ ssControlVec[0].pbStop.setState(true); ssControlVec[1].pbStop.setState(true); ssControlVec[2].pbStop.setState(true); },
 //       [this](){ ssControlVec[0].pbStop.setState(false); ssControlVec[1].pbStop.setState(false); ssControlVec[2].pbStop.setState(false); }
 //       );
    stopAllButt.init( sf::Vector2f(posX,posY), sf::Vector2f(W,H), 'C', msg.c_str(),
        [this](){ for( auto& ssc : ssControlVec ) ssc.pbStop.setState(true); },
        [this](){ for( auto& ssc : ssControlVec ) ssc.pbStop.setState(false); }
        );
    button::RegisteredButtVec.push_back( &stopAllButt );

    float offXa, offYa; fin >> offXa >> offYa;
    float offXb, offYb; fin >> offXb >> offYb;

    // testCon
    sf::Vector2f tcPos; fin >> tcPos.x >> tcPos.y;
    float tcW; fin >> tcW;
    unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
    testCon.init( nullptr, nullptr, tcPos, tcW, 'R', sf::Color(rd,gn,bu), "testCon", nullptr );
    testCon.com.setState( true );

    fin.close();

    loadCS.setPosition( ( LsVec[0].vtx[2].position.x + LsVec[1].vtx[2].position.x )*0.5f, LsVec[0].vtx[2].position.y );
    loadCS.setFillColor(sf::Color::Red);

    rod[0].color = rod[1].color = rod[2].color = rod[3].color = sf::Color::Black;
    rod[0].position = LsVec[0].vtx[2].position;
    rod[0].position.x += LsVec[0].Xopen;
    rod[1].position = LsVec[1].vtx[2].position;
    rod[1].position.x -= LsVec[1].Xopen;
    rod[2].position = LsVec[2].vtx[2].position;
    rod[2].position.y += LsVec[2].Xopen;
    rod[3].position = rod[0].position;

    sepU_Rt = rod[1].position - rod[0].position;
    float len = sqrtf( sepU_Rt.x*sepU_Rt.x + sepU_Rt.y*sepU_Rt.y );
    sepU_Rt.x /= len; sepU_Rt.y /= len;

    sepU_Up = rod[2].position - rod[1].position;
    len = sqrtf( sepU_Up.x*sepU_Up.x + sepU_Up.y*sepU_Up.y );
    sepU_Up.x /= len; sepU_Up.y /= len;

    sepU_Lt = rod[0].position - rod[2].position;
    len = sqrtf( sepU_Lt.x*sepU_Lt.x + sepU_Lt.y*sepU_Lt.y );
    sepU_Lt.x /= len; sepU_Lt.y /= len;

    // a 3rd set of contacts for each limit switch
    // Lt
    revConMC[0] = con2LsVec[0];
 //   revConMC[0].com.pNext = revConMC[0].state ? &revConMC[0].no : &revConMC[0].nc;
    revConMC[0].tmLabel.setString("revConLt");
    revConMC[0].setPosition( sf::Vector2f( con2LsVec[0].getPosition().x + offXa, con2LsVec[0].getPosition().y + offYa ) );
    revConMC[0].no.pNext = &ssControlVec[2].pbStart.Ld;
    revConMC[0].nc.pNext = &ssControlVec[1].pbStart.Ld;
    con2LsVec[0].no.pNext = &revConMC[0].com;
    //Rt
    revConMC[1] = con2LsVec[1];
 //   revConMC[1].com.pNext = revConMC[1].state ? &revConMC[1].no : &revConMC[1].nc;
    revConMC[1].tmLabel.setString("revConLt");
    revConMC[1].setPosition( sf::Vector2f( con2LsVec[1].getPosition().x + offXa, con2LsVec[1].getPosition().y + offYa ) );
    revConMC[1].no.pNext = &ssControlVec[0].pbStart.Ld;
    revConMC[1].nc.pNext = &ssControlVec[2].pbStart.Ld;
    con2LsVec[1].no.pNext = &revConMC[1].com;
    // Up
    revConMC[2] = con2LsVec[2];
 //   revConMC[2].com.pNext = revConMC[2].state ? &revConMC[2].no : &revConMC[2].nc;
    revConMC[2].tmLabel.setString("revConLt");
    revConMC[2].setPosition( sf::Vector2f( con2LsVec[2].getPosition().x + offXa, con2LsVec[2].getPosition().y + offYa ) );
    revConMC[2].no.pNext = &ssControlVec[1].pbStart.Ld;
    revConMC[2].nc.pNext = &ssControlVec[0].pbStart.Ld;
    con2LsVec[2].no.pNext = &revConMC[2].com;
    // Lt
    revConLD[0] = con2LsVec[2];
 //   revConLD[0].com.pNext = &revConLD[0].nc;
    revConLD[0].tmLabel.setString("revConLt_Ld");
    revConLD[0].setPosition( sf::Vector2f( ssControlVec[0].conP.no.getPosition().x + offXb, ssControlVec[0].conP.no.getPosition().y + offYb ) );
    revConLD[0].nc.pNext = &loadTermVec[0];
    revConLD[0].no.pNext = &loadTermVec[4];//
    ssControlVec[0].conP.no.pNext = &revConLD[0].com;
    // Rt
    revConLD[1] = con2LsVec[1];
 //   revConLD[1].com.pNext = &revConLD[1].nc;
    revConLD[1].tmLabel.setString("revConRt_Ld");
    revConLD[1].setPosition( sf::Vector2f( ssControlVec[1].conP.no.getPosition().x + offXb, ssControlVec[1].conP.no.getPosition().y + offYb ) );
    revConLD[1].nc.pNext = &loadTermVec[1];
    revConLD[1].no.pNext = &loadTermVec[5];//
    ssControlVec[1].conP.no.pNext = &revConLD[1].com;
    // Up
    revConLD[2] = con2LsVec[2];
 //   revConLD[2].com.pNext = &revConLD[2].nc;
    revConLD[2].tmLabel.setString("revConUp_Ld");
    revConLD[2].setPosition( sf::Vector2f( ssControlVec[2].conP.no.getPosition().x + offXb, ssControlVec[2].conP.no.getPosition().y + offYb ) );
    revConLD[2].nc.pNext = &loadTermVec[2];
    revConLD[2].no.pNext = &loadTermVec[3];
    ssControlVec[2].conP.no.pNext = &revConLD[2].com;

    // fill sf_terminal* containers
    pTermBindToOK.push_back( &t_Ln1 );
    pTermBindToOK.push_back( &t_Power1 );
    for( sf_limitSwitch& LS : LsVec ) pTermBindToOK.push_back( &LS );// all limit switches
    for( sf_contact& C : con2LsVec )// all 2nd contacts slaved to above
    {
        pTermBindToOK.push_back( &C );// the "coil"
        pTermBindToOK.push_back( &C.com );
    }
    for( unsigned int n = 0; n < 3; ++n )
    {
        pTermBindToOK.push_back( &revConLD[n] );
        pTermBindToOK.push_back( &revConLD[n].com );
    }

    pTermBindFromOK = pTermBindToOK;// all on both lists, for now
    // bind to ok only
    pTermBindToOK.push_back( &testCon );
    pTermBindToOK.push_back( &testCon.com );

    // bind from ok only

    initWires3();

    // test
  //  sf_terminal* pTerm = nullptr;

    return true;
}

// only wiring to ssControlVec[0]
void lvl_electricControls::initWires3()
{
    // line 1 : t_Ln1 to busA
    wireVec.push_back( sf::Vertex( t_Ln1.getPosition(), sf::Color::Black ) );
    wireVec.push_back( sf::Vertex( ssControlVec[0].busA.getPosition(), sf::Color::Black ) );
    // lines 2 : t_Power1 to conP.com
    wireVec.push_back( sf::Vertex( t_Power1.getPosition(), sf::Color::Red ) );
    wireVec.push_back( sf::Vertex( ssControlVec[0].conP.com.getPosition(), sf::Color::Red ) );
    // to the 6 t_Ld
    findWiringPath( revConLD[0].nc.getPosition(), 'H', loadTermVec[0].getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( revConLD[1].nc.getPosition(), 'H', loadTermVec[1].getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( revConLD[2].nc.getPosition(), 'H', loadTermVec[2].getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( revConLD[2].no.getPosition(), 'H', loadTermVec[3].getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( revConLD[1].no.getPosition(), 'H', loadTermVec[5].getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( revConLD[0].no.getPosition(), 'H', loadTermVec[4].getPosition(), 'V', wireVec, sf::Color::Red );

    // conP to revConLD
    findWiringPath( ssControlVec[0].conP.no.getPosition(), 'H', revConLD[0].com.getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( ssControlVec[1].conP.no.getPosition(), 'H', revConLD[1].com.getPosition(), 'V', wireVec, sf::Color::Red );
    findWiringPath( ssControlVec[2].conP.no.getPosition(), 'H', revConLD[2].com.getPosition(), 'V', wireVec, sf::Color::Red );
    // con2.no to revConLN.com
    findWiringPath( con2LsVec[0].no.getPosition(), 'H', revConMC[0].com.getPosition(), 'V', wireVec, sf::Color::Black );
    findWiringPath( con2LsVec[1].no.getPosition(), 'H', revConMC[1].com.getPosition(), 'V', wireVec, sf::Color::Black );
    findWiringPath( con2LsVec[2].no.getPosition(), 'H', revConMC[2].com.getPosition(), 'V', wireVec, sf::Color::Black );
}

// values for dir1 and dir2 sre 'V' (go vertical) and 'H'. 4 or 6 sf::Vertex will be pushed into vtxVec
void lvl_electricControls::findWiringPath( sf::Vector2f pos1, char dir1, sf::Vector2f pos2, char dir2, std::vector<sf::Vertex>& vtxVec, sf::Color clr )const
{
    size_t nLines = 2;// or maybe 3
    sf::Vector2f pt1, pt2;// bends in path. pt2 is used when nLines = 3

    if( dir1 == 'H' )
    {
        if( dir2 == 'H' )
        {
            pt1 = sf::Vector2f( ( pos1.x + pos2.x )/2.0f, pos1.y );
            pt2 = sf::Vector2f( ( pos1.x + pos2.x )/2.0f, pos2.y );
            nLines = 3;
        }
        else// dir2 == 'V'
        {
            pt1 = sf::Vector2f( pos2.x, pos1.y );
            nLines = 2;
        }
    }
    else// dir1 == 'V'
    {
        if( dir2 == 'H' )
        {
            pt1 = sf::Vector2f( pos1.x, pos2.y );
            nLines = 2;
        }
        else// dir2 == 'V'
        {
            pt1 = sf::Vector2f( pos1.x, ( pos1.y + pos2.y )/2.0f );
            pt2 = sf::Vector2f( pos2.x, ( pos1.y + pos2.y )/2.0f );
            nLines = 3;
        }
    }

    // line 1: pos1 to pt1
    vtxVec.push_back( sf::Vertex( pos1, clr ) );
    vtxVec.push_back( sf::Vertex( pt1, clr ) );
    vtxVec.push_back( sf::Vertex( pt1, clr ) );
    if( nLines == 3 )
    {
        vtxVec.push_back( sf::Vertex( pt2, clr ) );
        vtxVec.push_back( sf::Vertex( pt2, clr ) );
    }

    vtxVec.push_back( sf::Vertex( pos2, clr ) );
}

bool lvl_electricControls::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_electricControls::handleTerminalHitAll()
{
    const sf::Color termSelColor = sf::Color(0,200,0);

    if( pTermSel )// bind (assign pNext) or clear = 2nd click
    {
        for( sf_terminal* pt : pTermBindToOK )
            if( pt->pos_isOver( button::msePos() ) )
            {
                pTermSel->pNext = pt;// assign
                std::cout << "\nterminal hit: " << pt << " and bound";
                pTermSel->tmCS.setFillColor( sf_contact::termColorOff );
                pTermSel = nullptr;
                break;
            }

        if( pTermSel )// clear it then
        {
            pTermSel->pNext = nullptr;
            pTermSel->tmCS.setFillColor( sf_contact::termColorOff );
            pTermSel = nullptr;// C back to initial state
        }
    }
    else// handle 1st click = selection
    {
        for( sf_terminal* pt : pTermBindFromOK )
            if( pt->pos_isOver( button::msePos() ) )
            {
                pTermSel = pt;// assign
                std::cout << "\nterminal selected: " << pt;
                pTermSel->tmCS.setFillColor( termSelColor );
                break;
            }
    }
}

void lvl_electricControls::update( float dt )
{
    if( !button::pButtMse )
    {
        if( button::clickEvent_Lt() == 1 )
        {
            // limit switch hits
            bool hitLS = false;
            for( unsigned int j = 0; j < LsVec.size(); ++j )
            {
                if( button::hitWithinR( LsVec[j].vtx[2].position, 10.0f ) )
                {
                    LsVec[j].setState( !LsVec[j].state );
                    hitLS = true;
                    break;
                }
            }

            if( !hitLS )// hit testCon?
            {
                if( testButt.sel )
                {
                    handleTerminalHitAll();
                }
                else
                {
                    handleContactHit( testCon );
                }

            }
        }
        else if( button::clickEvent_Rt() == -1 )
            loadCS.setPosition( button::msePos() );
    }

    // move load?
    static sf::Vector2f sepUvec[] = { sepU_Lt, sepU_Rt, sepU_Up, -sepU_Lt, -sepU_Rt, -sepU_Up };
    sf::Vector2f ldPos = loadCS.getPosition();
    bool didMove = false;
    for( unsigned int j = 0; j < loadTermVec.size(); ++j )
    {
        if( loadTermVec[j].state )
        {
            ldPos += sepUvec[j]*Vld*dt;
            didMove = true;
        }
    }

    if( didMove )
    {
        loadCS.setPosition( ldPos );
   //     float x = ( ldPos.x - LsVec[0].getSwitchPosition().x )*LsVec[0].Nu.x;
   //     LsVec[0].update( x );
   //     x = ( ldPos.x - LsVec[1].getSwitchPosition().x )*LsVec[1].Nu.x;
   //     LsVec[1].update( x );
   //     x = ( ldPos.y - LsVec[2].getSwitchPosition().y )*LsVec[2].Nu.y;
   //     LsVec[2].update( x );

        LsVec[0].update( ldPos );
        LsVec[1].update( ldPos );
        LsVec[2].update( ldPos );
    }
}

void lvl_electricControls::handleContactHit( sf_contact& C )
{
    // testCon
         //       static sf_terminal* pTermSel = nullptr;
    sf_terminal* pTermHitNow = nullptr;
//       bool tcHit = testCon.pos_isOver( button::msePos(), pTermHit );// pTermHit overwritten on 2nd click
    bool tcHit = C.pos_isOver( button::msePos(), pTermHitNow );

    static bool selToMove = false;
    static sf::Color tcUnselColor = C.bodyRS.getFillColor();// issue here with initial value
//      const sf::Color tcSelColor = sf::Color(200,100,0);

    static sf::Color termUnselColor = sf_terminal::termColorOff;
    const sf::Color termSelColor = sf::Color(0,200,0);
    const sf::Color moveSelColor = sf::Color(200,0,100);

    if( tcHit )
    {
        std::cout << "\nContact hit!  ";

        if( selToMove )// deselect
        {
            C.bodyRS.setFillColor( tcUnselColor );
            selToMove = false;
            if( pTermSel ){ pTermSel->tmCS.setFillColor( termUnselColor ); pTermSel->pNext = nullptr; }
            pTermSel = nullptr;
        }
        else if( pTermHitNow )// program or operate
        {
            if( pTermHitNow == &C )// operate
            {
                C.setState( !C.state );
                 std::cout << "coil hit. state: " << ( C.state ? "true" : "false" );
            }
            else if( pTermHitNow == &C.com )// toggle state
            {
                C.com.setState( !C.com.state );
                 std::cout << "com hit. state: " << ( C.com.state ? "true" : "false" );
            }
            else// selected to program
            {
                if( pTermHitNow == &C.com ) std::cout << "com hit";
                if( pTermHitNow == &C.no ) std::cout << "no hit";
                if( pTermHitNow == &C.nc ) std::cout << "nc hit";
             //   if( tcSel ) testCon.bodyRS.setFillColor( tcUnselColor );
             //   tcSel = false;

                if( pTermHitNow == &C.no || pTermHitNow == &C.nc )
                {
                    if( pTermSel )// deselect
                    {
                        pTermSel->tmCS.setFillColor( termUnselColor );
                        pTermSel = nullptr;
                    }
                    else// select
                    {
                        pTermSel = pTermHitNow;
                        pTermSel->tmCS.setFillColor( termSelColor );
                    }
                }
            }
        }
        else// select to move
        {
            selToMove = true;
            std::cout << "no terminal hit. selToMove = true";
            C.tmCS.setFillColor( termUnselColor );
            C.bodyRS.setFillColor( moveSelColor );
        }
    }
    else// C not hit
    {
        std::cout << "\ntestCon missed";
        if( selToMove )// move
        {
            C.setPosition( button::msePos() );
            C.bodyRS.setFillColor( tcUnselColor );
            selToMove = false;
            if( pTermSel ) { pTermSel->tmCS.setFillColor( termUnselColor ); pTermSel->pNext = nullptr; }
            pTermSel = nullptr;
            std::cout << " and moved.";
        }
        else if( pTermSel )// find if another terminal was hit and assign pTermHit->pNext = &termHit
        {
            std::cout << "\ntestCon missed. assign pTermHit->pNext?";
            for( sf_terminal* pt : pTermBindToOK )
                if( pt->pos_isOver( button::msePos() ) )
                {
                    pTermSel->pNext = pt;// assign
                    std::cout << "\nterminal hit: " << pt << " and bound";
                    pTermSel->tmCS.setFillColor( termUnselColor );
                    pTermSel = nullptr;// C back to initial state
                    break;
                }

            if( pTermSel )
            {
                pTermSel->pNext = nullptr;
                pTermSel->tmCS.setFillColor( termUnselColor );
                pTermSel = nullptr;// C back to initial state
            }
        }
    }// end if C not hit
}

/*
void lvl_electricControls::update( float dt )
{
    if( !button::pButtMse )
    {
        if( button::clickEvent_Lt() == 1 )
        {
            // limit switch hits
            bool hitLS = false;
            for( unsigned int j = 0; j < LsVec.size(); ++j )
            {
                if( button::hitWithinR( LsVec[j].vtx[2].position, 10.0f ) )
                {
                    LsVec[j].setState( !LsVec[j].state );
                    hitLS = true;
                    break;
                }
            }

       //     if( !hitLS )// hit testCon?
            if( !( hitLS || testButt.sel ) )// not in program mode
            {
                // testCon
         //       static sf_terminal* pTermSel = nullptr;
                sf_terminal* pTermHitNow = nullptr;
         //       bool tcHit = testCon.pos_isOver( button::msePos(), pTermHit );// pTermHit overwritten on 2nd click
                bool tcHit = testCon.pos_isOver( button::msePos(), pTermHitNow );

                static bool selToMove = false;
                static sf::Color tcUnselColor = testCon.bodyRS.getFillColor();
                const sf::Color tcSelColor = sf::Color(200,100,0);

                static sf::Color termUnselColor = sf_terminal::termColorOff;
                const sf::Color termSelColor = sf::Color(0,200,0);
                const sf::Color moveSelColor = sf::Color(200,0,100);

                if( tcHit )
                {
                    std::cout << "\ntestCon hit!  ";

                    if( selToMove )// deselect
                    {
                        testCon.bodyRS.setFillColor( tcUnselColor );
                        selToMove = false;
                        if( pTermSel ){ pTermSel->tmCS.setFillColor( termUnselColor ); pTermSel->pNext = nullptr; }
                        pTermSel = nullptr;
                    }
                    else if( pTermHitNow )// program or operate
                    {
                        if( pTermHitNow == &testCon )// operate
                        {
                            testCon.setState( !testCon.state );
                             std::cout << "coil hit. state: " << ( testCon.state ? "true" : "false" );
                        }
                        else if( pTermHitNow == &testCon.com )// toggle state
                        {
                            testCon.com.setState( !testCon.com.state );
                             std::cout << "com hit. state: " << ( testCon.com.state ? "true" : "false" );
                        }
                        else// selected to program
                        {
                            if( pTermHitNow == &testCon.com ) std::cout << "com hit";
                            if( pTermHitNow == &testCon.no ) std::cout << "no hit";
                            if( pTermHitNow == &testCon.nc ) std::cout << "nc hit";
                         //   if( tcSel ) testCon.bodyRS.setFillColor( tcUnselColor );
                         //   tcSel = false;

                            if( pTermHitNow == &testCon.no || pTermHitNow == &testCon.nc )
                            {
                                if( pTermSel )// deselect
                                {
                                    pTermSel = nullptr;
                                    pTermSel->tmCS.setFillColor( termUnselColor );
                                }
                                else// select
                                {
                                    pTermSel = pTermHitNow;
                                    pTermSel->tmCS.setFillColor( termSelColor );
                                }
                            }
                        }
                    }
                    else// select to move
                    {
                        selToMove = true;
                        std::cout << "no terminal hit. selToMove = true";
                        testCon.tmCS.setFillColor( termUnselColor );
                        testCon.bodyRS.setFillColor( moveSelColor );
                    }
                }
                else// testCon not hit
                {

                    std::cout << "\ntestCon missed";
                    if( selToMove )// move
                    {
                        testCon.setPosition( button::msePos() );
                        testCon.bodyRS.setFillColor( tcUnselColor );
                        selToMove = false;
                        if( pTermSel ) { pTermSel->tmCS.setFillColor( termUnselColor ); pTermSel->pNext = nullptr; }
                        pTermSel = nullptr;
                        std::cout << " and moved.";
                    }
                    else if( pTermSel )// find if another terminal was hit and assign pTermHit->pNext = &termHit
                    {
                        std::cout << "\ntestCon missed. assign pTermHit->pNext?";
                        bool hit = false;
                        for( sf_terminal* pt : pTermBindToOK )
                            if( pt->pos_isOver( button::msePos() ) )
                            {
                        //        pTermHit->pNext = ( pTermHit->pNext == pt ) ? nullptr : pt;// toggle assignment to same pt
                                pTermSel->pNext = pt;// assign
                                std::cout << "\nterminal hit: " << pt << " and bound";
                                pTermSel->tmCS.setFillColor( termUnselColor );
                                pTermSel = nullptr;// testCon back to initial state
                                hit = true;
                                break;
                            }

                        if( !hit )
                        {
                            pTermSel->pNext = nullptr;
                            pTermSel->tmCS.setFillColor( termUnselColor );
                            pTermSel = nullptr;// testCon back to initial state
                        }
                    }
                }// end if testCon not hit
            }// end if( !hitLS )
        }
        else if( button::clickEvent_Rt() == -1 )
            loadCS.setPosition( button::msePos() );
    }

    // move load?
    static sf::Vector2f sepUvec[] = { sepU_Lt, sepU_Rt, sepU_Up, -sepU_Lt, -sepU_Rt, -sepU_Up };
    sf::Vector2f ldPos = loadCS.getPosition();
    bool didMove = false;
    for( unsigned int j = 0; j < loadTermVec.size(); ++j )
    {
        if( loadTermVec[j].state )
        {
            ldPos += sepUvec[j]*Vld*dt;
            didMove = true;
        }
    }

    if( didMove )
    {
        loadCS.setPosition( ldPos );
        LsVec[0].update( ldPos.x );
        LsVec[1].update( ldPos.x );
        LsVec[2].update( ldPos.y );
    }
}
*/

void lvl_electricControls::draw( sf::RenderTarget& RT ) const
{
    for( const startStopControl& rSSC : ssControlVec ) rSSC.draw(RT);
    RT.draw( &( wireVec[0] ), wireVec.size(), sf::Lines );
    t_Ln1.draw(RT);
    t_Power1.draw(RT);
    for( const sf_loadTerminal& rLT : loadTermVec ) rLT.draw(RT);

    RT.draw( rod, 4, sf::LineStrip );
    for( const sf_limitSwitch& rLS : LsVec ) rLS.draw(RT);
    for( const sf_contact& rCon : con2LsVec ) rCon.draw(RT);

    for( unsigned int i = 0; i < 3; ++i ) revConMC[i].draw(RT);
    for( unsigned int i = 0; i < 3; ++i ) revConLD[i].draw(RT);

    RT.draw( loadCS );

    testCon.draw(RT);
}
