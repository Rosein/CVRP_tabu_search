#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include <iomanip>
//potrzeba dokładnych danych
const double geo_degree_in_km = 111.1; 

//typ dla miasta szerokosc/długosc geo
struct Coordinate{
    double length;
    double width;
};

std::vector<std::string> name_of_cities { "Krakow", "Bialystok", "Bie-Biala", "Chrzanow", "Gdansk", "Gdynia", "Gliwice", "Gromnik", "Katowice", "Kielce", "Krosno", "Krynica", "Lublin", "Lodz", "Malbork", "Nowy Targ", "Olsztyn", "Poznan", "Pulawy", "Radom", "Rzeszow", "Sandomierz", "Szczecin", "Szczucin", "Szk Poreba", "Tarnow", "Warszawa", "Wieliczka", "Wroclaw", "Zakopane", "Zamosc" };

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
bool is_near_enough_to(double supect, double center, double measurement_error = 0.000001){
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
    std::vector<Coordinate> cities = {
        {50.0619474, 19.9368564},
        {53.132398, 23.1591679},
        {49.822118, 19.0448936},
        {50.1411926, 19.4028502},
        {54.347629, 18.6452324},
        {54.5164982, 18.5402738},
        {50.294113, 18.6657306},
        {53.7361111, 15.9897222},
        {50.2598987, 19.0215852},
        {50.8716657, 20.6314728},
        {49.6938045, 21.7651458},
        {53.0336093, 23.5880673},
        {51.250559, 22.5701022},
        {51.7687323, 19.4569911},
        {54.0359361, 19.0348817},
        {49.4818229, 20.030541},
        {53.7766839, 20.476507},
        {52.4082663, 16.9335199},
        {51.4171713, 21.9720582},
        {51.4022557, 21.1541546},
        {50.0374531, 22.0047174},
        {50.6793066, 21.7495055},
        {53.4301818, 14.5509623},
        {50.3090256, 21.075161},
        {50.8275855, 15.5211659},
        {50.0123784, 20.9880739},
        {52.2319581, 21.0067249},
        {49.9823773, 20.0602114},
        {51.1089776, 17.0326689},
        {49.2969446, 19.950659},
        {50.7170854, 23.2525711}
    }; 
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

int main() {
    distance_for_two_cities_with_coordinates_from_pythagorean_triangle();
    is_near_enough_to_test();
    distance_for_two_cities();
    distance_matrix_for_two_cities();
    make_real_distance_matrix();
}