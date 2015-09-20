#include <cstdio>
#include <complex>

typedef std::complex<double> vector;

#define DEFAULT_TIME_STEP 0.04
#define X real()
#define Y imag()

const double BALL_RADIUS = .029;
const double WIDTH = 3;
const double HEIGHT = 1.5;
const double FRICTION = 0.1;
const double RAIL_RES = 0.75;
const double BALL_RES = 0.95;

struct ball {
  vector pos;
  vector vel;
  int id;

  ball(vector _pos, int _id) {
    pos = _pos;
    vel = vector();
    id = _id;
  }

  void run(double dt) {
    pos += vel * dt;
    double speed = abs(vel);
    double newSpeed = speed - FRICTION * dt;
    if (newSpeed < 0) {
      newSpeed = 0;
    }
    vel *= newSpeed / speed;
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

double collideBalls(ball a, ball b, double dt) {
  double r = BALL_RADIUS;

  vector dp = a.pos - b.pos, dv = a.vel - b.vel;
  double cos = -1 * dot(dp, dv) / abs(dp * dv);
  double diffD = abs(dp), diffV = abs(dv);
  double disc = square(2 * diffD * cos) - 4 * (square(diffD) - 4 * square(r));
  if (disc < 0) {
    return -1;
  }
  double ans = (2 * diffD * cos - sqrt(disc)) / (2 * diffV);
  if (ans <= 0) return -1;
  else return ans;
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

state next(state cur) {
  double dt = DEFAULT_TIME_STEP;
  state nxt = cur;
  int n = cur.numballs;

  int collidei = -1, collidej = -1;
  int collideType = -1; // -1: no, 0: balls, 1: wall
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
    for(int j = 0; j < 4; ++j) {
      double t = collideWall(cur.balls[i], j, dt);
      if (t != -1 && t < dt) {
        collidei = i;
        collidej = j;
        collideType = 1;
        dt = t;
      }
    }
  }

  for(int i = 0; i < n; ++i) {
    cur.balls[i].run(dt);
  }

  if (collideType == 0) {
    printf("Collision type 0, dt = %.02lf\n", dt);
     // handle collision between collidei, collidej
    handleCollide(cur.balls[collidei], cur.balls[collidej]);
  } else if (collideType == 1) {
    printf("Collision type 1, dt = %.02lf\n", dt);
    handleCollideWall(cur.balls[collidei], collidej);
    // handle wall collision between collidei with wall collidej
  }

  nxt.time += dt;
  return nxt;
}

state makeDefaultState() {
  state s;
  s.time = 0;
  s.numballs = 2;
  s.balls = (ball *)malloc(s.numballs*sizeof(ball));
  for(int i = 0; i < s.numballs; ++i) {
    s.balls[i] = ball(vector(.5 + i *10* BALL_RADIUS, 0), 0);
    s.balls[i].vel = vector(0, 10);
  }
  s.balls[1] = ball(vector(.5, 1), 1);
  s.balls[1].vel = vector(0, -10);
  return s;
}

void disp(state cur) {
  printf("TIME=%f\n", cur.time);
  for(int i = 0; i < cur.numballs; ++i) {
    ball &b = cur.balls[i];
    printf("BALL(%d): p=(%.2lf, %.2lf) v=(%.2lf, %.2lf)\n", b.id, b.pos.X, b.pos.Y, b.vel.X, b.vel.Y);
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
