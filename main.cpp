#include <iostream>
#include <malloc.h>
#include <ctime>
#include <time.h>
#include <random>
#include <cstring>
#include "SDL2/SDL.h"


using namespace std;

void printGrid();
int neighbors(int i, int j);
int neighbors_wrap(int i, int j);
int ** create2DArray(int gridsize);
int set(int i, int j);
void drawGrid(SDL_Surface * surface);
void randomFill(int ** array, int arr_size);

//don't change
#define WRAP
#define SPAWNCHANCE .35
#define USING_RANDOM 1

#define RESET 1200

//change
int gridsize; 
int generations; 
int delay; 
int pixelsize;

#ifdef WRAP
#define NEIGHBORS neighbors_wrap
#else
#define NEIGHBORS neighbors
#endif

int ** grid, ** newgrid, ** temp;

int main(int argc, char * argv[]){
    if(argc == 5){
        gridsize = atoi(argv[1]);
        pixelsize = atoi(argv[2]);
        generations = atoi(argv[3]);
        delay = atoi(argv[4]);
    }else{
        cerr << "Correct use: ./gameoflife [grid size (it's a square grid)] [pixel size (px)] [# of generations, -1 for endless] [delay (ms)]" << endl;
        exit(1);
    }

    if(pixelsize * gridsize > 1200){
        cerr << "Pixel sixe x grid size must be less than 1200, supplied pixelsize [" << pixelsize << "] x gridsize [" << gridsize << "] = " << (pixelsize * gridsize) << "." << endl;
        exit(1);
    }

    //INIT SDL
    int WIDTH = gridsize * pixelsize;
    int HEIGHT = gridsize * pixelsize;

    SDL_Window * window = NULL;
    SDL_Surface * surface = NULL;

    int error = SDL_Init(SDL_INIT_VIDEO);
    if(error){
        cerr << "SDL init failed!" << endl;
        exit(1);
    }
    window = SDL_CreateWindow("Game of Life", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if(!window){
        cerr << "Window init failed!" << endl;
        exit(1);
    }
    surface = SDL_GetWindowSurface(window);

    //INIT
    grid = create2DArray(gridsize);
    newgrid = create2DArray(gridsize);
    temp = create2DArray(gridsize);
    if(!grid || !newgrid || !temp){
        cout << "alloc failed" << endl;
    }
    srandom(time(0));

    if(USING_RANDOM){
        randomFill(grid, gridsize);
    }
    else{ //whatever you want!

        // glider
        // grid[5][5] = 1;
        // grid[5][6] = 1;
        // grid[5][7] = 1;
        // grid[4][7] = 1;
        // grid[3][6] = 1;
        set(330, 330);
        set(330, 331);
        set(331, 331);
        set(329, 331);
        set(329, 332);

    }

    int loop = 0;
    u_int32_t blue_background_color = SDL_MapRGB(surface->format,8,79,229);

    //init
    //struct timespec * ts_last = (struct timespec *)malloc(sizeof(struct timespec *));
    struct timespec * ts_now =  (struct timespec *)malloc(sizeof(struct timespec *));

    clock_gettime(CLOCK_REALTIME, ts_now);
    long last_sec = ts_now -> tv_sec;
    long last_nsec = ts_now -> tv_nsec;


    while(loop++ != generations || generations == 0){
        clock_gettime(CLOCK_REALTIME, ts_now);
        long diff_seconds = ts_now->tv_sec - last_sec;
        long diff_nanoseconds = ts_now->tv_nsec - last_nsec;
        long total_diff_milliseconds = diff_seconds * 1000 + diff_nanoseconds / 1000000;
        long total_diff_nanoseconds = diff_seconds * 1000000000 + diff_nanoseconds; 

        cout << total_diff_milliseconds << endl;
        last_sec = ts_now->tv_sec;
        last_nsec = ts_now->tv_nsec;

        SDL_FillRect(surface, NULL, blue_background_color);

        drawGrid(surface);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(delay);

        SDL_Event evt;
        while(SDL_PollEvent(&evt)){
            switch(evt.type){
                case SDL_QUIT:
                    exit(0);
                default:
                    break;
            }
        }
        
        if(loop % RESET == 0){
            randomFill(grid, gridsize);
        }
        
    

        //printGrid();

        for(int i = 0 ; i < gridsize ; i++)
        {
            for(int j = 0 ; j < gridsize ; j++){
                int k = NEIGHBORS(i, j);

                
                if(grid[i][j]){ //if alive
                    if(k > 3){
                        newgrid[i][j] = 0;
                    }else if(k < 2){
                        newgrid[i][j] = 0;
                    }else{
                        newgrid[i][j] = 1;
                    }
                }else{ //dead
                    if(k == 3){
                        newgrid[i][j] = 1;
                    }else{
                        newgrid[i][j] = 0;
                    }
                }
            }
        }

        temp = grid;
        grid = newgrid;
        newgrid = temp;
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}

void drawGrid(SDL_Surface * surface){
    u_int32_t green_color = SDL_MapRGB(surface->format, 42,245,152);

    SDL_Rect r;

    for(int i = 0 ; i < gridsize ; i++)
    {
        for(int j = 0 ; j < gridsize ; j++){
            if(grid[i][j])
            {
                r.x = i * pixelsize;
                r.y = j * pixelsize;
                r.w = pixelsize;
                r.h = pixelsize;
                SDL_FillRect(surface, &r, green_color);
            }
        }
    }
}

int neighbors(int i, int j){
    int neighbors = 0;
    for(int di = -1 ; di <= 1 ; di++){
        for(int dj = -1 ; dj <= 1 ; dj++){
            if(di == 0 && dj == 0)
                continue;
            if(i + di >= 0 && i + di < gridsize && j + dj >= 0 && j + dj < gridsize)
                neighbors += grid[i+di][j+dj];
        }
    }
    return neighbors;
}

int neighbors_wrap(int i, int j){
    int neighbors = 0;
    int check_i, check_j;

    for(int di = -1 ; di <= 1 ; di++){
        for(int dj = -1 ; dj <= 1 ; dj++){
            if(di == 0 && dj == 0) //skip me
                continue;
            //wrap I
            if(i + di == gridsize)
                check_i = 0;
            else if(i + di < 0)
                check_i = gridsize - 1;
            else
                check_i = i + di;
            //wrap J
            if(j + dj == gridsize)
                check_j = 0;
            else if(j + dj < 0)
                check_j = gridsize - 1;
            else
                check_j = j + dj;
            
            neighbors += grid[check_i][check_j];
            
        }
    }
    return neighbors;
}

void printGrid(){
    for(int i = 0 ; i < gridsize ; i++)
    {
        cout << "|";
        for(int j = 0 ; j < gridsize ; j++){
            if(grid[i][j])
                cout << "#" ;
            else
                cout << " " ;
        }
        cout << "|" << endl;
    }
    cout << "\n---------------------------------\n" << endl;
}


int ** create2DArray(int gridsize){
    int ** m = (int **)calloc(gridsize, sizeof(int*));
    for(int i = 0 ; i < gridsize; i++)
        m[i] = (int *)calloc(gridsize, sizeof(int));

    return m;
}

void randomFill(int ** array, int arr_size){
    for(int i = 0 ; i < arr_size - 1 ; i++)
        {
            for(int j = 0 ; j < arr_size - 1 ; j++){
                array[i][j] = (rand() * 1.0 / RAND_MAX ) > (1-SPAWNCHANCE);
            }
        }
}
int set(int i, int j){
    if(i >= 0 && i < gridsize && j >= 0 && j < gridsize){
        grid[i][j] = 1;
        return 0;
    }else{
        cerr << "Invalid location to set [i = " << i << ", j = " << j << "]\n" << endl;
        return -1;
    }
}

int reset(int i, int j){
    if(i >= 0 && i < gridsize && j >= 0 && j < gridsize){
        grid[i][j] = 0;
        return 0;
    }else{
        cerr << "Invalid location to reset [i = " << i << ", j = " << j << "]\n" << endl;
        return -1;
    }
}