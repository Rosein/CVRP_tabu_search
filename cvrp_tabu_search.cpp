#include "cvrp_tabu_search.hpp"

//funkcja do obliczania odległosci dla miast
double distance_between_cities( City& first_city, City& second_city){
    return geo_degree_in_km * ( sqrt( pow(first_city.m_coordinate.length - second_city.m_coordinate.length, 2) +
                                      pow(first_city.m_coordinate.width  - second_city.m_coordinate.width,  2)  ) );
}

// sprawdzenie czy liczba nalezy do zakresu (bo double nie sa dokładne wiec trzeba zakres)
bool is_near_enough_to(double suspect, double center, double measurement_error ){
    return suspect >= center - measurement_error && suspect <= center + measurement_error;
}

void display(Route route){
    const int word_width = 10;
    std::cout << "{ " << std::setw( 7 ) <<  TSP::fitness(route) << " }";
    for (int i = 0; i < route.size(); i++){
        std::cout << std::setw( word_width ) <<  route[ i ].m_name << " ";
    }
    std::cout << std::endl;
}

void display(Routes routes)
{
    int sum = 0;
    for( int i = 0; i < routes.size(); ++i )
    {
        std::cout << "v[" << i << "]";
        sum += TSP::fitness(routes[i]);
        display( routes[i] );
    }
    std::cout << "Total distance: " << sum << '\t' << CVRP::counter << std::endl;
}

void draw_solution(Routes solution, int color)
{
    using namespace matplot;
    for( auto& route : solution )
    {
        std::vector<double> x;
        std::vector<double> y;
        std::vector<std::string> tag;

        for( auto& city : route )
        {
            x.push_back( city.m_coordinate.length );
            y.push_back( city.m_coordinate.width );
            tag.push_back( city.m_name );
        }

        std::vector<double> cols (route.size(), color);
        color += 25;
        geoplot(y,x)->marker("o").marker_colors(cols);
        text(x,y,tag);
    }
    show();
}


void swap_cities( City& first, City& second ){
    City temp = first;
    first = second;
    second = temp;
}

bool are_route_equal( Route first_route, Route second_route ){
    if( first_route.size() != second_route.size() )
        return false;
    for (int i=0; i < first_route.size(); i++){
        if(first_route[i].m_name != second_route[i].m_name){
            return false;
        }
    }
    return true;
}

bool are_routes_equal( Routes first_routes, Routes second_routes ){
    if( first_routes.size() != second_routes.size() )
        return false;
    for (int i=0; i < first_routes.size(); i++){
        if( are_route_equal( first_routes[ i ], second_routes[ i ] ) )
            return false;
    }
    return true;
}

