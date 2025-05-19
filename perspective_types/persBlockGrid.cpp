#include "persBlockGrid.h"

const sf::Font* persBlockGrid::pFont = nullptr;
persBall* persBlockGrid::pBall = nullptr;
bool persBlockGrid::doDrawAxes = true;
float persBlockGrid::ofstL = -10.0f;
float persBlockGrid::ofstPerp = -6.0f;// offset from axis end along axis (L) and perpendicular
unsigned int persBlockGrid::labelCharSz = 12;

bool persBlockGrid::getIndex( unsigned int row, unsigned int col, unsigned int& idx )const// to index
{
    if( row + 2 > Rows || col + 2 > Cols ) return false;
    idx = row*Cols + col;
    return true;
}

bool persBlockGrid::getIndex( unsigned int row, unsigned int col, unsigned int layer, unsigned int& idx )const// to index
{
    if( row + 2 > Rows || col + 2 > Cols || layer + 2 > Layers ) return false;
    idx = layer*Rows*Cols + row*Cols + col;
    return true;
}

bool persBlockGrid::getRowCol( unsigned int& row, unsigned int& col, unsigned int idx )const// from index
{
    if(  idx >= Rows*Cols ) return false;
    row = idx/Cols;
    col = idx%Cols;
    return true;
}

 void persBlockGrid::constructAxes( vec3f Zu_p )// from givenZup
 {
    Zu = Zu_p;
  //  Xu = Zu.cross( persPt::yu );
    Xu = persPt::yu.cross( Zu );
    Xu /= Xu.mag();
    Yu = Zu.cross( Xu );
    halfBlock = 0.5f*( Cols*cellW*Xu + Rows*cellH*Yu + Layers*cellD*Zu );
    halfCell = 0.5f*( cellW*Xu + cellH*Yu + cellD*Zu );

    assignPoints();
    zOrderCells();
 }

 void persBlockGrid::assignPoints()
 {
 //    halfBlock = 0.5f*( Cols*cellW*Xu + Rows*cellH*Yu + Layers*cellD*Zu );
     Origin = pos - halfBlock;

    for( size_t it = 0; it < ptGrid.size(); ++it )
         ptGrid[it] = Origin + (it%Cols)*cellW*Xu + (it/Cols)%Rows*cellH*Yu + ( it/(Rows*Cols) )*cellD*Zu;
 }

 void persBlockGrid::zOrderCells()
 {
 //   halfCell = 0.5f*( cellW*Xu + cellH*Yu + cellD*Zu );
    std::function<bool( const vec3f*, const vec3f* )> pComp = [this]( const vec3f* pvA, const vec3f* pvB )
    {
        vec3f sep = *pvA + halfCell - persPt::camPos;
        float distSqA = sep.dot(sep);
        sep = *pvB + halfCell - persPt::camPos;
        float distSqB = sep.dot(sep);
        return ( distSqA > distSqB );// the more distant point is "lesser"
    };

    std::sort( ppTvec.begin(), ppTvec.end(), pComp );

    // assign distSqVec
    vec3f sep;
    for( size_t j = 0; j < ppTvec.size(); ++j )
    {
        sep = *ppTvec[j] + halfCell - persPt::camPos;
        distSqVec[j] = sep.dot( sep );
    }

    // assign idxSorted
    vec3f* pPt0 = &( ptGrid[0] );
    for( size_t j = 0; j < ppTvec.size(); ++j )
    {
        idxSortedVec[j] = ppTvec[j] - pPt0;// not working
    }
 }

 void persBlockGrid::init( std::istream& is )
 {
     is >> pos.x >> pos.y >> pos.z;
     vec3f zu; is >> zu.x >> zu.y >> zu.z;
     zu /= zu.mag();

 //    zu = persPt::camDir;
  //   constructAxes( zu );
     // temp test
  //   Xu = persPt::xu;
  //   Yu = persPt::yu;
  //   Zu = persPt::camDir;

     is >> Rows >> Cols >> Layers;
     is >> cellW >> cellH >> cellD;
     // todo: read and assign colors

     // axis labels
     if( pFont ) xAxisLabel.setFont( *pFont );
     xAxisLabel.setCharacterSize( labelCharSz );
     yAxisLabel = zAxisLabel = xAxisLabel;
     xAxisLabel.setFillColor( sf::Color::Black );
     yAxisLabel.setFillColor( sf::Color::Red );
     zAxisLabel.setFillColor( sf::Color::Blue );
     xAxisLabel.setString("X");
     yAxisLabel.setString("Y");
     zAxisLabel.setString("Z");
     // label positions assigned in constructAxes()

     Rbound = 0.5f*sqrt( (Rows*cellH)*(Rows*cellH) + (Cols*cellW)*(Cols*cellW) + (Layers*cellD)*(Layers*cellD) );
     idxSplit = 0;// where ball draw is among cells

     unsigned int numCells = Rows*Cols*Layers;
     ptGrid.resize( numCells );
     ppTvec.resize( numCells );
     distSqVec.resize( numCells );
     idxSortedVec.resize( numCells );
     for( size_t j = 0; j < ptGrid.size(); ++j ) ppTvec[j] = &ptGrid[j];
     constructAxes( zu );
  //   for( size_t it = 0; it < ptGrid.size(); ++it )
  //       ptGrid[it] = pos + (it%Cols)*cellW*Xu + (it/Cols)*cellH*Yu;

     doDrawCell.resize( numCells );// one for each cell
     for( size_t it = 0; it < doDrawCell.size(); ++it ) doDrawCell[it] = true;// draw all

     // size the containers

 //   sf::Color twoColors[] = { sf::Color::Yellow, sf::Color::Cyan };
//    quadVec.resize( 12*numCells );
 //   lineVec.resize( 11*numCells );
 //   for( sf::Vertex& V : lineVec ) V.color = sf::Color::Black;
//    std::cout << "\n PBG::init() quadVec.size() = " << quadVec.size() << " lineVec.size() = " << lineVec.size();

    vtxArrayVec.resize( 2*numCells );// 2 draws for each cell

 //   std::cout << "\n PBG::init() vtxArrayVec.size() = " << vtxArrayVec.size();

     updateGrid();

     axisLines[0].color = axisLines[1].color = sf::Color::Black;// x axis
     axisLines[2].color = axisLines[3].color = sf::Color::Red;// y axis
     axisLines[4].color = axisLines[5].color = sf::Color::Blue;// z axis
   //  for( size_t j = 0; j < quadVec.size(); ++j ) quadVec[j].color = twoColors[ (j/12)%2 ];// alternate every 3 Quads
 }

