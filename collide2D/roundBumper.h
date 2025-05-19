#ifndef ROUNDBUMPER_H
#define ROUNDBUMPER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include<functional>
#include "../utility/vec2d.h"
#include "mvHits/ball.h"

class roundBumper
{
    public:
        sf::CircleShape core;// image
        sf::CircleShape ring;// image
        float bumpSpeed = 5.0f;// magnitude of bump
        float rRing = 20.0f;
        vec2d pos;
        vec2d vel;// assigned in hit() used in update()

        //scoring when lit
        float litTimer = 0.0f;
        sf::Color normalColor, litColor;
        void getLit( float lightTime );
        void getUnlit();// called when game over and at game start
        bool isLit() { return litTimer > 0.0f; }
        unsigned int scoreValue = 50;
        static unsigned int* pScore;
        std::function<void(void)> pFuncHit = nullptr;// play sounds

        // animation
        int state = 0;// -1, 0, +1
        int bumpSteps = 3, bumpIdx = 0;

        bool hit( ball& b );
        void draw( sf::RenderTarget& RT )const;
        void update( float dt );
        roundBumper(){}
        ~roundBumper(){}
        bool init(std::ifstream& is );
        roundBumper( std::ifstream& is ){ init( is ); }

};

#endif // ROUNDBUMPER_H
