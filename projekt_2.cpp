#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include <iomanip>

#include <matplot/matplot.h>

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
std::vector<std::pair<size_t, size_t>> get_edges();

std::vector<std::pair<size_t, size_t>> get_edges() {
    return {
        {0, 0},   {1, 0},   {5, 0},   {0, 1},   {1, 1},   {2, 1},   {6, 1},
        {1, 2},   {2, 2},   {3, 2},   {7, 2},   {2, 3},   {3, 3},   {4, 3},
        {8, 3},   {3, 4},   {4, 4},   {9, 4},   {0, 5},   {5, 5},   {6, 5},
        {10, 5},  {1, 6},   {5, 6},   {6, 6},   {7, 6},   {11, 6},  {2, 7},
        {6, 7},   {7, 7},   {8, 7},   {12, 7},  {3, 8},   {7, 8},   {8, 8},
        {9, 8},   {13, 8},  {4, 9},   {8, 9},   {9, 9},   {14, 9},  {5, 10},
        {10, 10}, {11, 10}, {15, 10}, {6, 11},  {10, 11}, {11, 11}, {12, 11},
        {16, 11}, {7, 12},  {11, 12}, {12, 12}, {13, 12}, {17, 12}, {8, 13},
        {12, 13}, {13, 13}, {14, 13}, {18, 13}, {9, 14},  {13, 14}, {14, 14},
        {19, 14}, {10, 15}, {15, 15}, {16, 15}, {20, 15}, {11, 16}, {15, 16},
        {16, 16}, {17, 16}, {21, 16}, {12, 17}, {16, 17}, {17, 17}, {18, 17},
        {22, 17}, {13, 18}, {17, 18}, {18, 18}, {19, 18}, {23, 18}, {14, 19},
        {18, 19}, {19, 19}, {24, 19}, {15, 20}, {20, 20}, {21, 20}, {25, 20},
        {16, 21}, {20, 21}, {21, 21}, {22, 21}, {26, 21}, {17, 22}, {21, 22},
        {22, 22}, {23, 22}, {27, 22}, {18, 23}, {22, 23}, {23, 23}, {24, 23},
        {28, 23}, {19, 24}, {23, 24}, {24, 24}, {29, 24}, {20, 25}, {25, 25},
        {26, 25}, {35, 25}, {21, 26}, {25, 26}, {26, 26}, {27, 26}, {36, 26},
        {22, 27}, {26, 27}, {27, 27}, {28, 27}, {37, 27}, {23, 28}, {27, 28},
        {28, 28}, {29, 28}, {38, 28}, {24, 29}, {28, 29}, {29, 29}, {30, 29},
        {39, 29}, {29, 30}, {30, 30}, {31, 30}, {40, 30}, {30, 31}, {31, 31},
        {32, 31}, {41, 31}, {31, 32}, {32, 32}, {33, 32}, {42, 32}, {32, 33},
        {33, 33}, {34, 33}, {43, 33}, {33, 34}, {34, 34}, {44, 34}, {25, 35},
        {35, 35}, {36, 35}, {45, 35}, {26, 36}, {35, 36}, {36, 36}, {37, 36},
        {46, 36}, {27, 37}, {36, 37}, {37, 37}, {38, 37}, {47, 37}, {28, 38},
        {37, 38}, {38, 38}, {39, 38}, {48, 38}, {29, 39}, {38, 39}, {39, 39},
        {40, 39}, {49, 39}, {30, 40}, {39, 40}, {40, 40}, {41, 40}, {50, 40},
        {31, 41}, {40, 41}, {41, 41}, {42, 41}, {51, 41}, {32, 42}, {41, 42},
        {42, 42}, {43, 42}, {52, 42}, {33, 43}, {42, 43}, {43, 43}, {44, 43},
        {53, 43}, {34, 44}, {43, 44}, {44, 44}, {54, 44}, {35, 45}, {45, 45},
        {46, 45}, {55, 45}, {36, 46}, {45, 46}, {46, 46}, {47, 46}, {56, 46},
        {37, 47}, {46, 47}, {47, 47}, {48, 47}, {57, 47}, {38, 48}, {47, 48},
        {48, 48}, {49, 48}, {58, 48}, {39, 49}, {48, 49}, {49, 49}, {50, 49},
        {59, 49}, {40, 50}, {49, 50}, {50, 50}, {51, 50}, {60, 50}, {41, 51},
        {50, 51}, {51, 51}, {52, 51}, {61, 51}, {42, 52}, {51, 52}, {52, 52},
        {53, 52}, {62, 52}, {43, 53}, {52, 53}, {53, 53}, {54, 53}, {63, 53},
        {44, 54}, {53, 54}, {54, 54}, {64, 54}, {45, 55}, {55, 55}, {56, 55},
        {65, 55}, {46, 56}, {55, 56}, {56, 56}, {57, 56}, {66, 56}, {47, 57},
        {56, 57}, {57, 57}, {58, 57}, {67, 57}, {48, 58}, {57, 58}, {58, 58},
        {59, 58}, {68, 58}, {49, 59}, {58, 59}, {59, 59}, {60, 59}, {69, 59},
        {50, 60}, {59, 60}, {60, 60}, {61, 60}, {70, 60}, {51, 61}, {60, 61},
        {61, 61}, {62, 61}, {71, 61}, {52, 62}, {61, 62}, {62, 62}, {63, 62},
        {72, 62}, {53, 63}, {62, 63}, {63, 63}, {64, 63}, {73, 63}, {54, 64},
        {63, 64}, {64, 64}, {74, 64}, {55, 65}, {65, 65}, {66, 65}, {56, 66},
        {65, 66}, {66, 66}, {67, 66}, {57, 67}, {66, 67}, {67, 67}, {68, 67},
        {58, 68}, {67, 68}, {68, 68}, {69, 68}, {59, 69}, {68, 69}, {69, 69},
        {70, 69}, {60, 70}, {69, 70}, {70, 70}, {71, 70}, {61, 71}, {70, 71},
        {71, 71}, {72, 71}, {62, 72}, {71, 72}, {72, 72}, {73, 72}, {63, 73},
        {72, 73}, {73, 73}, {74, 73}, {64, 74}, {73, 74}, {74, 74}};
}

void draw_example() {
    using namespace matplot;
    auto edges = get_edges();
    graph(edges);

    show();
}

int main() {
    // distance_for_two_cities_with_coordinates_from_pythagorean_triangle();
    // is_near_enough_to_test();
    // distance_for_two_cities();
    // distance_matrix_for_two_cities();
    // make_real_distance_matrix();
    draw_example();
}