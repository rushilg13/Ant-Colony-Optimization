#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
using namespace std;


int getRand(int min, int max) {
	return rand() % (max + 1 - min) + min;
}

struct Ant_params
{
	int available_cities_count, current_city, current_city_index, path_index;
	float total_cost, best_cost;
};

struct Ant
{
	float* local_pheromone;
	int* path, * best_path;
	int* available_cities;
	struct Ant_params params;
	int firstCity;
};

struct ACO_params
{
	int cities_count;
	int generations, ant_count, q_cnst_quAnt_phero;
	float alpha, beta, p_evp_constAnt;
};

struct ACO
{
	int* cities;
	float* eta, * pheromone;
	struct ACO_params params;
};



// remove the selected city from the remaining cities by pushing it at the end
void remove_current_from_available_city(Ant& atv)
{
	int temp = atv.available_cities[atv.params.available_cities_count];
	atv.available_cities[atv.params.available_cities_count] = atv.available_cities[atv.params.current_city_index];
	atv.available_cities[atv.params.current_city_index] = temp;
	atv.params.available_cities_count--;
}


// select the first city (starting city)
void select_city_first(Ant& atv, int city_num)
{
	atv.params.current_city = city_num;
	atv.params.current_city_index = atv.params.current_city;
	remove_current_from_available_city(atv);
	atv.path[atv.params.path_index] = atv.params.current_city;
	atv.params.path_index++;
}


void initializeAntOnce(Ant& atv, int cities_count)
{
	atv.available_cities = new int[cities_count];
	atv.local_pheromone = new float[cities_count];
	atv.path = new int[cities_count];
	atv.best_path = new int[cities_count];
}

void initializeAnt(Ant& atv, int cities_count)
{
	atv.params.path_index = 0;
	atv.params.total_cost = 0;
	atv.params.best_cost = -1;
	atv.params.available_cities_count = cities_count - 1;
	atv.params.path_index = 0;
	for (int i = 0; i < cities_count; i++)
	{
		atv.available_cities[i] = i;
		atv.local_pheromone[i] = -1;
	}
	select_city_first(atv, 0);
	atv.firstCity = 0;
}

void set_best_path(Ant& atv, int cities_count)
{
	if (atv.params.total_cost < atv.params.best_cost || atv.params.best_cost == -1)
	{
		atv.params.best_cost = atv.params.total_cost;
		for (int i = 0; i < cities_count; i++)
		{
			atv.best_path[i] = atv.path[i];
		}
	}
}

void print_best_path(Ant& atv, ACO& acv)
{
	int cities_count = acv.params.cities_count;
	cout << "Best Found Path: ";
	for (int i = 0; i < cities_count; i++)
	{
		cout << atv.best_path[i] << "-->";
	}
	cout << atv.firstCity;

	cout << endl << "Best total Cost: " << atv.params.best_cost << endl;

	cout << "Costs: ";
	int j;
	for (int k = 1; k < cities_count; k++)
	{
		int i = atv.best_path[k - 1];
		j = atv.best_path[k];
		cout << acv.cities[cities_count * i + j] << ",";
	}
	
	cout << acv.cities[cities_count * j + atv.firstCity] << endl;
}

void select_next_city(Ant& atv, ACO& acv)
{
	float pr_denomntr = 0;
	int cities_count = acv.params.cities_count;
	int current_city = atv.params.current_city;

	// find denominator summation of
	for (int s = 0; s <= atv.params.available_cities_count; s++)
	{
		// (pheromone^alpha) * (eta^beta)
		pr_denomntr += (pow(acv.pheromone[cities_count * current_city + s], acv.params.alpha) * pow(acv.eta[cities_count * current_city + s], acv.params.beta));
	}


	// compute probabilities of choosing each city
	float max_probability = 0;
	int city_num, temp_current_city_index;
	float probability_city = 0.0f;
	for (int i = 0; i <= atv.params.available_cities_count; i++)
	{
		int i_c = atv.available_cities[i];
		// (pheromone^alpha) * (eta^beta)
		float pr_numer = (pow(acv.pheromone[cities_count * current_city + i_c], acv.params.alpha) * pow(acv.eta[cities_count * current_city + i_c], acv.params.beta));
		if (pr_denomntr == 0)
			probability_city = 0;
		else
			probability_city = pr_numer / pr_denomntr;

		// if this city has maximum probability then choose this one
		if (probability_city >= max_probability)
		{
			max_probability = probability_city;
			city_num = i_c;
			temp_current_city_index = i;
		}

	}

	// update the path and total cost
	atv.params.total_cost += acv.cities[cities_count * current_city + city_num];
	atv.path[atv.params.path_index] = city_num;
	atv.params.path_index++;
	atv.params.current_city = city_num;
	atv.params.current_city_index = temp_current_city_index;
	remove_current_from_available_city(atv);
}


void update__local_pheromone(Ant& atv, int cities_count)
{
	// update pheromone according to last choosen city
	for (int k = 1; k < cities_count; k++)
	{
		int i = atv.path[k - 1];
		int j = atv.path[k];
		atv.local_pheromone[i] = j;
	}
}


void deleteAnt(Ant& atv)
{
	delete[] atv.local_pheromone;
	delete[] atv.available_cities;
	delete[] atv.path;
	delete[] atv.best_path;
}


