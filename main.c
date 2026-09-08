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
	float time_to_hit;
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
	
	//for add path
	float path_x[50];
	float path_y[50];
}Battleship;

//Global Variables
float canvas_D = 1000.0f;
int num_escorts = 10;
Battleship battleship;
EscortShip escort_ships[MAX_ESCORT_SHIPS];
int k;

Battleship starting_battleship;
EscortShip starting_escorts[MAX_ESCORT_SHIPS];
int starting_num_escorts;

int escort_attacked[MAX_ESCORT_SHIPS];
int setup_done = 0;
float battleship_damage = 0.0f;

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

//calculate maximum range of battleship
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

void save_starting_data()
{
	starting_battleship = battleship;
	starting_num_escorts = num_escorts;

	for (int i =0; i < num_escorts; i++)
	{
		starting_escorts[i] = escort_ships[i];
	}
}

void reset_battlefield()
{
	battleship = starting_battleship;
	num_escorts = starting_num_escorts;

	for(int i = 0; i < num_escorts; i++)
	{
		escort_ships[i] = starting_escorts[i];
		escort_attacked[i] = 0;
		battleship_damage = 0.0f;
	}
}

//find the maximum range for E ships
float get_escort_max_range(EscortShip escort)
{
	float range1,range2,max_range;

	range1 = calculate_range(escort.max_v, escort.min_ang);
	range2 = calculate_range(escort.max_v, escort.max_ang);

	max_range = range1;

	if(range2>max_range)
	{
		max_range = range2;
	}

	if(escort.min_ang <= 45.0f && escort.max_ang <= 45.0f)
	{
		max_range = calculate_range(escort.max_v, 45.0f);
	}
	return max_range;
}

float get_battleship_range_after_jam(float min_ang)
{
	if (min_ang <= 45.0f)
	{
		return calculate_range(battleship.v_max, 45.0f);
	}
	return calculate_range(battleship.v_max, min_ang);
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

	printf("Enter number of path points for Battleship(1-50) : ");
	scanf("%d", &k);

	if (k>50)
	{
		k = 50;
	}
	for (int i = 0; i < k; i++)
	{
		battleship.path_x[i] = ((float)rand() / RAND_MAX) * canvas_D;
		battleship.path_y[i] = ((float)rand() / RAND_MAX) * canvas_D;
	}


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
	fprintf(fp, "[BATTLESHIP PATH]\n");

	for(int i = 0; i < k ; i++)
	{
		fprintf(fp,"Point %d : (%.2f, %.2f)\n", i+1, battleship.path_x[i], battleship.path_y[i]);
	}
	fprintf(fp, "\n");


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

	save_starting_data();
	setup_done = 1;

	printf("\n[SUCCESS] Battlefield setup completed & initial_config.txt saved!\n");
}

void simulate_part1A()
{
	int sinking_ship_id = 0;
	int total_hits = 0;
	float battle_end_time = 0.0f;

	printf("\n---- BATTLE SIMULATION ----\n");

	for (int point = 0; point <k; point++)
	{
		battleship.x = battleship.path_x[point];
		battleship.y = battleship.path_y[point];

		printf("--SIMULATION 1 - POINT %d\n--", point + 1);
		printf("Battleship Position : (%.2f, %.2f)\n",battleship.x,battleship.y);
	}

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
			fprintf(fp, "BattleShip Status : DESTROYED\n");
			fprintf(fp, "E ship that sank B : E%d\n", sinking_ship_id);

			fclose(fp);
		}
		else
		{
			printf("Error creating battle_log.txt\n");
		}
		return ;
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
				
				escort_ships[i].time_to_hit = (2.0f * velocity * sinf(deg_to_rad(angle)))/GRAVITY;
				
				if(escort_ships[i].time_to_hit > battle_end_time)
				{
					battle_end_time = escort_ships[i].time_to_hit;
				}
				printf("\nNumber of E ships hit : %d\n", total_hits);
				printf("E%d was hit by Battleship.\n",escort_ships[i].id);
				printf("Time to hit E%d : %.2f seconds\n",escort_ships[i].id,escort_ships[i].time_to_hit);
			}
		}
		printf("\n NUmber of Escort ships hit : %d\n", total_hits);
		printf("Battle End Time : %.2f seconds", battle_end_time);

				// Save battleship surivived result
				FILE *fp = fopen("battle_log.txt", "w");
				
				if (fp != NULL)
				{
					fprintf(fp, "---- PART 1-A RESULT ----\n");
					fprintf(fp, "Battleship Status : SURVIVED\n");
					fprintf(fp, "Number of E ships hit : %d\n",total_hits);
					
					for (int i = 0; i < num_escorts; i++)
					{
						if(escort_ships[i].is_destroyed == 1)
						{
							fprintf(fp,"E%d | Time to hit : %.2f \n", escort_ships[i].id,escort_ships[i].time_to_hit);
						}
					}
					fprintf(fp, "\nBattle End Time: %.2f seconds\n", battle_end_time);
					
					fclose(fp);
				}
				else 
				{
					printf("Error creating battle_log.txt\n");
				}
	}
}

