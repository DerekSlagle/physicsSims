#include "persCluster.h"


bool persCluster::init( const std::vector<persPt*>& p_PersVec, const std::vector<persPt*>& p_TargetVec )
{
    if( p_PersVec.empty() ){ std::cout << "\n persCluster::init(): empty vector"; return false; }
    pPersVec = p_PersVec;
    pTargetVec = p_TargetVec;
    return init();
}

//bool persCluster::init( const std::vector<persPt*>& p_PersVec )
bool persCluster::init()
{
    if( pPersVec.empty() ){ std::cout << "\n persCluster::init(vec&): empty vector"; return false; }
    // copy vector
 //   pPersVec = p_PersVec;
    pSortedVec = pPersVec;

    // base member assigns
    inUse = true;
    isMoving = false;

    // assign pos at center of bounding box
    vec3f minBound, maxBound;
    minBound = maxBound = pPersVec[0]->pos;
    for( unsigned int j = 1; j < pPersVec.size(); ++j )
    {
        if( !pPersVec[j] ) { std::cout << "\n persCluster::init(): pPersVec[" << j << "] = nullptr"; continue; }
        // in x
        if( pPersVec[j]->pos.x - pPersVec[j]->Rbound < minBound.x ) minBound.x = pPersVec[j]->pos.x - pPersVec[j]->Rbound;
        else if( pPersVec[j]->pos.x + pPersVec[j]->Rbound > maxBound.x ) maxBound.x = pPersVec[j]->pos.x + pPersVec[j]->Rbound;
        // in y
        if( pPersVec[j]->pos.y - pPersVec[j]->Rbound < minBound.y ) minBound.y = pPersVec[j]->pos.y - pPersVec[j]->Rbound;
        else if( pPersVec[j]->pos.y + pPersVec[j]->Rbound > maxBound.y ) maxBound.y = pPersVec[j]->pos.y + pPersVec[j]->Rbound;
        // in z
        if( pPersVec[j]->pos.z - pPersVec[j]->Rbound < minBound.z ) minBound.z = pPersVec[j]->pos.z - pPersVec[j]->Rbound;
        else if( pPersVec[j]->pos.z + pPersVec[j]->Rbound > maxBound.z ) maxBound.z = pPersVec[j]->pos.z + pPersVec[j]->Rbound;
    }

    pos = 0.5f*( minBound + maxBound );
    float xDim = maxBound.x - minBound.x;
    float yDim = maxBound.y - minBound.y;
    float zDim = maxBound.z - minBound.z;
    Rbound = 0.5f*sqrtf( xDim*xDim + yDim*yDim + zDim*zDim );
    std::cout << "\ncluster Rbound = " << Rbound;

    // store relative positions
    relPosVec.reserve( pPersVec.size() );
    for( const persPt* pPers : pPersVec )
        relPosVec.push_back( pPers->pos - pos );

    // markers
    float Rcs = 4.0f;
    posCS.setRadius(Rcs);
    posCS.setOrigin(Rcs,Rcs);
    posCS.setFillColor( sf::Color(200,0,0) );
    posCS.setPosition( persPt::get_xyw( pos ) );
    boundCS.setFillColor( sf::Color(0,200,0,100) );
    float u = persPt::camDir.dot( pos - persPt::camPos );
    float Rb = Rbound*persPt::Z0/u;
    boundCS.setRadius(Rb);
    boundCS.setOrigin(Rb,Rb);
    boundCS.setPosition( posCS.getPosition() );

    update(0.0f);
    return true;
}

void persCluster::setPosition( vec3f Pos )
{
    pos = Pos;
    if( pPersVec.empty() ) return;
    for( unsigned int j = 0; j < pPersVec.size(); ++j )
        if( pPersVec[j] ) pPersVec[j]->setPosition( Pos + relPosVec[j] );
}

void persCluster::update( float dt )
{
    if( pPersVec.empty() ) return;
    if( !inUse ) return;

 //   bool doDrawLast = doDraw;
    // if camera is within Rbound doDraw = true
//    vec3f sep = pos - persPt::camPos;
    float U = persPt::camDir.dot( pos - persPt::camPos );
    if( U*U < Rbound*Rbound ) doDraw = true;// fudge factor so else returns true for sure
    else persPt::update_doDraw();
    // temp test output
 //   if( !doDrawLast && doDraw ) std::cout << "\ncluster doDraw: true";
 //   else if( doDrawLast && !doDraw ) std::cout << "\ncluster doDraw: false";

    if( !doDraw ) return;

    if( isMoving )
    {
        pos += vel*dt;
        for( unsigned int j = 0; j < pPersVec.size(); ++j )
            if( pPersVec[j] ) pPersVec[j]->setPosition( pos + relPosVec[j] );
    }
    else
    {
        for( persPt* pPers : pPersVec )
            if( pPers ) pPers->update(dt);
    }

    // update marker positions, etc
    if( showCenter )
    {
        posCS.setPosition( persPt::get_xyw( pos ) );
    }

    if( showBound )
    {
        boundCS.setPosition( persPt::get_xyw( pos ) );
        float u = persPt::camDir.dot( pos - persPt::camPos );
        float Rb = Rbound*persPt::Z0/u;
        boundCS.setRadius(Rb);
        boundCS.setOrigin(Rb,Rb);
    }

    numToDraw = 0;// some may be added before z order
 //   if( doZ_order ) z_order();
}

void persCluster::z_order()
{
    if( !(inUse && doDraw && doZ_order ) ) return;
    if( pPersVec.size() < 2 ) return;
 //   numToDraw = 0;
    for( persPt* pPers : pPersVec )
    {
        if( pPers && pPers->inUse && pPers->doDraw )
        {
            if( numToDraw < pSortedVec.size() ) pSortedVec[ numToDraw ] = pPers;
            else pSortedVec.push_back( pPers );
            ++numToDraw;
        }
    }

    std::sort( pSortedVec.begin(), pSortedVec.begin() + numToDraw, persPt::compare );
}

void persCluster::draw( sf::RenderTarget& RT )const
{
    if( pPersVec.empty() ) return;
    if( !(inUse && doDraw) ) return;

    if( doZ_order )
    {
        for( unsigned int j = 0; j < numToDraw; ++j )
            pSortedVec[j]->draw(RT);
    }
    else
    {
        for( const persPt* pPers : pPersVec )
            if( pPers ) pPers->draw(RT);
    }

    if( showBound ) RT.draw( boundCS );
    if( showCenter ) RT.draw( posCS );
}

bool persCluster::hitAll( persBall& PB, float dt )const
{
    if( pTargetVec.empty() ) return false;
    if( !inUse ) return false;// test even if doDraw = false
    if( !PB.inUse ) return false;

    // is the ball within Rbound of pos?
    vec3f sep = PB.pos - pos;
    if( sep.dot( sep ) > Rbound*Rbound ) return false;

    vec3f P, vu;
    for( const persPt* pPers : pTargetVec )
        if( pPers && pPers->hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
        {
            PB.setPosition( P + PB.Rbound*vu );
            PB.vel = vu*PB.vel.mag();
            return true;
        }

    return false;
}

bool persCluster::inBounds( vec3f Pos )const
{
    vec3f sep = Pos - pos;
    return sep.dot( sep ) < Rbound*Rbound;
}
