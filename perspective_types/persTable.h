#ifndef PERSTABLE_H
#define PERSTABLE_H

#include "persChair.h"

// a Table (ie chair) which manages the items on it's surface and maybe surrounding chairs
class persTable : public persPt
{
    public:
    persChair Table;// the table

    // items on the Table
    bool ownsItems = false;// items may be stored here
    std::vector<persPt*> pItemVec;// items for drawing on the Table. These will all be drawn after Table.topQ ( if viewer is above )
    size_t numItemsToDraw = 0;
    std::vector<persPt*> pSortedItemsVec;// only doDraw = true here. numToDraw varies

    // surrounding chairs?
    bool ownsChairs = false;
    std::vector<persChair*> pChairVec;// items for drawing chairs the Table
    size_t numChairsToDraw = 0;
    std::vector<persChair*> pSortedChairsVec;

    bool init( std::istream& is, std::vector<spriteSheet>& rSSvec );// if owning
    bool init( const std::vector<persPt*>& pPtToCopy, bool OwnsItems );// if owning or not. Deal with chairs separately
    bool init( const std::vector<persChair*>& pChToCopy, bool OwnsChairs );// if owning or not. Deal with chairs separately
    virtual void update_doDraw();
    virtual void update( float dt );
    void drawTableAndItems( sf::RenderTarget& RT ) const;
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( vec3f Pos );

    persTable(){}
    persTable( std::istream& is, std::vector<spriteSheet>& rSSvec ){ init( is, rSSvec ); }
    persTable( const std::vector<persPt*>& pPtToCopy, bool OwnsItems ){ init( pPtToCopy, OwnsItems ); }
    virtual ~persTable();
};

#endif // PERSTABLE_H
