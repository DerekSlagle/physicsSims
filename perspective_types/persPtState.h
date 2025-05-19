#ifndef PERSPTSTATE_H
#define PERSPTSTATE_H

#include "persPt.h"

class persPtState
{
    public:
    // persPt members
    vec3f pos;// position in scene
    vec3f vel;
    float Rbound = 1.0f;
    bool doDraw = true;
    bool isMoving = false;
    bool inUse = true;

    virtual void recordState() = 0;
    virtual void setState()const = 0;

    void recordBaseState( persPt* pPt );
    void setBaseState( persPt* pPt )const;

    persPtState(){}
    virtual ~persPtState(){}
};

class persBallState : public persPtState
{
    public:
    float mass = 1.0f;

    persBall* pPB = nullptr;// instance bound to
    virtual void recordState();
    virtual void setState()const;
    persBallState getFutureState( float t )const;

    void init( persBall& rPB );
    persBallState( persBall& rPB ){ init( rPB ); }
    persBallState(){}
    virtual ~persBallState(){}
};

class persBallTimedState : public persBallState
{
    public:
    float tElap = 0.0f;
    std::function<bool(vec3f)> pFdone = nullptr;

    persBallTimed* pPBT = nullptr;// instance bound to
    virtual void recordState();
    virtual void setState()const;
    persBallTimedState getFutureState( float t )const;

    void init( persBallTimed& rPBT );
    persBallTimedState( persBallTimed& rPBT ){ init( rPBT ); }
    persBallTimedState(){}
    virtual ~persBallTimedState(){}
};

class persQuadAniState : public persPtState
{
    public:
    spriteSheet* pSS = nullptr;
    size_t setNum = 0, frIdx = 0;
    int numLoops = -1, loopCount = 0;

    persQuadAni* pPQA = nullptr;// instance bound to
    virtual void recordState();
    virtual void setState()const;
    void init( persQuadAni& rPQA );
    persQuadAniState( persQuadAni& rPQA ){ init( rPQA ); }
    persQuadAniState(){}
    virtual ~persQuadAniState(){}
};

#endif // PERSPTSTATE_H
