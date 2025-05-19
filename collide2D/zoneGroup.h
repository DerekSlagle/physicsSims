#ifndef ZONEGROUP_H
#define ZONEGROUP_H

#include<vector>

#include "mvHits/mvHit.h"
#include "segHits/segHit.h"

class zoneGroup
{
    public:
    sf::FloatRect zoneRect;
 //   float zoneBuffer = 50.0f;// around area defined by the segHits
    std::vector<mvHit*> mhVec;// the ones presently in this zone
    std::vector<segHit*> segVec;// the ones assigned to this zone

    void update_mhVec( std::vector<mvHit*>& MhVec );
    void hitAll();// collision testing only. update all elsewhere
    bool hitZone( vec2d pt );// utility

    void init( std::istream& is, const std::vector<segHit*>& SegVec );// reading zoneRect
    zoneGroup();
    ~zoneGroup();

    protected:

    private:
};

#endif // ZONEGROUP_H
