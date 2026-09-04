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

//calculate maximum rangge of battleship
float calculate_battleship_range()
{
	return(battleship.v_max * battleship.v_max) / GRAVITY;
}

//calculate projective range
float calculate_range(float velocity,float angle)
{
	float angle_rad = deg_to_rad(angle);
	return(velocity * velocity * sinf(2.0f * angle_rad)) / GRAVITY;
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
				escort_ships[i].type_code = 'A';
				snprintf(escort_ships[i].type_name, 30,"1936A-class Destroyer");
				snprintf(escort_ships[i].gun_name, 30,"SK C/34 naval gun");
				escort_ships[i].impact_power =0.08f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 30.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 20.0f;
				escort_ships[i].min_v =10.0f + ((float)rand() / RAND_MAX) * 20.0f;
				escort_ships[i].max_v =1.2f*battleship.v_max;
				break;
			case 1:
				escort_ships[i].type_code = 'B';
				snprintf(escort_ships[i].type_name, 30,"Gabbiano-class Corvette");
				snprintf(escort_ships[i].gun_name, 30,"L/47 dual-purpose gun");
				escort_ships[i].impact_power =0.06f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 30.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 30.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
			case 2:
				escort_ships[i].type_code = 'C';
				snprintf(escort_ships[i].type_name, 30,"Matsu-class Destroyer");
				snprintf(escort_ships[i].gun_name, 30,"Type 89 dual-purpose gun");
				escort_ships[i].impact_power =0.07f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 30.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 25.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
			case 3:
				escort_ships[i].type_code = 'D';
				snprintf(escort_ships[i].type_name, 30,"F-class Escort Ships");
				snprintf(escort_ships[i].gun_name, 30,"SK C/32 naval gun");
				escort_ships[i].impact_power =0.05f;
				escort_ships[i].min_ang =((float)rand() / RAND_MAX) * 20.0f;
				escort_ships[i].max_ang =escort_ships[i].min_ang + 50.0f;
				escort_ships[i].min_v =10.0f +((float)rand() /RAND_MAX) * 10.0f;
				escort_ships[i].max_v =((float)rand() / RAND_MAX) * battleship.v_max;
				break;
			case 4:
				escort_ships[i].type_code = 'E';
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
	
	// Create & Open initial_config.txt file
	FILE *fp = fopen("initial_config.txt", "w");
	if (fp == NULL)
	{
		printf("Error creating initial_config.txt file!\n");
		return;
	}

	//write canvas size and battleship parameters
	fprintf(fp, "----INITIAL BATTLEFIELD CONFIGURATION----\n");
	fprintf(fp, "Canvas Size (D): %.2f\n\n", canvas_D);
	fprintf(fp, "[BATTLESHIP]\n");
	fprintf(fp, "Type Code: %c\n", battleship.type_code);
	fprintf(fp, "Type Name: %s\n", battleship.type_name);
	fprintf(fp, "Gun Name : %s\n", battleship.gun_name);
	fprintf(fp, "Position : (%.2f, %.2f)\n", battleship.x, battleship.y);
	fprintf(fp, "V_max    : %.2f\n\n", battleship.v_max);

	//write escort ships parameters
	fprintf(fp, "[ESCORT SHIPS (Total: %d)]\n", num_escorts);
	for (int i=0; i < num_escorts; i++)
	{
		fprintf(fp, "ID: %d | Code: %c | Name: %s | Position: (%.2f, %.2f) | Range V: [%.2f - %.2f] | Range Angles: [%.2f- %.2f] | Impact: %.2f\n",
				escort_ships[i].id,
				escort_ships[i].type_code,
				escort_ships[i].type_name,
				escort_ships[i].x,
				escort_ships[i].y,
				escort_ships[i].min_v,
				escort_ships[i].max_v,
				escort_ships[i].min_ang,
				escort_ships[i].max_ang,
				escort_ships[i].impact_power);
	}

	fclose(fp);
	printf("\n[SUCCESS] Battlefield setup completed & initial_config.txt saved!\n");
}

