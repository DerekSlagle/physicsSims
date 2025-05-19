#include "door.h"

const float Pi = 3.1415927f;

//bool door::init( const char* fName, char MoveDirection, sf::Vector2f Pos, float Scale )
bool door::init( sf::Texture& rTxt, char MoveDirection, sf::Vector2f Pos, float Scale )
{
    pDoorTxt = &rTxt;
 //   if( !pDoorTxt ) return false;// no texture
 //   if( !pDoorTxt->loadFromFile( fName ) ) { std::cout << "\nno doorTxt = " << fName; return false; }
    float W = pDoorTxt->getSize().x, H = pDoorTxt->getSize().y;// width and height of texture

    moveDirection = MoveDirection;
    pos = Pos;
    scale = Scale;
    sz.x = scale*W;
    sz.y = scale*H;

    if( moveDirection == 'L' )
    {
        cornerVtxA.position.x = 0.0f, cornerVtxA.position.y = sz.y;// dn left
        cornerVtxA.texCoords.x = 0.0f, cornerVtxA.texCoords.y = H;
        cornerVtxB.position.x = 0.0f, cornerVtxB.position.y = 0.0f;// up left
        cornerVtxB.texCoords.x = 0.0f, cornerVtxB.texCoords.y = 0.0f;
        // 0,1 texCoords vary  2,3 positions vary
        doorQuad[0] = cornerVtxA;// dn left
        doorQuad[1] = cornerVtxB;// up left
        doorQuad[2].position.x = sz.x; doorQuad[2].position.y = 0.0f;// up right
        doorQuad[2].texCoords.x = W; doorQuad[2].texCoords.y = 0.0f;
        doorQuad[3].position.x = sz.x; doorQuad[3].position.y = sz.y;// down right
        doorQuad[3].texCoords.x = W; doorQuad[3].texCoords.y = H;
    }
    else if( moveDirection == 'R' )
    {
        cornerVtxA.position.x = sz.x, cornerVtxA.position.y = 0.0f;// up right
        cornerVtxA.texCoords.x = W, cornerVtxA.texCoords.y = 0.0f;
        cornerVtxB.position.x = sz.x, cornerVtxB.position.y = sz.y;// down right
        cornerVtxB.texCoords.x = W, cornerVtxB.texCoords.y = H;
        // 2,3 texCoords vary  0,1 positions vary
   //     doorQuad[0].position.x = 0.0f; doorQuad[0].position.y = sz.y;// down left
   //     doorQuad[0].texCoords.x = 0; doorQuad[0].texCoords.y = H;
   //     doorQuad[1].position.x = 0.0f; doorQuad[1].position.y = 0.0f;// up left
   //     doorQuad[1].texCoords.x = 0; doorQuad[1].texCoords.y = 0;
   //     doorQuad[2] = cornerVtxA;// up right
   //     doorQuad[3] = cornerVtxB;// down right
        // 0,1 texCoords vary  2,3 positions vary
        doorQuad[0] = cornerVtxA;// up right
        doorQuad[1] = cornerVtxB;// down right
        doorQuad[2].position.x = 0.0f; doorQuad[2].position.y = sz.y;// down left
        doorQuad[2].texCoords.x = 0; doorQuad[2].texCoords.y = H;
        doorQuad[3].position.x = 0.0f; doorQuad[3].position.y = 0.0f;// up left
        doorQuad[3].texCoords.x = 0; doorQuad[3].texCoords.y = 0;
    }
    else if( moveDirection == 'U' )
    {
        cornerVtxA.position.x = 0.0f, cornerVtxA.position.y = 0.0f;// up left
        cornerVtxA.texCoords.x = 0.0f, cornerVtxA.texCoords.y = 0.0f;
        cornerVtxB.position.x = sz.x, cornerVtxB.position.y = 0.0f;// up right
        cornerVtxB.texCoords.x = W, cornerVtxB.texCoords.y = 0.0f;
        // 0,1 texCoords vary  2,3 positions vary
        doorQuad[0] = cornerVtxA;
        doorQuad[1] = cornerVtxB;
        doorQuad[2].position.x = sz.x; doorQuad[2].position.y = sz.y;// dn right
        doorQuad[2].texCoords.x = W; doorQuad[2].texCoords.y = H;
        doorQuad[3].position.x = 0.0f; doorQuad[3].position.y = sz.y;// dn left
        doorQuad[3].texCoords.x = 0.0f; doorQuad[3].texCoords.y = H;
    }
    else if( moveDirection == 'D' )
    {
        cornerVtxA.position.x = sz.x, cornerVtxA.position.y = sz.y;// dn right
        cornerVtxA.texCoords.x = W, cornerVtxA.texCoords.y = H;
        cornerVtxB.position.x = 0.0f, cornerVtxB.position.y = sz.y;// dn left
        cornerVtxB.texCoords.x = 0.0f, cornerVtxB.texCoords.y = H;
        // 0,1 texCoords vary  2,3 positions vary
        doorQuad[0] = cornerVtxA;
        doorQuad[1] = cornerVtxB;
        doorQuad[2].position.x = 0.0f; doorQuad[2].position.y = 0.0f;// up left
        doorQuad[2].texCoords.x = 0.0f; doorQuad[2].texCoords.y = 0.0f;
        doorQuad[3].position.x = sz.x; doorQuad[3].position.y = 0.0f;// up right
        doorQuad[3].texCoords.x = W; doorQuad[3].texCoords.y = 0.0f;
    }
    else return false;

    cornerVtxA.position += pos;
    cornerVtxB.position += pos;
    doorQuad[0].position += pos;
    doorQuad[1].position += pos;
    doorQuad[2].position += pos;
    doorQuad[3].position += pos;

    return true;
}

