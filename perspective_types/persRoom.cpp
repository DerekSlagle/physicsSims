#include "persRoom.h"

bool persRoom::init( std::istream& is, std::vector<spriteSheet>& rSSvec )
{
    if( !is ){ std::cout << "\ninit_room() bad is passed"; return false; }

    is >> pos.x >> pos.y >> pos.z >> Sz.x >> Sz.y >> Sz.z;

    // wallFt
    vec3f posStUp = persPt::yHat*Sz.y + 0.5f*( persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    vec3f posEndUp = persPt::yHat*Sz.y + 0.5f*( -persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    vec3f posStDn = 0.5f*( -persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    vec3f posEndDn = 0.5f*( persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    wallFt.init( pos + posStUp, pos + posEndUp, pos + posStDn, pos + posEndDn, vec3f(0.0f,0.0f,1.0f), sf::Color(100,100,100) );

    // wallBk
    posStUp = persPt::yHat*Sz.y + 0.5f*( persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    posEndUp = persPt::yHat*Sz.y + 0.5f*( -persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    posStDn = 0.5f*( -persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    posEndDn = 0.5f*( persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    wallBk.init( pos + posStUp, pos + posEndUp, pos + posStDn, pos + posEndDn, vec3f(0.0f,0.0f,-1.0f), sf::Color(140,140,140) );

    // wallLt
    posStUp = persPt::yHat*Sz.y + 0.5f*( -persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    posEndUp = persPt::yHat*Sz.y + 0.5f*( -persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    posStDn = 0.5f*( -persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    posEndDn = 0.5f*( -persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    wallLt.init( pos + posStUp, pos + posEndUp, pos + posStDn, pos + posEndDn, vec3f(-1.0f,0.0f,0.0f), sf::Color(100,100,200) );

    // wallRt
    posEndUp = persPt::yHat*Sz.y + 0.5f*( persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    posStUp = persPt::yHat*Sz.y + 0.5f*( persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    posEndDn = 0.5f*( persPt::xHat*Sz.x + persPt::zHat*Sz.z );
    posStDn = 0.5f*( persPt::xHat*Sz.x - persPt::zHat*Sz.z );
    wallRt.init( pos + posStUp, pos + posEndUp, pos + posStDn, pos + posEndDn, vec3f(1.0f,0.0f,0.0f), sf::Color(100,200,100) );

    pWallVec.reserve(4);
    pWallVec.push_back( &wallFt );
    pWallVec.push_back( &wallBk );
    pWallVec.push_back( &wallLt );
    pWallVec.push_back( &wallRt );

    roofQuad[0].position = persPt::get_xyw( wallFt.pt[0] );
    roofQuad[1].position = persPt::get_xyw( wallLt.pt[0] );
    roofQuad[2].position = persPt::get_xyw( wallBk.pt[0] );
    roofQuad[3].position = persPt::get_xyw( wallRt.pt[0] );

    unsigned int rd=0, gn=0, bu=0;
    is >> rd >> gn >> bu;
    roofQuad[0].color = roofQuad[1].color = roofQuad[2].color = roofQuad[3].color = sf::Color(rd,gn,bu);
    is >> rd >> gn >> bu;
    ceilingQuad[0].color = ceilingQuad[1].color = ceilingQuad[2].color = ceilingQuad[3].color = sf::Color(rd,gn,bu);
    is >> rd >> gn >> bu;
    floorQuad[0].color = floorQuad[1].color = floorQuad[2].color = floorQuad[3].color = sf::Color(rd,gn,bu);

    // set initial states
    // walls are drawn whether inside or out
    for( persWall* pPW : pWallVec ) pPW->update(0.0f);

    update_amInsideRoom();
    if( amInsideRoom ) update_indoors(0.0f);
    else update_outdoors(0.0f);

    // for general use for init
    size_t SSnum, SetNum, FrIdx;
    char chA, chB;// relates to rotation or flipping of image
    vec3f Pos, Nu;
    float W, H;

    // load some doors and windows
    size_t numPQ; is >> numPQ;
    PQwallVec.reserve( numPQ );
    for( size_t i = 0; i < numPQ; ++i )
    {
        is >> Pos.x >> Pos.y >> Pos.z >> W >> H >> Nu.x >> Nu.y >> Nu.z >> SSnum >> SetNum >> FrIdx >> chA >> chB;
        Pos += pos;// Pos given relative to this position
        PQwallVec.push_back( persQuad( Pos, W, H, Nu, sf::Color::White, &( rSSvec[SSnum].txt ) ) );
        PQwallVec.back().setTxtRect( rSSvec[SSnum].getFrRect( FrIdx, SetNum ), chA, chB );
    }

    size_t numDoors; is >> numDoors;
    if( numDoors > 0 )
    {
        pDoorVec.reserve( numDoors );
        for( size_t i = 0; i < numDoors; ++i )
        {
            is >> SSnum >> SetNum >> FrIdx;
            is >> Pos.x >> Pos.y >> Pos.z >> Nu.x >> Nu.y >> Nu.z;
            char MoveDirection, MoveMode; is >> MoveDirection >> MoveMode;// assign moveMode after init() OK
            float MoveSpeed, Scale; is >> MoveSpeed >> Scale;// assign moveSpeed after init() OK
            unsigned int rd,gn,bu; is >> rd >> gn >> bu;
            Pos += pos;// Pos given relative to this position
            persDoor* pDoor = new persDoor( rSSvec[SSnum], SetNum, FrIdx, MoveDirection, Pos, Nu, Scale, sf::Color(rd,gn,bu) );
            pDoorVec.push_back( pDoor );
            pDoor->moveMode = MoveMode;
            pDoor->moveSpeed = MoveSpeed;
        }
    }

    // inside wall, ceiling and floor persPt items - unsorted
    size_t numInWallpPt; is >> numInWallpPt;
//    std::cerr << "\n**** numInWallpPt = " << numInWallpPt;
    if( numInWallpPt > 0 )
    {
        pPtSurfaceVec.reserve( numInWallpPt );//  OWNER ***
        for( size_t i = 0; i < numInWallpPt; ++i )
        {
            is >> Pos.x >> Pos.y >> Pos.z >> W >> H >> Nu.x >> Nu.y >> Nu.z >> SSnum >> SetNum >> FrIdx >> chA >> chB;
            Pos += pos;// Pos given relative to this position
            persQuad* pPQ = new persQuad( Pos, W, H, Nu, sf::Color::White, &( rSSvec[SSnum].txt ) );
            pPQ->setTxtRect( rSSvec[SSnum].getFrRect( FrIdx, SetNum ), chA, chB );
            pPtSurfaceVec.push_back( pPQ );
        }
    }

    pos.y += 0.5f*Sz.y;// for z sort?

    // ALL sorted interior persPt types
    // pPtVec  **OWNER**
    char pPtType = 'N';
    size_t numPersPtItems; is >> numPersPtItems;
    std::cerr << "\n numPersPtItems = " << numPersPtItems << "\n";
    if( numPersPtItems > 0 )
    {
        pPtVec.reserve( numPersPtItems );
        for( size_t i = 0; i < numPersPtItems; ++i )
        {
            is >> pPtType;
            switch( pPtType )
            {
                case 'C' :// persChair
                {
                    std::cerr << " CHAIR ";
                    is >> SSnum;
                    if( SSnum < rSSvec.size() )
                    {
                        persChair* pChair = new persChair( is, rSSvec[SSnum] );
                        pPtVec.push_back( pChair );
                    }
                    break;
                }

                case 'T' :// persTable
                {
                    std::cerr << " TABLE ";
                    persTable* pTable = new persTable( is, rSSvec );
                    pPtVec.push_back( pTable );
                    break;
                }

                case 'Q' :// persQuad
                {
                    std::cerr << " QUAD ";
                    is >> Pos.x >> Pos.y >> Pos.z >> W >> H >> Nu.x >> Nu.y >> Nu.z >> SSnum >> SetNum >> FrIdx >> chA >> chB;
                    bool faceCam; is >> faceCam;
               //     Pos += C[ cornerIdx ].pos;// Pos given relative to this persCorner position
                    persQuad* pPQ = new persQuad( Pos, W, H, Nu, sf::Color::White, &( rSSvec[SSnum].txt ) );
                    pPQ->setTxtRect( rSSvec[SSnum].getFrRect( FrIdx, SetNum ), chA, chB );
                    pPQ->facingCamera = faceCam;
                    pPtVec.push_back( pPQ );
                    break;
                }

                case 'B' :// persBox_quad
                {
                    std::cerr << " BOX ";
                    vec3f Sz; is >> SSnum;// >> Pos.x >> Pos.y >> Pos.z >> Sz.x >> Sz.y >> Sz.z >> SetNum;
                    if( SSnum < rSSvec.size() )
                    {
                        //  Pos += there is no table yet
                   //     persBox_quad* pPBQ = new persBox_quad( Pos, Sz, sf::Color::White, rSSvec[SSnum], SetNum );
                        persBox_quad* pPBQ = new persBox_quad( is, rSSvec[SSnum] );
                        pPtVec.push_back( pPBQ );
                    }
                    else std::cout << "\n type B SSnum = " << SSnum;
                    break;
                }

                default :
                    std::cout << "unknown pPtType = " << pPtType;
                    return false;
            }
        }
    }

    std::cout << "\nPQwallVec.size() = " << PQwallVec.size() << " chairVec.size() = " << chairVec.size() << " doorVec.size() = " << pDoorVec.size();
    std::cout << " pPtVec.size() = " << pPtVec.size();

    std::string chkStr("\nroomA.init() complete");
    std::cout << chkStr;

    // fill the sorted pointer vec
    pPtSortedVec.reserve( pPtVec.size() );
    for( persPt* pPt : pPtVec ) pPtSortedVec.push_back( pPt );// establish size + fill with valid *'s

    update_amInsideRoom();

    return true;
}

persRoom::~persRoom()
{
    for( persPt* pPt : pPtVec ) delete pPt;
    for( persPt* pPt : pPtSurfaceVec ) delete pPt;
    for( persDoor* pDoor : pDoorVec ) delete pDoor;
}

void persRoom::update_amInsideRoom()
{
    amInsideRoom = true;

    if( persPt::camPos.x < pos.x - 0.5f*Sz.x ){ amInsideRoom = false; return; }// left of
    if( persPt::camPos.x > pos.x + 0.5f*Sz.x ){ amInsideRoom = false; return; }// right of

    if( persPt::camPos.y > pos.y + 0.5f*Sz.y ){ amInsideRoom = false; return; }// above
    if( persPt::camPos.y < pos.y - 0.5f*Sz.y ){ amInsideRoom = false; return; }// below

    if( persPt::camPos.z > pos.z + 0.5f*Sz.z ){ amInsideRoom = false; return; }// in front of
    if( persPt::camPos.z < pos.z - 0.5f*Sz.z ){ amInsideRoom = false; return; }// behind
    return;
}

void persRoom::update_doDraw()
{
    doDraw = wallFt.doDraw || wallBk.doDraw || wallLt.doDraw || wallRt.doDraw;
}

void persRoom::update_indoors( float dt )
{
    float U = persPt::camDir.dot( persPt::yHat );
    float yHorizon = persPt::Yh + persPt::Z0*U/sqrtf( 1.0f - U*U );

    ceilingQuad[0].position = sf::Vector2f( 0.0f, 0.0f );// up Lt
    ceilingQuad[1].position = sf::Vector2f( 2.0f*persPt::X0, 0.0f );// up Rt
    ceilingQuad[2].position = sf::Vector2f( 2.0f*persPt::X0, yHorizon );// dn Rt
    ceilingQuad[3].position = sf::Vector2f( 0.0f, yHorizon );// dn Lt

    floorQuad[0].position = sf::Vector2f( 0.0f, yHorizon );// up Lt
    floorQuad[1].position = sf::Vector2f( 2.0f*persPt::X0, yHorizon );// up Rt
    floorQuad[2].position = sf::Vector2f( 2.0f*persPt::X0, 2.0f*persPt::Yh );// dn Rt
    floorQuad[3].position = sf::Vector2f( 0.0f, 2.0f*persPt::Yh );// dn Lt
}

void persRoom::update_outdoors( float dt )
{
    float cDotZ = persPt::camDir.dot( persPt::zHat );
    float cDotX = persPt::camDir.dot( persPt::xHat );
    if( cDotZ*cDotZ > cDotX*cDotX )
    {
        roofQuad[0].position = persPt::get_xyw( wallLt.pt[0] );
        roofQuad[1].position = persPt::get_xyw( wallLt.pt[1] );
        roofQuad[2].position = persPt::get_xyw( wallRt.pt[0] );
        roofQuad[3].position = persPt::get_xyw( wallRt.pt[1] );
    }
    else
    {
        roofQuad[0].position = persPt::get_xyw( wallFt.pt[0] );
        roofQuad[1].position = persPt::get_xyw( wallFt.pt[1] );
        roofQuad[2].position = persPt::get_xyw( wallBk.pt[1] );
        roofQuad[3].position = persPt::get_xyw( wallBk.pt[0] );
    }
}

void persRoom::update_always( float dt )
{
    if( !pDoorVec.empty() ) { for( persDoor* pPD : pDoorVec ) pPD->update(dt); }
}

void persRoom::update( float dt )
{
    // roomA walls are drawn whether inside or out
    for( persWall* pPW : pWallVec ) pPW->update(dt);
    update_amInsideRoom();// call above 1st
    if( amInsideRoom ) update_indoors(dt);
    else update_outdoors(dt);

    update_doDraw();

    // keep
    if( doDraw )
    {
        // in/out persQuad
        for( persQuad& PQ : PQwallVec ) PQ.update(dt);// unsorted, drawn if in or outside of room
        // in/out persDoor
    //    for( persDoor& PD : doorVec ) PD.update(dt);// unsorted, drawn if in or outside of room
        // interior general persPt items
        if( amInsideRoom )
        {
            for( persPt* pPt : pPtVec ) pPt->update(dt);// any persPt type - sorted ( current use = 1 persTable )
            for( persPt* pPt : pPtSurfaceVec ) pPt->update(dt);// unsorted - wall pic, floor tile

            numToDraw = 0;
            for( persPt* pPt : pPtVec ) if( pPt->doDraw && numToDraw < pPtSortedVec.size() ) pPtSortedVec[ numToDraw++ ] = pPt;
            if( numToDraw > 1 ) std::sort( pPtSortedVec.begin(), pPtSortedVec.begin() + numToDraw, persPt::compare );
        }
    }
}

void persRoom::updateDrawInFromOut( sf::RenderTarget& RT )// to support draw through open door
{
    update_amInsideRoom();
    if( amInsideRoom ) return;

 //   for( persWall* pPW : pWallVec ) pPW->update(0.0f);
    update_indoors(0.0f);
    update_doDraw();
    if( doDraw )
    {
        RT.draw( ceilingQuad, 4, sf::Quads );
        RT.draw( floorQuad, 4, sf::Quads );

     //   int n = 0;
        for( persWall* pPW : pWallVec )
        {
      //      pPW->update(0.0f);
            vec3f sep = pPW->pos - persPt::camPos;
            if( sep.dot( pPW->Nu ) > 0.0f )
            {
                pPW->update(0.0f);
                pPW->draw(RT);
          //      if( pPW->doDraw ) ++n;
            }
        }
    //    std::cout << "numUp = " << n;

        for( persQuad& PQ : PQwallVec )
        {
         //   PQ.update(0.0f);
            vec3f sep = PQ.pos - persPt::camPos;
            if( sep.dot( PQ.Nu ) > 0.0f )
            {
                PQ.update(0.0f);
                PQ.draw(RT);
            }
        }

        for( persPt* pPt : pPtSurfaceVec )
        {
            pPt->update(0.0f);// unsorted - wall pic, floor tile
         //   vec3f sep = pPt->pos - persPt::camPos;
            pPt->draw(RT);
        }

        for( persPt* pPt : pPtVec ) pPt->update(0.0f);// any persPt type - sorted

        unsigned int N = 0;
        for( persPt* pPt : pPtVec ) if( pPt->doDraw && N < pPtSortedVec.size() ) pPtSortedVec[ N++ ] = pPt;
        if( N > 1 ) std::sort( pPtSortedVec.begin(), pPtSortedVec.begin() + N, persPt::compare );

        for( size_t i = 0; i < N; ++i ) pPtSortedVec[i]->draw(RT);// sorted
    }
}

void persRoom::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;

    if( amInsideRoom )
    {
        // new
        RT.draw( ceilingQuad, 4, sf::Quads );
        RT.draw( floorQuad, 4, sf::Quads );
        for( const persWall* pPW : pWallVec ) pPW->draw(RT);
        // existing
        for( const persQuad& PQ : PQwallVec ) PQ.draw(RT);// unsorted on walls, floor and ceiling (in and out)
        for( const persDoor* pPD : pDoorVec ) pPD->draw(RT);// unsorted on walls, floor and ceiling (in and out)
        for( const persPt* pPt : pPtSurfaceVec ) pPt->draw(RT);// unsorted on walls, floor and ceiling (inside only)

        for( size_t i = 0; i < numToDraw; ++i ) pPtSortedVec[i]->draw(RT);// sorted
    }
    else
    {
        // new
        RT.draw( roofQuad, 4, sf::Quads );// roof
        for( const persWall* pPW : pWallVec )// draw the walls
        {
            if( ( persPt::camPos - pPW->pos ).dot( pPW->Nu ) > 0.0f ) pPW->draw(RT);
        }// condition applies to outside draw only

        for( const persQuad& PQ : PQwallVec )
        { if( ( persPt::camPos - PQ.pos ).dot( PQ.Nu ) > 0.0f ) PQ.draw(RT); }

        for( const persDoor* pPD : pDoorVec )
        { if( ( persPt::camPos - pPD->pos ).dot( pPD->Nu ) > 0.0f ) pPD->draw(RT); }
    }

    return;
}

void persRoom::setPosition( vec3f Pos )
{
  //  vec3f dPos = Pos - pos;
    // etc...
}