void simulate_part1A()
{
	int sinking_ship_id = 0;
	int total_hits = 0;
	float battle_end_time = 0.0f;

	printf("\n---- BATTLE SIMULATION ----\n");

	//escort ships attack the battleship
	for (int i=0; i< num_escorts; i++)
	{
		float distance;

		distance = calculate_distance(battleship.x,battleship.y,escort_ships[i].x,escort_ships[i].y);
		float min_range = calculate_range(escort_ships[i].min_v,escort_ships[i].min_ang);
		float max_range = calculate_range(escort_ships[i].max_v,escort_ships[i].max_ang);

		if(distance >= min_range && distance <= max_range)
		{
			battleship.is_destroyed = 1;
			sinking_ship_id = escort_ships[i].id;

			printf("\nBattleship was hit by E%d.\n", sinking_ship_id);
			printf("Battleship is destroyed.\n");
			break;
		}
	}
	if(battleship.is_destroyed == 1)
	{
		FILE *fp = fopen("battle_log.txt", "w");
		if(fp !=NULL)
		{
			fprintf(fp, " ---- PART 1-A BATTLE RESULT ----\n");
			fprintf(fp, "Battle Status : DESTROYED\n");
			fprintf(fp, "E ship that sank B : E%d\n", sinking_ship_id);

			fclose(fp);
		}
	}
	else
	{
		float battleship_range = calculate_battleship_range();

		printf("\nBattleship survived the E attacks.\n");
		printf("Battleship Attack Range : %.2f m\n", battleship_range);

		//battleship attack escort ships
		for(int i=0; i < num_escorts; i++)
		{
			float distance;
			distance = calculate_distance(battleship.x,battleship.y,escort_ships[i].x,escort_ships[i].y);
			if (distance <= battleship_range)
			{
				escort_ships[i].is_destroyed = 1;
				total_hits++;
				
				float angle = 45.0f;
				float velocity = battleship.v_max;
				
				float time = (2.0f * velocity * sinf(deg_to_rad(angle)))/GRAVITY;
				
				if(time > battle_end_time)
				{
					battle_end_time = time;
				}
				printf("\nNumber of E ships hit : %d\n", total_hits);
				printf("Battle and time : %.2f seconds\n",battle_end_time);
				
				FILE *fp = fopen("battle_results.txt", "w");
				
				if (fp != NULL)
				{
					fprintf(fp, "---- PART 1-A RESULT ----\n");
					fprintf(fp, "Battleship Status : SURVIVED");
					fprintf(fp, "Number of E ships hit : %d\n",total_hits);
					
					for (int i = 0; i < num_escorts; i++)
					{
						if(escort_ships[i].is_destroyed == 1)
						{
							fprintf(fp,"E%d\n", escort_ships[i].id);
						}
					}
					fprintf(fp, "\nBattle End Time: %.2f seconds\n", battle_end_time);
					
					fclose(fp);
				}
			}
		}
	}
}

void save_final_conditions()
{
	FILE *fp = fopen("final_conditions.txt", "w");

	if (fp == NULL)
	{
		printf("Error creating final_conditions.txt\n");
		return;
	}
	fprintf(fp, "---- FINAL BATTLEFIELD CONDITIONS ----\n");
	fprintf(fp, "[BATTLESHIP]\n");
	fprintf(fp, "Position: (%.2f, %.2f)\n", battleship.x,battleship.y);
	if (battleship.is_destroyed == 1)
	{
		fprintf(fp, "Status : DESTROYED\n");
	}
	else{
		fprintf(fp, "Status : ALIVE\n");
	}
	fprintf(fp, "[ESCORT SHIPS]\n");

	for(int i = 0; i<num_escorts; i++)
	{
		if(escort_ships[i].is_destroyed == 1)
		{
			fprintf(fp, "E%d | Type: E%c | Status: DESTROYED\n", escort_ships[i].id,escort_ships[i].type_code);
		}
		else
		{
			fprintf(fp, "E%d | Type: E%c | Status: ALIVE\n", escort_ships[i].id,escort_ships[i].type_code);
		}
	}
	fclose(fp);

	printf("[SUCCESS] final_conditions.txt saved.\n");
}


int main()
{
    printf("Naval Battle Simulator Initialized. \n");
    srand(time(NULL));

    //call simulation initialization & file create
    init_simulation();

    //Run Simulation of part 1A Logic
    simulate_part1A();

    //save final conditionts file
    save_final_conditions();


    return 0;
}