void acoInitialize(ACO& acv, const char* filePath, int ant_count = 10, int generations = 100, float alpha = 1.0,
	float beta = 10, float p_evp_constAnt = 0.5, int q_cnst_quAnt_phero = 10)
{
	ifstream fin(filePath);
	fin >> acv.params.cities_count;

	int cities_count = acv.params.cities_count;
	acv.cities = new int[cities_count * cities_count];
	acv.pheromone = new float[cities_count * cities_count];

	int temp = 0;


	// initial pheromone value
	float pheromone_value = 1.0 / (cities_count * cities_count);
	for (int i = 0; i < cities_count; i++)
	{
		for (int j = 0; j < cities_count; j++)
		{
			acv.pheromone[cities_count * i + j] = pheromone_value;

			// read edge distance from the input file
			fin >> temp;
			acv.cities[cities_count * i + j] = temp;
		}
	}
	fin.close();


	// setting up parameter values
	acv.params.alpha = alpha;
	acv.params.beta = beta;
	acv.params.generations = generations;
	acv.params.ant_count = cities_count;
	acv.params.p_evp_constAnt = p_evp_constAnt;
	acv.params.q_cnst_quAnt_phero = q_cnst_quAnt_phero;
	acv.eta = new float[cities_count * cities_count];


	// setting etta for each edge
	for (int i = 0; i < cities_count; i++)
	{
		for (int j = 0; j < cities_count; j++)
		{
			if (i == j)
				acv.eta[cities_count * i + j] = 0;
			else
			{
				// eta = 1 / edge cost
				acv.eta[cities_count * i + j] = 1.0 / acv.cities[cities_count * i + j];
			}
		}
	}

}

void print_cities(ACO& acv)
{
	for (int i = 0; i < acv.params.cities_count; i++)
	{
		for (int j = 0; j < acv.params.cities_count; j++)
		{
			cout << acv.cities[acv.params.cities_count * i + j] << ",";
		}
		cout << endl;
	}
}

void print_pheromone(ACO& acv)
{
	for (int i = 0; i < acv.params.cities_count; i++)
	{
		for (int j = 0; j < acv.params.cities_count; j++)
		{
			cout << acv.pheromone[acv.params.cities_count * i + j] << ",";
		}
		cout << endl;
	}
}

void update_global_pheromone(ACO& acv, Ant* ants)
{
	// update global phermone based on the local pheromone
	int cities_count = acv.params.cities_count;
	int q_cnst_quAnt_phero = acv.params.q_cnst_quAnt_phero;
	for (int i = 0; i < cities_count; i++)
	{
		for (int j = 0; j < cities_count; j++)
		{
			acv.pheromone[cities_count * i + j] *= acv.params.p_evp_constAnt;
			for (int a = 0; a < acv.params.ant_count; a++)
			{
				// if ant choosen this path then add to the global pheromone
				if (ants[a].local_pheromone[i] == j)
					acv.pheromone[cities_count * i + j] += q_cnst_quAnt_phero;
			}
		}
	}
}

void execute_aco(ACO& acv)
{
	int ant_count = acv.params.ant_count;
	int cities_count = acv.params.cities_count;
	Ant* ants = new Ant[ant_count];
	for (int a = 0; a < ant_count; a++)
	{
		initializeAntOnce(ants[a], cities_count);
	}

	// continue for number of generation
	for (int g = 0; g < acv.params.generations; g++)
	{

		for (int a = 0; a < ant_count; a++)
		{
			initializeAnt(ants[a], cities_count);

			// keep selecting next city unless all city are choosen
			for (int c = 1; c < cities_count; c++)
			{
				select_next_city(ants[a], acv);
			}
			ants[a].params.total_cost += acv.cities[cities_count * ants[a].path[cities_count - 1] + ants[a].firstCity];
			update__local_pheromone(ants[a], cities_count);
			// check if new found path is the best path
			set_best_path(ants[a], cities_count);
		}

		update_global_pheromone(acv, ants);
	}


	// check which ant follows the path with minimum cost
	int best_ant_index = 0;
	float best_cost = ants[0].params.best_cost;
	for (int i = 1; i < ant_count; i++)
	{
		float cost_ant = ants[i].params.best_cost;
		if (cost_ant < best_cost)
		{
			best_ant_index = i;
			best_cost = cost_ant;
		}
	}

	// print the path of best ant
	print_best_path(ants[best_ant_index], acv);

	for (int i = 0; i < ant_count; i++)
	{
		deleteAnt(ants[i]);
	}
	delete[] ants;
}

void deleteAco(ACO& acv)
{
	delete[] acv.cities;
	delete[] acv.pheromone;
	delete[] acv.eta;
}

int main(int argc, char const* argv[])
{
	srand(time(NULL));

	const char *filePath = "inp60.txt";
	if (argc > 1)
	{
		filePath = argv[1];
	}
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	ACO aco_system;

	// initilize the values
	acoInitialize(aco_system, filePath);

	// print_pheromone(aco_system);

	execute_aco(aco_system);

	deleteAco(aco_system);
	clock_gettime(CLOCK_MONOTONIC, &finish);

	double timeElapsed = (finish.tv_sec - start.tv_sec);
	timeElapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	printf("Time elapsed Ant Colony= %lf secs\n", timeElapsed);

	return 0;
}