void persBlockGrid::setPosition( vec3f Pos )
{
    pos = Pos;
 //   constructAxes( Zu );
    assignPoints();
    zOrderCells();
    updateGrid();
}

bool persBlockGrid::hitBlock( vec3f posLast, bool KO )// take out a cell
{
    if( !pBall ) return false;
    // nothing yet
    idxSplit = 0;// to assure return to 0
    doDrawBall = false;

    vec3f P, Vel = pBall->vel;
    unsigned int Rhit = 0, Chit = 0, Lhit = 0;
    if( isInGrid( pBall->pos, Rhit, Chit, Lhit ) )
    {
        unsigned int idx = Chit + Cols*Rhit + Rows*Cols*Lhit;
        vec3f cellPos = getCellPos( Rhit, Chit, Lhit );
        bool tookOutCell = false;

    //    if( doDrawCell[idx] && hitCell( cellPos, ballPosLast, pBall->pos, P, Vel ) )// knock it out
        if( doDrawCell[idx] && hitCell( cellPos, posLast, pBall->pos, P, Vel ) )// knock it out
        {
            if( KO )
            {
                doDrawCell[idx] = false;
                tookOutCell = true;
                updateGrid();
       //         Score += 100;
       //         to_SF_string( scoreNumMsg, Score );
            }

            pBall->setPosition( P );
            pBall->vel = Vel;

        }

        std::cout << "\n** in grid";
        find_idxSplit();
        doDrawBall = true;

        return tookOutCell;// return is used for scoring
    }

    return false;
}

void persBlockGrid::update( float dt )
{

}

/*
void persBlockGrid::draw( sf::RenderTarget& RT ) const
{
    if( pBall && doDrawBall )
    {
        if( idxSplit == 0 )
        {
            pBall->draw(RT);
            std::cout << "\nblock draws ball first";
            for( const sf::VertexArray& VA : vtxArrayVec )
                RT.draw( VA );
        }
        else// idxSplit > 0
        {
            for( unsigned int j = 0; j < vtxArrayCnt; j += 2 )
            {
                if( j == 2*idxSplit )
                    { pBall->draw(RT); std::cout << "\nblock draws ball"; }

                RT.draw( vtxArrayVec[j] );
                RT.draw( vtxArrayVec[j+1] );
            }
            // ball draw after last cell?
            if( 2*idxSplit == vtxArrayCnt )
                { pBall->draw(RT); std::cout << "\nblock draws ball after last cell"; }
        }
    }
    else
    {
        for( const sf::VertexArray& VA : vtxArrayVec )
                RT.draw( VA );
    }

    if( doDrawAxes )
    {
        RT.draw( axisLines, 6, sf::Lines );
        RT.draw( xAxisLabel );
        RT.draw( yAxisLabel );
        RT.draw( zAxisLabel );
    }
}   */


void persBlockGrid::draw( sf::RenderTarget& RT ) const
{
 //   unsigned int lineIt = 0;// will iterate within loop over Quads


    // ball draw 1st?
    if( idxSplit == 0 && pBall && doDrawBall )
        { pBall->draw(RT); std::cout << "\nblock draws ball first"; }

    for( unsigned int j = 0; j < vtxArrayCnt; j += 2 )
    {
    //    if( pBall && idxSplit > 0 && j == 12*idxSplit ){ pBall->draw(RT); std::cout << "\ncell draws ball"; }
        if( pBall && doDrawBall && idxSplit > 0 && j == 2*idxSplit )
            { pBall->draw(RT); std::cout << "\nblock draws ball"; }

    //    if( doDrawCell[j/2] )
        if( true )
        {
            RT.draw( vtxArrayVec[j] );
            RT.draw( vtxArrayVec[j+1] );
        }
    }
    // ball draw after last cell?
    if( 2*idxSplit == vtxArrayCnt && pBall && doDrawBall )
        { pBall->draw(RT); std::cout << "\nblock draws ball after last cell"; }


    if( doDrawAxes )
    {
        RT.draw( axisLines, 6, sf::Lines );
        RT.draw( xAxisLabel );
        RT.draw( yAxisLabel );
        RT.draw( zAxisLabel );
    }
}


bool persBlockGrid::isInGrid( vec3f pt )const
{
 //   vec3f sep = pos - pt;
    vec3f sep = pt - Origin;

    if( sep.dot(Xu) < 0.0f ) return false;// left
    if( sep.dot(Xu) > Cols*cellW ) return false;// right

    if( sep.dot(Yu) < 0.0f ) return false;// below
    if( sep.dot(Yu) > Rows*cellH ) return false;// above

    if( sep.dot(Zu) < 0.0f ) return false;// behind
    if( sep.dot(Zu) > Layers*cellD ) return false;// in front

    // temp
 //   if( sep.dot(persPt::xu) < 0.0f ) return false;// left
 //   if( sep.dot(persPt::xu) > Cols*cellW ) return false;// right

 //   if( sep.dot(persPt::yu) < 0.0f ) return false;// below
 //   if( sep.dot(persPt::yu) > Rows*cellH ) return false;// above

  //  if( sep.dot(persPt::camDir) < 0.0f ) return false;// behind
  //  if( sep.dot(persPt::camDir) > Layers*cellD ) return false;// in front

    return true;
}

