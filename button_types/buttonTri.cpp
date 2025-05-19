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
    // 0=unsel, 1=sel, 2=unselMo, 3=selMo
    size_t i = sel ? 1 : 0;
    RT.draw( P+3*i, 3, sf::Triangles );// main triangle
    i = mseOver ? 10 : 6;
    RT.draw( P+i, 4, sf::LinesStrip );// mseOver triangle

    RT.draw(title);
}

void buttonTri::setPosition( sf::Vector2f Pos )// = position of P[0]
{
    title.setPosition(Pos);
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    for(size_t i=0; i<14; ++i)// each triangle
    {
        P[i].position += dPos;
    }
}

/*
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
}   */

void buttonTri::init( sf::Vertex Pt[3], float x, float y, sf::Color* pColor, const sf::Text& Title )// from array of 4 colos
{
    mseOver = sel = false;
 //   sf::Vector2f Pos(x,y);
    pos.x = x; pos.y = y;

    // 0=unsel, 1=sel, 2=unselMo, 3=selMo
    P[0] = Pt[0]; P[0].color = pColor[0];// unSel
    P[1] = Pt[1]; P[1].color = pColor[0];
    P[2] = Pt[2]; P[2].color = pColor[0];
    P[3] = Pt[0]; P[3].color = pColor[1];// unSel
    P[4] = Pt[1]; P[4].color = pColor[1];
    P[5] = Pt[2]; P[5].color = pColor[1];
    // LinesStrip
    P[6] = Pt[0]; P[6].color = pColor[2];// unselMo
    P[7] = Pt[1]; P[7].color = pColor[2];
    P[8] = Pt[2]; P[8].color = pColor[2];
    P[9] = Pt[0]; P[9].color = pColor[2];
    P[10] = Pt[0]; P[10].color = pColor[3];// selMo
    P[11] = Pt[1]; P[11].color = pColor[3];
    P[12] = Pt[2]; P[12].color = pColor[3];
    P[13] = Pt[0]; P[13].color = pColor[3];

    for(size_t i=0; i<14; ++i) P[i].position += pos;

    // title
    title = Title;
    title.setPosition(P[0].position);
}

float xProd( sf::Vector2f A, sf::Vector2f B ) { return A.x*B.y - A.y*B.x; }
