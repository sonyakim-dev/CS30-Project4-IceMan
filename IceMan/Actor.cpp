#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <queue>

/*================ ICEMAN ================*/
void Iceman::doSomething() {
  if (!isAlive()) return;
  
  int ch;
  if (getWorld()->getKey(ch) == true) {
    switch (ch) {
      case KEY_PRESS_UP :
        if (getDirection() == up) {
          moveTo(getX(), (getY() == 60 || getWorld()->isBouldery(getX(), getY(), up)) ? getY() : getY() + 1);
          getWorld()->digIce(getX(), getY(), KEY_PRESS_UP);
        }
        else { setDirection(up); }
        break;
        
      case KEY_PRESS_DOWN :
        if (getDirection() == down) {
          moveTo(getX(), (getY() == 0 || getWorld()->isBouldery(getX(), getY(), down)) ? getY() : getY() - 1);
          getWorld()->digIce(getX(), getY(), KEY_PRESS_DOWN);
        }
        else { setDirection(down); }
        break;
        
      case KEY_PRESS_RIGHT :
        if (getDirection() == right) {
          moveTo((getX() == 60 || getWorld()->isBouldery(getX(), getY(), right)) ? getX() : getX() + 1, getY());
          getWorld()->digIce(getX(), getY(), KEY_PRESS_RIGHT);
        }
        else { setDirection(right); }
        break;
        
      case KEY_PRESS_LEFT :
        if (getDirection() == left) {
          moveTo((getX() == 0 || getWorld()->isBouldery(getX(), getY(), left)) ? getX() : getX() - 1, getY());
          getWorld()->digIce(getX(), getY(), KEY_PRESS_LEFT);
        }
        else { setDirection(left); }
        break;
        
      case KEY_PRESS_SPACE :
        if (getWater() <= 0) return;
        getWorld()->squirtWater(getX(), getY(), getDirection());
        break;
        
      case KEY_PRESS_TAB :
        if (getGold() <= 0) return;
        getWorld()->dropGold(getX(), getY());
        break;
        
      case 'z' :
        if (getSonar() <= 0) return;
        getWorld()->discoverGoodies(getX(), getY());
        break;
        
      case KEY_PRESS_ESCAPE :
        getAnnoyed(100); /// completely get annoyed and abort the curr lev
        break;
    }
  }
}

void Iceman::getAnnoyed(unsigned int damage) {
  dropHP(damage);
  if (getHP() <= 0) {
    getWorld()->playSound(SOUND_PLAYER_ANNOYED);
    setDead();
  }
}


/*================ PROTESTER ================*/
bool Protester::findShortestPath(int startX, int startY, int finalX, int finalY) const {
  std::queue<std::pair<int, int>> xy;
  int stepArray[64][64];
  int currStep, currX, currY;
  
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 64; ++j) {
      stepArray[i][j] = 9999;
    }
  }
  
  stepArray[startX][startY] = 0;
  xy.push(std::make_pair(startX, startY));
  
  while (!xy.empty()) {
    currX = xy.front().first;
    currY = xy.front().second;
    xy.pop();
    
    if (currX == finalX && currY == finalY) return true;
    
    currStep = stepArray[currX][currY];
    ++currStep;
    
    if (!getWorld()->isIcy(currX, currY, up) && !getWorld()->isBouldery(currX, currY, up) &&
        stepArray[currX][currY+1] == 9999)
    {
      stepArray[currX][currY+1] = currStep;
      xy.push(std::make_pair(currX, currY+1));
    }
    if (!getWorld()->isIcy(currX, currY, down) && !getWorld()->isBouldery(currX, currY, down) &&
        stepArray[currX][currY-1] == 9999)
    {
      stepArray[currX][currY-1] = currStep;
      xy.push(std::make_pair(currX, currY-1));
    }
    if (!getWorld()->isIcy(currX, currY, right) && !getWorld()->isBouldery(currX, currY, right) &&
        stepArray[currX+1][currY] == 9999)
    {
      stepArray[currX+1][currY] = currStep;
      xy.push(std::make_pair(currX+1, currY));
    }
    if (!getWorld()->isIcy(currX, currY, left) && !getWorld()->isBouldery(currX, currY, left) &&
        stepArray[currX+1][currY] == 9999)
    {
      stepArray[currX-1][currY] = currStep;
      xy.push(std::make_pair(currX-1, currY));
    }
  }
  
  return false;
}