void simulate_path()
{
	printf("\n----- BATTLESHIP PATH SIMULATION -----\n");

	for (int point = 0; point < k; point++)
	{
		battleship.x = battleship.path_x[point];
		battleship.y = battleship.path_y[point];

		printf("Point %d : Battleship Position = ()%.2f, %.2f)\n", point +1 ,battleship.x,battleship.y);
	}
}

void simulate_part1B(int use_jam, int jam_after, float jam_ang, char file_name[])
{
	FILE *fp;
	int total_hits = 0;

	if (setup_done == 0)
	{
		printf("\nPlease setup the battlefield first.\n");
		return;
	}

	reset_battlefield();

	fp = fopen(file_name, "w");

	if (fp == NULL)
	{
		printf("Error creating part1B_log.txt file\n");
		return;
	}

	fprintf(fp, "---- PART 1_B SIMULATION ! ---- \n");

	for(int point = 0; point < k; point++)
	{
		float b_range;
		int gun_jammed = 0;

		printf("\n---- Path Points %d ----\n", point+1);

		battleship.x = battleship.path_x[point];
		battleship.y = battleship.path_y[point];

		b_range = calculate_battleship_range();

		if (use_jam ==1 && point + 1 > jam_after)
		{
			gun_jammed = 1;
			b_range = get_battleship_range_after_jam(jam_ang);
			printf("Gun is Jammed from this point.\n");
			
			fprintf(fp, "Gun Status: Jammed | Allowed Angle: %.2f to 90\n", jam_ang);
		}
		else
		{
			fprintf(fp, "Gun Status : Normal\n");
		}

		fprintf(fp, "\nPath points %d\n", point+1);
		fprintf(fp, "B Position: (%.2f, %.2f)\n",battleship.x,battleship.y);



		// E ships Battleship
		for (int i = 0; i < num_escorts; i++)
        {
            float distance;
            float min_range;
            float max_range;

            if (escort_ships[i].is_destroyed == 1)
            {
                continue;
            }

            if (escort_attacked[i] == 1)
            {
                continue;
            }

            distance = calculate_distance( battleship.x, battleship.y, escort_ships[i].x, escort_ships[i].y);
            min_range = calculate_range( escort_ships[i].min_v, escort_ships[i].min_ang );
            max_range = get_escort_max_range(escort_ships[i]);

            if (distance >= min_range && distance <= max_range)
            {
                battleship.is_destroyed = 1;
                escort_attacked[i] = 1;

                printf("B was destroyed by E%d\n", escort_ships[i].id);
                fprintf(fp, "B was destroyed by E%d\n", escort_ships[i].id);

                break;
            }
        }

        // Stop path simulation if B is destroyed 
        if (battleship.is_destroyed == 1)
        {
            fprintf(fp, "Simulation stopped at path point %d\n",
                    point + 1);
            break;
        }

        // B attacks escorts inside its range
        for (int i = 0; i < num_escorts; i++)
        {
            float distance;
            float b_range;

            if (escort_ships[i].is_destroyed == 1)
            {
                continue;
            }

            distance = calculate_distance( battleship.x, battleship.y, escort_ships[i].x, escort_ships[i].y);

            if (distance <= b_range)
            {
                escort_ships[i].is_destroyed = 1;
                escort_ships[i].time_to_hit = (2.0f * battleship.v_max * sinf(deg_to_rad(45.0f))) / GRAVITY;
                total_hits++;

                printf("B destroyed E%d\n", escort_ships[i].id);

                fprintf(fp, "B destroyed E%d | Time: %.2f seconds\n", escort_ships[i].id, escort_ships[i].time_to_hit );
            }
        }
        fprintf(fp, "Destroyed escorts so far: %d\n", total_hits);
    }

    fprintf(fp, "\n---- FINAL RESULT ----\n");

    if (battleship.is_destroyed == 1)
    {
        fprintf(fp, "B Status: DESTROYED\n");
    }
    else
    {
        fprintf(fp, "B Status: ALIVE\n");
    }

    for (int i = 0; i < num_escorts; i++)
    {
        if (escort_ships[i].is_destroyed == 1)
        {
            fprintf(fp, "E%d: DESTROYED\n",
                    escort_ships[i].id);
        }
        else
        {
            fprintf(fp, "E%d: ALIVE\n",
                    escort_ships[i].id);
        }
    }

    fclose(fp);
    printf("\n[SUCCESS] Part 1-B result saved to %s\n",file_name);
}

