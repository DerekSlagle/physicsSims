#include "lvlT.h"

bool lvlT::init()
{
    physicsSim::pScale = &scale;
//    physicsSim::pGravity = &gravity;
    physicsSim::pOrigin = &simPos;

    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(200,200,200);
//    button::setHoverBoxColor( Level::clearColor );
    button::setHoverBoxColor( sf::Color::Black );

    std::ifstream fin("include/levels/lvlT/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    fin >> gravity >> scale;
    fin >> simPos.x >> simPos.y;

    float posX, posY;
    unsigned int charSz = 10;
    fin >> charSz >> posX >> posY;
    dtMsg.setFont( *button::pFont );
    dtMsg.setString("0.0");
    dtMsg.setCharacterSize( charSz );
    dtMsg.setPosition( posX, posY );
    dtMsg.setFillColor(sf::Color::Black);
    fin >> charSz >> posX >> posY;
    tElapMsg.setFont( *button::pFont );
    tElapMsg.setString("0.0");
    tElapMsg.setCharacterSize( charSz );
    tElapMsg.setPosition( posX, posY );
    tElapMsg.setFillColor(sf::Color::Black);

    fin.close();

    if( !init_controls() ) return false;

    sim_1.label.setFont( *button::pFont );
    sim_1.label.setCharacterSize( 14 );
    sim_1.label.setPosition( simPos.x, simPos.y - 80.0f );
    sim_1.label.setFillColor(sf::Color::Black);
    sim_2.label = sim_3.label = sim_4.label = sim_1.label;
    if( !init_sims() ) return false;

    return true;
}

bool lvlT::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvlT::update( float dt )
{
 /*   if( !button::pButtMse )
    {
        if( button::clickEvent_Lt() == 1 )
        {
            float time = -1.0f, value = -1.0f;
            if( plot_1.getDataPoint( button::mseX, button::mseY, time, value ) )
                std::cout << "\ntime = " << time << "  value = " << value;
        }
        else if( button::clickEvent_Rt() == 1 )
            plot_1.setPosition( button::mseX, button::mseY );
    }   */

    if( !pSim ) return;
    if( !runButt.sel )
    {
        if( !button::mseDnLt_last && ( pSim->msVoh.hitLeft() || ( pSim->msVoh.hit() && button::didScroll ) ) )// new
        {
            pSim->pUpdate(0.0f);
            std::cout << "\n voh hit! ";
        }
        return;
    }
    dt *= timeRate;
    timeElap += dt;
    to_SF_string( dtMsg, dt );
    to_SF_string( tElapMsg, timeElap );

    if( pSim ) pSim->update(dt);

    return;
}

void lvlT::draw( sf::RenderTarget& RT ) const
{
    RT.draw( dtMsg );
    RT.draw( tElapMsg );

    if( pSim ) pSim->draw(RT);

    return;
}

bool lvlT::init_controls()
{
    std::ifstream fin("include/levels/lvlT/control_data.txt");
    if( !fin ) { std::cout << "\nNo control data"; return false; }

    // globalControls: scale, gravity, timeRate
    sf::Vector2f hbPos, csPos, csDims;
    fin >> hbPos.x >> hbPos.y >> csPos.x >> csPos.y >> csDims.x >> csDims.y;

    globalControls.init( hbPos, csPos, csDims, "globals" );
    globalControls.pButtVec.push_back( &globalMS );
    button::RegisteredButtVec.push_back( &globalControls );

    sf::Vector2f ofst(10.0f,10.0f);
    globalMS.init( csPos + ofst, &globalStrip, fin );

    if( globalMS.sdataVec.size() != 3 ) { std::cout << "\nbad globalMS data"; return false; }
    stripData* pSD = &globalMS.sdataVec.front();
    // scale
    pSD->pSetFunc =     [this](float x){ scale = x; };
    globalStrip.reInit( *pSD );
    scale = pSD->xCurr;
     // gravity
    (++pSD)->pSetFunc = [this](float x){ gravity = x; };
    gravity = pSD->xCurr;
    // timeRate
    (++pSD)->pSetFunc = [this](float x){ timeRate = x; };
    timeRate = pSD->xCurr;

    // runButt
    float posX, posY, width, height;
    unsigned int charSz = 10;
    fin >> charSz >> posX >> posY >> width >> height;
    sf::Text label("Run/Stop", *button::pFont, charSz);
    label.setFillColor( sf::Color::Black );
    runButt.init( posX, posY, width, height, nullptr, label );
    button::RegisteredButtVec.push_back( &runButt );
    // resetButt
    fin >> charSz >> posX >> posY >> width >> height;
    label.setString("Reset");
    label.setCharacterSize( charSz );
    label.setFillColor( sf::Color::Black );
    resetButt.init( posX, posY, width, height, nullptr, label );
    resetButt.pHitFunc = [this]()
    {
        runButt.setSel(false); timeElap = 0.0f; to_SF_string( tElapMsg, 0.0f );
        if( pSim )
        {
            for( stripData& sd : pSim->msControl.sdataVec )
                sd.pSetFunc( sd.xCurr );// set all values as selected

            pSim->pUpdateGraphics();

            if( pSim->funPlotVec.size() > 0 )
               for( functionPlot& fp : pSim->funPlotVec ) fp.reset();
        }
    };
    resetButt.mode = 'I';
    button::RegisteredButtVec.push_back( &resetButt );

    // simList
    fin >> charSz >> posX >> posY >> width >> height;

    fin.close();

    label.setCharacterSize( charSz );
    std::vector<sf::Text> TitleVec;
    std::vector< std::function<void(void)> > pFuncVec;

    std::function<void(physicsSim&)> pFunc = [this]( physicsSim& sim )
    {
        if( pSim )
        {
            pSim->reset();
            button::unRegisterMe( &(pSim->conSurf) );
        }

        pSim = &sim;
        button::RegisteredButtVec.push_back( &(sim.conSurf) );
        runButt.setSel(false); timeElap = 0.0f; to_SF_string( tElapMsg, 0.0f );
    };

    TitleVec.reserve(6);
    pFuncVec.reserve(6);
    label.setString("sim list"); TitleVec.push_back( label );
    pFuncVec.push_back( nullptr );
    label.setString("simple pendulum"); TitleVec.push_back( label );
    pFuncVec.push_back( [ this, pFunc ](){ pFunc( sim_1 ); } );
    label.setString("two masses"); TitleVec.push_back( label );
    pFuncVec.push_back( [ this, pFunc ](){ pFunc( sim_2 ); } );
    label.setString("double pendulum"); TitleVec.push_back( label );
    pFuncVec.push_back( [ this, pFunc ](){ pFunc( sim_3 ); } );
    label.setString("ball in pipe"); TitleVec.push_back( label );
    pFuncVec.push_back( [ this, pFunc ](){ pFunc( sim_4 ); } );
    label.setString("spring pendulum"); TitleVec.push_back( label );
    pFuncVec.push_back( [ this, pFunc ](){ pFunc( sim_5 ); } );
    // find longest title
    width = 0.0f;
    for( const sf::Text& txt : TitleVec )
    {
        if( txt.getLocalBounds().width > width )
            width = txt.getLocalBounds().width;
    }
    simList.init( posX, posY, width + 10.0f, height, height + 2.0f, TitleVec, pFuncVec );
    button::RegisteredButtVec.push_back( &simList );

    return true;
}

bool lvlT::init_sims()
{
    // sim_1: simple pendulum
    sim_1.label.setString( "simple pendulum" );
    std::ifstream fin("include/levels/lvlT/sim_1_data.txt");
    if( !fin ) { std::cout << "\nNo sim_1 init data"; return false; }
    if( !sim_1.init( fin ) ) return false;
    fin.close();
    sim_1.pUpdate = [this]( float dt )
    {
        sim_1.var[0][2] = -gravity*sin( sim_1.var[0][0] )/sim_1.paramVec[0];
        sim_1.var[0][0] += sim_1.var[0][1]*dt + sim_1.var[0][2]*dt*dt/2.0f;// new
        sim_1.var[0][1] += sim_1.var[0][2]*dt;
     //   sim_1.var[0][0] += sim_1.var[0][1]*dt;
    };
    sim_1.pUpdateGraphics = [this]()
    {
        sim_1.vtxVec[3].position = sim_1.vtxVec[2].position + sf::Vector2f( sin( sim_1.var[0][0] ), cos( sim_1.var[0][0] ) )*scale*(float)sim_1.paramVec[0];
        sim_1.circleVec[0].setPosition( sim_1.vtxVec[3].position );
    };
    sim_1.pUpdateGraphics();
    sim_1.numReps = 2;

    // sim_2: two masses
    sim_2.label.setString( "two masses" );
    fin.open( "include/levels/lvlT/sim_2_data.txt" );
    if( !fin ) { std::cout << "\nNo sim_2 init data"; return false; }
    if( !sim_2.init( fin ) ) return false;
    fin.close();
    {
        double &r = sim_2.var[0][0], &r1 = sim_2.var[0][1], &r2 = sim_2.var[0][2];
        double &a = sim_2.var[1][0], &a1 = sim_2.var[1][1], &a2 = sim_2.var[1][2];
        sim_2.pUpdate = [this,&r,&r1,&r2,&a,&a1,&a2]( float dt )
        {
            r2 = ( r*a1*a1 - gravity*( 1.0 - cos(a) ) )/2.0f;
            r += r1*dt + r2*dt*dt/2.0f;// new
            r1 += r2*dt;
       //     r += r1*dt;
            sim_2.paramVec[0] -= r1*dt;// left side length change
            a2 = (-1.0/r)*( 2.0*r1*a1 + gravity*sin(a) );
            a += a1*dt + a2*dt*dt/2.0f;// new
            a1 += a2*dt;
        //    a += a1*dt;
        };
        sim_2.pUpdateGraphics = [this,&a,&r]()
        {
            // right side
            sim_2.vtxVec[3].position = sim_2.vtxVec[2].position + sf::Vector2f( sinf( a ), cosf( a ) )*scale*(float)r;
            sim_2.circleVec[1].setPosition( sim_2.vtxVec[3].position );
            // left side
            sim_2.vtxVec[1].position.y = sim_2.vtxVec[0].position.y + scale*sim_2.paramVec[0];
            sim_2.circleVec[0].setPosition( sim_2.vtxVec[1].position );
        };
        sim_2.pUpdateGraphics();
    }

    // sim_3: double pendulum
    sim_3.label.setString( "double pendulum" );
    fin.open( "include/levels/lvlT/sim_3_data.txt" );
    if( !fin ) { std::cout << "\nNo sim_3 init data"; return false; }
    if( !sim_3.init( fin ) ) return false;
    fin.close();
    {
        double &a = sim_3.var[0][0], &a1 = sim_3.var[0][1], &a2 = sim_3.var[0][2];// angle 1
        double &b = sim_3.var[1][0], &b1 = sim_3.var[1][1], &b2 = sim_3.var[1][2];// angle 2
        double &L1 = sim_3.paramVec[0], &m1 = sim_3.paramVec[1];
        double &L2 = sim_3.paramVec[2], &m2 = sim_3.paramVec[3];
        sim_3.pUpdate = [this,&a,&a1,&a2,&b,&b1,&b2,&L1,&m1,&L2,&m2]( float dt )
        {
            double u = 1.0 + m1/m2;
            double snC = sin( b - a ), csC = cos( b - a );
            a2 = ( L2*b1*b1*snC - u*gravity*sin(a) + csC*( L1*a1*a1*snC + gravity*sin(b) ) )/( L1*( u - csC*csC ) );
            b2 = -1.0*( L1*a2*csC + L1*a1*a1*snC + gravity*sin(b) )/L2;

            a += a1*dt + a2*dt*dt/2.0f;// new
            a1 += a2*dt;
       //     a += a1*dt;
            b += b1*dt + b2*dt*dt/2.0f;// new
            b1 += b2*dt;
         //   b += b1*dt;
        };
        sim_3.pUpdateGraphics = [this,&a,&b,&L1,&L2]()
        {
            // m1
            sim_3.vtxVec[1].position = sim_3.vtxVec[2].position = sim_3.vtxVec[0].position + sf::Vector2f( sinf( a ), cosf( a ) )*scale*(float)L1;
            sim_3.circleVec[0].setPosition( sim_3.vtxVec[1].position );
            // m2
            sim_3.vtxVec[3].position = sim_3.vtxVec[2].position + sf::Vector2f( sinf( b ), cosf( b ) )*scale*(float)L2;
            sim_3.circleVec[1].setPosition( sim_3.vtxVec[3].position );
        };
        sim_3.pUpdateGraphics();
        sim_3.numReps = 5;
    }

    // sim_4: ball in pipe
    sim_4.label.setString( "ball in pipe" );
    fin.open( "include/levels/lvlT/sim_4_data.txt" );
    if( !fin ) { std::cout << "\nNo sim_4 init data"; return false; }
    if( !sim_4.init( fin ) ) return false;
    fin.close();
    {
        double &a = sim_4.var[0][0], &a1 = sim_4.var[0][1], &a2 = sim_4.var[0][2];// angle
        double &x = sim_4.var[1][0], &x1 = sim_4.var[1][1], &x2 = sim_4.var[1][2];// ball position
        double &L = sim_4.paramVec[0];// pipe length
        double &u = sim_4.paramVec[1];// Mball/Mpipe
        double r = sim_4.circleVec[0].getRadius()/scale;
        sim_4.pUpdate = [this,&a,&a1,&a2,&x,&x1,&x2,&L,&u,r]( float dt )
        {
            a2 = ( ( 0.5*L*u + x )*gravity*cos(a) - 2.0*x*x1*a1 )/( (L*L*u)/3.0 + x*x );
            x2 = x*a1*a1 + gravity*sin(a);

            a += a1*dt + a2*dt*dt/2.0f;// new
            a1 += a2*dt;
        //    a += a1*dt;
            x += x1*dt + x2*dt*dt/2.0f;// new
            x1 += x2*dt;
        //    x += x1*dt;

            if( x > L ) runButt.setSel(false);// stop motion
        };
        sim_4.pUpdateGraphics = [this,&a,&r,&x,&L]()
        {
            // pipe
            sf::Vector2f Rp( sin(a)*r, -cos(a)*r ), axis( cos(a), sin(a) );
            sim_4.vtxVec[0].position = simPos + scale*Rp;
            sim_4.vtxVec[3].position = simPos - scale*Rp;
            sim_4.vtxVec[1].position = simPos + scale*( (float)L*axis + Rp );
            sim_4.vtxVec[2].position = simPos + scale*( (float)L*axis - Rp );
            // ball
            sim_4.circleVec[0].setPosition( simPos + (float)x*axis*scale );
        };
        sim_4.pUpdateGraphics();
        sim_4.numReps = 5;
    }

    // sim_5: spring pendulum
    sim_5.label.setString( "spring pendulum" );
    fin.open( "include/levels/lvlT/sim_5_data.txt" );
    if( !fin ) { std::cout << "\nNo sim_5 init data"; return false; }
    if( !sim_5.init( fin ) ) return false;
    fin.close();
    sim_5.circleVec[0].setFillColor( sf::Color::Cyan );
    {
        double &a = sim_5.var[0][0], &a1 = sim_5.var[0][1], &a2 = sim_5.var[0][2];// angle
        double &x = sim_5.var[1][0], &x1 = sim_5.var[1][1], &x2 = sim_5.var[1][2];// spring displacement
        double &L = sim_5.paramVec[0], &kom = sim_5.paramVec[1];
        sim_5.pUpdate = [this,&a,&a1,&a2,&x,&x1,&x2,&L,&kom]( float dt )
        {
            x2 = (L+x)*a1*a1 + gravity*cos(a) - kom*x;
            a2 = -2.0*x1*a1 - gravity*sin(a);
            a2 /= L+x;

            a += a1*dt + a2*dt*dt/2.0f;// new
            a1 += a2*dt;
        //    a += a1*dt;
            x += x1*dt + x2*dt*dt/2.0f;// new
            x1 += x2*dt;
         //   x += x1*dt;
        };
        sim_5.pUpdateGraphics = [this,&a,&x,&L]()
        {
            // spring
            sim_5.circleVec[0].setPosition( sim_5.vtxVec[0].position + scale*(float)L*sf::Vector2f( sin(a), cos(a) ) );
            // m
            sim_5.vtxVec[1].position = sim_5.vtxVec[0].position + sf::Vector2f( sin(a), cos(a) )*scale*(float)(L+x);
            sim_5.circleVec[1].setPosition( sim_5.vtxVec[1].position );
        };
        sim_5.pUpdateGraphics();
        sim_5.numReps = 3;
    }

    return true;
}
