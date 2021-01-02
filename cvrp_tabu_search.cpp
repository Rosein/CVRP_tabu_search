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
    const int word_width = 15;
    std::cout << "Whole distance is " << fitness(route) << std::endl;
    for (int i = 0; i < route.size(); i++){
        std::cout << std::setw( word_width ) <<  route[ i ].m_name << " ";
    }
    std::cout << std::endl;
}

void draw_solution(std::vector<Route> solution, int color)
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

void swap_cities(City& first, City& second){
    City temp = first;
    first = second;
    second = temp;
}

Route createNeighbor(Route route){
    Route new_route(route);
    std::pair<int,int> cities_to_swap = generate_cities_to_swap(route);
    swap_cities(new_route[ cities_to_swap.first ], new_route[ cities_to_swap.second ]);
    return new_route;
}

bool are_route_equal( Route first_route, Route second_route){
    if( first_route.size() != second_route.size() )
        return false;
    for (int i=0; i < first_route.size(); i++){
        if(first_route[i].m_name != second_route[i].m_name){
            return false;
        }
    }
    return true;
}

bool is_already_in_neighbor(std::vector<Route> neighbors, Route candi ){
    for(int i=0; i< neighbors.size(); i++){
        if( are_route_equal(neighbors[i], candi) )
            return true;
    }
    return false;
}

std::vector<Route> getNeighbors(Route route){

    const int first_city = 1;
    const int last_city = 1;
    const int ignored_cities = first_city + last_city;
    const int current_route = 1;
    std::vector<Route> neighbors;
    int number_actually_needed_neighbors = ( route.size() - ignored_cities > NUMBER_OF_NEIGHBORS ) ? NUMBER_OF_NEIGHBORS : route.size() - ignored_cities - current_route;

    for(int i=0; i < number_actually_needed_neighbors; i++){
        Route new_neighbor = createNeighbor(route);
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

Route find_best_candidate(std::vector<Route> neighbors){

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
    std::reverse(std::begin(tabu_list), std::end(tabu_list));
    tabu_list.pop_back();
    std::reverse(std::begin(tabu_list), std::end(tabu_list));
}


bool stopping_condition( Route& sBest, Route& prevSBest, int threshold ){
    if( is_near_enough_to(fitness(sBest), fitness(prevSBest), 1 ) )
        counter++;
    else
    {   prevSBest = sBest;
        counter = 0;
    }
    // std::cout << std::setw(5) << counter << " " << fitness(prevSBest) << " " << fitness(sBest) << std::endl;

    if( counter > threshold )
        return false;
    else
        return true;
}

bool is_in_tabu_list(Route route){
    for(int i=0; i < tabu_list.size(); i++){
        if(are_route_equal(tabu_list[i], route)){
            return true;
        }
    }
    return false;
}

std::vector<Route> build_init_solution()
{
    std::vector<Route> solution( numb_of_vehicles, Route(1, cities[0]) );
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

Route tsp_tabu_search( Route s0 ){
    Route sBest {s0};
    Route bestCandidate {s0};
    tabu_list = {};
    tabu_list.push_back(s0);
    Route prevSBest{ sBest };

    while( stopping_condition( sBest, prevSBest, 1000 ) ){
        std::vector<Route> neighbors = getNeighbors(bestCandidate);
        bestCandidate = find_best_candidate(neighbors);

        if(fitness(bestCandidate) < fitness(sBest)){
            sBest = bestCandidate;
        }
        tabu_list.push_back(bestCandidate);

        if(tabu_list.size() > max_tabu_size){
            remove_first_route_from_tabu_list();
        }
    }
        display(sBest);
    return sBest;
}


// ========================================= MAIN ==================================
int main() {
    std::srand(time(0));
    auto solution = build_init_solution();
    // draw_solution( solution, 5 );
    for( auto& route : solution )
    {
        counter = 0;
        route = tsp_tabu_search( route );
    }
    draw_solution( solution, 25 );
    return 0;
}