bool persBlockGrid::isInGrid( vec3f pt, vec3f& cellPos )const// writes position of cell corner
{
    unsigned int R = 0, C = 0, L = 0;
    if( !isInGrid( pt, R, C, L ) ) return false;
    cellPos = Origin + C*cellW*Xu + R*cellH*Yu + L*cellD*Zu;
    return true;
}

bool persBlockGrid::isInGrid( vec3f pt, unsigned int& R, unsigned int& C, unsigned int& L )const// writes within which cell
{
    if( !isInGrid(pt) ) return false;
    vec3f sep = pt - Origin;
    R = static_cast<unsigned int>( sep.dot(Yu)/cellH );
    C = static_cast<unsigned int>( sep.dot(Xu)/cellW );
    L = static_cast<unsigned int>( sep.dot(Zu)/cellD );

    return true;
}

void persBlockGrid::find_idxSplit()// call when ball is found in cell
{
    if( !pBall ) return;

    vec3f sep = pBall->pos - persPt::camPos;
    float distSq = sep.dot(sep);
    idxSplit = 0;
    unsigned int numCells = Rows*Cols*Layers;
    for( unsigned int j = 0; j < numCells; ++j )
    {
        unsigned int Sj = ppTvec[j] - &( ptGrid[0] );// offset to cell
        if( doDrawCell[Sj] )
        {
          //  if( distSq < distSqVec[j] ) break;
            if( distSq > distSqVec[j] ) break;
            ++idxSplit;
        }
    }

    std::cout << "\n idxSplit = " << idxSplit;
}

bool persBlockGrid::lineIntersects( vec3f P0, vec3f v, vec3f& HitPt, vec3f& Ns )const
{
    // HitPt = P0 + v*t = origin + R
    // find the t to each planar surface
    float tx0 = Xu.dot( Origin - P0 )/Xu.dot(v);
    float txW = ( cellW*Cols + Xu.dot( Origin - P0 ) )/Xu.dot(v);
    float ty0 = Yu.dot( Origin - P0 )/Yu.dot(v);
    float tyH = ( cellH*Rows + Yu.dot( Origin - P0 ) )/Yu.dot(v);
    float tz0 = Zu.dot( Origin - P0 )/Zu.dot(v);
    float tzD = ( cellD*Layers + Zu.dot( Origin - P0 ) )/Zu.dot(v);

    float t = tx0;
    if( tx0 > 0.0f )
    {
        if( txW > 0.0f && txW < tx0 ) t = txW;
    }
    else if( txW > 0.0f )// check Y,Z
        t = txW;

    vec3f R = P0 + v*t - Origin;
    if( t > 0.0f && (R.dot(Yu) > 0.0f && R.dot(Yu) < cellH*Rows) && (R.dot(Zu) > 0.0f && R.dot(Zu) < cellD*Layers) )
    {
        HitPt = P0 + v*t;
        Ns = v.dot(Xu) < 0.0f ? Xu : -Xu;
        return true;
    }
    // hit plane y = constant?
    t = ty0;
    if( ty0 > 0.0f )
    {
        if( tyH > 0.0f && tyH < ty0 ) t = tyH;
    }
    else if( tyH > 0.0f )// check X,Z
        t = tyH;

    R = P0 + v*t - Origin;
    if( t > 0.0f && (R.dot(Xu) > 0.0f && R.dot(Xu) < cellW*Cols) && (R.dot(Zu) > 0.0f && R.dot(Zu) < cellD*Layers) )
    {
        HitPt = P0 + v*t;
        Ns = v.dot(Yu) < 0.0f ? Yu : -Yu;
        return true;
    }
    // hit plane z = constant?
    t = tz0;
    if( tz0 > 0.0f )
    {
        if( tzD > 0.0f && tzD < tz0 ) t = tzD;
    }
    else if( tzD > 0.0f )// check X,Y
        t = tzD;

    R = P0 + v*t - Origin;
    if( t > 0.0f && (R.dot(Xu) > 0.0f && R.dot(Xu) < cellW*Cols) && (R.dot(Yu) > 0.0f && R.dot(Yu) < cellH*Rows) )
    {
        HitPt = P0 + v*t;
        Ns = v.dot(Zu) < 0.0f ? Zu : -Zu;
        return true;
    }

    return false;
}

bool persBlockGrid::hit( vec3f posLast, vec3f posNow, vec3f& P, vec3f& vel )const
{
    bool inLast = isInGrid( posLast ), inNow = isInGrid( posNow );
    P = posNow;
//    float S = 1.0f;// to interpolate position

    if( inNow && !inLast )
    {
        float Sx = 1.0f, Sy = 1.0f, Sz = 1.0f;// to interpolate position. Least S wins!
        vec3f dVel = 2.0f*vel;// default is reverse vel

        vec3f Snow = posNow - Origin, Slast = posLast - Origin;
        vec3f dPos = posNow - posLast;
    //    vec3f dVel;
        // check Z first
        if( Slast.dot(Zu) < 0.0f && Snow.dot(Zu) > 0.0f )// crossed back
        {
            Sz = ( Origin - posLast ).dot(Zu)/dPos.dot(Zu);
            dVel = 2.0f*( vel.dot(Zu) )*Zu;
        }
        else if( Slast.dot(Zu) > Layers*cellD && Snow.dot(Zu) < Layers*cellD )// crossed front
        {
            Sz = ( Origin + Layers*cellD*Zu - posLast ).dot(Zu)/dPos.dot(Zu);
            dVel = 2.0f*( vel.dot(Zu) )*Zu;
        }

        // check X
        if( Slast.dot(Xu) < 0.0f && Snow.dot(Xu) > 0.0f )// crossed left
        {
            Sx = ( Origin - posLast ).dot(Xu)/dPos.dot(Xu);
            dVel = 2.0f*( vel.dot(Xu) )*Xu;
        }
        else if( Slast.dot(Xu) > Cols*cellW && Snow.dot(Xu) < Cols*cellW )// crossed right
        {
            Sx = ( Origin + Cols*cellW*Xu - posLast ).dot(Xu)/dPos.dot(Xu);
            dVel = 2.0f*( vel.dot(Xu) )*Xu;
        }

        // check Y
        if( Slast.dot(Yu) < 0.0f && Snow.dot(Yu) > 0.0f )// crossed bottom
        {
            Sy = ( Origin - posLast ).dot(Yu)/dPos.dot(Yu);
            dVel = 2.0f*( vel.dot(Yu) )*Yu;
        }
        else if( Slast.dot(Yu) > Rows*cellH && Snow.dot(Yu) < Rows*cellH )// crossed top
        {
            Sy = ( Origin + Rows*cellH*Yu - posLast ).dot(Yu)/dPos.dot(Yu);
            dVel = 2.0f*( vel.dot(Yu) )*Yu;
        }

        float S = Sx < Sy ? Sx : Sy;
        if( Sz < S ) S = Sz;

        P = posLast + S*dPos;
        vel -= dVel;

     //   std::cout << "\n In but no bounce";
        return true;// must be
    }

    return false;
}