namespace TSP{

std::pair<int,int> generate_cities_to_swap(Route route){

    const int FIRST_CITY = 1;
    const int LAST_CITY = 2;

    int first_city = FIRST_CITY + std::rand() % ( route.size() - LAST_CITY );
    int second_city = FIRST_CITY + std::rand() % ( route.size() - LAST_CITY );
    while(first_city == second_city){
        second_city = FIRST_CITY + std::rand() % ( route.size() - LAST_CITY );
    }
    return {first_city, second_city};
}

Route create_neighbor(Route route){
    Route new_route(route);
    std::pair<int,int> cities_to_swap = generate_cities_to_swap(route);
    swap_cities(new_route[ cities_to_swap.first ], new_route[ cities_to_swap.second ]);
    return new_route;
}

bool is_already_in_neighbor(Routes neighbors, Route candi ){
    for(int i=0; i< neighbors.size(); i++){
        if( are_route_equal(neighbors[i], candi) )
            return true;
    }
    return false;
}

Routes get_neighbors(Route route){

    const int first_city = 1;
    const int last_city = 1;
    const int ignored_cities = first_city + last_city;
    const int current_route = 1;
    Routes neighbors;
    int number_actually_needed_neighbors = ( route.size() - ignored_cities > NUMBER_OF_NEIGHBORS ) ? NUMBER_OF_NEIGHBORS : route.size() - ignored_cities - current_route;

    for(int i=0; i < number_actually_needed_neighbors; i++){
        Route new_neighbor = create_neighbor(route);
        if(!is_already_in_neighbor( neighbors, new_neighbor ))
            neighbors.push_back( new_neighbor );
        else
            --i;
    }
    return neighbors;
}

double fitness(Route route){
    double route_length = 0;
    for (int i = 0; i < route.size() - 1; i++)
        route_length += distance_between_cities( route[i], route[i + 1] );
    return route_length;
}

Route find_best_candidate(Routes neighbors){

    Route best_candidate = neighbors[0];

    for (int i = 0; i < neighbors.size(); i++){
        // display(neighbors[i]);
        if( !is_in_tabu_list(neighbors[i]) && fitness(neighbors[i]) <= fitness(best_candidate) ){
            best_candidate = neighbors[i];
        }
    }
    return best_candidate;
}

void remove_first_route_from_tabu_list(){
    std::reverse(std::begin(tsp_tabu_list), std::end(tsp_tabu_list));
    tsp_tabu_list.pop_back();
    std::reverse(std::begin(tsp_tabu_list), std::end(tsp_tabu_list));
}


bool stopping_condition( Route& sBest, Route& prevSBest, int threshold ){
    if( is_near_enough_to(fitness(sBest), fitness(prevSBest), 1 ) )
        counter++;
    else
    {   
        prevSBest = sBest;
        counter = 0;
    }

    return counter <= threshold;
}

bool is_in_tabu_list(Route route){
    for(int i=0; i < tsp_tabu_list.size(); i++){
        if(are_route_equal(tsp_tabu_list[i], route)){
            return true;
        }
    }
    return false;
}

Route tabu_search( Route s0 ){
    Route sBest {s0};
    Route bestCandidate {s0};
    tsp_tabu_list = {};
    tsp_tabu_list.push_back(s0);
    Route prevSBest{ sBest };

    while( stopping_condition( sBest, prevSBest, 10000 ) ){
        Routes neighbors = get_neighbors(bestCandidate);
        bestCandidate = find_best_candidate(neighbors);

        if(fitness(bestCandidate) < fitness(sBest)){
            sBest = bestCandidate;
        }
        tsp_tabu_list.push_back(bestCandidate);

        if(tsp_tabu_list.size() > max_tabu_size){
            remove_first_route_from_tabu_list();
        }
    }
    // display(sBest);

    return sBest;
}

};