void run_part1B_jammed_gun()
{
	int jam_after;
	float jam_ang;
	
	if (setup_done == 0)
	{
		printf("Please setup the battlefield first.\n");
		return;
	}

	if (k < 2)
	{
		printf("You need at least 2 path points.\n");
		return;
	}

	printf("Gun jams after iteration (1 to %d): ", k-1);
	scanf("%d", &jam_after);

	while (jam_after < 1 || jam_after >= k)
	{
		printf("Enter a valid iteration: ");
		scanf("%d", &jam_after);
	}

	printf("Minimum firing angle after jam(1-29): ");
	scanf("%f", &jam_ang);

	while (jam_ang <= 0 || jam_ang >= 30)
	{
		printf("Enter an angle from 1- 29 : ");
		scanf("%f", &jam_ang);
	}
	simulate_part1B(1,jam_after,jam_ang,"part1B_jammed_gun_log.txt");
}

void simulate_part1C_single_pos()
{
	FILE *fp;
	int total_hits = 0;

	if (setup_done == 0)
	{
		printf("Please Set up battlefield.\n");
		return;
	}

	reset_battlefield();

	fp = fopen ("part1C_singlep_log.txt","w");

	if (fp == NULL)
	{
		printf("Error creating part 1-c  file\n");
		return;
	}

	fprintf(fp, "---- PART 1-C SINGLE SIMULATION ----\n");

	//E ships attack B once
    for (int i = 0; i < num_escorts; i++)
    {
	    float distance;
	    float min_range;
	    float max_range;
	   
	    distance = calculate_distance(battleship.x, battleship.y, escort_ships[i].x, escort_ships[i].y);
	    min_range = calculate_range(escort_ships[i].min_v,escort_ships[i].min_ang);
	    max_range = get_escort_max_range(escort_ships[i]);
	    
	    if (distance >= min_range && distance <= max_range)
	    {
		    battleship_damage = battleship_damage + escort_ships[i].impact_power;
		    
		    printf("E%d hit B. Total damage: %.2f\n",escort_ships[i].id, battleship_damage);
		    
		    fprintf(fp,"E%d hit B | Impact: %.2f | Total damage: %.2f\n",escort_ships[i].id,escort_ships[i].impact_power,battleship_damage);
		    
		    if (battleship_damage >= 1.0f)
		    {
			     battleship.is_destroyed = 1;
			    fprintf(fp, "B was destroyed.\n");
			     break;
		    }
	    }
    }

    // B attacks only if it survived
    if (battleship.is_destroyed == 0)
    {
        float b_range = calculate_battleship_range();

        for (int i = 0; i < num_escorts; i++)
        {
            float distance;

            distance = calculate_distance(battleship.x, battleship.y,escort_ships[i].x, escort_ships[i].y);

            if (distance <= b_range)
            {
                escort_ships[i].is_destroyed = 1;
                total_hits++;

                escort_ships[i].time_to_hit = (2.0f * battleship.v_max * sinf(deg_to_rad(45.0f))) / GRAVITY;

                fprintf(fp,"B destroyed E%d | Time: %.2f seconds\n",escort_ships[i].id,escort_ships[i].time_to_hit);
               
            }
        }
    }

    fprintf(fp, "\n---- FINAL RESULT ----\n");
    fprintf(fp, "Total B damage: %.2f\n", battleship_damage);
    fprintf(fp, "Destroyed escorts: %d\n", total_hits);

    if (battleship.is_destroyed == 1)
    {
        fprintf(fp, "B Status: DESTROYED\n");
    }
    else
    {
        fprintf(fp, "B Status: ALIVE\n");
    }

    fclose(fp);

    printf("\n[SUCCESS] Saved to part1C_single_log.txt\n");
}

