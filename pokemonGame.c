#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BORDER '%'
#define TALL_GRASS ':'
#define GRASS '.'
#define MOUNTAIN '%'
#define TREE '^'
#define WATER '~'
#define ROAD '#'


struct border{
    int east,west,north,south;
};
struct border history;// I initialized a global structure so it can be modified in any function
int printTerrain(char grid[21][80]){
    for(int i =0; i < 21;i++){
        for(int j = 0; j < 80;j++){
            printf("%c", grid[i][j]);
        }
        printf("\n");
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



int generateRoad(char terrain[21][80], char *direction){// the parameter direction is going to be used for where I should still hold
int randNumNorthBorder;// the previous value of the border
int randNumSouthBorder;
int randNumEastBorder;
int randNumWestBorder;

srand(time(NULL));

int tmp = history.north;



randNumNorthBorder = rand() % 78 + 1;
randNumSouthBorder = rand() % 78 + 1;
randNumEastBorder = rand() % 19 + 1;
randNumWestBorder = rand() % 19 + 1;


if (strcmp(direction,"n") == 0){
    randNumSouthBorder = tmp;
}

history.north = randNumNorthBorder;

terrain[0][randNumNorthBorder] = ROAD;
terrain[20][randNumSouthBorder] = ROAD;
terrain[randNumWestBorder][0] = ROAD;
terrain[randNumEastBorder][79] = ROAD;

connectRoads(randNumNorthBorder, randNumSouthBorder, terrain,1);
connectRoads(randNumWestBorder, randNumEastBorder, terrain,2);

return 0;
}

int generateRegions(char *direction){// this parameter is here to send to the function generateRoad()
    char terrain[21][80];
    int randomNum;
    int spawnChance;
    
    for(int i = 0; i <21;i++){
        for(int j = 0; j<80; j++){
            if(i == 0 || j == 0 || j == 79 || i == 20){
            terrain[i][j] = BORDER;
            }
            else{
                terrain[i][j] = '.';
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
                    terrain[i][j] = TALL_GRASS;
                    }
                else if(randomNum == 2 && spawnChance == 2){
                    terrain[i][j] = WATER;
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
                        
                        terrain[i][j] = MOUNTAIN;
                    }
                    else if(randomNum == 2 && spawnChance == 1){
                        terrain[i][j] = TREE;
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
                        terrain[i][j] = TREE;
                    }
                    else if (randomNum == 2 && spawnChance == 2)
                    {
                        terrain[i][j] = WATER;
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
                        terrain[i][j] = GRASS;
                    }
                    else if (randomNum == 2 && spawnChance == 3)
                    {
                        terrain[i][j] = TALL_GRASS;
                    }
                }
            }
        }
        
    }
    generateRoad(terrain, direction);
    printTerrain(terrain);
    return 0;
}



int terrain(){
    int map[401][401];
    char input[10];
    int x, y;   
    x = 200;
    y = 200;

    map[x][y] = generateRegions(input);
    printf("[%d,%d]", x,y);
    printf("enter a random num: ");
    scanf("%s", input);
    while (strcmp(input,"q") != 0)
    {
       
        if(strcmp(input,"n") == 0){
            map[x][y++] = generateRegions(input);
        }
        printf("[%d,%d]", x,y);
        printf("enter a random num: ");
        scanf("%s", input);
    }
    

    
    return 0;
}

int main(int argc, char *argv[]){
terrain();
return 0;
}


// please update
// token for github "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"