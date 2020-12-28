#include "cvrp_tabu_search.hpp"


//funkcja do obliczania odległosci dla miast
double distance_between_cities(Coordinate first_city, Coordinate second_city){
    return sqrt(pow(first_city.length - second_city.length, 2) + pow(first_city.width- second_city.width, 2));
}

void distance_for_two_cities_with_coordinates_from_pythagorean_triangle(){

    const Coordinate Krakow{0, 3};
    const Coordinate Bialystok{4, 0};
    assert(distance_between_cities(Krakow,Bialystok) == 5);
}
// sprawdzenie czy liczba nalezy do zakresu (bo double nie sa dokładne wiec trzeba zakres)
bool is_near_enough_to(double supect, double center, double measurement_error ){
    return supect >= center - measurement_error && supect <= center + measurement_error;
}

void is_near_enough_to_test(){
    assert(is_near_enough_to(4,2,3));
    assert(!is_near_enough_to(10,2,3));
}

//obliczanie dystanu dla 2 miast
void distance_for_two_cities(){

    const Coordinate Krakow{50.0619474, 19.9368564};
    const Coordinate Bialystok{53.132398, 23.1591679};
    assert(is_near_enough_to(distance_between_cities(Krakow,Bialystok), 4.45095) == true);
}

// sprawdzenie macierzy dla 2 miast

void distance_matrix_for_two_cities(){

    const Coordinate Krakow{50.0619474, 19.9368564};
    const Coordinate Bialystok{53.132398, 23.1591679};

    std::vector<std::vector<double>> expected_distance_matrix = {
        {0, 4.45095},
        {4.45095, 0}
    };

     std::vector<std::vector<double>> distance_matrix = {
        {distance_between_cities(Krakow,Krakow), distance_between_cities(Krakow,Bialystok)},
        {distance_between_cities(Bialystok,Krakow), distance_between_cities(Bialystok,Bialystok)},
     };

     for(int i = 0; i < distance_matrix.size(); i++){
         for(int j = 0; j < distance_matrix.size(); j++){
             assert(is_near_enough_to(distance_matrix[i][j], expected_distance_matrix[i][j]));
         }
     }
}

void make_real_distance_matrix(){
 
    const int length_patter = 11;
    for( int i = 0; i < cities.size(); i++ )
    {
        std::cout << std::setw(length_patter) << name_of_cities[ i ] << " ";
    }
    std::cout << std::endl;

    for( int i = 0; i < cities.size(); i++ )
    {
        for( int j = 0; j < cities.size(); j++ )
            std::cout << std::setw(length_patter) << geo_degree_in_km * distance_between_cities( cities[ i ], cities[ j ] ) << " ";
        std::cout << std::endl;
    }
}

std::tuple<std::vector<double>,
           std::vector<double>,
           std::vector<std::string>> our_cities()
{
    std::vector<double> x;
    std::vector<double> y;
    std::vector<std::string> name;
    for( int i = 0; i < cities.size(); i++ )
    {
        x.push_back( cities[i].length );
        y.push_back( cities[i].width );
        name.push_back( name_of_cities[i] );
    }
    return std::make_tuple( x, y, name );
}

void draw_cities() 
{
    using namespace matplot;
    auto [lon,lat,names] = our_cities();
    // int i = 0;
    // while( names[ ++i ] != "Warsaw" );
    // std::cout << lon [ i ] << " " << lat [ i ] << " " << names [ i ] << std::endl;
    auto [lon2, lat2] = greedy_tsp(lon,lat);
    std::vector<double> cols (names.size(),1);
    cols[0]=15;
    cols[1]=15;
    cols[2]=15;
    geoplot(lat2,lon2)->marker("o").marker_colors(cols);
    text(lon,lat,names);
    show();
}

int main() {
    // distance_for_two_cities_with_coordinates_from_pythagorean_triangle();
    // is_near_enough_to_test();
    // distance_for_two_cities();
    // distance_matrix_for_two_cities();
    // make_real_distance_matrix();
    
    draw_cities();
    return 0;
}