#ifndef LINEBUMPERSOUND_H
#define LINEBUMPERSOUND_H

#include <SFML/Audio.hpp>
#include "lineBumper.h"

class lineBumperSound : public lineBumper
{
    public:
    static sf::Sound* pSound;

    virtual bool hit( mvHit& mh );
    lineBumperSound( std::istream& fin ): lineBumper( fin ) {}
    lineBumperSound() {}
    virtual ~lineBumperSound() {}
};

#endif // LINEBUMPERSOUND_H
