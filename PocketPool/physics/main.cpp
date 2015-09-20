#include <cstdio>
#include <cstdlib>
#include <complex>

typedef std::complex<double> vector;

#define DEFAULT_TIME_STEP 0.04
#define X real()
#define Y imag()

const double BALL_RADIUS = .029;
const double POCKET_RADIUS = .05;
const double WIDTH = 3;
const double HEIGHT = 1.5;
const double FRICTION = 0.1;
const double RAIL_RES = 0.75;
const double BALL_RES = 0.95;

struct ball {
  vector pos;
  vector vel;
  int id;
  int inpocket; // 0 if not in pocket

  ball(vector _pos, int _id) {
    pos = _pos;
    vel = vector();
    id = _id;
    inpocket = 0;
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

struct state {
  double time;
  int numballs;
  ball *balls;
};

double dot(vector a, vector b) {
  return a.X * b.X + a.Y * b.Y;
}

inline double square(double x) {
  return x * x;
}

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

double collideBalls(ball a, ball b, double dt) {
  return collideHelper(a, b, dt, BALL_RADIUS);
}

double collideWall(ball cur, int wallId, double dt) {
  double x = cur.pos.X, y = cur.pos.Y;
  double vx = cur.vel.X, vy = cur.vel.Y;
  double nx = x + vx * dt, ny = y + vy * dt;
  double r = BALL_RADIUS;
  double width = WIDTH, height = HEIGHT;

  if (wallId == 0) { // (width, 0) -- (0, 0)
    if (ny < r) {
      return (r - y) / vy;
    }
  } else if (wallId == 1) { // (width, height) -- (width, 0)
    if (nx > width - r) {
      return (width - r - x) / vx;
    }
  } else if (wallId == 2) { // (0, height) -- (width, height)
    if (ny  > height - r) {
      return (height - r - y) / vy;
    }
  } else if (wallId == 3) { // (0, 0) -- (0, height)
    if (nx < r) {
      return (r - x) / vx;
    }
  }
  return -1;
}

double collidePocket(ball cur, int pockID, double dt) {
  double width = WIDTH, height = HEIGHT;
  double x = 0,y = 0;
  if(pockID == 0){
    x = 0, y = 0;
  } else if (pockID == 1){
    x = width / 2, y = 0;
  } else if (pockID == 2){
    x = width, y = 0;
  } else if (pockID == 3){
    x = 0, y = height;
  } else if (pockID == 4){
    x = width / 2, y = height;
  } else if(pockID == 5){
    x = width, y = height;
  }
  ball pocketBall = ball(vector(x, y), pockID);
  return collideHelper(cur, pocketBall, dt, POCKET_RADIUS - BALL_RADIUS);
}

vector proj(vector a, vector b) {
  return dot(a, b) / square(abs(b)) * b;
}

void handleCollide(ball &a, ball &b) {
  vector dd = b.pos - a.pos;
  vector va = a.vel, vb = b.vel;
  vector tang = vector(dd.Y, -dd.X);
  vector vat = proj(va, tang), vbt = proj(vb, tang);
  a.vel = vat + BALL_RES * (vb - vbt);
  b.vel = vbt + BALL_RES * (va - vat);
}

void handleCollideWall(ball &a, int wallId) {
  if (wallId == 0 || wallId == 2) {
    a.vel = vector(a.vel.X, -RAIL_RES * a.vel.Y);
  } else if (wallId == 1 || wallId == 3) {
    a.vel = vector(-a.vel.X, RAIL_RES * a.vel.Y);
  }
}

void handleCollidePocket(ball &a, int pocketID) {
  a.inpocket = pocketID;
}

state next(state cur) {
  double dt = DEFAULT_TIME_STEP;
  state nxt = cur;
  int n = cur.numballs;

  int collidei = -1, collidej = -1;
  int collideType = -1; // -1: no, 0: balls, 1: pocket, 2: wall
  for(int i = 0; i < n; ++i) {
    for(int j = 1; j < n; ++j) {
      double t = collideBalls(cur.balls[i], cur.balls[j], dt);
      if (t != -1 && t < dt) {
        collidei = i;
        collidej = j;
        collideType = 0;
        dt = t;
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < 6; ++j) {
      double t = collidePocket(cur.balls[i], j, dt);
      if(t != -1 && t < dt) {
        collidei = i;
        collidej = j;
        collideType = 1;
        dt = t;
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < 4; ++j) {
      double t = collideWall(cur.balls[i], j, dt);
      if (t != -1 && t < dt) {
        collidei = i;
        collidej = j;
        collideType = 2;
        dt = t;
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    cur.balls[i].run(dt);
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

  nxt.time += dt;
  return nxt;
}

double rnd() {
  return (rand() % 100 - 50) / 100.;
}

state makeDefaultState() {
  state s;
  s.time = 0;
  s.numballs = 1;
  s.balls = (ball *)malloc(s.numballs*sizeof(ball));
  for(int i = 0; i < s.numballs; ++i) {
    s.balls[i] = ball(vector(1.4, .15), i);
    s.balls[i].vel = vector(.5, -.5);
  }
  return s;
}

void disp(state cur) {
  printf("TIME=%f\n", cur.time);
  for(int i = 0; i < cur.numballs; ++i) {
    ball &b = cur.balls[i];
    printf("BALL(%d): p=(%.2lf, %.2lf) v=(%.2lf, %.4lf)\n", b.id, b.pos.X, b.pos.Y, b.vel.X, b.vel.Y);
  }
  printf("==============\n");
}

int main() {
  state cur;
  cur = makeDefaultState();
  printf("STARTING SIMULATION\n");
  disp(cur);
  for(int i = 0; i < 10; ++i) {
    cur = next(cur);
    disp(cur);
  }
}