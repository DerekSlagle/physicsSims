#include "lvl_ControlBuilder.h"

bool lvl_ControlBuilder::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(170,170,200);

    std::ifstream fin("include/levels/lvlControlBuilder/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    fin >> termVecCapacity; std::cout << "\n termVecCapacity = " << termVecCapacity;
    terminalVec.reserve( termVecCapacity );

    fin >> conVecCapacity; std::cout << "\n conVecCapacity = " << conVecCapacity;
    contactVec.reserve( conVecCapacity );

    loadTermVec.reserve( termVecCapacity );
    pushButtVec.reserve( termVecCapacity );
    orGateVec.reserve( termVecCapacity );
    bus2outVec.reserve( termVecCapacity );
    LimitSwitchVec.reserve( termVecCapacity );

    unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
    contactColor = sf::Color(rd,gn,bu);
    fin >> rd >> gn >> bu;
    termSelColor = sf::Color(rd,gn,bu);
    fin >> rd >> gn >> bu;
    contactSelColor = sf::Color(rd,gn,bu);
    fin >> rd >> gn >> bu;
    moveSelColor = sf::Color(rd,gn,bu);

    LdTerm_toCopy.init( nullptr, nullptr, fin );

    pbNO_toCopy.init( false, nullptr, fin );
    pbNO_toCopy.pbButt.status = button::Status::inactive;
    pbNC_toCopy.init( true, nullptr, fin );
    pbNC_toCopy.pbButt.status = button::Status::inactive;

    orGate_toCopy.init( nullptr, fin );
    sf::Vector2f ofst; fin >> ofst.x >> ofst.y;
    unsigned int fontSz; fin >> fontSz;
    orGate_toCopy.label.setCharacterSize( fontSz );
    orGate_toCopy.label.setPosition( ofst + orGate_toCopy.getPosition() );

    bus2out_toCopy.init( nullptr, nullptr, fin );
    limitSwitch_toCopy.init( fin, nullptr, nullptr );

    // loadCS not drawn unless there is a loadPath
    float R; fin >> R;
    loadCS.setRadius(R);
    loadCS.setOrigin(R,R);
    fin >> rd >> gn >> bu;
    loadCS.setFillColor( sf::Color(rd,gn,bu) );
    fin >> ofst.x >> ofst.y;
    loadCS.setPosition( ofst );// for now

    fin.close();

    if( !init_controls() ) return false;

    pTermBindFromOK.push_back( &t_ControlPower );
    pTermBindFromOK.push_back( &t_LoadPower );

    // template
//    pTermBindToOK.push_back( &(pbNO_toCopy.Ln) );
 //   pTermBindFromOK.push_back( &(pbNO_toCopy.Ld) );
 //   pTermBindToOK.push_back( &(pbNC_toCopy.Ln) );
 //   pTermBindFromOK.push_back( &(pbNC_toCopy.Ld) );

    return true;
}

bool lvl_ControlBuilder::init_controls()
{
    std::ifstream fin("include/levels/lvlControlBuilder/init_controls.txt");
    if( !fin ) { std::cout << "\nNo init_controls data"; return false; }

    // type buttonRect
    // only 1 add button may be sel = true at once
    // use this lambda
    std::function<void( buttonRect*)> pDeSelAllBut = [this]( buttonRect* pBR_me )
    {
        if( !pBR_me->sel ) return;// act when "turning on"
      for( buttonRect* pBR : pAddButtVec )
        if( pBR != pBR_me && pBR->sel ) pBR->setSel(false);
    };

    buttonRect* pBR_me = &addTerminalButt;
    addTerminalButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addTerminalButt );
    button::RegisteredButtVec.push_back( &addTerminalButt );
    pBR_me = &addContactButt;
//    addContactButt.init( fin, nullptr );
    addContactButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addContactButt );
    button::RegisteredButtVec.push_back( &addContactButt );
    pBR_me = &addLoadTermButt;
//    addLoadTermButt.init( fin, nullptr );
    addLoadTermButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addLoadTermButt );
    button::RegisteredButtVec.push_back( &addLoadTermButt );
    pBR_me = &addPushButtNoButt;
//    addPushButtNoButt.init( fin, nullptr );
    addPushButtNoButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addPushButtNoButt );
    button::RegisteredButtVec.push_back( &addPushButtNoButt );
    pBR_me = &addPushButtNcButt;
//    addPushButtNcButt.init( fin, nullptr );
    addPushButtNcButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addPushButtNcButt );
    button::RegisteredButtVec.push_back( &addPushButtNcButt );
    pBR_me = &addOrGateButt;
