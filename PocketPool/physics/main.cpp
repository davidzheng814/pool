/*IMPORTANT API TO KNOW
ball structure = vector pos, vector vel, int id, int inPocket (-1 if not in pocket)
state structure = double time, int numballs, ball *balls

Given a beginning state with time = 0, return a giant list of states that are the states at multiples of a time DEFAULT_TIME_STEP
    state* allStates(state beginning)

Gives the best possible to hit the cue ball (balls[0]) at
idArray is the vector of the id's of balls that we can possibly sink
-10 is the default "we have no good move" value
    double getBestMove(state cur, std::vector<int> idArray)
    
Assuming you can hit any ball
    double getBestMoveAll(state cur)

Given an angle at which we strike the cue ball, give the vector position of the ghost of the cue ball upon first collision
    vector getGhostImage(state cur, double angle){

*/

#include <cstdio>
#include <cstdlib>
#include <complex>
#include <math.h>
#include <vector>

typedef std::complex<double> vector;

#define DEFAULT_TIME_STEP 0.04
#define X real()
#define Y imag()

const double CORNER_WITHIN_WALL = .045; //perp distance between center of pocket and the rail
const double CORNER_WALL_MISSING = .09; //amount of wall that's missing in each direction around the corner
const double CORNER_SLOPE = 1.4; //slope of the top left corner wall
const double CORNER_RADIUS = .076;

const double SIDE_WALL_MISSING = .076; //amount of wall missing on either side around the side
const double SIDE_WITHIN_WALL = .063;
const double SIDE_SLOPE = 3;
const double SIDE_RADIUS = .063;

const double BALL_RADIUS = .0285;
const double WIDTH = 2.5;
const double HEIGHT = 1.25;
const double FRICTION = 0.1;
const double RAIL_RES = 0.8;
const double BALL_RES = 0.95;

const double MAX_ANIMATION_LENGTH = .5;

//Structure that holds a ball object
struct ball {
  vector pos;
  vector vel;
  int id;
  int inPocket; // 0 if not in pocket

  ball(vector _pos, int _id) {
    pos = _pos;
    vel = vector();
    id = _id;
    inPocket = -1;
  }

  void run(double dt) {
    pos += vel * dt;
    double speed = abs(vel);
    double newSpeed = speed - FRICTION * dt;
    if (newSpeed < 0) {
      newSpeed = 0;
    }
    if (speed != 0) {
      vel *= newSpeed / speed;
    }
  }
};

//Snapshot of the state of the table
struct state {
  double time;
  int numballs;
  ball *balls;
};

//Take the dot product of two vectors
double dot(vector a, vector b) {
  return a.X * b.X + a.Y * b.Y;
}

//Lulz take the square of a number
inline double square(double x) {
  return x * x;
}

//Returns the projection of vector a onto vector b
vector proj(vector a, vector b) {
  return dot(a, b) / square(abs(b)) * b;
}

//Uses law of cosines to calculate when ball a travels within (BALL_RADIUS+radius2) of ball b
//Used within collideBalls and collidePocket
double collideHelper(ball a, ball b, double dt, double radius2) {
  double r = BALL_RADIUS;

  vector dp = a.pos - b.pos, dv = a.vel - b.vel;
  double cos = -1 * dot(dp, dv) / abs(dp * dv);
  double diffD = abs(dp), diffV = abs(dv);
  double disc = square(2 * diffD * cos) - 4 * (square(diffD) - 4 * r * radius2);
  if (disc < 0) {
    return -1;
  }
  double ans = (2 * diffD * cos - sqrt(disc)) / (2 * diffV);
  if (ans <= 0) return -1;
  else return ans;
}

//Returns at what time the two balls collide
double collideBalls(ball a, ball b, double dt) {
  return collideHelper(a, b, dt, BALL_RADIUS);
}

//Determines whether, given some x coordinate, there will be a wall at y=0 or y=height
bool isValidWallWidth(double x){
    double corner = CORNER_WALL_MISSING, side = SIDE_WALL_MISSING, width = WIDTH;
    return ( (corner < x && x < width / 2 - side) || (width / 2 + side < x && x < width - corner) );
}

//Determines whether, given some y coordinate, there will be a wall at x=0 or x=width
bool isValidWallHeight(double y){
    double corner = CORNER_WALL_MISSING, side = SIDE_WALL_MISSING, height = HEIGHT;
    return ( (corner < y && y < height / 2 - side) || (height / 2 + side < y && y < height - corner) );
}

