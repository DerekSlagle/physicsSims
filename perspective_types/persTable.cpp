#include "persTable.h"

bool persTable::init( std::istream& is, std::vector<spriteSheet>& rSSvec )// if owning
{
    if( !is || rSSvec.empty() ) return false;

    numItemsToDraw = 0;
    size_t SSnum; is >> SSnum;
    if( SSnum >= rSSvec.size() ) return false;
    Table.init( is, rSSvec[SSnum] );

     // items on the Table
    size_t numItems = 0, numPQ; is >> numPQ;
    numItems = numPQ;// just 1 type for now
    if( numItems > 0 )
    {
        pItemVec.reserve( numItems );
        // type persQuad
        if( numPQ > 0 )
        {
            ownsItems = true;
            for( size_t i = 0; i < numPQ; ++i )
            {
                size_t SSnum, SetNum, FrIdx;
                char chA, chB;
                bool faceCam;
                is >> SSnum >> SetNum >> FrIdx >> chA >> chB >> faceCam;
                vec3f Pos; is >> Pos.x >> Pos.y >> Pos.z;
                Pos += Table.topQ.pos;
                float W, H; is >> W >> H;
                vec3f nu; is >> nu.x >> nu.y >> nu.z;
                unsigned int rd, gn, bu; is >> rd >> gn >> bu;
                persQuad* pPQ = new persQuad(  Pos, W, H, nu, sf::Color(rd,gn,bu), &( rSSvec[SSnum].txt ) );
                pPQ->setTxtRect( rSSvec[SSnum].getFrRect( FrIdx, SetNum ), chA, chB );
                pPQ->facingCamera = faceCam;
                pItemVec.push_back( pPQ );
            }
        }
    }

    // stream still ok?
 //   if( !is ) return false;// the dtor will delete already allocated instances

    numChairsToDraw = 0;
    // chairs
    size_t numChairs; is >> numChairs;
    std::cout << "\n persTable::init() numChairs = " << numChairs;
    if( numChairs > 0 )
    {
        ownsChairs = true;
        pChairVec.reserve( numChairs );// handle the Table separately
   //     pChairVec.push_back( &Table );// exclude this element from deletion in the dtor!!
        for( size_t i = 0; i < numChairs; ++i )
        {
            size_t SSnum; is >> SSnum;
            if( SSnum < rSSvec.size() )
            {
                persChair* pCH = new persChair( is, rSSvec[SSnum] );
                vec3f Pos = pCH->pos;
                Pos.x += Table.pos.x;
                Pos.z += Table.pos.z;
                pCH->setPosition( Pos );
                pChairVec.push_back( pCH );
            }
            else { persChair dummyChair( is, rSSvec[0] ); }// just burn the remaining data for this chair. # of chairs will be 1 short
        }

        if( pChairVec.size() == 1 ) pChairVec.clear();// it was all bad. There will be just the Table and Items on it
    }

    // fill the pSortedChairsVec
    if( !pChairVec.empty() )
    {
        pSortedChairsVec.reserve( pChairVec.size() + 1 );
        pSortedChairsVec.push_back( &Table );
        for( persChair* pCH : pChairVec )  pSortedChairsVec.push_back( pCH );
    }

    // fill the pSortedItemsVec
    pSortedItemsVec.reserve( pItemVec.size() );
    for( persPt* pPt : pItemVec ) pSortedItemsVec.push_back( pPt );

//    std::string chkStr; is >> chkStr;
//    std::cout << "\n" << chkStr;

    return true;
}

bool persTable::init( const std::vector<persPt*>& pPtToCopy, bool OwnsItems )// if owning or not
{
    if( pPtToCopy.empty() ) return false;
    numChairsToDraw = numItemsToDraw = 0;
    ownsItems = OwnsItems;
    pItemVec.reserve( pPtToCopy.size() );
    pSortedItemsVec.reserve( pPtToCopy.size() );
    for( persPt* pPt : pPtToCopy )
    {
        pItemVec.push_back( pPt );
        pSortedItemsVec.push_back( pPt );
    }

    pItemVec = pSortedItemsVec = pPtToCopy;
    return true;
}

bool persTable::init( const std::vector<persChair*>& pChToCopy, bool OwnsChairs )
{
    if( pChToCopy.empty() ) return false;
    numChairsToDraw = numItemsToDraw = 0;
    ownsChairs = OwnsChairs;
    pChairVec.reserve( 1 + pChToCopy.size() );
    pChairVec.push_back( &Table );// am watching for this in the dtor
    pSortedChairsVec.reserve( 1 + pChToCopy.size() );
    pSortedChairsVec.push_back( &Table );// am watching for this in the dtor
    // pointers to both
    for( persChair* pCH : pChToCopy )
    {
        pChairVec.push_back( pCH );
        pSortedChairsVec.push_back( pCH );
    }

    return true;
}

persTable::~persTable()
{
    if( ownsItems ) for( persPt* pPt : pItemVec ) delete pPt;
    if( ownsChairs )
    {
        for( persChair* pCH : pChairVec )
            if( pCH != &Table ) delete pCH;//  the Table should no longer be present
    }
}

