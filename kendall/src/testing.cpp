#include "load_array.hpp" 
#include "./kendall_tau.hpp" 

#include <chrono> 
#include <iostream> 
#include <vector> 
#include <stdexcept>





// testing LoadArray 
// auto main() -> int
// {
//     // basic initialization (how to even call this?)
//     // TODO: best practice for initializing default class with empty initializer? 
//     // LoadArray test_array_1; 
//     LoadArray test_array_1{}; 
//     test_array_1.print_vals(); 

//     // initializing array from a vector 
//     std::vector<double> vec_for_copy { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 }; 
//     LoadArray test_array_2 ( vec_for_copy ); 
//     test_array_2.print_vals(); 

//     // initializing array from file 
//     LoadArray test_array_3 ( "../../data/data/05.csv", "X" ); 
//     test_array_3.print_vals(); 

//     // initializing array from file... with null values 
//     LoadArray test_array_4 ( "../../data/data/12.csv", "X" ); 
//     test_array_4.print_vals(); 



//     // error testing  

//     // looking for column that does not exist 
//     try { LoadArray error1 ( "../../data/data/05.csv", "DoesNotExist" ); } 
//     catch( const std::domain_error& e ) { std::cout << "Check for column selection good\n"; }

//     // loading file that does not exist 
//     try { LoadArray error2 ( "../../data/data/bad_file.csv", "X" ); }
//     catch( const std::invalid_argument& e ) { std::cout << "Check for bad file good\n"; }



//     return 0; 
// }





// Testing Kendall's Tau 
auto main() -> int
{
    auto t0 = std::chrono::high_resolution_clock::now(); 

    LoadArray x ( "../../data/data/05.csv", "X" ); 
    // LoadArray y ( "../../data/data/08.csv", "X" ); 
    LoadArray y ( "../../data/data/12.csv", "X" ); 

    // LoadArray x ( "../../data/data/s1.csv", "X" ); 
    // LoadArray y ( "../../data/data/s2.csv", "X" ); 

    double tau = compute_kendall_tau(x, y); 

    auto tf = std::chrono::high_resolution_clock::now(); 
    std::cout << "tau = " << tau << '\n'; 
    std::cout << "Computed in " << std::chrono::duration_cast<std::chrono::nanoseconds>(tf - t0).count() << "ns\n"; 

    return 0; 
}