//    addOrGateButt.init( fin, nullptr );
    addOrGateButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addOrGateButt );
    button::RegisteredButtVec.push_back( &addOrGateButt );
    pBR_me = &addBus2outButt;
 //   addBus2outButt.init( fin, nullptr );
    addBus2outButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addBus2outButt );
    button::RegisteredButtVec.push_back( &addBus2outButt );
    pBR_me = &addLimitSwitchButt;
    addLimitSwitchButt.init( fin, [ pDeSelAllBut, pBR_me ](){ pDeSelAllBut( pBR_me ); } );
    pAddButtVec.push_back( &addLimitSwitchButt );
    button::RegisteredButtVec.push_back( &addLimitSwitchButt );

    // 2 power supply terminals
    cPowerButt.init( fin, [this](){ t_ControlPower.setState( cPowerButt.sel ); } );
    button::RegisteredButtVec.push_back( &cPowerButt );
    t_ControlPower.init( nullptr, fin );
    LdPowerButt.init( fin, [this](){ t_LoadPower.setState( LdPowerButt.sel ); } );
    button::RegisteredButtVec.push_back( &LdPowerButt );
    t_LoadPower.init( nullptr, fin );

    bool& rSel = drawWiresButt.sel;
    drawWiresButt.init( fin, [this,&rSel](){ if( rSel )for( sf_terminal* pT : pTermBindFromOK ) pT->set_pNext( pT->pNext ); sf_terminal::doDrawWires = rSel; } );
    drawWiresButt.setSel(true);// on initially
    sf_terminal::doDrawWires = true;// setSel() above does NOT call pFunc(). A legacy oversight to correct in next project.
    button::RegisteredButtVec.push_back( &drawWiresButt );

    saveButt.init( fin, 'O', [this](){ saveControls(); }, nullptr );
    button::RegisteredButtVec.push_back( &saveButt );
    loadButt.init( fin, 'O', [this](){ loadControls(); }, nullptr );
    button::RegisteredButtVec.push_back( &loadButt );

    idFwdLeadButt.init( fin, 'O', nullptr, nullptr );
    idFwdLeadButt.status = button::Status::hidden;
    button::RegisteredButtVec.push_back( &idFwdLeadButt );
    idRevLeadButt.init( fin, 'O', nullptr, nullptr );
    idRevLeadButt.status = button::Status::hidden;
    button::RegisteredButtVec.push_back( &idRevLeadButt );

    speedStrip.init( fin, &loadSpeed );
    loadSpeed = speedStrip.iVal;
    button::RegisteredButtVec.push_back( &speedStrip );

    // a radioButton
    modeRadioButt.init( fin );
    float offX, offY; fin >> offX >> offY;
    std::vector<std::string> labelVec;
    labelVec.resize( modeRadioButt.Nbutts );
    for( std::string& strIn : labelVec ) fin >> strIn;
    modeRadioButt.setButtLabels( offX, offY, labelVec, 12, sf::Color::Black );
    button::RegisteredButtVec.push_back( &modeRadioButt );

    fin.close();

    modeRadioButt.pFuncIdx = [this](size_t mode )
    {
        pTermSel = nullptr;
        pPushButtSel = nullptr;
        pOrGateSel = nullptr;
        setRBmode( mode );
    };

    idFwdLeadButt.pFuncClose = [this]()
    {
        if( pTermSel && pTermFwdLeads.size() < pathDir.size() )
        {
            pTermFwdLeads.push_back( pTermSel );
            pTermSel->tmCS.setFillColor( sf_terminal::termColorOff );

            if( pTermFwdLeads.size() == pathDir.size() )
                idFwdLeadButt.status = button::Status::hidden;
        }
        pTermSel = nullptr;// eaten
    };

    idRevLeadButt.pFuncClose = [this]()
    {
        if( pTermRevLeads.size() < pathDir.size() )
        {
            if( pTermSel )
            {
                pTermRevLeads.push_back( pTermSel );
            }
            else
            {
                pTermRevLeads.clear();
                for( size_t j = 0; j < pathDir.size(); ++j ) pTermRevLeads.push_back( nullptr );
            }

            if( pTermRevLeads.size() == pathDir.size() )
                idRevLeadButt.status = button::Status::hidden;
        }

        if( pTermSel ) pTermSel->tmCS.setFillColor( sf_terminal::termColorOff );
        pTermSel = nullptr;// eaten
    };

    return true;
}