bool persBlockGrid::hitCell( vec3f cellPos, vec3f posLast, vec3f posNow, vec3f& P, vec3f& vel )const
{
 //   bool inLast = isInGrid( posLast ), inNow = isInGrid( posNow );
    bool inNow = isInGrid( posNow );
    P = posNow;
    float Sx = 1.0f, Sy = 1.0f, Sz = 1.0f;// to interpolate position. Least S wins!
    vec3f dVel = 2.0f*vel;// default is reverse vel

    if( inNow )
    {
    //    if( !inLast )// find which face was penetrated and *= -1 that vel component
     //   {
            vec3f Snow = posNow - cellPos, Slast = posLast - cellPos;
            vec3f dPos = posNow - posLast;


            // check Z first
            if( Slast.dot(Zu) < 0.0f && Snow.dot(Zu) > 0.0f )// crossed back
            {
                Sz = ( cellPos - posLast ).dot(Zu)/dPos.dot(Zu);
                dVel = 2.0f*( vel.dot(Zu) )*Zu;
            }
            else if( Slast.dot(Zu) > cellD && Snow.dot(Zu) < cellD )// crossed front
            {
                Sz = ( cellPos + cellD*Zu - posLast ).dot(Zu)/dPos.dot(Zu);
                dVel = 2.0f*( vel.dot(Zu) )*Zu;
            }

            // check X
            if( Slast.dot(Xu) < 0.0f && Snow.dot(Xu) > 0.0f )// crossed left
            {
                Sx = ( cellPos - posLast ).dot(Xu)/dPos.dot(Xu);
                dVel = 2.0f*( vel.dot(Xu) )*Xu;
            }
            else if( Slast.dot(Xu) > cellW && Snow.dot(Xu) < cellW )// crossed right
            {
                Sx = ( cellPos + cellW*Xu - posLast ).dot(Xu)/dPos.dot(Xu);
                dVel = 2.0f*( vel.dot(Xu) )*Xu;
            }

            // check Y
            if( Slast.dot(Yu) < 0.0f && Snow.dot(Yu) > 0.0f )// crossed bottom
            {
                Sy = ( cellPos - posLast ).dot(Yu)/dPos.dot(Yu);
                dVel = 2.0f*( vel.dot(Yu) )*Yu;
            }
            else if( Slast.dot(Yu) > cellH && Snow.dot(Yu) < cellH )// crossed top
            {
                Sy = ( cellPos + cellH*Yu - posLast ).dot(Yu)/dPos.dot(Yu);
                dVel = 2.0f*( vel.dot(Yu) )*Yu;
            }

            float S = Sx < Sy ? Sx : Sy;
            if( Sz < S ) S = Sz;
            P = posLast + S*dPos;
        //    std::cout << "\n inNow and NOT inLast";
            std::cout << "\n cell hit";
        //    vel -= dVel;
    //    }
     //   else// in now and last
     //   {
          //  vel *= -1.0f;
          //  P = posNow - 2.0f*( posNow - posLast );
      //      P = 2.0f*posLast - posNow ;
        //    std::cout << "\n inNow AND inLast";
     //   }

        vel -= dVel;

        return true;// must be
    }

    return false;
}

/*
// working but flawed. missing cases
bool persBlockGrid::hitCell( vec3f cellPos, vec3f posLast, vec3f posNow, vec3f& P, vec3f& vel )const
{
    bool inLast = isInGrid( posLast ), inNow = isInGrid( posNow );
    P = posNow;
    float Sx = 1.0f, Sy = 1.0f, Sz = 1.0f;// to interpolate position. Least S wins!

    if( inNow && !inLast )
    {
        vec3f Snow = posNow - cellPos, Slast = posLast - cellPos;
        vec3f dPos = posNow - posLast;
        vec3f dVel;

        // check Z first
        if( Slast.dot(Zu) < 0.0f && Snow.dot(Zu) > 0.0f )// crossed back
        {
            Sz = ( cellPos - posLast ).dot(Zu)/dPos.dot(Zu);
            dVel = 2.0f*( vel.dot(Zu) )*Zu;
        }
        else if( Slast.dot(Zu) > cellD && Snow.dot(Zu) < cellD )// crossed front
        {
            Sz = ( cellPos + cellD*Zu - posLast ).dot(Zu)/dPos.dot(Zu);
            dVel = 2.0f*( vel.dot(Zu) )*Zu;
        }

        // check X
        if( Slast.dot(Xu) < 0.0f && Snow.dot(Xu) > 0.0f )// crossed left
        {
            Sx = ( cellPos - posLast ).dot(Xu)/dPos.dot(Xu);
            dVel = 2.0f*( vel.dot(Xu) )*Xu;
        }
        else if( Slast.dot(Xu) > cellW && Snow.dot(Xu) < cellW )// crossed right
        {
            Sx = ( cellPos + cellW*Xu - posLast ).dot(Xu)/dPos.dot(Xu);
            dVel = 2.0f*( vel.dot(Xu) )*Xu;
        }

        // check Y
        if( Slast.dot(Yu) < 0.0f && Snow.dot(Yu) > 0.0f )// crossed bottom
        {
            Sy = ( cellPos - posLast ).dot(Yu)/dPos.dot(Yu);
            dVel = 2.0f*( vel.dot(Yu) )*Yu;
        }
        else if( Slast.dot(Yu) > cellH && Snow.dot(Yu) < cellH )// crossed top
        {
            Sy = ( cellPos + cellH*Yu - posLast ).dot(Yu)/dPos.dot(Yu);
            dVel = 2.0f*( vel.dot(Yu) )*Yu;
        }

        float S = Sx < Sy ? Sx : Sy;
        if( Sz < S ) S = Sz;
        P = posLast + S*dPos;
        vel -= dVel;

     //   std::cout << "\n In but no bounce";
        return true;// must be
    }

    return false;
}
*/

