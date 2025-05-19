#ifndef SCOREPOST_H
#define SCOREPOST_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include<functional>
#include "segHits/lineSeg.h"
#include "mvHits/ball.h"
#include "../button_types/button.h"

class scorePost
{
    public:
    static unsigned int* pScore;// pointer to game score
    unsigned int value = 0;
    sf::Text valueMsg;
    lineSeg postSeg;
    std::function<void(void)> pFuncHit = nullptr;// play sounds
    // color change on hit
    int countDown = 0, countDownLimit = 30;
    sf::Color normalColor, hitColor;
    void update();

    bool init( std::istream& is );
    bool hit( ball& b );
    void draw( sf::RenderTarget& RT )const;
    scorePost(){}
    scorePost( std::istream& is ){ init( is ); }
    ~scorePost(){}
};

#endif // SCOREPOST_H