void door::setPosition( sf::Vector2f Pos )
{

}

void door::update( float dt )
{
    if( !moving ) return;

    if( fractionOpenCurr < fractionOpen )// opening
    {
        fractionOpenCurr += moveSpeed;
        if( fractionOpenCurr >= fractionOpen )
        {
            fractionOpenCurr = fractionOpen;
            moving = false;
        }
    }
    else if( fractionOpenCurr > fractionOpen )// closing
    {
        fractionOpenCurr -= moveSpeed;
        if( fractionOpenCurr <= fractionOpen )
        {
            fractionOpenCurr = fractionOpen;
            moving = false;
        }
    }

    setOpenValue( fractionOpenCurr );
}

void door::startMotion( float FractionOpen )
{
    fractionOpenCurr = fractionOpen;
    fractionOpen = FractionOpen;
    moving = true;
 //   setOpenValue( fractionOpen );// temp
}

void door::setOpenValue( float FractionOpen )
{
    float swingFraction=0.0f, flareFraction=0.0f, flareDir=0.0f;//used if door is swinging
    if( moveMode != 'S' )// not sliding therefore swinging
    {
        swingFraction = cosf( Pi*FractionOpen );
        flareFraction = swingFlare*sinf( Pi*FractionOpen );
        flareDir = (moveMode == 'O') ? 1.0f : -1.0f;
    }

    // by direction of motion
    if( moveDirection == 'L' )
    {
        // 0,1 texCoords vary  2,3 positions vary
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords.x = cornerVtxA.texCoords.x + FractionOpen*( doorQuad[3].texCoords.x - cornerVtxA.texCoords.x );// dn left
            doorQuad[1].texCoords.x = cornerVtxB.texCoords.x + FractionOpen*( doorQuad[2].texCoords.x - cornerVtxB.texCoords.x );// up left
            doorQuad[2].position.x = pos.x + ( 1.0f - FractionOpen )*sz.x;// up right
            doorQuad[3].position.x = pos.x + ( 1.0f - FractionOpen )*sz.x;// dn right
        }
        else if( moveMode == 'I' || moveMode == 'O' )// swinging in or out
        {
            doorQuad[2].position.x = pos.x + swingFraction*sz.x;// up right
            doorQuad[3].position.x = pos.x + swingFraction*sz.x;// dn right
            // flare
            doorQuad[2].position.y = pos.y - flareDir*( 1.0f - yEyeFraction )*flareFraction*sz.x;// up right
            doorQuad[3].position.y = pos.y + sz.y + flareDir*yEyeFraction*flareFraction*sz.x;// down right
        }
    }
    else if( moveDirection == 'R' )
    {
        // 0,1 texCoords vary  2,3 positions vary
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords.x = cornerVtxA.texCoords.x + FractionOpen*( doorQuad[3].texCoords.x - cornerVtxA.texCoords.x );
            doorQuad[1].texCoords.x = cornerVtxB.texCoords.x + FractionOpen*( doorQuad[2].texCoords.x - cornerVtxB.texCoords.x );
            doorQuad[2].position.x = pos.x + FractionOpen*sz.x;// dn left
            doorQuad[3].position.x = pos.x + FractionOpen*sz.x;// up left
        }
        else if( moveMode == 'I' || moveMode == 'O' )// swinging in or out
        {
            doorQuad[2].position.x = pos.x + sz.x - swingFraction*sz.x;// dn left
            doorQuad[3].position.x = pos.x + sz.x - swingFraction*sz.x;// up left
            // flare
            doorQuad[2].position.y = pos.y + sz.y + flareDir*( 1.0f - yEyeFraction )*flareFraction*sz.x;// dn left
            doorQuad[3].position.y = pos.y - flareDir*yEyeFraction*flareFraction*sz.x;// up left
        }
    }
    else if( moveDirection == 'U' )
    {
        // 0,1 texCoords vary  2,3 positions vary
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords.y = cornerVtxA.texCoords.y + FractionOpen*( doorQuad[3].texCoords.y - cornerVtxA.texCoords.y );// up left
            doorQuad[1].texCoords.y = cornerVtxB.texCoords.y + FractionOpen*( doorQuad[2].texCoords.y - cornerVtxB.texCoords.y );// up right
            doorQuad[2].position.y = pos.y + ( 1.0f - FractionOpen )*sz.y;// dn right
            doorQuad[3].position.y = pos.y + ( 1.0f - FractionOpen )*sz.y;// dn left
        }
        else if( moveMode == 'I' || moveMode == 'O' )// swinging in or out
        {
            doorQuad[2].position.y = pos.y + swingFraction*sz.y;// dn right
            doorQuad[3].position.y = pos.y + swingFraction*sz.y;// dn left
            // flare
            doorQuad[2].position.x = pos.x + sz.x + flareDir*( 1.0f - yEyeFraction )*flareFraction*sz.y;// dn right
            doorQuad[3].position.x = pos.x - flareDir*yEyeFraction*flareFraction*sz.y;// down left
        }
    }
    else if( moveDirection == 'D' )
    {
        // 0,1 texCoords vary  2,3 positions vary
        if( moveMode == 'S' )// sliding
        {
            doorQuad[0].texCoords.y = cornerVtxA.texCoords.y + FractionOpen*( doorQuad[3].texCoords.y - cornerVtxA.texCoords.y );// dn right
            doorQuad[1].texCoords.y = cornerVtxB.texCoords.y + FractionOpen*( doorQuad[2].texCoords.y - cornerVtxB.texCoords.y );// dn left
            doorQuad[2].position.y = pos.y + FractionOpen*sz.y;// up left
            doorQuad[3].position.y = pos.y + FractionOpen*sz.y;// up right
        }
        else if( moveMode == 'I' || moveMode == 'O' )// swinging in or out
        {
            doorQuad[2].position.y = pos.y + sz.y - swingFraction*sz.y;// up left
            doorQuad[3].position.y = pos.y + sz.y - swingFraction*sz.y;// up right
            // flare
            doorQuad[2].position.x = pos.x - flareDir*( 1.0f - yEyeFraction )*flareFraction*sz.y;// up left
            doorQuad[3].position.x = pos.x + sz.x + flareDir*yEyeFraction*flareFraction*sz.y;// up right
        }
    }

    return;
}
