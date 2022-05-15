#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
bool Iceman::isAlive() const {
  return getWorld()->getLives() > 0;
}

void Iceman::doSomething() {
  if (!isAlive()) return;
  
  int ch;
  if (getWorld()->getKey(ch) == true) {
    switch (ch) {
      case KEY_PRESS_UP :
        setDirection(up);
        moveTo(getX(), (getY()==60) ? getY() : getY()+1);
        break;

      case KEY_PRESS_DOWN :
        setDirection(down);
        moveTo(getX(), (getY()==0) ? getY() : getY()-1);
        break;
        
      case KEY_PRESS_LEFT :
        setDirection(left);
        moveTo((getX()==0) ? getX() : getX()-1, getY());
        break;
        
      case KEY_PRESS_RIGHT :
        setDirection(right);
        moveTo((getX()==60) ? getX() : getX()+1, getY());
        break;
        
      case KEY_PRESS_SPACE :
        
        break;
      
      case KEY_PRESS_ESCAPE :
        
        break;
    }
  }
}