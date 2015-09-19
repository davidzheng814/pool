#include <cstdio>
#include <complex>

typedef std::complex<double> vector;

#define DEFAULT_TIME_STEP 0.04
#define X real()
#define Y imag()

const double BALL_RADIUS = 1;
const double WIDTH = 100;
const double HEIGHT = 100;

struct ball {
  vector position;
  vector velocity;
  int id;
  ball(vector pos, int _id) {
    position = pos;
    velocity = vector();
    id = id;
  }
};

struct state {
  double time;
  int numballs;
  ball *balls;
};

double collideBalls(ball a, ball b, double dt) {

}

double collideWall(ball cur, int wall_id, double dt) {

}

state next(state cur) {
  double dt = DEFAULT_TIME_STEP;
  state nxt = cur;
  int n = cur.numballs;

  int collidei = -1, collidej = -1;
  double collidet = dt;
  for(int i = 0; i < n; ++i) {
    for(int j = 1; j < n; ++j) {
      double t = collideBalls(i, j, dt);
      if (t < collidet) {
        collidei = i;
        collidej = j;
        collidet = t;
      }
    }
  }

  int collideWalli = -1, collideWallj = -1;
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < 4; ++j) {
      double t = collideWall(cur.balls[i], j, dt);
      if (t < collidet) {
        collideWalli = i;
        collideWallj = j;
        collidet = t;
      }
    }
  }

  nxt.time += dt;
  return nxt;
}

state makeDefaultState() {
  state s;
  s.time = 0;
  s.numballs = 1;
  s.balls = (ball *)malloc(s.numballs*sizeof(ball));
  for(int i = 0; i < s.numballs; ++i) {
    s.balls[i] = ball(vector(i *10* BALL_RADIUS, 0), 0);
  }
  return s;
}

void disp(state cur) {
  printf("TIME=%f\n", cur.time);
  for(int i = 0; i < cur.numballs; ++i) {
    ball &b = cur.balls[i];
    printf("BALL(%d): (%lf, %lf)\n", b.id, b.position.X, b.position.Y);
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
