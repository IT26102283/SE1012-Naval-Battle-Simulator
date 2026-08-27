#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define PI 3.14159265358979323846
#define GRAVITY 9.81
#define MAX_ESCORT_SHIPS 50 //array for escort ships

//Struct for Escort Ships
typedef struct
{
	int id;
	char type_code;
	char type_name[30];
	char gun_name[30];
	float x, y;
	float impact_power;
	float min_ang;
	float max_ang;
	float min_v;
	float max_v;
	int is_destroyed;//logically alive or not
}EscortShip;

//Struct for Battleship
typedef struct
{
	char type_code;
	char type_name;
	char gun_name;
	float x, y;
	float v_max;
	int is_destroyed;//logically alive or not
}Battleship;

//Global Variables
float canvas_D = 1000.0f;
int num_escorts = 10;
Battleship battleship;
EscortShip escort_ships[MAX_ESCORT_SHIPS];

int main()
{
    printf("Naval Battle Simulator Initialized. \n");
    return 0;
}