void Protester::getAnnoyed(unsigned int damage) {
  dropHP(damage);
  
  if (getHP() <= 0 && getState() != LEAVE) {
    getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
    setState(LEAVE);
  }
  else {
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
  }
}

/*================ REGULAR PROTESTER ================*/
void RegProtester::doSomething() {
  if (!isAlive()) return;
  
  if (resting_ticks == std::max(0, 3 - (int)getWorld()->getLevel() / 4)) { /// when it's time to move
    switch (getState()) {
      case WAIT : /// when shot by water
        if (stalled_ticks == std::max(50, 100 - (int)getWorld()->getLevel() * 10)) {
          setState(STAY);
        }
        else {
          ++stalled_ticks;
        }
        break;
        
      case LEAVE : /// fully get annoyed and leave the oil field
        resting_ticks = 0;
        
        if (getX() == 60 && getY() == 60) { setDead(); return; }
        // ADD: findShortestPath(getX(), getY(), 60, 60)
        moveTo(getX()+1, getY()); // THIS IS TEST
        break;
        
      case STAY : /// non-leave-the-oil-field state
        resting_ticks = 0;
        
        int icemanX = getWorld()->getIce_man()->getX();
        int icemanY = getWorld()->getIce_man()->getY();
        
        /// if iceman is close enough and protester is facing him
        if (canShout && getWorld()->isInRange(getX(), getY(), icemanX, icemanY, 4.0f)) {
          switch (getDirection()) {
            case up :
              if (getY() < icemanY) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
            case down :
              if (getY() > icemanY) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
            case right :
              if (getX() < icemanX) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
            case left :
              if (getX() > icemanX) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
          }
          return;
        }
        
        if (!canShout) {
          if (non_resting_ticks == 15) {
            non_resting_ticks = 0;
            canShout = true;
          }
          else { ++non_resting_ticks; }
        }
        
        /// if protester is in a straight horizontal or vertical line of sight to iceman
        if (getX() == icemanX || getY() == icemanY) {
          Direction dir = none;
          if (getWorld()->canSeeIceman(getX(), getY(), icemanX, icemanY, dir)) {
            setDirection(dir);
            switch (dir) {
              case up :
                moveTo(getX(), getY()+1); break;
              case down :
                moveTo(getX(), getY()-1); break;
              case right :
                moveTo(getX()+1, getY()); break;
              case left :
                moveTo(getX()-1, getY()); break;
            }
            setMoveStraightDistance_0();
            return;
          }
        }
        else { /// if protester can't directly see the iceman
          if (getMoveStraightDistance() == 0) {
            while (true) {
              int dir = rand() % 4 + 1; /// pick a random direction(1-4)
              switch (dir) {
                case up :
                  if (getWorld()->isIcy(getX(), getY(), up) || getWorld()->isBouldery(getX(), getY(), up)) continue; // maybe improvement
                case down :
                  if (getWorld()->isIcy(getX(), getY(), down) || getWorld()->isBouldery(getX(), getY(), down)) continue;
                case right :
                  if (getWorld()->isIcy(getX(), getY(), right) || getWorld()->isBouldery(getX(), getY(), right)) continue;
                case left :
                  if (getWorld()->isIcy(getX(), getY(), left) || getWorld()->isBouldery(getX(), getY(), left)) continue;

                  setDirection((Direction)dir);
                  pickMoveStraightDistance();
                  // continue with step 8
                  break;
              }
              break;
            }
            return;
          }
          else { /// if MoveStraightDistance > 0
            decMoveStraightDistance();
//            switch (getDirection()) {
//              case up :
//                moveTo(getX(), getY()+1); break;
//              case down :
//                moveTo(getX(), getY()-1); break;
//              case right :
//                moveTo(getX()+1, getY()); break;
//              case left :
//                moveTo(getX()-1, getY()); break;
//            }
          }
        }
        
        
        
        
        break; /// end of "stay" state
    }
  }
  else { ++resting_ticks; return; }
}

