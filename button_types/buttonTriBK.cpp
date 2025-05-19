#include "buttonTri.h"

buttonTri::buttonTri()
{
    //ctor
}

buttonTri::~buttonTri()
{
    //dtor
}

bool buttonTri::hit()const
{
    sf::Vector2f ms(mseX, mseY);

    sf::Vector2f v = ms - P[0].position;
    float v01 = xProd( v, P[1].position - P[0].position );
    float v02 = xProd( v, P[2].position - P[0].position );
    if( v02/v01 > 0.0 ) return false;

    v = ms - P[1].position;
    v01 = xProd( v, P[0].position - P[1].position );
    v02 = xProd( v, P[2].position - P[1].position );
    return v02/v01 < 0.0;
}

void buttonTri::draw( sf::RenderTarget& RT )const
{
    size_t i = sel ? 2 : 0;
    RT.draw( P+3*i, 3, sf::Triangles );// main triangle
    i = mseOver ? 3 : 1;
    RT.draw( P+3*i, 3, sf::LinesStrip );// mseOver triangle

    RT.draw(title);
}

void buttonTri::setPosition( sf::Vector2f Pos )// = position of P[0]
{
    title.setPosition(Pos);
    for(size_t i=0; i<4; ++i)// each triangle
    {
        sf::Vector2f ofst1 = P[3*i+1].position - P[3*i].position;
        sf::Vector2f ofst2 = P[3*i+2].position - P[3*i].position;
        P[3*i].position = Pos;
        P[3*i+1].position = Pos + ofst1;
        P[3*i+2].position = Pos + ofst2;
    }
}

void buttonTri::init( sf::Vertex Pt[12], float x, float y, const sf::Text& Title )// from prepared set using texture
{
    mseOver = sel = false;
    sf::Vector2f Pos(x,y);
    for(size_t i=0; i<12; ++i)
    {
        P[i] = Pt[i];
        P[i].position += Pos;
    }

    // title
    title = Title;
    title.setPosition(Pos);
}

void buttonTri::init( sf::Vertex Pt[3], float x, float y, sf::Color* pColor, const sf::Text& Title )// from array of 4 colos
{
    mseOver = sel = false;
 //   sf::Vector2f Pos(x,y);
    pos.x = x; pos.y = y;
    for(size_t i=0; i<12; ++i)
    {
        P[i] = Pt[i%3];
        P[i].position += pos;
        P[i].color = pColor[i/3];
    }


    float k = 0.05f;
    P[3].position += ( P[0].position - P[1].position )*k;
    P[3].position += ( P[0].position - P[2].position )*k;
    P[4].position += ( P[1].position - P[2].position )*k;
    P[4].position += ( P[1].position - P[0].position )*k;
    P[5].position += ( P[2].position - P[0].position )*k;
    P[5].position += ( P[2].position - P[1].position )*k;
    P[9].position = P[3].position;
    P[10].position = P[4].position;
    P[11].position = P[5].position;

    // title
    title = Title;
 //   title.setPosition(Pos);
    title.setPosition(P[0].position);
}

float xProd( sf::Vector2f A, sf::Vector2f B ) { return A.x*B.y - A.y*B.x; }