bool persBlockGrid::hitCellImage( sf::Vector2f hitPos, unsigned int& rowHit, unsigned int& colHit )const
{
    int senseZ = persPt::camDir.dot(Zu) > 0.0f ? 1 : -1;

    // define grid bounds
    sf::Vector2f p00 = persPt::get_xyw( Origin );// down left
    sf::Vector2f p10 = persPt::get_xyw( Origin + Cols*cellW*Xu );// down right
    sf::Vector2f p01 = persPt::get_xyw( Origin + Rows*cellH*Yu );// up left

 /*   if( senseZ == 1 )
    {
        p00 = persPt::get_xyw( pos );// down left
        p10 = persPt::get_xyw( pos + Cols*cellW*Xu );// down right
        p01 = persPt::get_xyw( pos + Rows*cellH*Yu );// up left
    }
    else// FAIL!!
    {
        p00 = persPt::get_xyw( pos - Cols*cellW*Xu );// down left
        p10 = persPt::get_xyw( pos );// down right
        p01 = persPt::get_xyw( pos + Rows*cellH*Yu );// up left
    }   */
//    sf::Vector2f p11 = persPt::get_xyw( pos + Cols*cellW*Xu  + Rows*cellH*Yu );// up right

    if( hitPos.x < p00.x ) return false;// left of
    if( hitPos.x > p10.x ) return false;// right of
    if( hitPos.y > p00.y ) return false;// below
    if( hitPos.y < p01.y ) return false;// above

    std::cout << "\n Hit Grid!";
    // which column? check x position
    float sf = persPt::Z0/pos.z;
    float x0 = p00.x;
    for( unsigned int c = 0; c < Cols; ++c )
    {
        if( ( hitPos.x - x0 > sf*c*cellW ) && ( hitPos.x - x0 < sf*(c+1)*cellW ) )
        {
            if( senseZ == 1 ) colHit = c;
            else colHit = Cols - ( 1 + c );// does not fix
            break;
        }
    }

    // which row? check y position
    float y0 = p00.y;
    for( unsigned int r = 0; r < Rows; ++r )
    {
        if( ( y0 - hitPos.y > sf*r*cellH ) && ( y0 - hitPos.y < sf*(r+1)*cellH ) )
        {
            rowHit = r;
            break;
        }
    }
    std::cout << "  row = " << rowHit << "  col = " << colHit;

    return true;
}

vec3f persBlockGrid::getCellPos( unsigned int R, unsigned int C, unsigned int L )const// returns origin
{
    return Origin + C*cellW*Xu + R*cellH*Yu + L*cellD*Zu;
}

//void persBlockGrid::updateGrid_2()
void persBlockGrid::updateGrid()
{
    update_doDraw();
    if( !doDraw ) return;

    // sort grid vec3f by distance in constructAxes()

    // new way
    static sf::VertexArray quadVA( sf::Quads, 12 );// for the 3 Quads
    static sf::VertexArray lineVA( sf::LineStrip, 11 );// for the LineStrip
    vtxArrayCnt = 0;

    const sf::Color twoColors[] = { sf::Color::Yellow, sf::Color::Cyan };

    unsigned int colorIdx = 0;
    for( const vec3f* pV : ppTvec )
    {
        int idx = pV - &( ptGrid[0] );
        unsigned int row = ( idx/Cols )%Rows, col = idx%Cols, layer = idx/(Rows*Cols);

        if( getCellPoints( pV, quadVA, lineVA ) )
        {
            colorIdx = ( row + col + layer )%2;
            for( unsigned int n = 0; n < quadVA.getVertexCount(); ++n ) quadVA[n].color = twoColors[ colorIdx ];
            for( unsigned int n = 0; n < lineVA.getVertexCount(); ++n ) lineVA[n].color = sf::Color::Black;

            // 2 draws per cell
            vtxArrayVec[ vtxArrayCnt++ ] = quadVA;
            vtxArrayVec[ vtxArrayCnt++ ] = lineVA;
        }
    }

 //   std::cout << "\n updateGrid() vtxArrayCnt = " << vtxArrayCnt;

    axisLines[0].position = persPt::get_xyw( Origin );// x axis
    axisLines[1].position = persPt::get_xyw( Origin + ( Cols*cellW + 200.0f )*Xu );
    axisLines[2].position = persPt::get_xyw( Origin );// y axis
    axisLines[3].position = persPt::get_xyw( Origin + ( Rows*cellH + 200.0f )*Yu );
    axisLines[4].position = persPt::get_xyw( Origin );// z axis
    axisLines[5].position = persPt::get_xyw( Origin + ( Layers*cellD + 200.0f )*Zu );

    vec3f labelPos = Origin + ( Cols*cellW + ofstL + 200.0f )*Xu + ofstPerp*Yu;// x
    xAxisLabel.setPosition( persPt::get_xyw( labelPos ) );
    labelPos = Origin + ( Rows*cellH + ofstL + 200.0f )*Yu - ofstPerp*Xu;// y
    yAxisLabel.setPosition( persPt::get_xyw( labelPos ) );
    labelPos = Origin + ( Layers*cellD + ofstL + 200.0f )*Zu + ofstPerp*Yu;// y
    zAxisLabel.setPosition( persPt::get_xyw( labelPos ) );

}

