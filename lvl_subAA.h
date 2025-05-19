#ifndef LVL_SUBAA_H_INCLUDED
#define LVL_SUBAA_H_INCLUDED

#include "Level.h"

class lvl_subAA : public Level
{
    public:
    sf::Text msg;
    sf::RectangleShape bars[4];// 0, 1 = horizontal
    sf::Text ltr[3][3];
    char board[3][3] = { {'N','N','N'}, {'N','N','N'}, {'N','N','N'} };
    float wBoard = 400.0f, wSpc = wBoard/3.0f;
    sf::Vector2f boardPos, pos0;// center of upper left space
    size_t turn = 0;// even for X
    char winner = 'N';
    bool Draw = false;
    buttonRect resetButt, playButt;
    //buttonRect runAllGamesButt;
    int nPlays = 0;
    //bool isRunningAllGames = false;
    //int currentAiAutoPlayMove = 0;

    // functions
    lvl_subAA():Level() { std::cout << "Hello from lvl_subAA ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    bool hitBoard( size_t& row, size_t& col )const;
    bool checkForWin( char ch )const;
    bool isDraw()const;
    size_t findPlay( char player );// automatically find a play
    void setPlay( size_t row, size_t col, char player );
    void reset();
    void cleanup(){}
    virtual ~lvl_subAA() { cleanup(); }
};

#endif // LVL_SUBAA_H_INCLUDED
