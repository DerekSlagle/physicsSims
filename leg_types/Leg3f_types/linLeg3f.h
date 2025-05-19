#ifndef LINLEG3F_H
#define LINLEG3F_H

#include "Leg3f.h"

class linLeg3f : public Leg3f
{
    public:
   //     vec3f posF;// end position on leg
        vec3f Tu;
        linLeg3f(): Leg3f() { posF = vec3f(0.0f,0.0f,1.0f); len = 1.0f; }

        void init( vec3f Pos0, vec3f PosF );// 1st leg
        linLeg3f( vec3f Pos0, vec3f PosF ){ init( Pos0, PosF ); }

        void init( Leg3f& rPrev, vec3f PosF );// mid legs
        linLeg3f( Leg3f& rPrev, vec3f PosF ){ init( rPrev, PosF ); }// mid legs

        void init( Leg3f& rPrev, Leg3f& rNext );// last (tie) leg
        linLeg3f( Leg3f& rPrev, Leg3f& rNext ){ init( rPrev, rNext ); }

        void init( vec3f Pos0 , Leg3f& rNext );// entry leg
        linLeg3f( vec3f Pos0 , Leg3f& rNext ){ init( Pos0 ,rNext ); }

     //   virtual float x( float s )const { return pos0.x + s*(posF.x-pos0.x)/len; }
     //   virtual float y( float s )const { return pos0.y + s*(posF.y-pos0.y)/len; }
     //   virtual float z( float s )const { return pos0.z + s*(posF.z-pos0.z)/len; }
        virtual vec3f getPos( float s )const{ return pos0 + s*Tu; }
        virtual vec3f accel( float s, float v )const { return vec3f(); }// zero

        virtual ~linLeg3f(){}

        virtual vec3f T( float s )const{ return Tu; }// provide unit tangent to curve at s
};

#endif // LINLEG3F_H
