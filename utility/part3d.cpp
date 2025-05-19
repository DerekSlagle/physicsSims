#include "part3d.h"

float part3d::winHeight = 720.0f;

void part3d::init( sf::Color OutlineColor, sf::Color FillColor )
{
    outlineColor = OutlineColor;
    fillColor = FillColor;
    isComplete = false;
    outlineVA.setPrimitiveType( sf::LinesStrip );
    fillVA.setPrimitiveType( sf::TriangleStrip );
}

void part3d::setVisitOrder()
{
    if( !isComplete ) return;
    visitOrder.clear();

    unsigned int idxLo = ptVec.size()/2, idxHi = ptVec.size()/2;
    std::cout << "\n visit order: ";
    while( idxHi < ptVec.size() && idxLo > 0 )
    {
        std::cout << ' ' << idxHi;
        visitOrder.push_back( idxHi++ );
        if( idxHi == ptVec.size() ) visitOrder.back() = 0;
        visitOrder.push_back( --idxLo );
        std::cout << ' ' << idxLo;
    }

    if( ptVec.size()%2 == 0 )// size is even
        visitOrder.pop_back();

    ptVec.pop_back();// expel the duplicate closing point
    vec3f origin = ptVec[0];
    for( vec3f& V : ptVec ) V -= origin;

    if( visitOrder.size() == ptVec.size() )
    {
        fillVA.clear();
        for( unsigned int j = 0; j < visitOrder.size(); ++j )
        {
            sf::Vector2f vPos( ptVec[ visitOrder[j] ].x, winHeight - ptVec[ visitOrder[j] ].y );
            fillVA.append( sf::Vertex( vPos, sf::Color::Blue ) );
        }
    }
    else
        std::cout << "\n setVisitOrder: VO.sz = " << visitOrder.size() << ", ptVec.sz = " << ptVec.size();
}

void part3d::addPoint( vec3f pt )
{
    sf::Vertex vtx( sf::Vector2f( pt.x, winHeight - pt.y ), sf::Color::Black );
    outlineVA.append( vtx );
    ptVec.push_back( pt );
    if( isComplete )
    {
        setVisitOrder();
        findBBctrPos();
    }
}

void part3d::findBBctrPos()
{
    if( !isComplete ) return;
    vec3f minPos = ptVec[0], maxPos = ptVec[0];
    for( vec3f V : ptVec )// 1st iter a waste
    {
        if( V.x < minPos.x ) minPos.x = V.x;
        if( V.y < minPos.y ) minPos.y = V.y;
        if( V.z < minPos.z ) minPos.z = V.z;

        if( V.x > maxPos.x ) maxPos.z = V.x;
        if( V.y > maxPos.y ) maxPos.z = V.y;
        if( V.z > maxPos.x ) maxPos.z = V.z;
    }

    BBctrPos = 0.5f*( minPos + maxPos );
}

// get a modified part
part3d part3d::flipX()const
{
    part3d retVal( *this );
    for( vec3f& V : retVal.ptVec ) V.y *= -1.0f;
    retVal.BBctrPos.y *= -1.0f;
    return retVal;
}

part3d part3d::flipY()const
{
    part3d retVal( *this );
    for( vec3f& V : retVal.ptVec ) V.x *= -1.0f;
    retVal.BBctrPos.x *= -1.0f;
    return retVal;
}

part3d part3d::toBasis( vec3f Xu, vec3f Yu, vec3f Zu )const
{
    part3d retVal( *this );
    for( vec3f& V : retVal.ptVec )
        V = Xu*V.x + Yu*V.y + Zu*V.z;

    retVal.findBBctrPos();
    return retVal;
}

void part3d::clear()
{
    ptVec.clear();
    outlineVA.clear();
    fillVA.clear();
    isComplete = false;
}

void part3d::update( vec3f Origin, vec3f Xu, vec3f Yu, vec3f Zu )
{
    if( !isComplete ) return;

    for( unsigned int j = 0; j < ptVec.size(); ++j )
    {
        vec3f pt = ptVec[j];// - partOrigin;
        pt = Origin + Xu*pt.x + Yu*pt.y + Zu*pt.z;
        if( j < outlineVA.getVertexCount() )
            outlineVA[j].position = sf::Vector2f( pt.x, winHeight - pt.y );

        if( j < fillVA.getVertexCount() )
        {
            pt = ptVec[ visitOrder[j] ];// - partOrigin;
            pt = Origin + Xu*pt.x + Yu*pt.y + Zu*pt.z;
            fillVA[j].position = sf::Vector2f( pt.x, winHeight - pt.y );
        }

    }

    outlineVA[ outlineVA.getVertexCount() - 1 ].position = outlineVA[0].position;
}

void part3d::draw( sf::RenderTarget& RT ) const
{
    if( outlineVA.getVertexCount() > 1 ) RT.draw( outlineVA );
    if( !isComplete ) return;
    if( fillVA.getVertexCount() > 2 ) RT.draw( fillVA );
}