//Returns at what time the ball collides with the wall
double collideWall(ball cur, int wallId, double dt) {
  double x = cur.pos.X, y = cur.pos.Y;
  double vx = cur.vel.X, vy = cur.vel.Y;
  double nx = x + vx * dt, ny = y + vy * dt;
  double r = BALL_RADIUS;
  double width = WIDTH, height = HEIGHT;

  if (wallId == 0) { // (width, 0) -- (0, 0)
    if (ny < r) {
      double t = (r - y) / vy;
      double xAtT = x + vx * t;
      if(isValidWallWidth(xAtT)) { return t;}
    }
  } else if (wallId == 1) { // (width, height) -- (width, 0)
    if (nx > width - r) {
      double t = (width - r - x) / vx;
      double yAtT = y + vy * t;
      if( isValidWallHeight(yAtT)) { return t;}
    }
  } else if (wallId == 2) { // (0, height) -- (width, height)
    if (ny  > height - r) {
      double t = (height - r - y) / vy;
      double xAtT = x + vx * t;
      if(isValidWallWidth(xAtT)) { return t;}
    }
  } else if (wallId == 3) { // (0, 0) -- (0, height)
    if (nx < r) {
      double t = (r - x) / vx;
      double yAtT = y + vy * t;
      if( isValidWallHeight(yAtT)) { return t;}
    }
  }
  return -1;
}

//Returns at what time the ball colliddes with the pocket (ie gets sunk)
//Pockets are labeled as such - top row is 012, bottom row is 345
double collidePocket(ball cur, int pockID, double dt) {
  double width = WIDTH, height = HEIGHT, corner = CORNER_WITHIN_WALL, side = SIDE_WITHIN_WALL;
  double x = 0,y = 0, pocket = 0;
  if(pockID == 0){
    x = -corner, y = -corner, pocket = CORNER_RADIUS;
  } else if (pockID == 1){
    x = width / 2, y = -side, pocket = SIDE_RADIUS;
  } else if (pockID == 2){
    x = width + corner, y = -corner, pocket = CORNER_RADIUS;
  } else if (pockID == 3){
    x = -corner, y = height + corner, pocket = CORNER_RADIUS;
  } else if (pockID == 4){
    x = width / 2, y = height + side, pocket = SIDE_RADIUS;
  } else if(pockID == 5){
    x = width + corner, y = height + corner, pocket = CORNER_RADIUS;
  }
  ball pocketBall = ball(vector(x, y), pockID);
  return collideHelper(cur, pocketBall, dt, pocket - BALL_RADIUS);
}

double min(double x, double y){
  if(x < y){return x;}
  else{return y;}
}