void simulate_part1C_path()
{
	FILE *fp;
	int total_hits = 0;

	if (setup_done == 0)
	{
		printf("Please Setup the Battlefield.\n");
		return;
	}

	reset_battlefield();

	fp = fopen("part1C_path_log.txt", "w");
	if (fp == NULL)
	{
		printf("Error creating Part 1C path file\n");
		return;
	}

	fprintf(fp, "---- PART 1-C PATH SIMULATION ----\n");

	for (int point = 0; point < k ; point++)
	{
		battleship.x = battleship.path_x[point];
		battleship.y = battleship.path_y[point];

		printf("\n --- PATH POINT %d ---\n", point +1);

		fprintf(fp,"\nPath Point %d\n", point + 1);
		fprintf(fp, "B Position: (%.2f, %.2f)\n", battleship.x, battleship.y);

		//E ships attack B once
		for (int i = 0; i < num_escorts; i++)
		{
			float distance;
			float min_range;
			float max_range;

			if (escort_ships[i].is_destroyed == 1)
			{
				continue;
			}
			if (escort_attacked[i] == 1)
			{
				continue;
			}

			distance = calculate_distance(battleship.x, battleship.y, escort_ships[i].x, escort_ships[i].y);
			min_range = calculate_range(escort_ships[i].min_v, escort_ships[i].min_ang);
			max_range = get_escort_max_range(escort_ships[i]);

			if (distance >= min_range && distance <= max_range)
			{
				escort_attacked[i] = 1;
				battleship_damage = battleship_damage + escort_ships[i].impact_power;

				printf("E%d hit Battleship damage : %.2f\n",escort_ships[i].id,battleship_damage);
				fprintf(fp,"e%d hit B | Total damage : %.2f\n", escort_ships[i].id, battleship_damage);

				if(battleship_damage >= 1.0f)
				{
					battleship.is_destroyed = 1;
					fprintf(fp, "Battleship was DESTROYED\n");
					break;
				}
			}
		}

		//stop when B is destroyed
		if (battleship.is_destroyed ==1)
		{
			fprintf(fp,"Simulation stopped at path point %d.\n", point+1);
			break;
		}

		//b attacks active escorts
		for (int i = 0; i < num_escorts; i++)
		{
			float distance;
			float b_range;

			if(escort_ships[i].is_destroyed == 1)
			{
				continue;

			distance = calculate_distance(battleship.x,battleship.y,escort_ships[i].x,escort_ships[i].y);

			b_range = calculate_battleship_range();

			if (distance <= b_range)
			{
				escort_ships[i].is_destroyed = 1;
				total_hits++;

				escort_ships[i].time_to_hit = (2.0f * battleship.v_max * sinh(deg_to_rad(45.0f)))/GRAVITY;
				
				printf("B destroyed E%d\n", escort_ships[i].id);
				fprintf(fp, "B destroyed E%d | Time : %.2f seconds \n",escort_ships[i].id,escort_ships[i].time_to_hit);
			}
			}
			fprintf(fp,"B damage so far: %.2f\n",battleship_damage);
			fprintf(fp,"Destroyed escorts so far: %d \n", total_hits);
		}
		fprintf(fp,"\n----- FINAL RESULT -----");
		fprintf(fp,"Total B damage : %.2f\n", battleship_damage);
		fprintf(fp,"Destroyed escorts : %d \n", total_hits);

		if (battleship.is_destroyed == 1)
		{
			fprintf(fp,"B Status : DESTROYED\n");
		}
		else 
		{
			fprintf(fp, "B Status : ALIVE\n");
		}
		fclose(fp);
		printf("\n[SUCCESS] Saved to part1C_path_log.txt\n");
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
	fprintf(fp,"Type name: %s\n",battleship.type_name);
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
	int choice; // for add menu mode
       	
	srand(time(NULL));

	do
	{
		printf("\n ---- NAVAL BATTLE SIMULATION ----\n");
		printf("01. Setup Battlefield\n");
		printf("02. Run Part 1-A\n");
		printf("03. Run Part 1-B Simulation 1\n");
		printf("04. Run Part 1-B Simulation 2\n");
		printf("05. Run Part 1-C Single Position Damage Model\n");
		printf("06. Run Part 1-C Path Damage Model\n");
		printf("07. Exit\n");
		printf("Enter Your Choice: ");

		scanf("%d", &choice);

		switch(choice)
		{
			case 1:
				init_simulation();
				break;
			case 2:
				if(setup_done == 0)
				{
					printf("Please Set up First\n");
				}
				else
				{
					reset_battlefield();
					simulate_part1A();
					save_final_conditions();
				}
				break;
			case 3:
				simulate_part1B(0,0,0.0f,"part1B_log.txt");
				break;
			case 4:
				run_part1B_jammed_gun();
				break;
			case 5:
				simulate_part1C_single_pos();
				break;
			case 6:
				simulate_part1C_path();
				break;
			case 7:
				printf("Program closed.\n");
				break;
			default:
				printf("Invalid Choice...Please select the another number\n");
		}
	}while(choice != 7);
	return 0;
}