void lvl_ControlBuilder::setRBmode( size_t mode )
{
    switch( mode )// on entering the mode do:
    {
        case 0 :// +devices
            for( buttonRect* pButt : pAddButtVec ) { pButt->setSel( false ); pButt->status = button::Status::active; }
            for( sf_pushButton& PB : pushButtVec ) PB.pbButt.status = button::Status::inactive;
            idFwdLeadButt.status = button::Status::hidden;
            idRevLeadButt.status = button::Status::hidden;
            break;
        case 1 :// move devices
            for( buttonRect* pButt : pAddButtVec ) pButt->status = button::Status::hidden;
            for( sf_pushButton& PB : pushButtVec ) PB.pbButt.status = button::Status::inactive;
            idFwdLeadButt.status = button::Status::hidden;
            idRevLeadButt.status = button::Status::hidden;
            break;
        case 2 :// operate
            for( sf_pushButton& PB : pushButtVec ) PB.pbButt.status = button::Status::active;
      //      break;// ok to fall through for now
        case 3 :// program
            // assign all states to false;
            if( t_ControlPower.state ) cPowerButt.setSel( false );
            if( t_LoadPower.state ) LdPowerButt.setSel( false );
            for( sf_terminal* pT : pTermBindFromOK ) pT->setState( false );
            for( sf_terminal* pT : pTermBindToOK ) pT->setState( false );

            for( buttonRect* pButt : pAddButtVec ) pButt->status = button::Status::hidden;

            if( pathIsComplete )
            {
                if( pTermFwdLeads.size() < pathDir.size() ) idFwdLeadButt.status = button::Status::active;
                if( pTermRevLeads.size() < pathDir.size() ) idRevLeadButt.status = button::Status::active;
            }
        break;

        default: break;
    }
}

bool lvl_ControlBuilder::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_ControlBuilder::update( float dt )
{
    // move the loadCS
    if( pathIsComplete )
    {
        if( pTermFwdLeads.size() == pathDir.size() && pTermRevLeads.size() == pathDir.size() )
        {
            sf::Vector2f loadPos = loadCS.getPosition();
            bool didMove = false;
            size_t j = 0;// co iterator for pathDir
            for( sf_terminal* pLead : pTermFwdLeads )
            {
                if( pLead && pLead->state ){ didMove = true; loadPos += loadSpeed*pathDir[j]*dt; }
                ++j;
            }
            j = 0;
            for( sf_terminal* pLead : pTermRevLeads )
            {
                if( pLead && pLead->state ){ didMove = true; loadPos -= loadSpeed*pathDir[j]*dt; }
                ++j;
            }

            if( didMove )
            {
                loadCS.setPosition( loadPos );
                for( sf_limitSwitch& LS : LimitSwitchVec ) LS.update( loadPos );
            }
        }
    }

    // handle click events
    if( button::pButtMse ) return;
    if( button::clickEvent_Lt() == 1 )
    {
        if( modeRadioButt.selIdx == 0 )// +devices
        {
            handleAddDevice();
        }
        else if( modeRadioButt.selIdx == 1 )// move devices
        {
            handleMovingDevice();
        }
        else if( modeRadioButt.selIdx == 2 )// operate
        {
            operateDevices();
        }
        else if( modeRadioButt.selIdx == 3 )// program
        {
            programDevices();
        }
    }

    // set loadCS position
    if( button::clickEvent_Rt() == 1 )
    {
       if( modeRadioButt.selIdx == 2 ) loadCS.setPosition( button::msePos() );// in operate mode only
    }

    return;
}

