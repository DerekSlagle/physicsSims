#ifndef SUBMENUA_H
#define SUBMENUA_H

#include "subMenuABS.h"
#include "LvlQM_FSW.h"
#include "LvlQM_HarmOsci.h"

class subMenuA : public subMenuABS
{
    public:
    virtual bool init();// instantiable opens file and reads some data
    virtual bool loadLevel( size_t I );// true if load is good

    subMenuA(){}
    virtual ~subMenuA(){}
};

#endif // SUBMENUA_H