//Used to help determine corner pocket walls
double collideCornerPocketWallHelper0(double x, double y, vector v, double dt){
  double corner = CORNER_WALL_MISSING, slope = CORNER_SLOPE, r = BALL_RADIUS;
  double newX = x + dt * v.X, newY = y + dt * v.Y;
  vector norm = v - proj(v, vector(1, slope));
  if(abs(proj(vector(-newX,corner-newY), norm)) < r && newX < 0){
    double t1 = (abs(proj(vector(corner-x,-y), norm) - r) / abs(norm));
    double t2 = (r - x) / v.X;
    double tFinal = min(t1, t2);
    if(tFinal > 0){return tFinal;}
  } return -1;
}
double collideCornerPocketWallHelper1(double x, double y, vector v, double dt){
  double corner = CORNER_WALL_MISSING, slope = 1 / CORNER_SLOPE, r = BALL_RADIUS;
  double newX = x + dt * v.X, newY = y + dt * v.Y;
  vector norm = v - proj(v, vector(1, slope));
  if(abs(proj(vector(corner-newX,-newY), norm)) < r && newY < 0){
    double t1 = (abs(proj(vector(corner-x,-y), norm) - r) / abs(norm));
    double t2 = (r - y) / v.Y;
    double tFinal = min(t1, t2);
    if(tFinal > 0){return tFinal;}
  } return -1;
}
double collideCornerPocketWallHelper2(double x, double y, vector v, double dt){
  double side = SIDE_WALL_MISSING, slope = -SIDE_SLOPE, r = BALL_RADIUS;
  double newX = x + dt * v.X, newY = y + dt * v.Y;
  vector norm = v - proj(v, vector(1, slope));
  if(abs(proj(vector(-side-newX,-newY), norm)) < r && newY < 0){
    double t1 = (abs(proj(vector(-side-x,-y), norm) - r) / abs(norm));
    double t2 = (r - y) / v.Y;
    double tFinal = min(t1, t2);
    if(tFinal > 0){return tFinal;}
  } return -1;
}
double collideCornerPocketWallHelper3(double x, double y, vector v, double dt){
  double side = SIDE_WALL_MISSING, slope = SIDE_SLOPE, r = BALL_RADIUS;
  double newX = x + dt * v.X, newY = y + dt * v.Y;
  vector norm = v - proj(v, vector(1, slope));
  if(abs(proj(vector(side-newX,-newY), norm)) < r && newY < 0){
    double t1 = (abs(proj(vector(side-x,-y), norm) - r) / abs(norm));
    double t2 = (r - y) / v.Y;
    double tFinal = min(t1, t2);
    if(tFinal > 0){return tFinal;}
  } return -1;
}
//Returns at what time the ball collides with the pocket walls (ie the tiny ones right next to the pockets)
//Pocket walls are labeled as such - top row is 012345, bottom row is 67891011
//For bottom pocket walls, REFLECT everything vertically to create parallel cases
//For right corner pocket walls, REFLECT everything horizontally to create parallel cases
double collidePocketWall(ball cur, int pockWallID, double dt) {
    double width = WIDTH, height = HEIGHT;
    double x = cur.pos.X, y = cur.pos.Y; //ball location relative to the center of the pocket
    double vx = cur.vel.X, vy = cur.vel.Y;
    double slope; 
    if(pockWallID == 0){
      return collideCornerPocketWallHelper0(cur.pos.X, cur.pos.Y, cur.vel, dt);
    } else if(pockWallID == 1){
      return collideCornerPocketWallHelper1(cur.pos.X, cur.pos.Y, cur.vel, dt);
    }else if(pockWallID == 2){
      return collideCornerPocketWallHelper2(cur.pos.X - width / 2, cur.pos.Y, cur.vel, dt);
    }else if(pockWallID == 3){
      return collideCornerPocketWallHelper3(cur.pos.X - width / 2, cur.pos.Y, cur.vel, dt);
    }else if(pockWallID == 4){
      return collideCornerPocketWallHelper1(-(cur.pos.X - width), cur.pos.Y, vector(-cur.vel.X, cur.vel.Y), dt);
    }else if(pockWallID == 5){
      return collideCornerPocketWallHelper1(-(cur.pos.X - width), cur.pos.Y, vector(-cur.vel.X, cur.vel.Y), dt);
    }else if(pockWallID == 6){
      return collideCornerPocketWallHelper1(cur.pos.X, -(cur.pos.Y - height), vector(cur.vel.X, -cur.vel.Y), dt);
    }else if(pockWallID == 7){
      return collideCornerPocketWallHelper1(cur.pos.X, -(cur.pos.Y - height), vector(cur.vel.X, -cur.vel.Y), dt);
    }else if(pockWallID == 8){
      return collideCornerPocketWallHelper1(cur.pos.X - width / 2, -(cur.pos.Y - height), vector(cur.vel.X, -cur.vel.Y), dt);
    }else if(pockWallID == 9){
      return collideCornerPocketWallHelper1(cur.pos.X - width / 2, -(cur.pos.Y - height), vector(cur.vel.X, -cur.vel.Y), dt);
    }else if(pockWallID == 10){
      return collideCornerPocketWallHelper1(-(cur.pos.X - width), -(cur.pos.Y - height), vector(-cur.vel.X, -cur.vel.Y), dt);
    }else if(pockWallID == 11){
      return collideCornerPocketWallHelper1(-(cur.pos.X - width), -(cur.pos.Y - height), vector(-cur.vel.X, -cur.vel.Y), dt);
    }
}

//Adjusts velocities when two balls collide
void handleCollide(ball &a, ball &b) {
  vector dd = b.pos - a.pos;
  vector va = a.vel, vb = b.vel;
  vector tang = vector(dd.Y, -dd.X);
  vector vat = proj(va, tang), vbt = proj(vb, tang);
  a.vel = vat + BALL_RES * (vb - vbt);
  b.vel = vbt + BALL_RES * (va - vat);
}