void lvl_ControlBuilder::handleAddDevice()
{
    if( addTerminalButt.sel )
    {
     //   addTerminalButt.setSel( false );
//        if( terminalVec.size() < termVecCapacity )
        if( terminalVec.size() < terminalVec.capacity() )
        {
            terminalVec.push_back( sf_terminal() );
            sf_terminal* pT = &terminalVec.back();
            pT->init( nullptr, "term", 5.0f, button::mseX, button::mseY, 0.0f, 5.0f );
            pTermBindFromOK.push_back( pT );
            typeOrderVec.push_back('A');
            pTermBindToOK.push_back( pT );
            std::cout << "\n terminal added";
        }
        else std::cout << "\no more terminals left.";
    }
    else if( addContactButt.sel )
    {
     //   addContactButt.setSel( false );
        if( contactVec.size() < contactVec.capacity() )
        {
            contactVec.push_back( sf_contact() );
            sf_contact* pC = &contactVec.back();
            pC->init( nullptr, nullptr, button::msePos(), 40.0f, 'R', contactColor, "con" );
            pTermBindFromOK.push_back( pC );// bind next contact
            pTermBindFromOK.push_back( &pC->no );// out terms
            pTermBindFromOK.push_back( &pC->nc );
            pTermBindToOK.push_back( pC );// operate contact (coil)
            pTermBindToOK.push_back( &pC->com );
            std::cout << "\n contact added";
            typeOrderVec.push_back('B');
        }
        else std::cout << "\n no more contacts left.";
    }
    else if( addLoadTermButt.sel )// 'C'
    {
    //    addLoadTermButt.setSel( false );
    //    if( loadTermVec.size() < conVecCapacity )
        if( loadTermVec.size() < loadTermVec.capacity() )
        {
            loadTermVec.push_back( LdTerm_toCopy );
            sf_loadTerminal* pLdT = &loadTermVec.back();
            pLdT->setPosition( button::msePos() );
            pTermBindFromOK.push_back( pLdT );
            pTermBindToOK.push_back( pLdT );
            std::cout << "\n loadTerminal added";
            typeOrderVec.push_back('C');
        }
        else std::cout << "\n no more loadTerminals left.";
    }
    else if( addPushButtNoButt.sel )// 'D'
    {
    //    addPushButtNoButt.setSel( false );
    //    if( pushButtVec.size() < conVecCapacity )
        if( pushButtVec.size() < pushButtVec.capacity() )
        {
            pushButtVec.push_back( pbNO_toCopy );
            sf_pushButton* pPB = &pushButtVec.back();
            pPB->pbButt.pFuncClose = [pPB](){ pPB->setState(true); };
            pPB->pbButt.pFuncOpen = [pPB](){ pPB->setState(false); };
            pPB->setPosition( button::msePos() );
            pPB->pbButt.status = button::Status::inactive;
            button::RegisteredButtVec.push_back( &(pPB->pbButt) );
       //     pPB->Ln.pNext = nullptr;
            pPB->Ln.set_pNext( nullptr );
            pTermBindFromOK.push_back( &pPB->Ld );
            pTermBindToOK.push_back( &pPB->Ln );
            std::cout << "\n type NO pushButton added";
            typeOrderVec.push_back('D');
        }
        else std::cout << "\n no more pushButtons left.";
    }
    else if( addPushButtNcButt.sel )// 'E'
    {
    //    addPushButtNcButt.setSel( false );
    //    if( pushButtVec.size() < conVecCapacity )
        if( pushButtVec.size() < pushButtVec.capacity() )
        {
            pushButtVec.push_back( pbNC_toCopy );
            sf_pushButton* pPB = &pushButtVec.back();
            pPB->pbButt.pFuncClose = [pPB](){ pPB->setState(true); };
            pPB->pbButt.pFuncOpen = [pPB](){ pPB->setState(false); };
            pPB->setPosition( button::msePos() );
            pPB->pbButt.status = button::Status::inactive;
            button::RegisteredButtVec.push_back( &(pPB->pbButt) );
     //       pPB->Ln.pNext = &(pPB->Ld);
            pPB->Ln.set_pNext( &(pPB->Ld) );
            pTermBindFromOK.push_back( &pPB->Ld );
            pTermBindToOK.push_back( &pPB->Ln );
            std::cout << "\n type NC pushButton added";
            typeOrderVec.push_back('E');
        }
        else std::cout << "\n no more pushButtons left.";
    }
    else if( addOrGateButt.sel )// 'F'
    {
    //    addOrGateButt.setSel( false );
    //    if( orGateVec.size() < conVecCapacity )
        if( orGateVec.size() < orGateVec.capacity() )
        {
            orGateVec.push_back( orGate_toCopy );
            sf_orGate* pOG = &orGateVec.back();
         //   pOG->reBindLambdas();
         //   pOG->tA.pSetState = [pOG](bool st){ pOG->setState(); };
         //   pOG->tB.pSetState = [pOG](bool st){ pOG->setState(); };

        //    pOG->tA.pSetState = [pOG](bool st){ bool newSt = st || pOG->tB.state; pOG->tOut.setState(newSt); };
        //    pOG->tB.pSetState = [pOG](bool st){ bool newSt = st || pOG->tA.state; pOG->tOut.setState(newSt); };
            pOG->setPosition( button::msePos() );
            pTermBindFromOK.push_back( &pOG->tOut );
            pTermBindFromOK.push_back( &pOG->tA );// pNext available ( = nullptr otherwise)
            pTermBindFromOK.push_back( &pOG->tB );// pNext available ( = nullptr otherwise)
            pTermBindToOK.push_back( &pOG->tA );
            pTermBindToOK.push_back( &pOG->tB );
            std::cout << "\n orGate added";
            typeOrderVec.push_back('F');
        }
        else std::cout << "\n no more orGates left.";
    }
    else if( addBus2outButt.sel )// 'G'
    {
    //    addBus2outButt.setSel( false );
    //    if( orGateVec.size() < conVecCapacity )
        if( bus2outVec.size() < bus2outVec.capacity() )
        {
            bus2outVec.push_back( bus2out_toCopy );
            sf_bus2out* pB2O = &bus2outVec.back();
            pB2O->pTermA = &( pB2O->tOutA );// correct the
            pB2O->pTermB = &( pB2O->tOutB );// corrupt addresses
            pB2O->setPosition( button::msePos() );
            pTermBindFromOK.push_back( &pB2O->tOutA );
            pTermBindFromOK.push_back( &pB2O->tOutB );
            pTermBindFromOK.push_back( pB2O );
            pTermBindToOK.push_back( pB2O );
            std::cout << "\n bus2out added";
            typeOrderVec.push_back('G');
        }
        else std::cout << "\n no more bus2out left.";
    }
    else if( addLimitSwitchButt.sel )// 'H'
    {
        if( LimitSwitchVec.size() < LimitSwitchVec.capacity() )
        {
            LimitSwitchVec.push_back( limitSwitch_toCopy );
            sf_limitSwitch* pLS = &LimitSwitchVec.back();
            pLS = &LimitSwitchVec.back();
            pLS->setPosition( button::msePos() );
            pTermBindFromOK.push_back( pLS );// bind next contact
            pTermBindFromOK.push_back( &pLS->no );// out terms
            pTermBindFromOK.push_back( &pLS->nc );
       //     pTermBindToOK.push_back( pLS );// operate contact (coil) - not appropriate to limit switch (they are manually operated)
            pTermBindToOK.push_back( &pLS->com );
            std::cout << "\n limitSwitch added";
            typeOrderVec.push_back('H');
        }
        else std::cout << "\n no more limitSwitch left.";
    }
    else std::cout << "\n no device selected.";// ???? possible??
}

