#include "lvl_JustDraw.h"

bool lvl_JustDraw::init()
{
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );


    std::ifstream fin("include/levels/lvl_JustDraw/init_data.txt");
    if( !fin ) { std::cout << "\nNo init data"; return false; }

    unsigned int rd, gn, bu;
    fin >> rd >> gn >> bu;
    Level::clearColor = sf::Color(rd,gn,bu);
    button::setHoverBoxColor( Level::clearColor );

    char NuDir; fin >> NuDir;
    fin >> partOrigin.x >> partOrigin.y;
    partOrigin.z = 0.0f;
    Xu = vec3f( 1.0f, 0.0f, 0.0f );
    Yu = vec3f( 0.0f, 1.0f, 0.0f );
    Zu = vec3f( 0.0f, 0.0f, 1.0f );

    // init the partAxes
    partAxes.pXu = &Xu;// assigns
    partAxes.pYu = &Yu;// required
    partAxes.pZu = &Zu;// prior to
    partAxes.pOrigin = &partOrigin;// init() call

 //   partAxes.init( sf::Color::Black, 400.0f, 20.0f, 6.0f, 12, nullptr, button::pFont );
    partAxes.init( sf::Color::Black, 400.0f, 20.0f, 6.0f, 12, [this](vec3f pt){ return sf::Vector2f( pt.x, Level::winH - pt.y ); }, button::pFont );

    partCS.setFillColor( Level::clearColor );
    partCS.setOutlineColor( sf::Color::Blue );
    partCS.setOutlineThickness( 2.0f );
    partCS.setRadius( RpartCS );
    partCS.setOrigin( RpartCS, RpartCS );

    part3d::winHeight = Level::winH;
    partX.init( sf::Color::Magenta, sf::Color::Blue );

    fin.close();

    return true;
}

bool lvl_JustDraw::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        // List: Y, P, R, Lshift, Rshift
        if( rEvent.key.code == sf::Keyboard::Y )
        {
            YkeyDown = true;
            rotDir = ( LShiftKeyDown || RShiftKeyDown ) ? -1.0f : 1.0f;
        }
        else if( rEvent.key.code == sf::Keyboard::P )
        {
            PkeyDown = true;
            rotDir = ( LShiftKeyDown || RShiftKeyDown ) ? -1.0f : 1.0f;
        }
        else if( rEvent.key.code == sf::Keyboard::R )
        {
            RkeyDown = true;
            rotDir = ( LShiftKeyDown || RShiftKeyDown ) ? -1.0f : 1.0f;
        }
        else if( rEvent.key.code == sf::Keyboard::LShift ) LShiftKeyDown = true;
        else if( rEvent.key.code == sf::Keyboard::RShift ) RShiftKeyDown = true;
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if( rEvent.key.code == sf::Keyboard::Y ) YkeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::P ) PkeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::R ) RkeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::LShift ) LShiftKeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::RShift ) RShiftKeyDown = false;
        else if( rEvent.key.code == sf::Keyboard::F ) partX = partX.toBasis( Xu, Zu, Yu );
        else if( rEvent.key.code == sf::Keyboard::X )
        {
                partX = partX.flipX();
        }
        else if( rEvent.key.code == sf::Keyboard::Z )
        {
                partX = partX.flipY();
        }
    }

    return true;
}

void lvl_JustDraw::update( float dt )
{
    partAxes.update();
    if( button::pButtMse ) return;// mouse is over a button

    // make the part
    if( !partX.isComplete )
    {
        if( button::clickEvent_Lt() == 1 )// add a point
        {
            if( partX.ptVec.empty() ) partCS.setPosition( button::msePos() );
            vec3f pt( button::mseX, button::mseY, 0.0f );// inverting y axis
            if( partX.ptVec.size() > 2 )// at least 4 points to complete a part
            {
                // close = hit on partCS
                sf::Vector2f cPos = partCS.getPosition();
                float rSq = ( button::mseX - cPos.x )*( button::mseX - cPos.x ) + ( button::mseY - cPos.y )*( button::mseY - cPos.y );
                if( rSq < RpartCS*RpartCS )// close it
                {
                    partX.isComplete = true;
                    pt.x = cPos.x;// join to
                    pt.y = cPos.y;// start
                }
            }

            sf::Vertex vtx( sf::Vector2f( pt.x, pt.y ), sf::Color::Black );
            pt.y = Level::winH - pt.y;
            partX.addPoint(pt);
        }

    }
    else if( button::clickEvent_Rt() == 1 )// clear all
    {
        partX.clear();
        Xu = vec3f( 1.0f, 0.0f, 0.0f );
        Yu = vec3f( 0.0f, 1.0f, 0.0f );
        Zu = vec3f( 0.0f, 0.0f, 1.0f );
    }
    else// part is complete
    {
        // apply rotations
        if( YkeyDown ) vec3f::yaw( rotDir*rotSpeed*dt, Xu, Yu, Zu );
        if( PkeyDown ) vec3f::pitch( rotDir*rotSpeed*dt, Xu, Yu, Zu );
        if( RkeyDown ) vec3f::roll( rotDir*rotSpeed*dt, Xu, Yu, Zu );

        partX.update( partOrigin, Xu, Yu, Zu );
    }
}

