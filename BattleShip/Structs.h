#define SIZE 10 
#define NUM_SHIPS 5
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


typedef enum {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
} Direction;


typedef struct {
    char* name;
    int size;
    int hits;            // How many times it's been hit
    char sunk;            // Boolean: 0 = not sunk, 1 = sunk
}ship;

typedef struct {
    char ships[SIZE][SIZE];     // Player's ship board
    char tracking[SIZE][SIZE];  // Tracks hits/misses on player's board
}gameBoard;

typedef struct {
    gameBoard board;
    ship data[NUM_SHIPS];
} Player;

typedef struct {
    int row;
    int col;
} Point;


const ship defaultShips[NUM_SHIPS] = {
    {"Carrier", 5, 0, 0},
    {"Battleship", 4, 0, 0},
    {"Cruiser", 3, 0, 0},
    {"Submarine", 3, 0, 0},
    {"Destroyer", 2, 0, 0}
};

const Point directions[4] = {
  [UP] = { -1, 0 },
  [RIGHT] = {0, 1},
  [DOWN] = {1, 0},
  [LEFT] = {0, -1}
};

typedef struct {
    int mode;               // 0 = random, 1 = hunt, 2 = track
    Point origin;           // First hit location
    Point current;          // Current position in direction
    Direction dirIndex;     // Current direction being tested
    int tried[4];           // Flags for UP/RIGHT/DOWN/LEFT tried
    int reverse;            // Reverse direction flag for tracking
} AIState;


AIState aiState = { 0, {-1, -1}, {-1, -1}, UP, {0, 0, 0, 0}, 0 };


int visionUsesLeft = 3;
int visionActive = 0;
Point visionCenter = { -1, -1 };
