#include <stdio.h>
#include <stdlib.h>

#define GRID_ROWS 20 // y
#define GRID_COLS 20 // x
#define ALIVE '*'
#define DEAD '.'


/* Translate the specified x,y grid point into the index in
   the linear array. This function implments wrapping, so
   both negative and positive coordinates that are out of the
   grid will wrap around. 
*/
int cell_to_index(int x, int y) {

    /* NEGATIVE UNDERFLOW WRAPPING
        Example:
        x = -1 --> UNDERFLOW --> (x(-1) < 0) --> x = (-x = 1) % 25 = 1 --> x = 25 - 1 = 24
        x = -28 --> UNDERFLOW --> (x(-28) < 0) --> x = (-x = 28) % 25 = 3 --> x = 25 - 3 = 22
    */
    if (x < 0) {
        x = (-x) % GRID_COLS;  // Get the positive equivalent
        x = GRID_COLS - x;     // Wrap around
    }
    if (y < 0) {
        y = (-y) % GRID_ROWS;  // Get the positive equivalent
        y = GRID_ROWS - y;     // Wrap around
    }

    /* POSITIVE OVERFLOW WRAPPING
        Example:
        x = 25 --> OVERFLOW --> (x(25) >= 25) --> x = 25 % 25 = 0 
        x = 26 --> OVERFLOW --> (x(-1) >= 25) --> x = 26 % 25 = 1 
    */
    if (x >= GRID_COLS)
        x = x % GRID_COLS;
    if (y >= GRID_ROWS)
        y = y % GRID_ROWS;

    return y * GRID_COLS + x;

}


/* The function sets the specified cell at x,y to the specified state(AlIVE OR DEAD)*/
void set_cell(char *grid, int x, int y, int state) {
    grid[cell_to_index(x, y)] = state;
}

/* This function return the state of the grid at x,y */
char get_cell(char *grid, int x, int y) {
    return grid[cell_to_index(x, y)];  
}

/* Print the grid on the screen, cleaning the terminal using the required VT100 escape sequence */
void print_grid(char *grid) {
    for (int y = 0; y < GRID_ROWS; y++) {
        for(int x = 0; x < GRID_COLS; x++) {
            printf("%c", get_cell(grid, x, y));
        }
        printf("\n");
    }
}

/* Set all the grid cells to the specified state. */
void set_grid(char *grid, int state) {
    for (int y = 0; y < GRID_ROWS; y++) {
        for(int x = 0; x < GRID_COLS; x++) {
            set_cell(grid, x, y, state);
        }

    }
}

int main(void) {
    char grid[GRID_COLS * GRID_ROWS];
    set_grid(grid, DEAD);
    set_cell(grid, 10, 10, ALIVE);
    print_grid(grid);
    return 0;
}

/*

****
*..*
****

*/