void lvl_ControlBuilder::handleMovingDevice()
{
    if( pTermSel )// 2nd click = move
    {
        pTermSel->setPosition( button::msePos() );
        pTermSel = nullptr;
    }
    else if( pPushButtSel )
    {
        pPushButtSel->setPosition( button::msePos() );
        pPushButtSel = nullptr;
    }
    else if( pOrGateSel )
    {
        pOrGateSel->setPosition( button::msePos() );
        pOrGateSel = nullptr;
    }
    else if( pLS_moveCon )
    {
        pLS_moveCon->setContactPosition( button::msePos() );
        pLS_moveCon = pLS_moveSw = nullptr;// extra safe?
    }
    else if( pLS_moveSw )
    {
        pLS_moveSw->setSwitchPosition( button::msePos() );
        pLS_moveCon = pLS_moveSw = nullptr;// extra safe?
    }
    else// 1st click = selection for move
    {
        bool hit = false;

        // hit a terminal?
        for( sf_terminal& T : terminalVec )
        if( T.pos_isOver( button::msePos() ) )
        {
            pTermSel = &T;// assign
            std::cout << "\nterminal selected: ";
            hit = true;
            break;
        }

        if( !hit )// maybe a contact was hit (terminal type)
        {
            sf_terminal* ptHit = nullptr;
            {
                for( sf_contact& C : contactVec )
                if( C.pos_isOver( button::msePos(), ptHit ) )
                {
                    pTermSel = &C;
                    std::cout << "\nterminal selected: ";
                    hit = true;
                    break;
                }
            }

        }

        if( !hit )// loadTerminal? (terminal type)
        {
            {
                for( sf_loadTerminal& LdT : loadTermVec )
                if( LdT.pos_isOver( button::msePos() ) )
                {
                    pTermSel = &LdT;
                    std::cout << "\n loadTerminal selected: ";
                    hit = true;
                    break;
                }
            }

        }

        if( !hit )// bus2out? (terminal type)
        {
            {
                for( sf_bus2out& B20 : bus2outVec )
                if( B20.pos_isOver( button::msePos() ) )
                {
                    pTermSel = &B20;
                    std::cout << "\n bus2out selected: ";
                    hit = true;
                    break;
                }
            }

        }

        if( !hit )//  non terminal type?
        {
            for( sf_pushButton& PB : pushButtVec )// sf_pushButton?
                if( PB.pbButt.hit() )
                {
                    pPushButtSel = &PB;
                    hit = true;
                    std::cout << "\n pushButton selected: ";
                    break;
                }

            if( !hit )// orGate?
            {
                for( sf_orGate& OG : orGateVec )
                    if( OG.hit( button::msePos() ) )
                    {
                        pOrGateSel = &OG;
                        hit = true;
                        std::cout << "\n orGate selected: ";
                        break;
                    }
            }

            if( !hit )// sf_limitSwitch?
            {
                sf_terminal* ptHit = nullptr;
                {
                    for( sf_limitSwitch& LS : LimitSwitchVec )
                    {
                        if( LS.pos_isOver( button::msePos(), ptHit ) )// contacts hit
                        {
                            pLS_moveCon = &LS;
                            std::cout << "\n limit switch (contacts) selected: ";
                            hit = true;
                            break;
                        }
                        else if( LS.hitSwitch( button::msePos() ) )// switch hit
                        {
                            pLS_moveSw = &LS;
                            std::cout << "\n limit switch (switch) selected: ";
                            hit = true;
                            break;
                        }
                    }
                }
            }// end if limitSwitch
        }

        if( !hit ) std::cout << "\n move miss";
    }
}

