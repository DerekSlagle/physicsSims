#include "persPtState.h"

// persPtState
void persPtState::recordBaseState( persPt* pPt )
{
    if( !pPt ) return;
    pos = pPt->pos;// position in scene
    vel = pPt->vel;
    Rbound = pPt->Rbound;
    doDraw = pPt->doDraw;
    isMoving = pPt->isMoving;
    inUse = pPt->inUse;
}

void persPtState::setBaseState( persPt* pPt )const
{
    if( !pPt ) return;
 //   pPt->pos = pos;
    pPt->vel = vel;
    pPt->Rbound = Rbound;
    pPt->doDraw = doDraw;
    pPt->isMoving = isMoving;
    pPt->inUse = inUse;
    pPt->setPosition( pos );
}

// persBallState
void persBallState::init( persBall& rPB )
{
    pPB = &rPB;
    recordState();
}

void persBallState::recordState()
{
    if( !pPB ) return;
    recordBaseState( pPB );
    mass = pPB->mass;
}

void persBallState::setState()const
{
    if( !pPB ) return;
    setBaseState( pPB );
    pPB->mass = mass;
}

persBallState persBallState::getFutureState( float t )const
{
    if( !pPB ) return *this;// current state
    if( !inUse || !isMoving ) return *this;
    persBall PB( *pPB );
    PB.update(t);

    return persBallState( PB );
}

// persBallTimedState
void persBallTimedState::init( persBallTimed& rPBT )
{
    pPBT = &rPBT;
    recordState();
}

void persBallTimedState::recordState()
{
    if( !pPBT ) return;
    persBallState::recordState();
    tElap = pPBT->tElap;
    pFdone = pPBT->pFdone;
}

void persBallTimedState::setState()const
{
    if( !pPBT ) return;
    persBallState::setState();
    pPBT->tElap = tElap;
    pPBT->pFdone = pFdone;
}

persBallTimedState persBallTimedState::getFutureState( float t )const
{
    if( !pPBT ) return *this;// current state
    if( !inUse || !isMoving ) return *this;
    persBallTimed PBT( *pPBT );
    PBT.update(t);// may call pFdone()
    return persBallTimedState( PBT );
}

// persQuadAniState
void persQuadAniState::init( persQuadAni& rPQA )
{
    pPQA = &rPQA;
    recordState();
}

void persQuadAniState::recordState()
{
    if( !pPQA ) return;
    recordBaseState( pPQA );
    pSS = pPQA->pSS;
    setNum = pPQA->setNum;
    frIdx = pPQA->frIdx;
    numLoops = pPQA->numLoops;
    loopCount = pPQA->loopCount;
}

void persQuadAniState::setState()const
{
    if( !pPQA ) return;
    setBaseState( pPQA );
    pPQA->pSS = pSS;
    pPQA->setNum = setNum;
    pPQA->frIdx = frIdx;
    pPQA->numLoops = numLoops;
    pPQA->loopCount = loopCount;
    pPQA->setTxtRect( pSS->getFrRect( frIdx, setNum ), 'R', '0' );
}
