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
	char type_name[30];
	char gun_name[30];
	float x, y;
	float v_max;
	int is_destroyed;//logically alive or not
}Battleship;

//Global Variables
float canvas_D = 1000.0f;
int num_escorts = 10;
Battleship battleship;
EscortShip escort_ships[MAX_ESCORT_SHIPS];

//convert angles from degrees to radians
float deg_to_rad(float deg)
{
	return deg *(PI/180.0f);
}

//calculate the distance between two 2D points
float calculate_distance(float x1,float y1,float x2,float y2)
{
	return sqrtf((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

//calculate maximum horizontl projectile range for given velocity and angles
float calculate_max_range(float v_max,float min_angle_deg,float max_angle_deg)
{
	float max_R = 0.0f;
	for (float angle = min_angle_deg; angle<=max_angle_deg; angle +=0.5f)//check min_angle to max_angle by 0.5 degrees
	{
		float rad = deg_to_rad(angle);//convert deg to rad
		float R = (v_max*v_max*sinf(2.0f*rad))/GRAVITY;// standard projectile range calculation formular
		if (R > max_R){
			max_R = R;//max_R update with new maximum range
		}
	}
	return max_R;
}

//Initialize user inputs and Setup battlefir=eld coordinates
void init_simulation()
{
	printf("\n---- SIMULATION SETUP ----\n");
	printf("Enter Canvas Size D (1-1000) : ");
	scanf("%f", &canvas_D);

	printf("Enter Battleship Maximum Velocity V_max : ");
	scanf("%f", &battleship.v_max);

	int b_choice; //for store user choice
	printf("Select Battleship Type(1-4):\n");
	printf("1. USS Iowa (BB-61) (U)\n2. MS King George V (M)\n3. Richelieu (R)\n4. Sovetsky Soyuz-class (S)\nSelect your One: ");
	scanf("%d",&b_choice);
	
	//Assign battleship properties based on user selection
	switch (b_choice)
	{
		case 1:
			battleship.type_code = 'U';
			snprintf(battleship.type_name, 30, "USS Iowa (BB-61)");
			snprintf(battleship.gun_name, 30, "50-caliber Mark 7 gun");
			break;
		case 2:
			battleship.type_code = 'M';
			snprintf(battleship.type_name, 30, "MS King George V");
			snprintf(battleship.gun_name, 30, "(356mm) Mark VII gun");
			break;
		case 3:
			battleship.type_code = 'R';
			snprintf(battleship.type_name, 30, "Richelieu");
			snprintf(battleship.gun_name, 30, "(15 inch) Mle 1935 gun");
			break;
		default:
			battleship.type_code = 'S';
			snprintf(battleship.type_name, 30, "Sovetsky Soyuz-class");
			snprintf(battleship.gun_name, 30, "(16 inch) B-37 gun");
			break;
	}

	//randomly get battleship position
	battleship.x = ((float)rand() / RAND_MAX) * canvas_D;
	battleship.y = ((float)rand() / RAND_MAX) * canvas_D;
	battleship.is_destroyed = 0;


	//Escort ship count inputs
	printf("Enter number of Escort Ships (MAX=50) : ");
	scanf("%d", &num_escorts);

	if(num_escorts > MAX_ESCORT_SHIPS){
		num_escorts = MAX_ESCORT_SHIPS;
	}	
	
	
	//Randomly generate escort ship details
	for (int i = 0; i< num_escorts; i++){
		escort_ships[i].id = i+1;
		escort_ships[i].x = ((float)rand() /RAND_MAX) * canvas_D;
		escort_ships[i].y = ((float)rand() /RAND_MAX) * canvas_D;
		escort_ships[i].is_destroyed = 0;

		int type_rand = rand() % 5;
		switch (type_rand){
			case 0:
				escort_ships[i].type_code ='A';
				snprintf(escort_ships[i].type_name, 30,"1936A-class Destroyer");
				snprintf(escort_ships[i].gun_name, 30,"SK C/32 naval gun");
				escort_ships[i].impact_power =0.08f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 30.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 20.0f;
				escort_ships[i].min_v =10.0f + ((float)rand() / RAND_MAX) * 20.0f;
				escort_ships[i].max_v =1.2f*battleship.v_max;
				break;
			case 1:
				escort_ships[i].type_code ='B';
				snprintf(escort_ships[i].type_name, 30,"Gabbiano-class Corvette");
				snprintf(escort_ships[i].gun_name, 30,"L/47 dual-purpose gun");
				escort_ships[i].impact_power =0.06f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 30.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 30.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
			case 2:
				escort_ships[i].type_code ='C';
				snprintf(escort_ships[i].type_name, 30,"Matsu-class Destroyer");
				snprintf(escort_ships[i].gun_name, 30,"Type 89 dual-purpose gun");
				escort_ships[i].impact_power =0.07f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 30.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 25.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
			case 3:
				escort_ships[i].type_code ='D';
				snprintf(escort_ships[i].type_name, 30,"F-class Escort Ships");
				snprintf(escort_ships[i].gun_name, 30,"SK C/32 naval gun");
				escort_ships[i].impact_power =0.05f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 20.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 50.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
			case 4:
				escort_ships[i].type_code ='E';
				snprintf(escort_ships[i].type_name, 30,"Japanese Kaibokan");
				snprintf(escort_ships[i].gun_name, 30,"4.7 inch naval guns");
				escort_ships[i].impact_power =0.04f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 10.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 70.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
		}
	}



}


int main()
{
    printf("Naval Battle Simulator Initialized. \n");
    return 0;
}
