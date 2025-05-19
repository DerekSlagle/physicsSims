#ifndef DOOR_H_INCLUDED
#define DOOR_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

class door
{
    public:
    sf::Texture *pDoorTxt = nullptr;
    sf::Vertex doorQuad[4];// 0,1 texCoords vary  2,3 positions vary
    sf::Vertex cornerVtxA, cornerVtxB;// fixed edge
    sf::Vector2f pos, sz;

    bool moving = false;
    char moveDirection = 'L';// 'L'=Left, 'R'=Right, 'U'=Up, 'D'=Down
    char moveMode = 'S';// 'S'=sliding, 'I'=swinging in, 'O'=swinging out
    float swingFlare = 0.1f;// leading edge grows if 'O' or shrinks if 'I'
    float yEyeFraction = 0.5f;// eye level mid-door. 1.0f = eye at top of door. Affects flare

    float fractionOpen = 0.0f;// 0.0f to 1.0f. Swinging door through pi radians. 0.5f = edge on view
    float fractionOpenCurr = 0.0f;// seeks fractionOpen during motion
    float moveSpeed = 0.01f;// fraction/frame
    float scale = 1.0f;

 //   bool init( const char* fName, char MoveDirection, sf::Vector2f Pos, float Scale );
    bool init( sf::Texture& rTxt, char MoveDirection, sf::Vector2f Pos, float Scale );
    void setPosition( sf::Vector2f Pos );
    void setPosition( float x, float y ) { setPosition( sf::Vector2f(x,y) ); }
    void update( float dt );
    void draw( sf::RenderTarget& RT ) const { if( pDoorTxt ) RT.draw( doorQuad, 4, sf::Quads, pDoorTxt ); }
    void setOpenValue( float FractionOpen );
    void startMotion( float FractionOpen );
};

#endif // DOOR_H_INCLUDED
