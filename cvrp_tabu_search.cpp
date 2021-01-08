#include "cvrp_tabu_search.hpp"

//funkcja do obliczania odległosci dla miast
double distance_between_cities( City& first_city, City& second_city ){
    return geo_degree_in_km * ( sqrt( pow(first_city.m_coordinate.length - second_city.m_coordinate.length, 2) +
                                      pow(first_city.m_coordinate.width  - second_city.m_coordinate.width,  2)  ) );
}

//sprawdzenie czy liczba nalezy do zakresu (bo double nie sa dokładne wiec trzeba zakres)
bool is_near_enough_to( double suspect, double center, double measurement_error ){
    return suspect >= center - measurement_error && suspect <= center + measurement_error;
}

int sum_capacity( Route route )
{
    int sum{0};
    for( auto& city : route )
        sum += city.m_demand_capacity;
    return sum;
}

void display( Route route ){
    const int word_width = 10;
    std::cout << " { " << std::setw( 7 ) <<  TSP::fitness(route) << " }";
    std::cout << " ( " << std::setw( 7 ) <<  sum_capacity(route) << " )";
    for (int i = 0; i < route.size(); i++){
        std::cout << std::setw( word_width ) <<  route[ i ].m_name << " ";
    }
    std::cout << std::endl;
}

void display( Routes routes )
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

void draw_solution( Routes solution, int color ) // rysowanie rozwiazania, przy uzyciu matplot++
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

        std::vector<double> cols (route.size(), color); // określa kolor dróg 
        color += 25;
        geoplot(y,x)->marker("o").marker_colors(cols); // wstawia punkt wezła i zonacza punkt kolorem
        text(x,y,tag);
    }
    show();
}

void swap_cities( City& first, City& second ){ // funkcja zamieniajaca miasta
    City temp = first;
    first = second;
    second = temp;
}

bool are_route_equal( Route first_route, Route second_route ){ // porowanie trasy
    if( first_route.size() != second_route.size() )
        return false;
    for( int i = 0; i < first_route.size(); i++ ){
        if(first_route[ i ].m_name != second_route[ i ].m_name){
            return false;
        }
    }
    return true;
}

bool are_routes_equal( Routes first_routes, Routes second_routes ){ //porowanie zbioru tras
    if( first_routes.size() != second_routes.size() )
        return false;
    for( int i = 0; i < first_routes.size(); i++ ){
        if( are_route_equal( first_routes[ i ], second_routes[ i ] ) )
            return false;
    }
    return true;
}

