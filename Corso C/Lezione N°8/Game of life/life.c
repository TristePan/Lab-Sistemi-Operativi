#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define GRID_ROWS 20 // y
#define GRID_COLS 20 // x
#define GRID_SIZE (GRID_ROWS * GRID_COLS)
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
    printf("\033[H\033[J"); // VT100 escape sequence to cl ear the terminal
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

/* Return the number of living cells neighbors of x,y */
int count_alive_neighbors(char *grid, int x, int y) {
    int alive_count = 0;
    for(int yo = -1; yo <= 1; yo++) {
        for(int xo = -1; xo <= 1; xo++) {
            if (xo == 0 && yo == 0)
                continue; // Skip the cell itself
            if (get_cell(grid, x + xo, y + yo) == ALIVE)
                alive_count++;
        }
    }
    return alive_count;
}


/* Compute the new state of the grid according the rules */
void new_state(char *old_g, char *new_g) {
    for (int y = 0; y < GRID_ROWS; y++) {
        for(int x = 0; x < GRID_COLS; x++) {
            int alive_n = count_alive_neighbors(old_g, x, y);
            int new_state = DEAD;
            if(get_cell(old_g, x, y) == ALIVE) {
                if(alive_n == 2 || alive_n == 3)
                    new_state = ALIVE;
            } else {
                if(alive_n == 3)
                    new_state = ALIVE;
            }
            set_cell(new_g, x, y, new_state);
        }
    }

}


int main(void) {
    char old_grid[GRID_SIZE];
    char new_grid[GRID_SIZE];
    
    set_grid(old_grid, DEAD);
    set_cell(old_grid, 10, 10, ALIVE);
    set_cell(old_grid, 9, 10, ALIVE);
    set_cell(old_grid, 11, 10, ALIVE);
    set_cell(old_grid, 11, 9, ALIVE);
    set_cell(old_grid, 10, 8, ALIVE);

    /* Method with pointers */
    char *old = old_grid;
    char *new = new_grid;
    
    while(1) {
        new_state(old, new);
        print_grid(new);

        usleep(100000);

        // Swap pointers
        char *temp = old;
        old = new;
        new = temp;

        /* No pointer method
        new_state(new_grid, old_grid);
        print_grid(old_grid);
        */
    }


    /*
    print_grid(old_grid);
    
    printf("\n");
    printf("---------------------------------------------\n");
    printf("---------------------------------------------\n");
    printf("\n");
    
    new_state(old_grid, new_grid);
    print_grid(new_grid);
    */
    return 0;
}