bool persBlockGrid::getCellPoints( const vec3f* pPt, sf::VertexArray& quadVA, sf::VertexArray& lineVA )
{
    if( !pPt ) return false;

    int Idx = pPt - &( ptGrid[0] );
    if( Idx < 0 ) return false;
    if( !doDrawCell[ Idx ] ) return false;

//    vec3f halfCell = 0.5f*( cellW*Xu + cellH*Yu + cellD*Zu );
    vec3f cellCtr = *pPt + halfCell;
    vec3f sep = persPt::camPos - cellCtr;

    bool drawRight = sep.dot( Xu ) > 0.5f*cellW;
    bool drawLeft = sep.dot( Xu ) < -0.5f*cellW;
    bool drawTop = sep.dot( Yu ) > 0.5f*cellH;
    bool drawBottom = sep.dot( Yu ) < -0.5f*cellH;

    int senseX = 1;// 1 or -1  affects points 3, 10
    int senseY = 1;// or -1    affects points 2, 4, 7
    int senseZ = sep.dot( Zu ) < 0.0f ? 1 : -1;// affects only point 6

    unsigned int quadVAsize = 4, lineVAsize = 5;
    if( drawTop || drawBottom ){ quadVAsize += 4; lineVAsize += 3; }
    if( drawLeft || drawRight ){ quadVAsize += 4; lineVAsize += 3; }
    quadVA.resize( quadVAsize );
    lineVA.resize( lineVAsize );

    vec3f pt1 = *pPt;
    if( senseZ == -1 ) pt1 += cellD*Zu;

    if( drawRight ) { pt1 += cellW*Xu; senseX = -1; }
    if( drawBottom ) { pt1 += cellH*Yu; senseY = -1; }

    // assign 5, 8 or 11 points
    unsigned int idxQd = 0, idxLn = 0;

    // draw face
    sf::Vector2f xyW = persPt::get_xyw( pt1 );
    quadVA[ idxQd++ ].position = xyW;// q 1
    lineVA[ idxLn++ ].position = xyW;// L 1

    vec3f pt2 = pt1 + senseY*cellH*Yu;// pt 2
    xyW = persPt::get_xyw( pt2 );
    quadVA[ idxQd++ ].position = xyW;// q 2
    lineVA[ idxLn++ ].position = xyW;// L 2

    vec3f pt3 = pt2 + senseX*cellW*Xu;// pt 3
    xyW = persPt::get_xyw( pt3 );
    quadVA[ idxQd++ ].position = xyW;// q 3
    lineVA[ idxLn++ ].position = xyW;// L 3

    xyW = persPt::get_xyw( pt3 - senseY*cellH*Yu );// pt 4
    quadVA[ idxQd++ ].position = xyW;// q 4 - 1st quad complete
    lineVA[ idxLn++ ].position = xyW;// L 4

    xyW = persPt::get_xyw( pt1 );// pt 5
    lineVA[ idxLn++ ].position = xyW;// L 5

    vec3f pt6 = pt1 + senseZ*cellD*Zu;// pt 6
    vec3f pt7 = pt6 + senseY*cellH*Yu;// pt 7

 // side
    if( drawLeft || drawRight )
    {
        quadVA[ idxQd++ ].position = xyW;// q 5 - 2nd quad start

        xyW = persPt::get_xyw( pt6 );// pt 6
        quadVA[ idxQd++ ].position = xyW;// q 6
        lineVA[ idxLn++ ].position = xyW;// L 6

        xyW = persPt::get_xyw( pt7 );// pt 7
        quadVA[ idxQd++ ].position = xyW;// q 7
        lineVA[ idxLn++ ].position = xyW;// L 7

        xyW = persPt::get_xyw( pt2 );// pt 8
        quadVA[ idxQd++ ].position = xyW;// q 8 - 2nd quad complete
        lineVA[ idxLn++ ].position = xyW;// L 8
    }

    if( drawTop || drawBottom )
    {
        xyW = persPt::get_xyw( pt2 );// pt 8
        quadVA[ idxQd++ ].position = xyW;// q 9 - 3rd quad start

        xyW = persPt::get_xyw( pt7 );// pt 9
        quadVA[ idxQd++ ].position = xyW;// q 10
        lineVA[ idxLn++ ].position = xyW;// L 9

        xyW = persPt::get_xyw( pt7 + senseX*cellW*Xu );// pt 10
        quadVA[ idxQd++ ].position = xyW;// q 11
        lineVA[ idxLn++ ].position = xyW;// L 10

        xyW = persPt::get_xyw( pt3 );// pt 11
        quadVA[ idxQd++ ].position = xyW;// q 12 - 3rd quad complete
        lineVA[ idxLn++ ].position = xyW;// L 11
    }

    return true;
}

