#include "persPtRecorder.h"

bool* persPtRecorder::pFiring = nullptr;// static
bool* persPtRecorder::pRunLevel = nullptr;// static

bool persPtRecorder::init( const std::string& inFileName, const sf::Font& rFont )
{
    std::ifstream fin( inFileName.c_str() );
    if( !fin ) { std::cout << "\nno file: " << inFileName; return false; }
    sf::Vector2f pos; fin >> pos.x >> pos.y;
    recordMsg.setPosition( pos );
    recordMsg.setFont( rFont );
    unsigned int fontSz; fin >> fontSz;
    recordMsg.setCharacterSize( fontSz );
    recordMsg.setString("REC");
    recordMsg.setFillColor( sf::Color::Red );
    playMsg = recordMsg;
    playMsg.setString("PLAY");
    playMsg.setFillColor( sf::Color::Green );

    fin >> numFrames;
    cameraStateVec.resize( 3*numFrames );
    mode = 'O';
    endIdx = playIdx = 0;
    buffIsFull = false;

    firingVec.resize( numFrames, false );

    return true;
}

void persPtRecorder::reset()
{
    endIdx = playIdx = 0;
    buffIsFull = false;
}

void persPtRecorder::update()
{
    if( mode == 'O' ) return;// off
    if( mode == 'R' )// record another frame
    {
        cameraStateVec[ 3*endIdx ] = persPt::camPos;
        cameraStateVec[ 3*endIdx + 1 ] = persPt::xu;
        cameraStateVec[ 3*endIdx + 2 ] = persPt::camDir;
        if( pFiring ) firingVec[ endIdx ] = *pFiring;
        if( ++endIdx >= numFrames )// cycle around
        {
            endIdx %= numFrames;
            if( !buffIsFull )
            {
                buffIsFull = true;
                if( pRunLevel ) *pRunLevel = false;
            }

        }
        return;
    }

    if( mode == 'P' )// play another frame
    {
        persPt::camPos = cameraStateVec[ 3*playIdx ];
        persPt::xu = cameraStateVec[ 3*playIdx + 1 ];
        persPt::camDir = cameraStateVec[ 3*playIdx + 2 ];
        persPt::yu = persPt::xu.cross( persPt::camDir );
        if( pFiring ) *pFiring = firingVec[ playIdx ];
        if( ++playIdx >= numFrames )// cycle around
        {
            playIdx %= numFrames;
        }

        if( playIdx == endIdx )
        {
            mode = 'O';// off
            if( pFiring ) *pFiring = false;
        }

        return;
    }
}

void persPtRecorder::draw( sf::RenderTarget& RT ) const
{
    if( mode == 'R' ) RT.draw( recordMsg );
    else if( mode == 'P' ) RT.draw( playMsg );
}

// event triggers
void persPtRecorder::startNewRecording()
{
    mode = 'R';// record
    buffIsFull = false;// start at beginning
    endIdx = 0;
}

void persPtRecorder::continueRecording()
{
    mode = 'R';
}

void persPtRecorder::stop()
{
    mode = 'O';
}

void persPtRecorder::play( size_t playOffset )
{
    mode = 'P';// play
    if( buffIsFull ) playIdx = ( endIdx + playOffset + 1 )%numFrames;
    else playIdx = playOffset%numFrames;
}