void lvl_JustDraw::draw( sf::RenderTarget& RT ) const
{
    partAxes.draw(RT);
    if( !( partX.isComplete || partX.ptVec.empty() ) ) RT.draw( partCS );
    partX.draw(RT);
}

/*
void lvl_JustDraw::setVisitOrder()// call after part is complete
{
    if( !partIsComplete ) return;
    visitOrder.clear();

    unsigned int idxLo = partPtVec.size()/2, idxHi = partPtVec.size()/2;
    std::cout << "\n visit order: ";
    while( idxHi < partPtVec.size() && idxLo > 0 )
    {
        std::cout << ' ' << idxHi;
        visitOrder.push_back( idxHi++ );
        if( idxHi == partPtVec.size() ) visitOrder.back() = 0;
        visitOrder.push_back( --idxLo );
        std::cout << ' ' << idxLo;
    }

    if( partPtVec.size()%2 == 0 )// size is even
        visitOrder.pop_back();

    partPtVec.pop_back();// expel the duplicate closing point
    for( vec3f& V : partPtVec ) V -= partOrigin;

    if( visitOrder.size() == partPtVec.size() )
    {
        partFill.clear();
        for( unsigned int j = 0; j < visitOrder.size(); ++j )
        {
            sf::Vector2f vPos( partPtVec[ visitOrder[j] ].x, Level::winH - partPtVec[ visitOrder[j] ].y );
            partFill.append( sf::Vertex( vPos, sf::Color::Blue ) );
        }
    }
    else
        std::cout << "\n setVisitOrder: VO.sz = " << visitOrder.size() << ", ptVec.sz = " << partPtVec.size();
}

void lvl_JustDraw::draw( sf::RenderTarget& RT ) const
{
    partAxes.draw(RT);
    if( !( partIsComplete || partPtVec.empty() ) ) RT.draw( partCS );
    if( partIsComplete && partFill.getVertexCount() > 2 ) RT.draw( partFill );
    if( partOutline.getVertexCount() > 1 ) RT.draw( partOutline );
}

void lvl_JustDraw::update( float dt )
{
    partAxes.update();
    if( button::pButtMse ) return;// mouse is over a button

    // make the part
    if( !partIsComplete )
    {
        if( button::clickEvent_Lt() == 1 )// add a point
        {
            if( partPtVec.empty() ) partCS.setPosition( button::msePos() );
            vec3f pt( button::mseX, button::mseY, 0.0f );// inverting y axis
            if( partPtVec.size() > 2 )// at least 4 points to complete a part
            {
                // close = hit on partCS
                sf::Vector2f cPos = partCS.getPosition();
                float rSq = ( button::mseX - cPos.x )*( button::mseX - cPos.x ) + ( button::mseY - cPos.y )*( button::mseY - cPos.y );
                if( rSq < RpartCS*RpartCS )// close it
                {
                    partIsComplete = true;
                    pt.x = cPos.x;// join to
                    pt.y = cPos.y;// start
                }
            }

            sf::Vertex vtx( sf::Vector2f( pt.x, pt.y ), sf::Color::Black );
            partOutline.append( vtx );

            pt.y = Level::winH - pt.y;
            partPtVec.push_back( pt );
            if( partIsComplete ) setVisitOrder();
        }

    }
    else if( button::clickEvent_Rt() == 1 )// clear all
    {
        partPtVec.clear();
        partOutline.clear();
        partFill.clear();
        partIsComplete = false;
        Xu = vec3f( 1.0f, 0.0f, 0.0f );
        Yu = vec3f( 0.0f, 1.0f, 0.0f );
        Zu = vec3f( 0.0f, 0.0f, 1.0f );
    }
    else// part is complete
    {
        // apply rotations
        if( YkeyDown ) vec3f::yaw( rotDir*rotSpeed*dt, Xu, Yu, Zu );
        if( PkeyDown ) vec3f::pitch( rotDir*rotSpeed*dt, Xu, Yu, Zu );
        if( RkeyDown ) vec3f::roll( rotDir*rotSpeed*dt, Xu, Yu, Zu );

        // assign
        for( unsigned int j = 0; j < partPtVec.size(); ++j )
        {
            vec3f pt = partPtVec[j];// - partOrigin;
            pt = partOrigin + Xu*pt.x + Yu*pt.y + Zu*pt.z;
            if( j < partOutline.getVertexCount() )
                partOutline[j].position = sf::Vector2f( pt.x, Level::winH - pt.y );

            if( j < partFill.getVertexCount() )
            {
                pt = partPtVec[ visitOrder[j] ];// - partOrigin;
                pt = partOrigin + Xu*pt.x + Yu*pt.y + Zu*pt.z;
                partFill[j].position = sf::Vector2f( pt.x, Level::winH - pt.y );
            }

        }

        partOutline[ partOutline.getVertexCount() - 1 ].position = partOutline[0].position;
    }
}
*/