/*
bool persBlockGrid::getCellPoints( const vec3f* pPt, sf::VertexArray& quadVA, sf::VertexArray& lineVA )
{
    if( !pPt ) return false;

    int Idx = pPt - &( ptGrid[0] );
    if( Idx < 0 ) return false;
    if( !doDrawCell[ Idx ] ) return false;

    quadVA.resize(12);
    lineVA.resize(11);

//    vec3f halfCell = 0.5f*( cellW*Xu + cellH*Yu + cellD*Zu );
    vec3f cellCtr = *pPt + halfCell;
    vec3f sep = cellCtr - persPt::camPos;

    bool drawRight = sep.dot( Xu ) < 0.0f;
    bool drawTop = sep.dot( Yu ) < 0.0f;

    int senseX = 1;// 1 or -1  affects points 3, 10
    int senseY = 1;// or -1    affects points 2, 4, 7
    int senseZ = sep.dot( Zu ) > 0.0f ? 1 : -1;// affects only point 6


    vec3f pt1 = *pPt;


    // assigns for the 4 cases
    if( drawTop )
    {
        if( drawRight )
        {
            pt1 += cellW*Xu;
            senseX = -1;// affects points 3, 10
            senseY = 1;//
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n top and right!";
        }
        else// draw left
        {
        //  no offset for pt1
            senseX = 1;
            senseY = 1;
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n top and left!  ";
        }
    }
    else// draw bottom
    {
        if( drawRight )
        {
            senseX = -1;
            senseY = -1;
            pt1 += cellW*Xu + cellH*Yu;
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n bottom and right!";
        }
        else// draw left
        {
            senseX = 1;
            senseY = -1;
            pt1 += cellH*Yu;
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n bottom and left!";
        }
    }

 //   std::cout << "\n senseX: " << senseX << " senseY: " << senseY << " senseZ: " << senseZ;

    // assign the 11 points
    unsigned int idxQd = 0, idxLn = 0;

    sf::Vector2f xyW = persPt::get_xyw( pt1 );
    quadVA[ idxQd++ ].position = xyW;// q 1
    lineVA[ idxLn++ ].position = xyW;// L 1


    vec3f pt2 = pt1 + senseY*cellH*Yu;// pt 2
    xyW = persPt::get_xyw( pt2 );
    quadVA[ idxQd++ ].position = xyW;// q 2
    lineVA[ idxLn++ ].position = xyW;// L 2

    vec3f pt3 = pt2 + senseX*cellW*Xu;// pt 3
    xyW = persPt::get_xyw( pt3 );
    quadVA[ idxQd++ ].position = xyW;// q 3
    lineVA[ idxLn++ ].position = xyW;// L 3

    xyW = persPt::get_xyw( pt3 - senseY*cellH*Yu );// pt 4
    quadVA[ idxQd++ ].position = xyW;// q 4 - 1st quad complete
    lineVA[ idxLn++ ].position = xyW;// L 4

    xyW = persPt::get_xyw( pt1 );// pt 5
    quadVA[ idxQd++ ].position = xyW;// q 5 - 2nd quad start
    lineVA[ idxLn++ ].position = xyW;// L 5

 //   vec3f pt6 = pt1 + cellD*Zu;
    vec3f pt6 = pt1 + senseZ*cellD*Zu;
    xyW = persPt::get_xyw( pt6 );// pt 6
    quadVA[ idxQd++ ].position = xyW;// q 6
    lineVA[ idxLn++ ].position = xyW;// L 6

    vec3f pt7 = pt6 + senseY*cellH*Yu;// pt 7
    xyW = persPt::get_xyw( pt7 );
    quadVA[ idxQd++ ].position = xyW;// q 7
    lineVA[ idxLn++ ].position = xyW;// L 7

    xyW = persPt::get_xyw( pt2 );// pt 8
    quadVA[ idxQd++ ].position = xyW;// q 8 - 2nd quad complete
    quadVA[ idxQd++ ].position = xyW;// q 9 - 3rd quad start
    lineVA[ idxLn++ ].position = xyW;// L 8

    xyW = persPt::get_xyw( pt7 );// pt 9
    quadVA[ idxQd++ ].position = xyW;// q 10
    lineVA[ idxLn++ ].position = xyW;// L 9

    xyW = persPt::get_xyw( pt7 + senseX*cellW*Xu );// pt 10
    quadVA[ idxQd++ ].position = xyW;// q 11
    lineVA[ idxLn++ ].position = xyW;// L 10

    xyW = persPt::get_xyw( pt3 );// pt 11
    quadVA[ idxQd++ ].position = xyW;// q 12 - 3rd quad complete
    lineVA[ idxLn++ ].position = xyW;// L 11


    return true;
}
*/

