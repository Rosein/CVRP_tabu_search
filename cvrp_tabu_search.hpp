#ifndef CVRP_TABU_SEARCH_GUARD
#define CVRP_TABU_SEARCH_GUARD

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <thread>
#include <tuple>
#include <random>

#include <vector>
#include <matplot/matplot.h>

#include <chrono>
#include <thread>

#define ERROR_NUMBER -1
#define NUMBER_OF_NEIGHBORS 20
//potrzeba dokładnych danych
const double geo_degree_in_km = 111.1; 
const int numb_of_vehicles = 5;
const int max_vehicle_capacity = 1000;
const int max_tabu_size = 15;
const int cvrp_max_same_iterations_result = 50000;
const int tsp_max_same_iterations_result = 10000;
const int error_threshold = 25;
//typ dla miasta szerokosc/długosc geo
struct Coordinate{
    double width;
    double length;
};

struct City{
    Coordinate m_coordinate;
    std::string m_name;
    int m_demand_capacity;
};

using Route = std::vector<City>;
using Routes = std::vector<Route>;

namespace TSP{
    using Tabu = Routes;
    int counter{0};
};

namespace CVRP{
    using Tabu = std::vector<Routes>;
    using Route_City = std::pair<int,int>;
    int counter{0};
};

TSP::Tabu tsp_tabu_list {};
CVRP::Tabu cvrp_tabu_list {};

std::vector<City> cities = {
    { {50.0619474, 19.9368564}, "Krakow", 0 },
    { {53.132398,  23.1591679}, "Bialystok", 500 },
    { {49.822118,  19.0448936}, "Bie-Biala", 50 },
    { {50.1411926, 19.4028502}, "Chrzanow", 400 },
    { {54.347629,  18.6452324}, "Gdansk", 200 },
    { {54.5164982, 18.5402738}, "Gdynia", 100 },
    { {50.294113,  18.6657306}, "Gliwice", 40 },
    { {53.7361111, 15.9897222}, "Gromnik", 200 },
    { {50.2598987, 19.0215852}, "Katowice", 300 },
    { {50.8716657, 20.6314728}, "Kielce", 30 },
    { {49.6938045, 21.7651458}, "Krosno", 60 },
    { {53.0336093, 23.5880673}, "Krynica", 50 },
    { {51.250559,  22.5701022}, "Lublin", 60 },
    { {51.7687323, 19.4569911}, "Lodz", 160 },
    { {54.0359361, 19.0348817}, "Malbork", 100 },
    { {49.4818229, 20.0305410}, "Nowy Targ", 120 },
    { {53.7766839, 20.4765070}, "Olsztyn", 300 },
    { {52.4082663, 16.9335199}, "Poznan", 100 },
    { {51.4171713, 21.9720582}, "Pulawy", 200 },
    { {51.4022557, 21.1541546}, "Radom", 100 },
    { {50.0374531, 22.0047174}, "Rzeszow", 60 },
    { {50.6793066, 21.7495055}, "Sandomierz", 200 },
    { {53.4301818, 14.5509623}, "Szczecin", 150 },
    { {50.3090256, 21.0751610}, "Szczucin", 60 },
    { {50.8275855, 15.5211659}, "Szk Poreba", 50 },
    { {50.0123784, 20.9880739}, "Tarnow", 70 },
    { {52.2319581, 21.0067249}, "Warszawa", 200 },
    { {49.9823773, 20.0602114}, "Wieliczka", 90 },
    { {51.1089776, 17.0326689}, "Wroclaw", 40 },
    { {49.2969446, 19.9506590}, "Zakopane", 200 },
    { {50.7170854, 23.2525711}, "Zamosc", 30 }
};

double distance_between_cities( City& first_city, City& second_city);
bool is_near_enough_to(double suspect, double center, double measurement_error = 0.000001);
void distance_for_two_cities();
void draw_solution(Routes solution);
void display(Route route);
void display(Routes routes);
bool are_route_equal(Route first_route, Route second_route);
void swap_cities(City& first, City& second);

template<typename T>
void pop_first( T& vector );

namespace CVRP{
    std::pair<Route_City, Route_City> generate_indexes_for_city_swapping( Routes routes );
    Routes create_neighbor( Routes routes );
    std::vector<Routes> get_neighbors( Routes routes );
    Routes find_best_candidate(std::vector<Routes> neighbors);
    double fitness(Routes routes);
    bool is_already_in_neighbor( std::vector<Routes> neighbors, Routes candi );
    bool is_in_tabu_list(Routes routes);
    bool stopping_condition( Routes& sBest, Routes& prevSBest, int threshold );
    Routes tabu_search( Routes s0 );
    
    bool is_enough_space_in_vehicle( Route route );
    bool is_enough_space_in_vehicles( Routes routes );
    City cut_random_city( std::vector<City>& clients_cities );
    int get_index_of_route_with_enough_capacity( Routes routes, int city_capacity_demand );
    Routes build_init_solution( std::vector<City> clients_cities );
    std::pair<Route_City, Route_City> generate_indexes_for_city_swapping(Routes routes);
};

namespace TSP{
    std::pair<int,int> generate_cities_to_swap(Route route);
    Route create_neighbor(Route route);
    Routes get_neighbors(Route route);
    Route find_best_candidate(Routes neighbors);
    double fitness(Route route);
    bool is_already_in_neighbor(Routes neighbors, Route candi );
    bool is_in_tabu_list(Route route);
    bool stopping_condition( Route& sBest, Route& prevSBest, int threshold );
    Route tabu_search( Route s0 );
};
#endif //CVRP_TABU_SEARCH_GUARD