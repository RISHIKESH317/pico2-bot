
#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>

// Maze grid dimensions
#define SIZE 16

// Wall bit flags (each cell can have N, E, S, W walls)
#define WALL_N 0x01
#define WALL_E 0x02
#define WALL_S 0x04
#define WALL_W 0x08

// Global arrays to store maze data
int walls[SIZE][SIZE];       // wall bitmask per cell
int distMap[SIZE][SIZE];     // flood-fill distance map
bool visited[SIZE][SIZE];    // visited flag for each cell

// Initialize maze data: set all walls to unknown and visited false.
// Also set the outer border walls of the maze.
void initMaze() {
    for(int x = 0; x < SIZE; x++) {
        for(int y = 0; y < SIZE; y++) {
            walls[x][y] = 0;
            distMap[x][y] = 0;
            visited[x][y] = false;
        }
    }
    // Mark outer borders as walls
    for(int i = 0; i < SIZE; i++) {
        walls[i][0]       |= WALL_S;  // south border
        walls[i][SIZE-1]  |= WALL_N;  // north border
        walls[0][i]       |= WALL_W;  // west border
        walls[SIZE-1][i]  |= WALL_E;  // east border
    }
}

// Perform a flood-fill (BFS) from the goal cells to compute distMap[][]. 
// Cells in the center 2×2 (7,7),(7,8),(8,7),(8,8) are distance 0.
// Walls in 'walls[][]' block propagation.
void floodFill() {
    // Initialize all distances to a large number
    for(int x = 0; x < SIZE; x++) {
        for(int y = 0; y < SIZE; y++) {
            distMap[x][y] = 1000;
        }
    }
    // Define goal cells (center 2x2)
    struct Cell { int x, y; };
    Cell queue[SIZE*SIZE];
    int qh = 0, qt = 0;
    // Push all goal cells into queue with distance 0
    int goals[4][2] = { {7,7}, {7,8}, {8,7}, {8,8} };
    for(int i = 0; i < 4; i++) {
        int gx = goals[i][0], gy = goals[i][1];
        distMap[gx][gy] = 0;
        queue[qt++] = { gx, gy };
    }
    // BFS propagation
    while(qh < qt) {
        Cell c = queue[qh++];
        int x = c.x, y = c.y;
        int d = distMap[x][y];
        // Check all 4 neighbors for open paths (no wall)
        // North
        if(y < SIZE-1 && !(walls[x][y] & WALL_N)) {
            if(distMap[x][y+1] > d+1) {
                distMap[x][y+1] = d+1;
                queue[qt++] = {x, y+1};
            }
        }
        // South
        if(y > 0 && !(walls[x][y] & WALL_S)) {
            if(distMap[x][y-1] > d+1) {
                distMap[x][y-1] = d+1;
                queue[qt++] = {x, y-1};
            }
        }
        // East
        if(x < SIZE-1 && !(walls[x][y] & WALL_E)) {
            if(distMap[x+1][y] > d+1) {
                distMap[x+1][y] = d+1;
                queue[qt++] = {x+1, y};
            }
        }
        // West
        if(x > 0 && !(walls[x][y] & WALL_W)) {
            if(distMap[x-1][y] > d+1) {
                distMap[x-1][y] = d+1;
                queue[qt++] = {x-1, y};
            }
        }
    }
}

// Decide the next move direction from (cx,cy) facing 'orientation'. 
// We look for an accessible neighbor with minimum distMap value.
// Returns: 0 = forward, 1 = turn left then forward, 2 = turn right then forward, 3 = U-turn then forward.
// If no move is found, returns -1.
int chooseMove(int cx, int cy, int orientation) {
    // Directions: 0=North,1=East,2=South,3=West
    int bestDir = -1;
    int bestDist = 1000;
    // Check all 4 possible moves (global directions)
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    int wallFlag[4] = {WALL_N, WALL_E, WALL_S, WALL_W};
    for(int dir = 0; dir < 4; dir++) {
        int nx = cx + dx[dir];
        int ny = cy + dy[dir];
        if(nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) continue;
        // Check if there's a wall in that direction
        if(walls[cx][cy] & wallFlag[dir]) continue;
        // Candidate neighbor
        int d = distMap[nx][ny];
        // Prefer unvisited cells first
        if(!visited[nx][ny]) {
            if(d < bestDist) {
                bestDist = d;
                bestDir = dir;
            }
        }
    }
    // If no unvisited neighbor found, allow visited ones
    if(bestDir < 0) {
        for(int dir = 0; dir < 4; dir++) {
            int nx = cx + dx[dir];
            int ny = cy + dy[dir];
            if(nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) continue;
            if(walls[cx][cy] & wallFlag[dir]) continue;
            int d = distMap[nx][ny];
            if(d < bestDist) {
                bestDist = d;
                bestDir = dir;
            }
        }
    }
    if(bestDir < 0) return -1;
    // Determine relative turn command based on current orientation
    int diff = (bestDir - orientation + 4) % 4;
    if(diff == 0) return 0;        // forward
    else if(diff == 1) return 2;   // turn right
    else if(diff == 3) return 1;   // turn left
    else return 3;                 // U-turn
}

#endif  // MAZE_H
