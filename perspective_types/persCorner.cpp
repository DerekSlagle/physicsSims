#include "persCorner.h"

float persCorner::winW = 1280.0f, persCorner::winH = 720.0f;// window width and height. Assign where using corners

void persCorner::update_behindCamera()
{
    float U = persPt::camDir.dot( pos - persPt::camPos );
    if( U < Rbound )
    {
        behindCamera = true;
        doDraw = false;
    }
    else behindCamera = false;
}

void persCorner::update( float dt )
{
    persPt::update_doDraw();
    update_behindCamera();

   if( behindCamera )
   {
       // move xtx to positions in front of camera. If tHat.dot( camDir ) < 0 then leave in place?
       // If tHat.dot( camDir ) < 0  then the camera is outside of the room
       float tDotZc[3];
       for( unsigned int i = 0; i < 3; ++i ) tDotZc[i] = tHat[i].dot( camDir );// so the + ones can be found

       for( unsigned int i = 0; i < 3; ++i )
       {
           if( tDotZc[i] > 0.1f )
           {
               float s = persPt::camDir.dot( persPt::camPos - pos )/tDotZc[i];
               vec3f V = pos + s*tHat[i] + 10.0f*persPt::camDir;// + 10.0f*tHat[i]/tDotZc;// plus a bump in front
               pVtx[i]->position = pVtxQuad[i]->position = persPt::get_xyw( V );
           }
       }

       // move trapped vtx to another point in front of the camera
       for( unsigned int i = 0; i < 3; ++i )
       {
       //    pVtx[i]->position = pVtxQuad[i]->position = get_xyw();// keep in place?
           // or...
           if( tDotZc[i] < 0.1f )
           {
               // move it along another line to in front
               for( unsigned int k = 0; k < 3; ++k )
                    if( tDotZc[k] > 0.1f )
                    {
                        pVtx[i]->position = pVtx[k]->position;
                        pVtxQuad[i]->position = pVtxQuad[k]->position;
                        break;// at 1st found ??
                    }
           }
       }
   }
   else
   {
        sf::Vector2f Cpos = get_xyw();
        for( unsigned int i = 0; i < 3; ++i )
        {
            // assign own vtx positions for each direction x,y,z
            pVtx[i]->position = pVtxQuad[i]->position = Cpos;// for doDraw true or false
        }
   }

    return;
}

/*
void persCorner::update( float dt )
{
    persPt::update_doDraw();
    update_behindCamera();

   if( behindCamera ) return;

    // assign vtx positions
    // for each direction x,y,z
    for( unsigned int i = 0; i < 3; ++i )
    {
        // assign own vtx
        pVtx[i]->position = pVtxQuad[i]->position = get_xyw();// for doDraw true or false

        if( pC[i]->behindCamera )
        {
            sf::Vector2f Va = get_xyw( pos ), Vc = Va;

            if( doDraw )
            {
                sf::Vector2f Vb = get_xyw( pos + tHat[i]*Rbound/2.0f );// should also be IFOC
                sf::Vector2f dV = Vb - Va;
                double Ylim = dV.y > 0 ? winH : 0.0f;// down or up

                if( dV.x*dV.x < 0.0001f )// straight down
                {
                    Vc.x = Va.x;
                    Vc.y = winH;
                }
                else if(  dV.x > 0.0 )// to right
                {
                    float kx = ( winW - Va.x )/dV.x;
                    float ky = ( Ylim - Va.y )/dV.y;
                    if( ky < 0.0f ) ky *= -1.0f;
                    float k = kx < ky ? kx : ky;// least
                    Vc = Va + k*dV;
                }
                else// to left
                {
                    float kx = -Va.x/dV.x;
                    float ky = ( Ylim - Va.y )/dV.y;
                    if( ky < 0.0f ) ky *= -1.0f;
                    float k = kx < ky ? kx : ky;// least
                    Vc = Va + k*dV;
                }
            }

            pC[i]->pVtx[i]->position = Vc;// for sf::Lines draw
            pC[i]->pVtxQuad[i]->position = Vc;// for sf::Quads draw
        }

    }// end for each direction x,y,z

    return;
}
*/