/*================ HARD PROTESTER ================*/
void HardProtester::doSomething() {
  if (!isAlive()) return;
  
  if (resting_ticks == std::max(0, 3 - (int)getWorld()->getLevel() / 4)) { /// when it's time to move
    switch (getState()) {
      case WAIT : /// when bribed by gold or shot by water
        if (stalled_ticks == std::max(50, 100 - (int)getWorld()->getLevel() * 10)) {
          setState(STAY);
        }
        else {
          ++stalled_ticks;
        }
        break;
        
      case LEAVE : /// fully get annoyed and leave the oil field
        resting_ticks = 0;
        
        if (getX() == 60 && getY() == 60) { setDead(); return; }
        // ADD: findShortestPath(getX(), getY(), 60, 60)
        break;
        
      case STAY : /// non-leave-the-oil-field state
        resting_ticks = 0;
        
        int icemanX = getWorld()->getIce_man()->getX();
        int icemanY = getWorld()->getIce_man()->getY();
        
        /// if iceman is close enough and protester is facing him
        if (canShout && getWorld()->isInRange(getX(), getY(), icemanX, icemanY, 4.0f)) {
          switch (getDirection()) {
            case up :
              if (getY() < icemanY) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
            case down :
              if (getY() > icemanY) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
            case right :
              if (getX() < icemanX) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
            case left :
              if (getX() > icemanX) { getWorld()->shoutAtIceman(); canShout = false; }
              break;
          }
          return;
        }
        
        if (!canShout) {
          if (non_resting_ticks == 15) {
            non_resting_ticks = 0;
            canShout = true;
          }
          else { ++non_resting_ticks; }
        }
        
        if (getX() == icemanX || getY() == icemanY) {
          Direction dir = none;
          if (getWorld()->canSeeIceman(getX(), getY(), icemanX, icemanY, dir)) {
            setDirection(dir);
            switch (dir) {
              case up :
                moveTo(getX(), getY()+1); break;
              case down :
                moveTo(getX(), getY()-1); break;
              case right :
                moveTo(getX()+1, getY()); break;
              case left :
                moveTo(getX()-1, getY()); break;
            }
            setMoveStraightDistance_0();
            return;
          }
        }
        else { /// if protester can't directly see the iceman
          if (getMoveStraightDistance() == 0) {
            while (true) {
              int dir = rand() % 4 + 1; /// pick a random direction
              switch (dir) {
                case up :
                  if (getWorld()->isIcy(getX(), getY(), up) || getWorld()->isBouldery(getX(), getY(), up)) continue; // maybe improvement
                case down :
                  if (getWorld()->isIcy(getX(), getY(), down) || getWorld()->isBouldery(getX(), getY(), down)) continue;
                case right :
                  if (getWorld()->isIcy(getX(), getY(), right) || getWorld()->isBouldery(getX(), getY(), right)) continue;
                case left :
                  if (getWorld()->isIcy(getX(), getY(), left) || getWorld()->isBouldery(getX(), getY(), left)) continue;
                  
                  setDirection((Direction)dir);
                  pickMoveStraightDistance();
                  break;
              }
              break;
            }
            return;
          }
          else {
            decMoveStraightDistance();
          }
        }
        
        
        // !!!!!!!!! THIS IS HARDCORE !!!!!!!!!!!
        
        
        break; /// end of "stay" state
    }
  }
  else { ++resting_ticks; return; }
}


/*================ OIL ================*/
void Oil::doSomething() {
    if (!isAlive()) return;
  
    auto x = getWorld()->getIce_man()->getX();
    auto y = getWorld()->getIce_man()->getY();
  
    if (!isVisible() && getWorld()->isInRange(x, y, getX(), getY(), 4.0f)) {
        setVisible(true);
        return;
    }
    /// if iceman and oil is in range 3.0, take that oil!
    else if (getWorld()->isInRange(x, y, getX(), getY(), 3.0f)) {
      getWorld()->foundOil(); /// this decrement the number of oil has to be found
      setDead();
      return;
    }
}