template<typename T>
void pop_first( T& vector )
{
    std::reverse(std::begin(vector), std::end(vector));
    vector.pop_back();
    std::reverse(std::begin(vector), std::end(vector));
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

double fitness(Route route){ //funkcja liczy dystans z pojedynczej trasy pojedynczego samochodu
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
        if( are_route_equal(tsp_tabu_list[i], route ) ){
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

    while( stopping_condition( sBest, prevSBest, tsp_max_same_iterations_result ) ){
        Routes neighbors = get_neighbors(bestCandidate);
        bestCandidate = find_best_candidate(neighbors);

        if(fitness(bestCandidate) < fitness(sBest)){
            sBest = bestCandidate;
        }
        tsp_tabu_list.push_back(bestCandidate);

        if(tsp_tabu_list.size() > max_tabu_size){
            pop_first(tsp_tabu_list);
        }
    }

    return sBest;
}

};

namespace CVRP {

int getDiffrentRandomIndex( int first_index, int container_size )
{
    int second_index = std::rand() % ( container_size );
    while( first_index == second_index ){
        second_index = std::rand() % ( container_size );
    }
    return second_index;
}

std::pair<int,int> getTwoRandomIdexes( int container_size )
{
    int first_index = std::rand() % ( container_size );
    return { first_index, getDiffrentRandomIndex( first_index, container_size ) };
}

int getRandomIndexCityFromInsideOfRoad( int route_size )
{
    const int FIRST_CITY = 1;
    const int LAST_CITY = 2;
    return FIRST_CITY + std::rand() % ( route_size - LAST_CITY );
}

int getRandomIndexOfRouteWithEnoughCapacity( Routes& routes, std::pair<int,int> xy_city )
{
    // std::cout << __PRETTY_FUNCTION__ << __LINE__ << std::endl;

    int new_route_index;

    do{
        new_route_index = getDiffrentRandomIndex( xy_city.first, routes.size() );
         
    }
    while( !is_enough_space_in_vehicle( routes[new_route_index], routes[ xy_city.first ][ xy_city.second ].m_demand_capacity ) );

    return new_route_index;
}

std::pair<Route_City, Route_City> generate_indexes_for_city_swapping(Routes routes){

    if( routes.size() == 1 )
        throw std::logic_error( "Too few routes." );

    std::pair<int,int> route = getTwoRandomIdexes( routes.size() );

    int first_city = getRandomIndexCityFromInsideOfRoad( routes[ route.first ].size() );
    int second_city = getRandomIndexCityFromInsideOfRoad( routes[ route.second ].size() );

    return { { route.first, first_city }, { route.second, second_city} };
}

Routes create_neighbor_by_swapping( Routes routes ){ // funkcja tworzy nowego sąsiada
    Routes new_route(routes); //kopiuje na początku wszystkie trasy
    std::pair<Route_City,Route_City> cities_to_swap = generate_indexes_for_city_swapping(routes);
    swap_cities(new_route[ cities_to_swap.first.first  ] [ cities_to_swap.first.second ], 
                new_route[ cities_to_swap.second.first ] [ cities_to_swap.second.second ] ); // w nowej drodze zamienia miasta
    return new_route;
}

Routes createRoutesWithoutOneCity( Routes routes, std::pair<int,int> xy_city )
{
    Routes routes_with_delated_city( routes );
    auto it_city_to_move = std::next( std::begin(routes_with_delated_city[xy_city.first]), xy_city.second );
    routes_with_delated_city[xy_city.first].erase( it_city_to_move );
    return routes_with_delated_city;
}

std::vector<Routes> create_neighbors_by_move_one_city( Routes & routes, 
                                                       std::pair<int,int> xy_city, 
                                                       int index_destination_route ){
    const int FIRST_CITY = 1;
    
    Routes routes_with_delated_city =  createRoutesWithoutOneCity( routes, xy_city );
    City moving_city = routes[xy_city.first][xy_city.second];
    std::vector<Routes> new_neighbors( routes[ index_destination_route ].size() - FIRST_CITY, routes_with_delated_city );
    for( int i = 0; i < new_neighbors.size(); i++ )
    {
        auto place_to_insert = std::next( std::begin( new_neighbors[ i ][ index_destination_route ] ), i + 1 );
        new_neighbors[ i ][ index_destination_route ].insert( place_to_insert, moving_city );
    }
    return new_neighbors;
}

bool is_already_in_neighbor( std::vector<Routes> neighbors, Routes candi ){
    for(int i=0; i< neighbors.size(); i++){
        if( are_routes_equal(neighbors[i], candi) )
            return true;
    }
    return false;
}

bool is_enough_space_in_vehicle( Route route, int extra_load ) //sprawdza czy suma zapotrzebowania dla danej trasy jest mniejsza badz rowna ładownosci dla danego samochodu 
{
    int current_load = extra_load;

    for( auto& city : route )
        current_load += city.m_demand_capacity; // suma zapotrzebowania dla danej trasy

    return current_load <= max_vehicle_capacity; 
}

bool is_enough_space_in_vehicles( Routes routes ) // sprawdza czy wygenerowany zestaw tras spełnia warunek ładownosci 
{
    for( auto& route : routes )
    {
        if( !is_enough_space_in_vehicle( route ) )
            return false;
    }
    return true;
}

void add_swapped_neighbors( Routes& routes, std::vector<Routes>& neighbors )
{
    int number_actually_needed_neighbors =  NUMBER_OF_NEIGHBORS;
    for(int i=0; i < number_actually_needed_neighbors; i++){
        int shuffle_times = std::rand() % 3;
        Routes new_neighbor = create_neighbor_by_swapping(routes);
        while( !is_enough_space_in_vehicles( new_neighbor ) ) //jesli nowy sasaid nie spełnai arunku pojemnosci, tworzy nowego sasiada
        {
            // std::cout << __PRETTY_FUNCTION__ << __LINE__ << std::endl;
            new_neighbor = create_neighbor_by_swapping( routes );
            for(int i = 0; i < shuffle_times; ++i ){
                new_neighbor = create_neighbor_by_swapping( new_neighbor );
            }
        }
        if( !is_already_in_neighbor( neighbors, new_neighbor ) )
            neighbors.push_back( new_neighbor );
        else
            --i;
    }
}

bool weCanAddCityToAnyOtherRoute( Routes& routes, std::pair<int,int> xy_city )
{

    for( int i = 0; i < routes.size(); i++ )
    {
        if( i != xy_city.first &&
            is_enough_space_in_vehicle( routes[i], routes[ xy_city.first ][ xy_city.second ].m_demand_capacity ) )
        return true;
    }
    return false;
}

void add_moved_one_city_neighbors( Routes& routes, std::vector<Routes>& neighbors )
{
    std::pair<int,int> xy_city_to_moved;
    int break_counter = 0;
    do {
        if( break_counter++ == break_threshold )
            return;
        
        int first_route_index = std::rand() % ( routes.size() );
        while( routes[ first_route_index ].size() < min_size_of_route )
            first_route_index = std::rand() % ( routes.size() );

        int index_random_city = getRandomIndexCityFromInsideOfRoad( routes[ first_route_index ].size() ) ;
        xy_city_to_moved = { first_route_index, index_random_city };

    } 
    while( !weCanAddCityToAnyOtherRoute( routes, xy_city_to_moved ) );

    int index_destination_route = getRandomIndexOfRouteWithEnoughCapacity( routes, 
                                                                            xy_city_to_moved );
    std::vector<Routes> new_neighbors = create_neighbors_by_move_one_city( routes, 
                                                                            xy_city_to_moved, 
                                                                            index_destination_route);
    for( Routes& new_neighbor : new_neighbors )
            neighbors.push_back( new_neighbor );
}


std::vector<Routes> get_neighbors( Routes routes ){ //funkcja tworzaca sąsiada - nasze roziwazanie, zmiany miast pomiedzy drogami - liczba zmian max 3 min 1 przechodza tylko te, ktore spełniaja warunek pojemnosci
    std::vector<Routes> neighbors{};
    neighbors.reserve( 3 * NUMBER_OF_NEIGHBORS );
    add_swapped_neighbors( routes, neighbors );
    add_moved_one_city_neighbors( routes, neighbors );
    return neighbors;
}

City cut_random_city( std::vector<City>& clients_cities )
{
    std::random_device rd; 
    std::mt19937 g(rd()); 
    std::shuffle(clients_cities.begin(), clients_cities.end(), g);

    City last_city = clients_cities.back();
    clients_cities.pop_back();
    return last_city;
}

bool vehicle_have_enough_load_for_city_after_route( Route route, int city_capacity_demand )
{
    return sum_capacity( route ) + city_capacity_demand <= max_vehicle_capacity;
}

int get_index_of_route_with_enough_capacity( Routes routes, int city_capacity_demand )
{
    int index_of_route;
    int guard_counter = 0;
    do{ 
        index_of_route = std::rand() % routes.size();
        if( guard_counter++ == error_threshold )
            return ERROR_NUMBER;
    }
    while( !vehicle_have_enough_load_for_city_after_route( routes[index_of_route], city_capacity_demand ) );
    return index_of_route;
}

Routes build_init_solution( std::vector<City> clients_cities ) //buduje początkowe rozwiązanie - pierwsze trasy w zalezności od ilości samochodów
{
    CVRP::counter = 0;
    City depot_city = clients_cities[0];
    Routes solution( numb_of_vehicles, Route(1, depot_city) ); // buduje puste drogi i daje kazdej Kraków na poczatku
    pop_first( clients_cities );

    while( clients_cities.size() > 0 )
    {
        City city = cut_random_city( clients_cities );
        int index_route = get_index_of_route_with_enough_capacity( solution, city.m_demand_capacity );
        if( index_route == ERROR_NUMBER )
            std::cerr << "Error. There is not enough space in cars." << std::endl;
        solution[ index_route ].push_back( city );
    }

    for( auto& route : solution )
        route.push_back( depot_city );

    return solution;
}

bool is_in_tabu_list( Routes routes ){
    for(int i=0; i < cvrp_tabu_list.size(); i++){
        if(are_routes_equal(cvrp_tabu_list[i], routes)){
            return true;
        }
    }
    return false;
}

Routes find_best_candidate( std::vector<Routes> neighbors ){ //funkcja znajdujaca nalepszego sąsiada

    Routes best_candidate = neighbors[0]; // przypisanie do zmiennej tymczasowej pierwszego sasiada

    for (int i = 0; i < neighbors.size(); i++){
        if( !is_in_tabu_list( neighbors[i] ) && fitness( neighbors[i] ) <= fitness( best_candidate ) ){
            best_candidate = neighbors[i];
        }
    }
    return best_candidate;
}

double fitness(Routes routes){ //funkcja sprawdzajaca dlugosc trasy jednego rozwiazania - wszystkich samochodów 
    double routes_length = 0;
    for( Route& route : routes )
        routes_length += TSP::fitness( route );
    return routes_length;
}

bool stopping_condition( Routes& sBest, Routes& prevSBest, int threshold ){ 
    // display(sBest);
    if( is_near_enough_to(fitness(sBest), fitness(prevSBest), 1 ) )
    {
        CVRP::counter++;
    }
    else
    {   
        display(sBest);
        prevSBest = sBest;
        CVRP::counter = 0;
    }

    return CVRP::counter <= threshold;
}

//CVRP
Routes tabu_search( Routes s0 ){
    Routes sBest {s0};
    Routes bestCandidate {s0};
    cvrp_tabu_list = {};
    cvrp_tabu_list.push_back(s0);
    Routes prevSBest{ sBest }; //potrzebne do waunku stopu
    while( stopping_condition( sBest, prevSBest, cvrp_max_same_iterations_result ) ){
        std::vector<Routes> neighbors = get_neighbors(bestCandidate); // wektor zbioru drog 
        bestCandidate = find_best_candidate(neighbors);

        if( fitness(bestCandidate) < fitness(sBest) ){
            sBest = bestCandidate;
        }
        cvrp_tabu_list.push_back( bestCandidate );

        if( cvrp_tabu_list.size() > max_tabu_size ){
            pop_first(cvrp_tabu_list);
        }
    }
    return sBest;
}

};

// ========================================= MAIN ==================================
int main() 
{
    std::srand(time(0));
    std::cout << "Start building init solution" << std::endl;
    Routes solution = CVRP::build_init_solution( cities ); //buduje początkowe rozwiązanie - pierwsze trasy w zalezności od ilości samochodów
    std::cout << "End building init solution" << std::endl;
    std::cout << "Display init solution" << std::endl;
    display( solution );
    
    // draw_solution( solution, 25 );

    auto start = std::chrono::high_resolution_clock::now(); 
    solution = CVRP::tabu_search( solution ); // wykorzystanie algorytmu tabu search do znalezenia możliwie optymalnego rozwiaząnia 
    auto stop = std::chrono::high_resolution_clock::now(); 
  
    std::cout << "Time taken by cvrp_tabu_search: "
         << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1000000 << " seconds" << std::endl; 

   
    for( auto& route : solution )  //wykorzystanie algorytmu tsp tabu search dla poprawienia każdej drogi z osobana z ostatcznego rozwiązania
    {
        TSP::counter = 0;
        route = TSP::tabu_search( route );
    }

    display( solution );
    draw_solution( solution, 25 );
    return 0;
}
