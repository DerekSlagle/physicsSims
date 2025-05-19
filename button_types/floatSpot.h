#ifndef FLOATSPOT_H_INCLUDED
#define FLOATSPOT_H_INCLUDED

#include "hoverSpot.h"
#include "buttonValOnHit.h"

class floatSpot : public hoverSpot
{
    public:
    buttonValOnHit voh;

    virtual bool hit()const;
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual bool MseOver();
    virtual bool hitLeft();
    virtual bool hitRight();
    virtual void setSel( bool Sel );// set hover box size
    virtual void init_delayBox();// need extra space for float display
    void init2( float minX, float maxX, float xi, const char* voh_label, float* pVal );
    void init2( float minX, float maxX, float xi, const char* voh_label, std::function<void(float)> p_SetFunc );
    void makeBox( bool Big ) const;// utility
    virtual void setPosition( sf::Vector2f Pos );
};

#endif // FLOATSPOT_H_INCLUDED