void lvl_ControlBuilder::operateDevices()// 1 click
{
    // click to toggle terminal state, any
    bool hit = false;
    for( sf_terminal* pT : pTermBindFromOK )
    {
        if( pT->pos_isOver( button::msePos() ) )
        {
            pT->setState( !pT->state );
            hit = true;
            std::cout << "\n operate: BindFromOK terminal HIT";
            break;
        }
    }

    if( !hit )
    {
        for( sf_terminal* pT : pTermBindToOK )
        {
            if( pT->pos_isOver( button::msePos() ) )
            {
                pT->setState( !pT->state );
                hit = true;
                std::cout << "\n operate: BindToOK terminal HIT";
                break;
            }
        }
    }

    if( !hit )// limit switch operation?
    {
        for( sf_limitSwitch& LS : LimitSwitchVec )
            if( LS.hitSwitch( button::msePos() ) )
            {
                LS.setState( !LS.state );
                hit = true;
          //      std::cout << "\n operate: limit SWITCH Hit";
                break;
            }
    }

 //   if( !hit ) std::cout << "\n operate miss";
}

void lvl_ControlBuilder::programDevices()
{
    const sf::Color termSelColor = sf::Color(0,200,0);

    if( pTermSel )// 2nd click = bind (assign pNext) or clear ( pNext = nullptr )
    {
        int j = 0;
        for( sf_terminal* pt : pTermBindToOK )
        {
            if( pt->pos_isOver( button::msePos() ) )
            {
            //    pTermSel->pNext = pt;// assign
                pTermSel->set_pNext( pt );// assign
                pTermSel->bindIdx = j;// for save / load of controls
                std::cout << "\nterminal tB hit and tA.pNext = &tB assigned";
                pTermSel->tmCS.setFillColor( sf_contact::termColorOff );
                pTermSel = nullptr;
                std::cout << "\n program: 2nd click hit";
                break;
            }
            ++j;
        }

        if( pTermSel )// clear it then
        {
            pTermSel->pNext = nullptr;
            pTermSel->tmCS.setFillColor( sf_contact::termColorOff );
            pTermSel = nullptr;// C back to initial state
            std::cout << "\n program: 2nd click: link cleared";
        }
    }
    else// 1st click = selection
    {
        bool hit = false;
        for( sf_terminal* pt : pTermBindFromOK )
            if( pt->pos_isOver( button::msePos() ) )
            {
                pTermSel = pt;// assign
                std::cout << "\nterminal tA selected: " << pt;
                pTermSel->tmCS.setFillColor( termSelColor );
                hit = true;
                std::cout << "\n program: 1st click hit";
                break;
            }

        if( !hit )
        {
            for( sf_limitSwitch& LS : LimitSwitchVec )
                if( LS.hitSwitch( button::msePos() ) )
                {
                    addToLoadPath(LS);
                    return;
                }

             std::cout << "\n program: 1st click miss";
        }
    }
}

// now called during program mode, not +device mode
void lvl_ControlBuilder::addToLoadPath( sf_limitSwitch& LS )
{
//    static bool pathIsComplete = false;
    if( pathIsComplete ) { std::cout << "\n path already complete";  return; }

    // to find if path is closed
    static sf_limitSwitch* pLSfirst = &LS;// on 1st call


    static sf_limitSwitch* pLSprev = &LS;// on 1st call
    static sf::Vector2f* pDirPrev = nullptr;// on 1st call

    if( pDirPrev && pLSprev->hitSwitch( button::msePos() ) )
    {
        // path is open and complete
        finishLoadPath();
        return;
    }

    loadPath.push_back( LS.getSwitchPosition() );
    loadPath.back().color = sf::Color::Blue;
    std::cout << "\n program: LS hit. path vtx added";

    size_t sz = LimitSwitchVec.size();
    if( sz > 1 && loadPath.size() > 1 )// add a pathDir
    {
        sf::Vector2f newDir = LS.getSwitchPosition() - pLSprev->getSwitchPosition();
        float mag = sqrtf( newDir.x*newDir.x + newDir.y*newDir.y );
        newDir /= mag;
        pathDir.push_back( newDir );
        std::cout << "\n newDir = ( "  << newDir.x << ", " << newDir.y << " )";

        if( pathDir.size() == 1 )// assign Nu for 1st 2 LS
        {
            pLSprev->setNu( pathDir[0] );
            LS.setNu( -1.0f*pathDir[0] );
        }
        else if( pathDir.size() > 1 && pDirPrev )// assign Nu for last pair of LS
        {
            LS.setNu( -1.0f*pathDir.back() );
            sf::Vector2f nu = pathDir.back() - *pDirPrev;
            mag = sqrtf( nu.x*nu.x + nu.y*nu.y );
      //      nu /= mag;
            std::cout << "\n nu = ( "  << nu.x << ", " << nu.y << " )";
            nu /= mag;
            pLSprev->setNu( nu );
        }

        if( pLSfirst->hitSwitch( button::msePos() ) )
        {
            // path is closed on LSfirst and complete
            finishLoadPath();
            return;// last call
        }

        pDirPrev = &pathDir.back();
    }

    pLSprev = &LS;// for next call
}

