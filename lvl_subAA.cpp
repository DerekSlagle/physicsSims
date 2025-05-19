#include "lvl_subAA.h"

#include <cstdlib>

bool lvl_subAA::init()
{
    // set position of the 1 button already in button::RegisteredButtVec
//    if( button::RegisteredButtVec.size() > 0 ) button::RegisteredButtVec.back()->setPosition();
    nPlays = 0;
    //isRunningAllGames = false;
    //currentAiAutoPlayMove = 0;
    srand(time(0));
    Level::quitButt.setPosition( {Level::winW - 80.0f,20.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW - 80.0f,80.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );

    resetButt.init( Level::winW - 80.0f, 200.0f, 60.0f, 30.0f, [this](){ reset(); }, "reset" );
    resetButt.mode = 'I';
    button::RegisteredButtVec.push_back( &resetButt );

    playButt.init( Level::winW - 80.0f, 260.0f, 60.0f, 30.0f, nullptr, "play" );
    playButt.mode = 'I';
    playButt.pHitFunc = [this]()
    {
        if( turn < 9 && winner == 'N' && !Draw )
        {
            char player = turn%2==0 ? 'X' : 'O';
            size_t val = findPlay( player );
            setPlay( val/3, val%3, player );
        }
    };
    button::RegisteredButtVec.push_back( &playButt );

    //runAllGamesButt.init( Level::winW - 180.0f, 200.0f, 50.0f, 30.0f, [this](){ reset(); }, "auto" );
    //button::RegisteredButtVec.push_back( &runAllGamesButt );

    if( button::pFont ) msg.setFont( *button::pFont );
    msg.setFillColor( sf::Color::Magenta );
    msg.setCharacterSize(30);
    msg.setString( "X" );// = "Ek = ";

    std::ifstream inFile("include/levels/ticTacToe/init_data.txt");
    if( !inFile ) return false;
    sf::Vector2f sz;
    inFile >> sz.x >> sz.y;
    inFile.close();

    bars[0].setSize(sz);// horizontal
    bars[0].setOrigin( sz.x*0.5f, sz.y*0.5f );
    bars[1].setSize(sz);
    bars[1].setOrigin( sz.x*0.5f, sz.y*0.5f );
    bars[2].setSize( sf::Vector2f(sz.y,sz.x) );// vertical
    bars[2].setOrigin( sz.y*0.5f, sz.x*0.5f );
    bars[3].setSize( sf::Vector2f(sz.y,sz.x) );
    bars[3].setOrigin( sz.y*0.5f, sz.x*0.5f );

    boardPos.x = Level::winW*0.5f - sz.x*0.5f;
    boardPos.y = Level::winH*0.5f - sz.x*0.5f;
    wBoard = sz.x;

    for( auto& rect : bars )
    {
        rect.setFillColor( sf::Color::Magenta );
    }

    bars[0].setPosition( Level::winW*0.5f, Level::winH*0.5f - sz.x/6.0f );
    bars[1].setPosition( Level::winW*0.5f, Level::winH*0.5f + sz.x/6.0f );
    bars[2].setPosition( Level::winW*0.5f - sz.x/6.0f, Level::winH*0.5f );
    bars[3].setPosition( Level::winW*0.5f + sz.x/6.0f, Level::winH*0.5f );

    wSpc = sz.x/3.0f;
    pos0.x = Level::winW*0.5f - wSpc; pos0.y =  Level::winH*0.5f - wSpc;
    for( size_t i = 0; i < 9; ++i )
    {
        ltr[i/3][i%3].setFont( *button::pFont );
        ltr[i/3][i%3].setCharacterSize( 50 );
        ltr[i/3][i%3].setString("X");
        ltr[i/3][i%3].setFillColor( sf::Color::Red );
        ltr[i/3][i%3].setOrigin( ltr[i/3][i%3].getLocalBounds().width/2.0f, ltr[i/3][i%3].getLocalBounds().height/2.0f );
        ltr[i/3][i%3].setPosition( pos0.x + wSpc*(i%3), pos0.y + wSpc*(i/3) );
    }

    msg.setPosition( boardPos.x +wSpc, 100.0f );

    return true;
}

bool lvl_subAA::handleEvent( sf::Event& rEvent )
{
    if( button::pButtMse ) return true;// mouse is over a button

    if (rEvent.type == sf::Event::MouseButtonPressed )
    {
       size_t row=0, col=0;
       char ch = turn%2==0 ? 'X' : 'O';
       if( turn < 9 && hitBoard( row, col ) )
           setPlay( row, col, ch );
    }

    return true;
}

void lvl_subAA::setPlay( size_t row, size_t col, char player )
{
    std::cout << "Player " << player << ": " << row << "," << col << '\n';
   ltr[row][col].setString( player );
   ltr[row][col].setFillColor( player=='X' ? sf::Color::Red : sf::Color::Green );
   board[row][col] = player;
   if( checkForWin(player) )
   {
       winner = player;
       turn = 9;
       msg.setString( player=='X' ? "X wins!" : "O wins!" );
       msg.setFillColor( player=='X' ? sf::Color::Red : sf::Color::Green );
   }
   else if( isDraw() )
   {
       Draw = true;
       turn = 9;
       msg.setString( "Draw!" );
       msg.setFillColor( sf::Color::Yellow );
   }

   ++turn;
   ++nPlays;
}

size_t lvl_subAA::findPlay( char player )// automatically find a play
{
    if (nPlays == 0)
    {
        size_t pos = static_cast<size_t>(rand() % 9);
        return pos;
    }

    if (nPlays == 1)
    {
        if( board[0][0] != 'N') return 8;
        if( board[0][2] != 'N') return 6;
        if( board[2][0] != 'N') return 2;
        if( board[2][2] != 'N') return 0;
    }

    size_t playCnt[8] = {0}, oppoCnt[8] = {0};
    const size_t px[8][3] = { {0,1,2}, {3,4,5}, {6,7,8},// rows
                              {0,3,6}, {1,4,7}, {2,5,8},// cols
                              {0,4,8}, {2,4,6} };// diagonals
    // get the counts
    for( size_t i = 0; i < 8; ++i )
    {
        playCnt[i] = oppoCnt[i] = 0;
        for( size_t j = 0; j < 3; ++j )
        {
            size_t r = px[i][j]/3, c = px[i][j]%3;
            if( board[r][c] == player ) ++playCnt[i];
            else if( board[r][c] != 'N' ) ++oppoCnt[i];
        }
    }

 //   size_t retVal = 0;// index 0-8 to play
    // check for any with playerCnt = 2
    for( size_t i = 0; i < 8; ++i )// win
        if( playCnt[i] == 2 )// find the 'N' one and play there
            for( size_t j = 0; j < 3; ++j )
                if( board[0][ px[i][j] ] == 'N' ) return px[i][j];
    // check for any with oppoCnt = 2
    for( size_t i = 0; i < 8; ++i )// block win
        if( oppoCnt[i] == 2 )// find the 'N' one and play there
            for( size_t j = 0; j < 3; ++j )
                if( board[0][ px[i][j] ] == 'N' ) return px[i][j];

    if( board[1][1] == 'N' ) return 4;// take center space
    // check for any with oppoCnt = 1
    for( size_t i = 0; i < 8; ++i )// block win
        if( playCnt[i] == 0 && oppoCnt[i] == 1 )// find the 'N' one and play there
            for( size_t j = 0; j < 3; ++j )
                if( board[0][ px[i][j] ] == 'N' ) return px[i][j];

 //   if( board[1][1] == 'N' ) return 4;// take center space


    // temp: take 1st available
    for( size_t i = 0; i < 9; ++i )
        if( board[i/3][i%3] == 'N' ) return i;

    return 0;
}

void lvl_subAA::update( float dt )
{
    /*
    if (isRunningAllGames)
    {
        if (checkForWin('X') || checkForWin('O') || Draw)
        {
            reset();
            isRunningAllGames = true;
            return;
        }

        if( turn < 9 && winner == 'N' && !Draw )
        {
            char player = turn%2==0 ? 'X' : 'O';
            size_t val = findPlay( player );
            setPlay( val/3, val%3, player );
        }
    }
*/
    return;
}

void lvl_subAA::draw( sf::RenderTarget& RT ) const
{

    for( const auto& rect : bars ) RT.draw( rect );
    for( size_t i = 0; i < 9; ++i ) if( board[i/3][i%3] != 'N' ) RT.draw( ltr[i/3][i%3] );
    if( winner != 'N' || Draw ) RT.draw( msg );
    return;
}

bool lvl_subAA::hitBoard( size_t& row, size_t& col )const
{
    if( button::mseX < boardPos.x ) return false;
    if( button::mseX > boardPos.x + wBoard ) return false;
    if( button::mseY < boardPos.y ) return false;
    if( button::mseY > boardPos.y + wBoard ) return false;

    col = floor( (button::mseX - boardPos.x)/wSpc );
    row = floor( (button::mseY - boardPos.y)/wSpc );
    return true;
}

bool lvl_subAA::checkForWin( char ch )const
{
    for(size_t i = 0; i < 3; ++i )// rows
        if( board[i][0] == ch && board[i][1] == ch && board[i][2] == ch ) return true;
    for(size_t i = 0; i < 3; ++i )// cols
        if( board[0][i] == ch && board[1][i] == ch && board[2][i] == ch ) return true;

    if( board[0][0] == ch && board[1][1] == ch && board[2][2] == ch ) return true;// diagonal
    if( board[0][2] == ch && board[1][1] == ch && board[2][0] == ch ) return true;// diagonal

    return false;
}

bool lvl_subAA::isDraw()const
{
    // if count is one of each in each direction of play
    int Xcnt = 0, Ocnt = 0;
    int bad = 0;

    for(size_t i = 0; i < 3; ++i )
    {
        Xcnt = Ocnt = 0;
        for(size_t j = 0; j < 3; ++j )
        {
            if( board[i][j] =='X' ) ++Xcnt;
            else if( board[i][j] =='O' ) ++Ocnt;
        }
        if( Xcnt > 0 && Ocnt > 0 ) ++bad;
        // cols
        Xcnt = Ocnt = 0;
        for(size_t j = 0; j < 3; ++j )
        {
            if( board[j][i] =='X' ) ++Xcnt;
            else if( board[j][i] =='O' ) ++Ocnt;
        }
        if( Xcnt > 0 && Ocnt > 0 ) ++bad;
    }

    Xcnt = Ocnt = 0;// diagonal
    for(size_t j = 0; j < 3; ++j )
    {
        if( board[j][j] =='X' ) ++Xcnt;
        else if( board[j][j] =='O' ) ++Ocnt;
    }
    if( Xcnt > 0 && Ocnt > 0 ) ++bad;

    Xcnt = Ocnt = 0;// diagonal
    for(size_t j = 0; j < 3; ++j )
    {
        if( board[j][2-j] =='X' ) ++Xcnt;
        else if( board[j][2-j] =='O' ) ++Ocnt;
    }
    if( Xcnt > 0 && Ocnt > 0 ) ++bad;

    return bad == 8;
}

void lvl_subAA::reset()
{
    std::cout << "New game\n\n";
    //isRunningAllGames = false;
    //currentAiAutoPlayMove = 0;
    nPlays = 0;
    for( size_t i = 0; i < 9; ++i ) board[i/3][i%3] = 'N';
    winner = 'N';
    turn = 0;
    Draw = false;
}