/*
//void persBlockGrid::updateGrid()
void persBlockGrid::updateGrid_2()
{
    // sort grid vec3f by distance in constructAxes()

    std::vector<const vec3f*> ppTvec( ptGrid.size(), nullptr );
    for( size_t j = 0; j < ptGrid.size(); ++j ) ppTvec[j] = &ptGrid[j];

    std::function<bool( const vec3f*, const vec3f* )> pComp = [this]( const vec3f* pvA, const vec3f* pvB )
    {
        vec3f sep = *pvA - persPt::camPos;
        float distSqA = sep.dot(sep);
        sep = *pvB - persPt::camPos;
        float distSqB = sep.dot(sep);
        return ( distSqA > distSqB );// the more distant point is "lesser"
    };

    std::sort( ppTvec.begin(), ppTvec.end(), pComp );


    // map 3 x sf::Quads and 11 element sf::LinesStrip to each cell

    // new way
    static sf::Vector2f quadPos[12];// for the 3 Quads
    static sf::Vector2f linePos[11];// LinesStrip around face + side + top/bottom
    quadVtxCnt = lineVtxCnt = 0;

//    return;// temp for crash test

    const sf::Color twoColors[] = { sf::Color::Yellow, sf::Color::Cyan };
  //  int senseZ = persPt::camDir.dot(Zu) > 0.0f ? 1 : -1;
 //   for( const vec3f& V : ptGrid )
    std::cout << "\n colorIdx:";
    unsigned int colorIdx = 0;
    for( const vec3f* pV : ppTvec )
    {
        int idx = pV - &( ptGrid[0] );
        unsigned int row = ( idx/Cols )%Rows, col = idx%Cols, layer = idx/(Rows*Cols);
        std::cout << ' ' << col;
        if( getCellPoints( pV, quadPos, linePos ) )
        {
            colorIdx = ( row + col + layer )%2;
      //      std::cout << ' ' << col;
            for( unsigned int n = 0; n < 12; ++n )
            {
                quadVec[ quadVtxCnt ].position = quadPos[n];
                quadVec[ quadVtxCnt++ ].color = twoColors[ colorIdx ];// finally! This works
            }
            for( unsigned int n = 0; n < 11; ++n )
            {
                lineVec[ lineVtxCnt++ ].position = linePos[n];
            }
            ++colorIdx;
        }
    }

    std::cout << "\n updateGrid() quadVtxCnt = " << quadVtxCnt;// sb 12xRows*Cols*Layers
    std::cout << "\n lineVtxCnt = " << lineVtxCnt;// sb 11xRows*Cols*Layers


  //  for( size_t j = 0; j < quadVec.size(); ++j ) quadVec[j].color = twoColors[ (j/12)%2 ];// alternate every 3 Quads
}


bool persBlockGrid::getCellPoints( const vec3f* pPt, sf::Vector2f* QuadPos, sf::Vector2f* LinePos )
{
    if( !pPt ) return false;

    int Idx = pPt - &( ptGrid[0] );
    if( Idx < 0 ) return false;
    if( !doDrawCell[ Idx ] ) return false;

 //   vec3f halfCell = 0.5f*( cellW*Xu + cellH*Yu + cellD*Zu );
    vec3f cellCtr = *pPt + halfCell;
    vec3f sep = cellCtr - persPt::camPos;

 //   bool drawRight = sep.dot( persPt::xu ) < 0.0f;
 //   bool drawTop = sep.dot( persPt::yu ) < 0.0f;
    bool drawRight = sep.dot( Xu ) < 0.0f;
    bool drawTop = sep.dot( Yu ) < 0.0f;

    int senseX = 1;// 1 or -1  affects points 3, 10
    int senseY = 1;// or -1    affects points 2, 4, 7
    int senseZ = sep.dot( Zu ) > 0.0f ? 1 : -1;// affects only point 6


    vec3f pt1 = *pPt;


    // assigns for the 4 cases
    if( drawTop )
    {
        if( drawRight )
        {
            pt1 += cellW*Xu;
            senseX = -1;// affects points 3, 10
            senseY = 1;//
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n top and right!";
        }
        else// draw left
        {
        //  no offset for pt1
            senseX = 1;
            senseY = 1;
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n top and left!  ";
        }
    }
    else// draw bottom
    {
        if( drawRight )
        {
            senseX = -1;
            senseY = -1;
            pt1 += cellW*Xu + cellH*Yu;
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n bottom and right!";
        }
        else// draw left
        {
            senseX = 1;
            senseY = -1;
            pt1 += cellH*Yu;
            if( senseZ == -1 ) pt1 += cellD*Zu;
        //    std::cout << "\n bottom and left!";
        }
    }

 //   std::cout << "\n senseX: " << senseX << " senseY: " << senseY << " senseZ: " << senseZ;

    // assign the 11 points
    unsigned int idxQd = 0, idxLn = 0;

    sf::Vector2f xyW = persPt::get_xyw( pt1 );
    QuadPos[ idxQd++ ] = xyW;// q 1
    LinePos[ idxLn++ ] = xyW;// L 1

    vec3f pt2 = pt1 + senseY*cellH*Yu;// pt 2
    xyW = persPt::get_xyw( pt2 );
    QuadPos[ idxQd++ ] = xyW;// q 2
    LinePos[ idxLn++ ] = xyW;// L 2

    vec3f pt3 = pt2 + senseX*cellW*Xu;// pt 3
    xyW = persPt::get_xyw( pt3 );
    QuadPos[ idxQd++ ] = xyW;// q 3
    LinePos[ idxLn++ ] = xyW;// L 3

    xyW = persPt::get_xyw( pt3 - senseY*cellH*Yu );// pt 4
    QuadPos[ idxQd++ ] = xyW;// q 4 - 1st quad complete
    LinePos[ idxLn++ ] = xyW;// L 4

    xyW = persPt::get_xyw( pt1 );// pt 5
    QuadPos[ idxQd++ ] = xyW;// q 5 - 2nd quad start
    LinePos[ idxLn++ ] = xyW;// L 5

 //   vec3f pt6 = pt1 + cellD*Zu;
    vec3f pt6 = pt1 + senseZ*cellD*Zu;
    xyW = persPt::get_xyw( pt6 );// pt 6
    QuadPos[ idxQd++ ] = xyW;// q 6
    LinePos[ idxLn++ ] = xyW;// L 6

    vec3f pt7 = pt6 + senseY*cellH*Yu;// pt 7
    xyW = persPt::get_xyw( pt7 );
    QuadPos[ idxQd++ ] = xyW;// q 7
    LinePos[ idxLn++ ] = xyW;// L 7

    xyW = persPt::get_xyw( pt2 );// pt 8
    QuadPos[ idxQd++ ] = xyW;// q 8 - 2nd quad complete
    QuadPos[ idxQd++ ] = xyW;// q 9 - 3rd quad start
    LinePos[ idxLn++ ] = xyW;// L 8

    xyW = persPt::get_xyw( pt7 );// pt 9
    QuadPos[ idxQd++ ] = xyW;// q 10
    LinePos[ idxLn++ ] = xyW;// L 9

    xyW = persPt::get_xyw( pt7 + senseX*cellW*Xu );// pt 10
    QuadPos[ idxQd++ ] = xyW;// q 11
    LinePos[ idxLn++ ] = xyW;// L 10

    xyW = persPt::get_xyw( pt3 );// pt 11
    QuadPos[ idxQd++ ] = xyW;// q 12 - 3rd quad complete
    LinePos[ idxLn++ ] = xyW;// L 11

    return true;
}
*/