void lvl_ControlBuilder::finishLoadPath()
{
    for( sf_limitSwitch& LS : LimitSwitchVec )
    {
        sf::Vector2f finalPos = LS.vtx[2].position - LS.Xopen*LS.Nu;
        LS.setSwitchPosition( finalPos );
    }

    pathIsComplete = true;
    pTermFwdLeads.clear();
    pTermRevLeads.clear();
    idFwdLeadButt.status = button::Status::active;
    idRevLeadButt.status = button::Status::active;
}

void lvl_ControlBuilder::draw( sf::RenderTarget& RT ) const
{
    if( loadPath.size() > 1 ) RT.draw( &loadPath[0], loadPath.size(), sf::LineStrip );

    if( !terminalVec.empty() ) for( const sf_terminal& T : terminalVec ) T.draw(RT);
    if( !contactVec.empty() ) for( const sf_contact& C : contactVec ) C.draw(RT);
    if( !loadTermVec.empty() ) for( const sf_loadTerminal& LdT : loadTermVec ) LdT.draw(RT);
    if( !pushButtVec.empty() ) for( const sf_pushButton& PB : pushButtVec ) PB.draw(RT);
    if( !orGateVec.empty() ) for( const sf_orGate& OG : orGateVec ) OG.draw(RT);
    if( !bus2outVec.empty() ) for( const sf_bus2out& B2O : bus2outVec ) B2O.draw(RT);
    if( !LimitSwitchVec.empty() ) for( const sf_limitSwitch& LS : LimitSwitchVec ) LS.draw(RT);

    t_ControlPower.draw(RT);
    t_LoadPower.draw(RT);

    RT.draw( loadCS );

//    limitSwitch_toCopy.draw(RT);
//    bus2out_toCopy.draw(RT);
//    orGate_toCopy.draw(RT);
//    pbNO_toCopy.draw(RT);
//    pbNC_toCopy.draw(RT);
 //   LdTerm_toCopy.draw(RT);
}

void lvl_ControlBuilder::saveControls()const
{
    std::ofstream fout("include/levels/lvlControlBuilder/save_data.txt");
    unsigned int typeCount[8] = {0};
    sf::Vector2f pos;

    // power terminals
    fout << t_ControlPower.bindIdx << ' ' << t_LoadPower.bindIdx;

    for( char type : typeOrderVec )
    {
        switch( type )
        {
            case 'A' :// terminal
            pos = terminalVec[ typeCount[0] ].getPosition();
            fout << '\n' << 'A' << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << terminalVec[ typeCount[0] ].bindIdx;
            ++typeCount[0];
            break;

            case 'B' :// contact
            pos = contactVec[ typeCount[1] ].getPosition();
            fout << '\n' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << contactVec[ typeCount[1] ].bindIdx;
            fout << ' ' << contactVec[ typeCount[1] ].no.bindIdx;
            fout << ' ' << contactVec[ typeCount[1] ].nc.bindIdx;
            ++typeCount[1];
            break;

            case 'C' :// loadTerminal
            pos = loadTermVec[ typeCount[2] ].getPosition();
            fout << '\n' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << loadTermVec[ typeCount[2] ].bindIdx;
            ++typeCount[2];
            break;

            case 'D' :// pbNO ( not a terminal type)
            pos = pushButtVec[ typeCount[3] ].getPosition();
            fout << '\n' << ' ' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << pushButtVec[ typeCount[3] ].Ld.bindIdx;
            ++typeCount[3];
            break;

            case 'E' :// pbNC ( not a terminal type)
            pos = pushButtVec[ typeCount[3] ].getPosition();
            fout << '\n' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << pushButtVec[ typeCount[3] ].Ld.bindIdx;
            ++typeCount[3];
            break;

            case 'F' :// orGate ( not a terminal type)
            pos = orGateVec[ typeCount[4] ].getPosition();
            fout << '\n' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << orGateVec[ typeCount[4] ].tA.bindIdx;
            fout << ' ' << orGateVec[ typeCount[4] ].tB.bindIdx;
            fout << ' ' << orGateVec[ typeCount[4] ].tOut.bindIdx;
            ++typeCount[4];
            break;

            case 'G' :// bus2out
            pos = bus2outVec[ typeCount[5] ].getPosition();
            fout << '\n' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << bus2outVec[ typeCount[5] ].bindIdx;
            fout << ' ' << bus2outVec[ typeCount[5] ].tOutA.bindIdx;
            fout << ' ' << bus2outVec[ typeCount[5] ].tOutB.bindIdx;
            ++typeCount[5];
            break;

            case 'H' :// limitSwitch
            pos = LimitSwitchVec[ typeCount[6] ].getPosition();
            fout << '\n' << type << ' ' << pos.x << ' ' << pos.y;
            fout << ' ' << LimitSwitchVec[ typeCount[6] ].bindIdx;
            fout << ' ' << LimitSwitchVec[ typeCount[6] ].no.bindIdx;
            fout << ' ' << LimitSwitchVec[ typeCount[6] ].nc.bindIdx;
            fout << ' ' << LimitSwitchVec[ typeCount[6] ].getSwitchPosition().x;
            fout << ' ' << LimitSwitchVec[ typeCount[6] ].getSwitchPosition().y;
            ++typeCount[6];
            break;
        }
    }
}