void persTable::update_doDraw()
{
    Table.update_doDraw();
    for( persPt* pPt : pItemVec ) pPt->update_doDraw();
    for( persChair* pCH : pChairVec ) pCH->update_doDraw();

    doDraw = Table.doDraw;// true if any items or chairs doDraw true
    if( !doDraw )
    {   // most likely a chair if the Table is not in view
        for( persChair* pCH : pChairVec )
            if( pCH->doDraw ){ doDraw = true; break; }
    }

    if( !doDraw )// still not true
    {   // maybe some items on the table are in view, but no Table top or chairs are visible?
        for( persPt* pPt : pItemVec )// just cover this case
            if( pPt->doDraw ){ doDraw = true; break; }
    }

}

void persTable::update( float dt )
{
    update_doDraw();
    if( !doDraw ) return;

    Table.update(dt);
    for( persPt* pPt : pItemVec ) pPt->update(dt);
    for( persChair* pCH : pChairVec ) pCH->update(dt);

    if( !pItemVec.empty() )
    {
        numItemsToDraw = 0;
        for(  persPt* pPt : pItemVec )
            if( pPt->doDraw && numItemsToDraw < pSortedItemsVec.size() ) pSortedItemsVec[ numItemsToDraw++ ] = pPt;

        if( pSortedItemsVec.size() > 1 )
            std::sort( pSortedItemsVec.begin(), pSortedItemsVec.begin() + numItemsToDraw, persPt::compare );
    }

    numChairsToDraw = 0;
    if( !pChairVec.empty() )
   {
        if( Table.doDraw ) pSortedChairsVec[ numChairsToDraw++ ] = &Table;
        for( persChair* pCH : pChairVec )
            if( pCH->doDraw && numChairsToDraw < pSortedChairsVec.size() ) pSortedChairsVec[ numChairsToDraw++ ] = pCH;

        if( pSortedChairsVec.size() > 1 )
            std::sort( pSortedChairsVec.begin(), pSortedChairsVec.begin() + numChairsToDraw, persChair::compare );
   }

}

void persTable::drawTableAndItems( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    vec3f sep = persPt::camPos - Table.pos;
    vec3f sepTop = persPt::camPos - Table.topQ.pos;

    // draw back or front 1st
    if( sep.dot( Table.ftQ.Nu ) > 0.0f )  Table.bkQ.draw(RT);// am in front
    else Table.ftQ.draw(RT);// am behind

    // draw the sides, the topQ and the items upon topQ
    if( sep.dot( Table.rtQ.Nu ) > 0.0f )// am right of
    {
        Table.ltQ.draw(RT);

        if( sepTop.dot( Table.topQ.Nu ) > 0.0f )// am above
        {
            Table.topQ.draw(RT);//  Draw Table 1st
            for( size_t k = 0; k < numItemsToDraw; ++k )
                pSortedItemsVec[k]->draw(RT);// draw sorted items
        }
        else// am below
        {
            for( size_t k = 0; k < numItemsToDraw; ++k )
                pSortedItemsVec[k]->draw(RT);// draw sorted items

            Table.topQ.draw(RT);//  Draw Table last
        }

        Table.rtQ.draw(RT);
    }
    else// am left of
    {
        Table.rtQ.draw(RT);

        if( sepTop.dot( Table.topQ.Nu ) > 0.0f )// am above
        {
            Table.topQ.draw(RT);//  Draw Table 1st
            for( size_t k = 0; k < numItemsToDraw; ++k )
                pSortedItemsVec[k]->draw(RT);// draw sorted items after top
        }
        else// am below
        {
            for( size_t k = 0; k < numItemsToDraw; ++k )
                pSortedItemsVec[k]->draw(RT);// draw sorted items before top
            Table.topQ.draw(RT);//  Draw Table last
        }

        Table.ltQ.draw(RT);
    }

    // draw front or back last
    if( sep.dot( Table.ftQ.Nu ) > 0.0f )  Table.ftQ.draw(RT);// am in front
    else Table.bkQ.draw(RT);// am behind

}

void persTable::draw( sf::RenderTarget& RT ) const
{
    if( numChairsToDraw > 0 )
    {
        unsigned int j = 0;
        for( j = 0; j < numChairsToDraw; ++j )// draw chairs further away than the Table
        {
            if( pSortedChairsVec[j] == &Table ){ ++j;  break; }
            pSortedChairsVec[j]->draw(RT);
        }

        drawTableAndItems(RT);

        for( unsigned int k = j; k < numChairsToDraw; ++k )// draw chairs closer than the Table
            pSortedChairsVec[k]->draw(RT);
    }
    else
        drawTableAndItems(RT);
}

void persTable::setPosition( vec3f Pos )
{
    vec3f dPos = Pos - pos;
    Table.setPosition( Pos );
    for(  persPt* pPt : pItemVec ) pPt->setPosition( pPt->pos + dPos );
    for( persChair* pCH : pChairVec ) pCH->setPosition( pCH->pos + dPos );
}
