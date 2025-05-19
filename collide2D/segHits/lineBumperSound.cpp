#include "lineBumperSound.h"

sf::Sound* lineBumperSound::pSound = nullptr;

bool lineBumperSound::hit( mvHit& mh )
{
    if( lineBumper::hit( mh ) )
    {
        if( pSound && state == 1 && bumpIdx == 0 ) pSound->play();
        return true;
    }
    return false;
}
