#include "subMenuA.h"

// defining the 2 pv functions
bool subMenuA::init( )
{
    std::ifstream fin("include/levels/menu_subA_data.txt");
    if( !fin ) return false;
    return subMenuABS::init( fin );
}

bool subMenuA::loadLevel( size_t I )
{
    if( p_subLevel ) delete p_subLevel;
    p_subLevel = nullptr;

    switch( I )
    {
        case 0 :
        p_subLevel = new LvlQM_FSW;// finite square well
        break;

        case 1 :
        p_subLevel = new LvlQM_HarmOsci;// harmonic oscillator well
        break;
    }

    if( !p_subLevel ) return false;
    button::RegisteredButtVec.clear();
    button::RegisteredButtVec.push_back( &gotoMeButt );
    if( p_subLevel->init() ) return true;
    else { delete p_subLevel; p_subLevel = nullptr; return false; }
}