/*================ GOLD ================*/
void Gold::doSomething() {
  if (!isAlive()) return;

  switch (getState()) {
    case TEMP : /// TEMP means gold is pickable by protestor and it will be deleted after few ticks
      if (getWorld()->bribeProtester(getX(), getY())) {
        setDead();
        return;
      }
      else {
        if (life_time == 100) { setDead(); return; }
        else { ++life_time; }
      }
      break;
      
    case PERM : /// PERM means gold is pickable by iceman
      auto x = getWorld()->getIce_man()->getX();
      auto y = getWorld()->getIce_man()->getY();
      
      if (!isVisible() && getWorld()->isInRange(x, y, getX(), getY(), 4.0f)) {
        setVisible(true);
      }
      else if (getWorld()->isInRange(x, y, getX(), getY(), 3.0f)) {
        getWorld()->foundGold();
        setDead();
      }
      break;
  }
}

/*================ SONAR ================*/
void Sonar::doSomething() {
  if (!isAlive()) return;
  
  if (getWorld()->isInRange(getX(), getY(), getWorld()->getIce_man()->getX(), getWorld()->getIce_man()->getY(), 3.0f)) {
    getWorld()->foundSonar();
    setDead();
    return;
  }
  
  if (life_time == std::max(100, 300 - 10 * (int)getWorld()->getLevel())) { // need improvement
    setDead();
  }
  else { ++life_time; }
}

/*================ WATER ================*/
void Water::doSomething() {
  if (!isAlive()) return;
  
  if (getWorld()->isInRange(getX(), getY(), getWorld()->getIce_man()->getX(), getWorld()->getIce_man()->getY(), 3.0f)) {
    getWorld()->foundWater();
    setDead();
    return;
  }
  
  if (life_time == std::max(100, 300 - 10 * (int)getWorld()->getLevel())) { // need improvement
    setDead();
  }
  else { ++life_time; }
}

/*================ BOULDER ================*/
void Boulder::doSomething() {
  if (!isAlive()) return;

  switch (getState()) {
    case STABLE :
      /// if there's no ice below, boulder will be in wait state.
      (getWorld()->isIcy(getX(), getY(), down)) ? setState(STABLE) : setState(WAIT);
      break;
      
    case WAIT :
      /// wait for 30 ticks
      if (time_wait == 30) {
        setState(FALL);
        getWorld()->playSound(SOUND_FALLING_ROCK);
      }
      else { ++time_wait; }
      break;
      
    case FALL :
          /// 1) when it hit the ground OR
      if (getY() <= 0 ||
          /// 2) when it hit another boulder OR
          getWorld()->isBouldery(getX(), getY(), down) ||
          /// 3) when it hit the ice
          getWorld()->isIcy(getX(), getY(), down))
      { setDead(); return; }
      
      /// when it hit the iceman
      if (getWorld()->isInRange(getX(), getY(), getWorld()->getIce_man()->getX(), getWorld()->getIce_man()->getY(), 3.0f)) {
        getWorld()->getIce_man()->getAnnoyed(100);
      }
      /// when it hit the protester
      if (getWorld()->bonkProtester(getX(), getY())) {
        // ADD: 
      }
      
      moveTo(getX(), getY()-1);
      break;
  }
}

/*================ SQUIRT ================*/
void Squirt::doSomething() {
  if (!isAlive()) return;
  
  if (getWorld()->shootProtester(getX(), getY())) { setDead(); return; }
  
  if (life_time > 4) { setDead(); return; }
  else ++life_time;
  
  /// when it hit the ice or boulder set it dead
  if (getWorld()->isIcy(getX(), getY(), getDirection()) ||
      getWorld()->isBouldery(getX(), getY(), getDirection()))
  {
    setDead();
    return;
  }
  else {
    switch (getDirection()) {
      case Actor::up :
        moveTo(getX(), getY()+1);
        break;
      case Actor::down :
        moveTo(getX(), getY()-1);
        break;
      case Actor::right :
        moveTo(getX()+1, getY());
        break;
      case Actor::left :
        moveTo(getX()-1, getY());
        break;
    }
  }
}