namespace CVRP {

std::pair<Route_City, Route_City> generate_routes_with_swapped_cities(Routes routes){

    if( routes.size() == 1 )
        throw std::logic_error( "Too few routes." );

    int first_route = std::rand() % ( routes.size() );
    int second_route = std::rand() % ( routes.size() );
    while(first_route == second_route){
        second_route = std::rand() % ( routes.size() );
    }

    const int FIRST_CITY = 1;
    const int LAST_CITY = 2;

    int first_city = FIRST_CITY + std::rand() % ( routes[ first_route ].size() - LAST_CITY );
    int second_city = FIRST_CITY + std::rand() % ( routes[ second_route ].size() - LAST_CITY );

    return { { first_route, first_city }, { second_route, second_city} };
}

Routes create_neighbor(Routes routes){
    Routes new_route(routes);
    std::pair<Route_City,Route_City> cities_to_swap = generate_routes_with_swapped_cities(routes);
    swap_cities(new_route[ cities_to_swap.first.first  ] [ cities_to_swap.first.second ], 
                new_route[ cities_to_swap.second.first ] [ cities_to_swap.second.second ] );
    return new_route;
}


bool is_already_in_neighbor(std::vector<Routes> neighbors, Routes candi ){
    for(int i=0; i< neighbors.size(); i++){
        if( are_routes_equal(neighbors[i], candi) )
            return true;
    }
    return false;
}

bool is_enough_space_in_vehicle( Route route )
{
    int current_load = 0;
    for( auto& city : route )
        current_load += city.m_demand_capacity;

    return current_load <= max_vehicle_capacity;
}

bool is_enough_space_in_vehicles( Routes routes )
{
    for( auto& route : routes )
    {
        if( !is_enough_space_in_vehicle( route ) )
            return false;
    }
    return true;
}

std::vector<Routes> get_neighbors(Routes routes){

    std::vector<Routes> neighbors;
    int number_actually_needed_neighbors =  NUMBER_OF_NEIGHBORS;
    for(int i=0; i < number_actually_needed_neighbors; i++){
        Routes new_neighbor = create_neighbor(routes);
        int shuffle_times = std::rand() % 3;
        while( !is_enough_space_in_vehicles( new_neighbor ) )
        {
            new_neighbor = create_neighbor( routes );
            for(int i = 0; i < shuffle_times; ++i )
                new_neighbor = create_neighbor( new_neighbor );
        }
        if( !is_already_in_neighbor( neighbors, new_neighbor ) )
            neighbors.push_back( new_neighbor );
        else
            --i;
    }


    return neighbors;
}

void remove_first_routes_from_tabu_list(){
    std::reverse(std::begin(cvrp_tabu_list), std::end(cvrp_tabu_list));
    cvrp_tabu_list.pop_back();
    std::reverse(std::begin(cvrp_tabu_list), std::end(cvrp_tabu_list));
}

Routes build_init_solution()
{
    Routes solution( numb_of_vehicles, Route(1, cities[0]) );
    for( auto& route : solution)
    {
        int current_load = max_vehicle_capacity;
        while( current_load > 0 )
        {
            auto it_new_city = std::find_if( std::next( std::begin( cities ) ),
                                             std::end( cities ),
                                             [current_load]( City & c ){
                                                 return *(c.m_visit_counter) == 0 &&
                                                        c.m_demand_capacity < current_load;
                                             });

            if( it_new_city != std::end( cities ) )
            {
                (*(it_new_city->m_visit_counter))++;
                current_load -= it_new_city->m_demand_capacity;
                route.push_back( *it_new_city );
            }
            else
                break;
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    for( auto& route : solution)
        std::shuffle(std::next(route.begin()), route.end(), g);

    for( auto& route : solution)
        route.push_back( cities[0] );

    // for( auto& route : solution)
    // {
    //     for( auto& city : route )
    //         std::cout << city.m_name << " ";
    //     std::cout << std::endl;
    // }
    return solution;
}

bool is_in_tabu_list(Routes routes){
    for(int i=0; i < cvrp_tabu_list.size(); i++){
        if(are_routes_equal(cvrp_tabu_list[i], routes)){
            return true;
        }
    }
    return false;
}

Routes find_best_candidate(std::vector<Routes> neighbors){

    Routes best_candidate = neighbors[0];

    for (int i = 0; i < neighbors.size(); i++){
        // display(neighbors[i]);
        if( !is_in_tabu_list(neighbors[i]) && fitness(neighbors[i]) <= fitness(best_candidate) ){
            best_candidate = neighbors[i];
        }
    }
    return best_candidate;
}

double fitness(Routes routes){
    double routes_length = 0;
    for( Route& route : routes )
        routes_length += TSP::fitness( route );
    return routes_length;
}

bool stopping_condition( Routes& sBest, Routes& prevSBest, int threshold ){
    if( is_near_enough_to(fitness(sBest), fitness(prevSBest), 1 ) )
        counter++;
    else
    {   
        prevSBest = sBest;
        counter = 0;
    }

    return counter <= threshold;
}

//CVRP
Routes tabu_search( Routes s0 ){
    Routes sBest {s0};
    Routes bestCandidate {s0};
    cvrp_tabu_list = {};
    cvrp_tabu_list.push_back(s0);
    Routes prevSBest{ sBest };

    while( stopping_condition( sBest, prevSBest, 100000 ) ){
        std::vector<Routes> neighbors = get_neighbors(bestCandidate);
        bestCandidate = find_best_candidate(neighbors);

        if(fitness(bestCandidate) < fitness(sBest)){
            sBest = bestCandidate;
        }
        cvrp_tabu_list.push_back(bestCandidate);

        if(cvrp_tabu_list.size() > max_tabu_size){
            remove_first_routes_from_tabu_list();
        }
        display(sBest);
    }
    return sBest;
}
};


// ========================================= MAIN ==================================
int main() {
    std::srand(time(0));
    auto solution = CVRP::build_init_solution();
    solution = CVRP::tabu_search( solution );

    for( auto& route : solution )
    {
        TSP::counter = 0;
        route = TSP::tabu_search( route );
    }

    draw_solution( solution, 25 );
    return 0;
}
