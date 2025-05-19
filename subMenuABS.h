#ifndef SUBMENUABS_H
#define SUBMENUABS_H

#include<string>
#include "Level.h"

// bool init() and loadLevel(size_t) are pure virtual
class subMenuABS : public Level
{
    public:
    std::vector<button*> myButtVec;// load in init() and use to restore to button::RegisteredButtVec in reset()
    std::vector<sf::Text> msgVec;
    std::vector< sf::Rect<float> > rectVec;
    sf::Color msgClrReg;
    sf::Color msgClrMseOver;
    size_t numOptions = 0;
    size_t lvlNum = 0;

    Level* p_subLevel = nullptr;
    buttonRect gotoMeButt;// for sub level use to return here

    // functions
    subMenuABS():Level() { std::cout << "Hello from subMenuABS ctor\n"; }

    virtual bool init() = 0;// instantiable opens file and reads some data
    bool init( std::istream& is );// bulk of init done here is common to all instances
    virtual void reset();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual bool loadLevel( size_t I ) = 0;// true if load is good
    void cleanup();

    virtual ~subMenuABS(){ cleanup(); }
};

#endif // SUBMENUABS_H