bool lvl_ControlBuilder::loadControls()
{
    std::ifstream fin("include/levels/lvlControlBuilder/save_data.txt");
    if( !fin ) { std::cout << "\nNo init_controls data"; return false; }

    typeOrderVec.clear();
    pTermBindFromOK.clear();
    pTermBindToOK.clear();
    // power terminals
    fin >> t_ControlPower.bindIdx >> t_LoadPower.bindIdx;
    pTermBindFromOK.push_back( &t_ControlPower );
    pTermBindFromOK.push_back( &t_LoadPower );

    terminalVec.clear();
    contactVec.clear();
    loadTermVec.clear();
    pushButtVec.clear();
    orGateVec.clear();
    bus2outVec.clear();
    LimitSwitchVec.clear();
    sf::Vector2f MsePos( button::mseX, button::mseY );// to restore later
    char type;
    sf::Vector2f pos;
 //   int bindIndex = -1;

    while( fin >> type >> pos.x >> pos.y )
    {
        button::mseX = pos.x;
        button::mseY = pos.y;

        switch( type )
        {
            case 'A' :// terminal
            addTerminalButt.sel = true;
            handleAddDevice();
            addTerminalButt.sel = false;
            fin >> terminalVec.back().bindIdx;
            break;

            case 'B' :// contact
            addContactButt.sel = true;
            handleAddDevice();
            addContactButt.sel = false;
            fin >> contactVec.back().bindIdx;
            fin >> contactVec.back().no.bindIdx;
            fin >> contactVec.back().nc.bindIdx;
            break;

            case 'C' :// loadTerminal
            addLoadTermButt.sel = true;
            handleAddDevice();
            addLoadTermButt.sel = false;
            fin >> loadTermVec.back().bindIdx;
            break;

            case 'D' :// pbNO ( not a terminal type)
            addPushButtNoButt.sel = true;
            handleAddDevice();
            addPushButtNoButt.sel = false;
            fin >> pushButtVec.back().Ld.bindIdx;
            break;

            case 'E' :// pbNC ( not a terminal type)
            addPushButtNcButt.sel = true;
            handleAddDevice();
            addPushButtNcButt.sel = false;
            fin >> pushButtVec.back().Ld.bindIdx;
            break;

            case 'F' :// orGate ( not a terminal type)
            addOrGateButt.sel = true;
            handleAddDevice();
            addOrGateButt.sel = false;
            fin >> orGateVec.back().tA.bindIdx;
            fin >> orGateVec.back().tB.bindIdx;
            fin >> orGateVec.back().tOut.bindIdx;
            break;

            case 'G' :// bus2out
            addBus2outButt.sel = true;
            handleAddDevice();
            addBus2outButt.sel = false;
            fin >> bus2outVec.back().bindIdx;
            fin >> bus2outVec.back().tOutA.bindIdx;
            fin >> bus2outVec.back().tOutB.bindIdx;
            break;

            case 'H' :// limitSwitch
            addLimitSwitchButt.sel = true;
            handleAddDevice();
            addLimitSwitchButt.sel = false;
            fin >> LimitSwitchVec.back().bindIdx;
            fin >> LimitSwitchVec.back().no.bindIdx;
            fin >> LimitSwitchVec.back().nc.bindIdx;
            fin >> pos.x >> pos.y;
            LimitSwitchVec.back().setSwitchPosition( pos );
            break;
        }
    }

    // assign links
    for( sf_terminal* pTerm : pTermBindFromOK )
    {
        sf_terminal* pNext = ( pTerm->bindIdx == -1 ) ? nullptr : pTermBindToOK[ pTerm->bindIdx ];
        pTerm->set_pNext( pNext );
    }


    // restore
    button::mseX = MsePos.x;
    button::mseY = MsePos.y;
    return true;
}
