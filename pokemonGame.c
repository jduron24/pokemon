#include <stdio.h>
#include <stdbool.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>

#include "heap.c"

#define BORDER '%'
#define TALL_GRASS ':'
#define GRASS '.'
#define MOUNTAIN '%'
#define TREE '^'
#define WATER '~'
#define ROAD '#'
#define PC '@'
#define MAP_Y 21
#define MAP_X 80
#define dim_y 1
#define dim_x 0




//new code start --------------

#define mapxy(x, y) (m->map[y][x])
#define heightxy(x, y) (m->height[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])


typedef struct path{
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
}path_t;


static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

typedef uint8_t pair_t[2];

static int32_t edge_penalty(uint8_t x, uint8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}
//new code end --------------



typedef struct pc{
    int x, y, lever;// this will be the locations the characters will be at 
}pc_location;

struct border{
    int east,west,north,south;
    bool lever;
    char direction[10];
    int userX, userY;
};

typedef struct map{
    char map[21][80];
    int north, east, south, west;
    int generated;


    uint8_t height[MAP_Y][MAP_X];// new code
}map_t;

typedef struct worldMap{
    map_t **map;
    int *generated;
} world_t;


pc_location location; // This will keep track of the location at all times 
struct border history;// I initialized a global structure so it can be modified in any function