//Adjusts velocities when a ball collides with the wall
void handleCollideWall(ball &a, int wallId) {
  if (wallId == 0 || wallId == 2) {
    a.vel = vector(a.vel.X, -RAIL_RES * a.vel.Y);
  } else if (wallId == 1 || wallId == 3) {
    a.vel = vector(-a.vel.X, RAIL_RES * a.vel.Y);
  }
}

//Adjusts velocities when a ball goes in a pocket
void handleCollidePocket(ball &a, int pocketID) {
  a.inPocket = pocketID;
  //PROBABLY SHOULD DO SOMETHING HERE
}

//Adjusts velocities when a ball collides with a pocketwall
void handleCollidePocketWall(ball &a, int pocketWallID) {
  double slope = 0;
    if(pocketWallID == 0 || pocketWallID == 1 || pocketWallID == 10 || pocketWallID == 11){
      slope = CORNER_SLOPE;
    } else if(pocketWallID == 4 || pocketWallID == 5 || pocketWallID == 6 || pocketWallID == 7){
      slope = -CORNER_SLOPE;
    } else if(pocketWallID == 2 || pocketWallID == 9){
      slope = -SIDE_SLOPE;
    } else if(pocketWallID == 3 || pocketWallID == 8){
      slope = SIDE_SLOPE;
    }
    vector tan = proj(a.vel, vector(1, slope));
    a.vel = tan - RAIL_RES * (a.vel - tan);
}

bool onTable(state cur, int ballID){
  return cur.balls[ballID].inPocket == -1;
}

