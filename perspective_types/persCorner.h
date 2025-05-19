#ifndef PERSCORNER_H
#define PERSCORNER_H

#include "persPt.h"

class persCorner : public persPt
{
    public:
    static float winW, winH;// window width and height. Assign where using corners
    // inherits
    // vec3f pos;// position in scene
    // float Rbound = 1.0f;// boundary in front of camera and outside of window. Closer = don't draw
    // bool doDraw = true;// calculate in update() and use in zSort() and draw()
    // bool isMoving = false;
    bool behindCamera = true;
    void update_behindCamera();
    vec3f tHat[3];// unit vectors towards other corners - can be obtained from pC's below
    float k[3] = { 1.0, 1.0, 1.0 };// distance to other corners = same for all 8 corners in a room
    persCorner *pC[3] = { nullptr, nullptr, nullptr };// must assign for adjacent corners behind camera
    // position assigned for these 3 vtx at this corner
    sf::Vertex* pVtx[3] = { nullptr, nullptr, nullptr };// drawing wall edges as sf::Lines
    sf::Vertex* pVtxQuad[3] = { nullptr, nullptr, nullptr };// drawing wall surfaces as sf::Quads
    virtual void update( float dt );// assign positions of the 3 vtx
    virtual void draw( sf::RenderTarget& RT )const{ return; }// still nothing to draw though

    persCorner(){ Rbound = 20.0f; }
    virtual ~persCorner(){}
};

#endif // PERSCORNER_H
