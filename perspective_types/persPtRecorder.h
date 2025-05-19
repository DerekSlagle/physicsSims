#ifndef PERSPTRECORDER_H
#define PERSPTRECORDER_H

#include "persPt.h"

class persPtRecorder
{
    public:
    static bool* pFiring;
    static bool* pRunLevel;
    char mode = 'O';// 'R' = recording, 'P' = playback, 'O' = off
    size_t numFrames = 500;
    std::vector<vec3f> cameraStateVec;// 3 vec3f per frame: camPos, xu, zu then yu = xu.cross(zu)
    size_t endIdx = 0, playIdx = 0;
    bool buffIsFull = false;

    sf::Text recordMsg, playMsg;
    std::vector<bool> firingVec;// record firing events

    bool init( const std::string& inFileName, const sf::Font& rFont );
    void reset();
    void update();
    void draw( sf::RenderTarget& RT ) const;
    // event triggers
    void startNewRecording();
    void continueRecording();
    void stop();
    void play( size_t playOffset = 0 );

    persPtRecorder(){}
    ~persPtRecorder(){}
};

#endif // PERSPTRECORDER_H