//From one state, calculates the next step
state next(state cur) {
  double next_default_time = ceil(cur.time / DEFAULT_TIME_STEP +.0001) * DEFAULT_TIME_STEP;
  double dt = next_default_time - cur.time;
  state nxt = cur;
  int n = cur.numballs;

  int collidei = -1, collidej = -1;
  int collideType = -1; // -1: no, 0: balls, 1: pocket, 2: wall
  for(int i = 0; i < n; ++i) {
    if(onTable(cur, i)){
      for(int j = 1; j < n; ++j) {
        if(onTable(cur, j)){
          double t = collideBalls(cur.balls[i], cur.balls[j], dt);
          if (t != -1 && 0 < t && t < dt) {
            collidei = i;
            collidej = j;
            collideType = 0;
            dt = t;
          }
        }
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    if(onTable(cur, i)){
      for(int j = 0; j < 6; ++j) {
        double t = collidePocket(cur.balls[i], j, dt);
        if(t != -1 && 0 < t && t < dt) {
          collidei = i;
          collidej = j;
          collideType = 1;
          dt = t;
        }
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    if(onTable(cur,i)){
      for(int j = 0; j < 4; ++j) {
        double t = collideWall(cur.balls[i], j, dt);
        if (t != -1 && 0 < t && t < dt) {
          collidei = i;
          collidej = j;
          collideType = 2;
          dt = t;
        }
      }
    }
  }
  
  for(int i = 0; i < n; ++i) {
    if(onTable(cur,i)){
      for(int j = 0; j < 12; ++j) {
        double t = collidePocketWall(cur.balls[i], j, dt);
        if (t != -1 && 0 < t && t < dt) {
          collidei = i;
          collidej = j;
          collideType = 3;
          dt = t;
        }
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    if(onTable(cur, i)){
      cur.balls[i].run(dt);
    }
  }

  if (collideType == 0) {
    printf("Collision type 0, dt = %.02lf, i: %d, j: %d\n", dt, collidei, collidej);
     // handle collision between collidei, collidej
    handleCollide(cur.balls[collidei], cur.balls[collidej]);
  } else if(collideType == 1) {
    printf("Collision type 1, dt = %.02lf, i: %d j: %d\n", dt, collidei, collidej);
    handleCollidePocket(cur.balls[collidei], collidej);
  }
  else if (collideType == 2) {
    printf("Collision type 2, dt = %.02lf, i: %d j: %d\n", dt, collidei, collidej);
    handleCollideWall(cur.balls[collidei], collidej);
    // handle wall collision between collidei with wall collidej
  }
  else if (collideType == 3) {
    printf("Collision type 3, dt = %.02lf, i: %d j: %d\n", dt, collidei, collidej);
    handleCollidePocketWall(cur.balls[collidei], collidej);
    // handle wall collision between collidei with pocket wall collidej
  }

  nxt.time += dt;
  return nxt;
}

//Returns random num from -.5 to .5
double rnd() {
  return (rand() % 100 - 50) / 100.;
}

//Returns if x is close enough to an integer
bool isInteger(double x){
  double epsilon = .0001;
  double fracPart = x - (int) x;
  return ((fracPart < epsilon) || (fracPart > 1 - epsilon));
}

//Main method for the API
//Returns an array of all the states
state* allStates(state beginning) {
  int numFrames = MAX_ANIMATION_LENGTH / DEFAULT_TIME_STEP + 1;
  state *stateList = new state[numFrames];
  state cur = beginning;
  for(int i = 0; i < numFrames; ++i) {
    while(!isInteger(cur.time / DEFAULT_TIME_STEP)){
      cur = next(cur);
    }
    stateList[i] = cur;
    stateList[i].balls = (ball*) malloc(sizeof(ball) * cur.numballs);
    for(int j = 0; j < cur.numballs; ++j) {
      stateList[i].balls[j] = cur.balls[j];
    }
    
    cur = next(cur);
  }
  free(cur.balls);
  return stateList;
}

//If a ball goes on a straight line path from ball a to ball b, will there be miscellaneous collisions?
bool isCollision(state cur, ball a, ball b){
  for(int i = 0; i < cur.numballs; ++i){
    ball c = cur.balls[i];
    if(c.pos != a.pos && c.pos != b.pos){
      vector bRel = b.pos - a.pos;
      vector cRel = c.pos - a.pos;
      double perpDist =  abs(cRel - proj(cRel, bRel));
      if(perpDist < 2 * BALL_RADIUS && dot(bRel, cRel) > 0 && dot(bRel, b.pos - c.pos) > 0) {return true;}
    }
  }
  return false;
}

//Hit ball a to hit ball b to go on a straight line path to ball c - return the unit vector at which we hit the first ball (radians)
//If the such path intersects something else on the way, return -10
double directShot(state cur, ball a, ball b, ball c){
  vector cRelB = c.pos - b.pos;
  ball ghostBall = ball(b.pos - 2 * BALL_RADIUS * cRelB / abs(cRelB), -1);
  if(isCollision(cur, a, ghostBall) || isCollision(cur, b, c)){ return -10;}
  vector path = ghostBall.pos - a.pos;
  return atan2(path.Y, path.X);
}

//Same thing as above, just with a combo
double comboShot(state cur, ball a, ball b, ball c, ball d){
  vector dRelC = d.pos - c.pos;
  ball ghostBall = ball(c.pos - 2 * BALL_RADIUS * dRelC / abs(dRelC), -1);
  if(isCollision(cur, c, d)) { return -10;}
  return directShot(cur, a, b, ghostBall);
}

//Gives the best possible angle to hit the cue ball (balls[0]) at
//idArray is a vector of the id's of balls that we can possibly sink in
//-10 is the default "we have no good move"
double getBestMove(state cur, std::vector<int> idArray){
  double width = WIDTH, height = HEIGHT;
  double cVar = CORNER_WALL_MISSING - 1.4 * BALL_RADIUS;
  double sVar = SIDE_WALL_MISSING - 1.1 * BALL_RADIUS;
  ball cue = cur.balls[0];
  double bestMove = -10;
  double bestRange = 0;
  ball lowPockets[6] = {ball(vector(cVar, 0), -1), ball(vector(width / 2 - cVar, 0), -1), ball(vector(width - cVar, 0), -1), 
    ball(vector(cVar, height), -1), ball(vector(width / 2, height), -1), ball(vector(width - cVar, height), -1)};
  ball pockets[6] = {ball(vector(0, 0), -1), ball(vector(width / 2, -SIDE_RADIUS / 2), -1), ball(vector(width, 0), -1), 
    ball(vector(0, height), -1), ball(vector(width / 2, height + SIDE_RADIUS / 2), -1), ball(vector(width, height), -1)};
  ball highPockets[6] = {ball(vector(0, cVar), -1), ball(vector(width / 2 + cVar, 0), -1), ball(vector(width, cVar), -1), 
    ball(vector(0, height - cVar), -1), ball(vector(width / 2 + cVar, height), -1), ball(vector(width, height - cVar), -1)};
  for(int i = 0; i < idArray.size(); ++i){
    if(onTable(cur, i)){
      ball newBall = cur.balls[idArray[i]];
      for(int j = 0; j < 6; ++j){ //Check direct shot
        double ang1 = directShot(cur, cue, newBall, lowPockets[j]);
        double ang2 = directShot(cur, cue, newBall, highPockets[j]);
        if(ang1 != -10 && ang2 != -10){
          double diff = ang1 - ang2;
          if(diff < 0) {diff = -diff;}
          if(diff > bestRange){
            bestRange = diff;
            bestMove = directShot(cur, cue, newBall, pockets[j]);
          }
        }
        for(int k = 0; k < idArray.size(); ++k){  //Check comboes
          if(i != k && onTable(cur, k)){
            ball endBall = cur.balls[idArray[k]];
            double ang1 = comboShot(cur, cue, newBall, endBall, lowPockets[j]);
            double ang2 = comboShot(cur, cue, newBall, endBall, highPockets[j]);
            if(ang1 != -10 && ang2 != -10){
              double diff = abs(ang1 - ang2);
              if(diff > bestRange){
                bestRange = diff;
                bestMove = comboShot(cur, cue, newBall, endBall, pockets[j]);
              }
            }
          }
        }
      }
    }
  }
  return bestMove;
}

double getBestMoveAll(state cur){
  static const int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  std::vector<int> v (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  return getBestMove(cur, v);
}

//Given an angle at which we strike the cue ball, give the vector position of the ghost of the cue ball upon first collision
vector getGhostImage(state cur, double angle){
  int n = cur.numballs;

  ball copyCue = ball(vector(cur.balls[0].pos.X, cur.balls[0].pos.Y), -1);
  copyCue.vel = vector(cos(angle), sin(angle));

  double dt = 10000000;
  for(int j = 1; j < n; ++j) {
    if(onTable(cur, j)){
      double t = collideBalls(copyCue, cur.balls[j], dt);
          if (0 < t && t < dt) { dt = t; 
  printf("balls: %.2lf %d \n", dt, j);}
      }
  }

  for(int j = 0; j < 6; ++j) {
    double t = collidePocket(copyCue, j, dt);
    if (0 < t && t < dt) {
      dt = t; 
      printf("pocket: %2lf %d \n", dt, j);}
  }

  for(int j = 0; j < 4; ++j) {
    double t = collideWall(copyCue, j, dt);
    if (0 < t && t < dt) {
      dt = t; 
      printf("wall: %.2lf %d \n", dt, j);}
  }
  
  for(int j = 0; j < 12; ++j) {
    double t = collidePocketWall(copyCue, j, dt);
    if (0 < t && t < dt) {
      dt = t; 
      printf("pocketwall: %.2lf %d \n", dt, j);}
  }
  
  printf("%.2lf \n", dt);
  copyCue.run(dt);
  return copyCue.pos;
}

//Displays stuff
void disp(state cur) {
  printf("TIME=%f\n", cur.time);
  for(int i = 0; i < cur.numballs; ++i) {
    ball &b = cur.balls[i];
    printf("BALL(%d): p=(%.2lf, %.2lf) v=(%.2lf, %.4lf)\n", b.id, b.pos.X, b.pos.Y, b.vel.X, b.vel.Y);
  }
  printf("==============\n");
}

//Default state for our simulation
state makeDefaultState() {
  state s;
  s.time = 0;
  s.numballs = 4;
  s.balls = (ball *)malloc(s.numballs*sizeof(ball));
  for(int i = 0; i < s.numballs; ++i) {
    s.balls[i] = ball(vector((rand() % 100) / 50., (rand() % 60) / 60.), i);
    s.balls[i].vel = vector(rnd(), rnd());
  }
  return s;
}

//Whooo starts simulation
int main() {
  state cur;
  cur = makeDefaultState();
  printf("STARTING SIMULATION\n");
  /*for(int i = 0; i < 10; ++i) {
    cur = next(cur);
    disp(cur);
  }
  state *stateList = allStates(cur);
  for(int i = 0; i < 10; ++i) {
    disp(stateList[i]);
  }*/
  disp(cur);
  static const int arr[] = {1,2,3};
  std::vector<int> v (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  vector ghost = getGhostImage(cur, 0);
}