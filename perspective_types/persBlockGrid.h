#ifndef PERSBLOCKGRID_H
#define PERSBLOCKGRID_H

#include <algorithm>
#include "persPt.h"


class persBlockGrid : public persPt
{
    public:
    static const sf::Font* pFont;
    static persBall* pBall;// drawn by grid if found within. z-ordered with cells in isInGrid()
    static bool doDrawAxes;
    static float ofstL, ofstPerp;// offset from axis end along axis (L) and perpendicular
    static unsigned int labelCharSz;

    vec3f Origin;// of X,Y,Z coordinates. inherited pos must be at the geometric center for z-ordering
    unsigned int Rows = 3, Cols = 3, Layers = 3;
    float cellW = 40.0f, cellH = 40.0f, cellD = 40.0f;// width, height and depth of each cell
    vec3f Xu, Yu, Zu;// local coordinates
    vec3f halfCell, halfBlock;

    std::vector<vec3f> ptGrid;// cellW x cellH x cellD
    std::vector<const vec3f*> ppTvec;// z ordered pointers to above grid points assigned in constructAxes()
    std::vector<float> distSqVec;// cellW x cellH x cellD
    std::vector<bool> doDrawCell;// cellW x cellH x cellD
    std::vector<unsigned int> idxSortedVec;// lookup idx into ppTvec given idx into ptGrid
    bool doDrawBall = false;
    unsigned int idxSplit = 0;// drawing of cells split here to insert ball draw. Assign in isInGrid()
    void find_idxSplit();// call when ball is found in cell
    sf::Vertex axisLines[6];
    sf::Text xAxisLabel, yAxisLabel, zAxisLabel;
    void constructAxes( vec3f Zu_p );// from givenZup
    void assignPoints();
    void zOrderCells();

    // true if args in range
    bool getIndex( unsigned int row, unsigned int col, unsigned int& idx )const;// to index on base level
    bool getIndex( unsigned int row, unsigned int col, unsigned int layer, unsigned int& idx )const;// to index on base level
    bool getRowCol( unsigned int& row, unsigned int& col, unsigned int idx )const;// from index
 //   bool getRC( float x, float y, unsigned int& row, unsigned int& col )const;// from x, y in camera frame

    // full draw until sf::RenderTexture
//    std::vector<sf::Vertex> quadVec, lineVec;// draw Quads from the 1st and LineStrip from the 2nd
 //   unsigned int quadVtxCnt = 0, lineVtxCnt = 0;// limit for drawing each frame
    // the new way
    std::vector<sf::VertexArray> vtxArrayVec;// for the quads and LinesStrips
    unsigned int vtxArrayCnt = 0;

    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    void updateGrid();
    void updateGrid_2();
    virtual void draw( sf::RenderTarget& RT )const;
//    void drawCell( sf::RenderTarget& RT )const;
    bool getCellPoints( const vec3f* pPt, sf::Vector2f* QuadPos, sf::Vector2f* LinePos );
    bool getCellPoints( const vec3f* pPt, sf::VertexArray& quadVA, sf::VertexArray& lineVA );
    bool hitCellImage( sf::Vector2f hitPos, unsigned int& rowHit, unsigned int& colHit )const;
    bool isInGrid( vec3f pt )const;
    bool isInGrid( vec3f pt, vec3f& cellPos )const;// writes position of cell corner
    bool isInGrid( vec3f pt, unsigned int& R, unsigned int& C, unsigned int& L )const;// writes within which cell
    virtual bool hit( vec3f posLast, vec3f posNow, vec3f& P, vec3f& vel )const;// uses above
    bool hitCell( vec3f cellPos, vec3f posLast, vec3f posNow, vec3f& P, vec3f& vel )const;
    bool lineIntersects( vec3f P0, vec3f v, vec3f& HitPt, vec3f& Ns )const;
    vec3f getCellPos( unsigned int R, unsigned int C, unsigned int L )const;// returns origin

    bool hitBlock( vec3f posLast, bool KO = false );// take out a cell

    void init( std::istream& is );
    persBlockGrid(){}
    virtual ~persBlockGrid(){}

    protected:

    private:
};

#endif // PERSBLOCKGRID_H