// DIJKSTRAS ALGORTIHM
static void dijkstra_path(map_t *m, pair_t to)// parmeters will be from the position to the player
{

    // he had the cost up here 

  static path_t path[MAP_Y][MAP_X], *p;// references the map from the world map
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;
  int terrian_cost;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[to[dim_y]][to[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
        if(terrian_cost != INT_MAX){
            path[y][x].hn = heap_insert(&h, &path[y][x]);
        }
        else{
            p->hn = NULL;
        }
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != to[dim_x]) || (y != to[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        mapxy(x, y) = 3;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}
// DIJKSTRAS ALGORTIHM


int printTerrain( map_t *map){
    for(int i =0; i < 21;i++){
        for(int j = 0; j < 80;j++){
            printf("%c", map->map[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int random_location_for_pc(int row, int col, map_t *map){// takes in the row and column it should be in 
    int randomNum;
    int randomX, randomY;
    srand(time(NULL));
    randomNum = rand() % 4+1;

    if(randomNum == 1){
        randomX = rand() % 30 + 1 ;
        location.x = randomX;
        map->map[row][randomX] = PC;// internally this changes the x axis 
        location.y = row;
        location.lever = 1;
    }
    else {
        randomY = rand() % 10;
        location.y = randomY;
        map->map[randomY][col] = PC;// internally this gets the y axis 
        location.x = col;
        location.lever = 1;
    }
    return 0;
}


int connectRoads(int borderNumA, int borderNumB, char terrain[21][80], int num){
    
    srand(time(NULL));
    int randomNum = rand() % borderNumB +1;
    if(num == 1){// north(numA) and south(numB)
    for(int i = 20; i > 10; i--){
        terrain[i][borderNumB] = ROAD;
    }
    int lengthFromOtherPath = borderNumA - borderNumB;
    if(borderNumA >= borderNumB){
        for(int i = 0; i < lengthFromOtherPath; i++){
                terrain[11][borderNumB +i] = ROAD;
            }    
    }
    else{
    int lengthFromOtherPath = borderNumB - borderNumA;
    for(int i = lengthFromOtherPath; i > 0; i--){
        terrain[11][borderNumB - i] = ROAD;
    }
    }
    int randomNum = rand() % lengthFromOtherPath +1;
    terrain[10][borderNumB] = 'C';
    terrain[10][borderNumB+1] = 'C';

    for (int i = 11; i > 0; i--)
            {
                terrain[i][lengthFromOtherPath + borderNumB] = ROAD;
            }
    }
   else if(num == 2){//boardNumA(west) and boardNumB(east)
    for(int i = 0; i < 40;i++){
        terrain[borderNumA][i] = ROAD;
    }
    int lengthFromOtherPath = borderNumA - borderNumB;
    if(borderNumA >= borderNumB){
        for(int i = lengthFromOtherPath; i > 0; i--){
                terrain[borderNumA - i][39] = ROAD;
            }    
    }
    else{
        int lengthFromOtherPath = borderNumB - borderNumA;
        for(int i = 0; i < lengthFromOtherPath+1; i++){
            terrain[borderNumA+i][40] = ROAD;
    }
    }
    terrain[borderNumA][40] = 'M';
    terrain[borderNumA][41] = 'M';
    for(int i = 40; i < 79; i++ ){
        terrain[borderNumA - lengthFromOtherPath][i] = ROAD;
    }
   }   
   // finish adding in pokemart and all that 

    return 0;
}



int generateRoad(map_t *map, char *direction){// the parameter direction is going to be used for where I should still hold
int randNumNorthBorder;// the previous value of the border
int randNumSouthBorder;
int randNumEastBorder;
int randNumWestBorder;

srand(time(NULL));

int tmp = history.north;
int tmp2 = history.east;
int tmp3 = history.south;
int tmp4 = history.west;
//test
int tmpT = map->north;
int tmpT2 = map->east;
int tmpT3 = map->south;
int tmpT4 = map->west;
//test


randNumNorthBorder = rand() % 78 + 1;
randNumSouthBorder = rand() % 78 + 1;
randNumEastBorder = rand() % 19 + 1;
randNumWestBorder = rand() % 19 + 1;

//test
int randNumNorthBorder1 = rand() % 78 + 1;
int randNumSouthBorder2 = rand() % 78 + 1;
int randNumEastBorder3 = rand() % 19 + 1;
int randNumWestBorder4 = rand() % 19 + 1;
//test


if (strcmp(direction,"n") == 0){
    if(history.lever == true){
        randNumEastBorder = history.east;
    }
    randNumSouthBorder = tmp;

    randNumSouthBorder2 = tmpT;

}
else if(strcmp(direction,"e") == 0){
    if(history.lever == true){
        randNumSouthBorder = history.south;
    }
    randNumWestBorder = tmp2;
    
    randNumWestBorder4 = tmpT2;
}
else if(strcmp(direction, "s") == 0){
    if(history.lever == true){
        randNumWestBorder = history.west;
    }
    randNumNorthBorder = tmp3;

    randNumNorthBorder1 = tmpT3;
}
else if(strcmp(direction,"w") == 0){

    if(history.lever == true){
        randNumNorthBorder = history.north;
    }
    randNumEastBorder = tmp4;
   
    randNumEastBorder3 = tmpT4;
}


history.north = randNumNorthBorder;
history.east = randNumEastBorder;
history.south = randNumSouthBorder;
history.west = randNumWestBorder;

//test
map->north = randNumNorthBorder1;
map->east = randNumEastBorder3;
map->south = randNumSouthBorder2;
map->west = randNumWestBorder4;
//test

map->map[0][randNumNorthBorder] = ROAD;
map->map[20][randNumSouthBorder] = ROAD;
map->map[randNumWestBorder][0] = ROAD;
map->map[randNumEastBorder][79] = ROAD;


connectRoads(randNumNorthBorder, randNumSouthBorder, map->map,1);
connectRoads(randNumWestBorder, randNumEastBorder, map->map,2);
if(history.userX == 200 && history.userY == 200){
    map->map[randNumEastBorder][79] = BORDER;
    map->map[0][randNumNorthBorder] = BORDER;

}
else if(history.userY == 200 && history.userX == -200 ){
    map->map[randNumWestBorder][0] = BORDER;
    map->map[0][randNumNorthBorder] = BORDER;
    
 }

else if(history.userY == -200 && history.userX == -200){
    map->map[20][randNumSouthBorder] = BORDER;
    map->map[randNumWestBorder][0] = BORDER;
}
else if(history.userY == -200 && history.userX == 200){
    map->map[randNumEastBorder][79] = BORDER;
    map->map[20][randNumSouthBorder] = BORDER;
}
else if(history.userX == 200){
 map->map[randNumEastBorder][79] = BORDER;
}
else if(history.userY == -200){
map->map[20][randNumSouthBorder] = BORDER;
}
else if(history.userX == -200){
     map->map[randNumWestBorder][0] = BORDER;
}
else if(history.userY == 200){
    map->map[0][randNumNorthBorder] = BORDER;
}


return 0;
}

int generateRegions(map_t *map){// this parameter is here to send to the function generateRoad()
    int randomNum;
    int spawnChance;
    
    
    for(int i = 0; i <21;i++){
        for(int j = 0; j<80; j++){
            if(i == 0 || j == 0 || j == 79 || i == 20){
            //terrain[i][j] = BORDER;
            map->map[i][j] = BORDER;
            }
            else{
                //terrain[i][j] = '.';
                map->map[i][j] = '.';
            }
            }
        }
    
    for(int i = 4; i > 0;i--){
        srand(time(NULL));
       
        if(i == 1){//1st quad

            for(int i = 1; i < 10; i++){
                randomNum = rand() % 2 + 1;
                for(int j = 40; j < 79; j++){
                spawnChance = rand() % 3 +1;
                if(randomNum == 1 && spawnChance == 3){
                    map->map[i][j] = TALL_GRASS;
                    }
                else if(randomNum == 2 && spawnChance == 2){
                    map->map[i][j] = WATER;
                }
                }
            }
        }
        else if(i == 2){// 2nd quad
            for(int i = 1; i < 10; i++){
                randomNum = rand() % 3 + 1;
                for(int j = 1; j < 40; j++){
                    spawnChance = rand() % 3 +1;
                    if(randomNum == 1 && spawnChance == 2){
                        
                        map->map[i][j] = MOUNTAIN;
                    }
                    else if(randomNum == 2 && spawnChance == 1){
                        map->map[i][j] = TREE;
                    }
                }
            }
        }

        else if (i == 3){// 4th quad
            for(int i = 10; i < 20; i++){
                randomNum = rand() % 3 + 1;
                for(int j = 40; j < 79; j++){
                    spawnChance = rand() % 2 +1;
                    if(randomNum == 1 && spawnChance == 1){
                        map->map[i][j] = TREE;
                    }
                    else if (randomNum == 2 && spawnChance == 2)
                    {
                        map->map[i][j] = WATER;
                    }
                    
                }
            }
        }
        else if(i == 4){// third quad
            for(int i = 8; i < 20; i++){
                randomNum = rand() % 2 + 1;
                for(int j = 1; j < 40; j++){
                    spawnChance = rand() % 3 +1;
                    if(randomNum == 1 && spawnChance == 2){
                        map->map[i][j] = GRASS;
                    }
                    else if (randomNum == 2 && spawnChance == 3)
                    {
                        map->map[i][j] = TALL_GRASS;
                    }
                }
            }
        }
    }
    generateRoad(map, history.direction);
    //should put our pc below 
    if(location.lever != 1){// only runs if lever == 1
    random_location_for_pc(history.west, history.north,map);
    }
    printTerrain(map);
    map->generated = 1;
    return 0;
}



int terrain(){
    char input[10];
    int x, y;   
    x = 200;
    y = 200;
    int externalX = 0, externalY = 0;
    bool switc;

    // MALLOC TESTING
    world_t w;
    
    w.map = (map_t**)malloc(sizeof(map_t*)*401);
    for (int i = 0; i < 401; i++)
    {
        w.map[i] = (map_t*)malloc(sizeof(map_t)*401);
    }    
    //FINISHED MALLOCING
     
     //w.map[200][200] = generateRegions(input);// test
    pair_t to;
    
    printf("original map\n");
    generateRegions(&(w.map[x][y]));

    dijkstra_path(&(w.map[x][y]), to);

    
    printf("[%d,%d]\n", externalX,externalY);
    w.map[x][y].north = history.north;
    w.map[x][y].east = history.east;
    w.map[x][y].south = history.south;
    w.map[x][y].west = history.west;
    printf("North %d, East %d, West %d, South %d \n", history.north, history.east, history.west, history.south);
    printf("enter a direction: ");
    scanf("%s", history.direction);

    while (strcmp(history.direction,"q") != 0)
    {       
       
        if(strcmp(history.direction,"n") == 0 && x > 0){
            history.userY+=1;
            //printf("Value of &: %d\n", &(w.map[x-1][y]));
            if(w.map[x-1][y].generated == 1){
                x-=1;
                printf("INTERNAL x: %d y: %d\n", x,y);//test
                printTerrain(&(w.map[x][y]));
                externalY++;
            }
            else{
            x-=1;
            if(w.map[x][y+1].generated == 1){
                printf("This is the first west border: %d\n", w.map[x][y+1].west);
                history.east = w.map[x][y+1].west;
                history.lever = true;
            }
            printf("INTERNAL x: %d y: %d\n", x,y);//test
            generateRegions(&(w.map[x][y]));
            externalY++;
            }
        }
        else if(strcmp(history.direction,"e") == 0 && y<400){
            history.userX+=1;
            
            if(y == 0){
                y+=1;
                printf("INTERNAL x: %d y: %d Hisory.userx: %d\n", x,y, history.userX);//test
                generateRegions(&(w.map[x][y]));
                history.lever = false;
                externalX++;
            }
            else if(w.map[x][y+1].generated == 1){
                y+=1;
                printf("INTERNAL x: %d y: %d\n", x,y);//test
                printTerrain(&(w.map[x][y]));
                externalX++;
            }
            
            else{
            y+=1;
            
            if(w.map[x+1][y].generated == 1){
                    printf("INTERNAL x: %d y: %d\n", x,y);//test
                    printf("This is the first norths border: %d\n", w.map[x+1][y].north);
                    history.south = w.map[x+1][y].north;
                    history.lever = true;
                }
            printf("INTERNAL x: %d y: %d\n", x,y);//test
            generateRegions(&(w.map[x][y]));
            externalX++;
            }
        }
        else if(strcmp(history.direction,"s") == 0 && x < 400){
            history.userY-=1;
            if(w.map[x+1][y].generated == 1){
                x+=1;
                printTerrain(&(w.map[x][y]));
                externalY--;
            }
            else{
            x+=1;
            //test
            if(w.map[x][y-1].generated == 1){
                    printf("This is the first east border: %d\n", w.map[x][y-1].east);
                    history.west = w.map[x][y-1].east;
                    history.lever = true;
                }
            //test

            printf("INTERNAL x: %d y: %d\n", x,y);//test
            generateRegions( &(w.map[x][y]));
            externalY--;
            }
        }
        else if(strcmp(history.direction, "w") == 0 && y > 0){
            history.userX-=1;
            if(w.map[x][y-1].generated == 1){
                y-=1;
                printf("INTERNAL x: %d y: %d\n", x,y);//test
                printTerrain(&(w.map[x][y]));
                externalX--;
            }
            else{
                y-=1;  
                //test
                if(x == 0){
                    printf("INTERNAL x: %d y: %d Hisory.userx: %d\n", x,y, history.userX);//test
                    generateRegions(&(w.map[x][y]));
                    history.lever = false;
                
                    externalX--;
                }
                else if(w.map[x-1][y].generated == 1){
                    printf("This is the first south border: %d\n", w.map[x-1][y].south);
                    history.north = w.map[x-1][y].south;
                    history.lever = true;

                    printf("INTERNAL x: %d y: %d\n", x,y);//test
                    generateRegions(&(w.map[x][y]));
                    history.lever = false;
                
                    externalX--;
                }
                else{
                //test
                printf("INTERNAL x: %d y: %d Hisory.userx: %d\n", x,y, history.userX);//test
                generateRegions(&(w.map[x][y]));
                history.lever = false;
                
                externalX--;
                }
            }
        }
        else if(strcmp(history.direction,"f") == 0){
            int xCord, yCord;
            printf("Where would you like to go?  'x y' \n");
            scanf("%d %d", &xCord, &yCord);
            

            
            if(xCord > 200 || yCord >200 || xCord < -200 || yCord < -200){
                printf("Error, out of bounds\n");
            }
       
            if(xCord < 0 && yCord >0){
                x = 200;
                y = 200;
                y += xCord;
                x -= yCord;
                
                externalX = xCord;
                externalY = yCord;  
                history.userX = xCord;
                history.userY = yCord; 

                printf("INTERNAL x: %d y: %d\n", x,y);
                printf("EXTERNAL x: %d y: %d\n", externalX,externalY);
                generateRegions(&(w.map[x][y]));
            }
            else if (xCord < 0 && yCord <0){
                x = 200;
                y = 200;
                y += xCord;
                x -= yCord;
                
                externalX = xCord;
                externalY = yCord;  
                history.userX = xCord;
                history.userY = yCord; 

                printf("INTERNAL x: %d y: %d\n", x,y);
                printf("EXTERNAL x: %d y: %d\n", externalX,externalY);
                generateRegions(&(w.map[x][y]));
            }
            else if(xCord > 0 && yCord < 0){
                x = 200;
                y = 200;
                y += xCord;
                x -= yCord;
                externalX = xCord;
                externalY = yCord;  
                history.userX = xCord;
                history.userY = yCord; 

                printf("INTERNAL x: %d y: %d\n", x,y);
                printf("EXTERNAL x: %d y: %d\n", externalX,externalY);
                generateRegions(&(w.map[x][y]));
            }
            else if(yCord > 0){
                 x= 200;
                 y = 200;
                 x -= yCord;
                 y+= xCord;
                externalX = yCord;
                externalY = xCord;  
                history.userX = xCord;
                history.userY = yCord; 
                printf("INTERNAL x: %d y: %d\n", x,y);
                printf("EXTERNAL x: %d y: %d\n", externalX,externalY);
                generateRegions(&(w.map[x][y]));
            }
            else{
            x= 200;
            y = 200;
            y += xCord;
            x += yCord;
            externalX = yCord;
            externalY = xCord;  
            history.userX = xCord;
            history.userY = yCord; 
            printf("INTERNAL x: %d y: %d  history.userx: %d\n", x,y,history.userX);
            printf("EXTERNAL x: %d y: %d\n", externalX,externalY);
            generateRegions(&(w.map[x][y]));
            }
        }
        else{
            printf("Invalid input, Try again\n");
        }
       
        printf("[%d,%d]\n", externalX,externalY);
        w.map[x][y].north = history.north;
        w.map[x][y].east = history.east;
        w.map[x][y].south = history.south;
        w.map[x][y].west = history.west;
        printf("North %d, East %d, West %d, South %d \n", history.north, history.east, history.west, history.south);
        printf("enter a direction: ");

        scanf("%s", history.direction);
    }

    for (int i = 0; i < 401; i++){
        free(w.map[i]);
    }
    free(w.map);
    return 0;
}

int main(int argc, char *argv[]){
terrain();
return 0;
}


// please update
// token for github "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"