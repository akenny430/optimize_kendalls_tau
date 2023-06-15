#include "./load_array.hpp"

// #include <algorithm> 
#include <cmath> 
#include <iostream> 
#include <numeric> 
#include <string> 
#include <vector> 

LoadArray::LoadArray()
: data{}, mean{ 0.0 }, median{ 0.0 }, var{ 0.0 }, std{ 0.0 }
{}

LoadArray::LoadArray(const std::vector<double>& copy_data)
: data( copy_data )
{
    if( copy_data.empty() )
    {
        mean = 0.0; 
        median = 0.0; 
        var = 0.0; 
        std = 0.0; 
    }

    double _n { static_cast<double>(copy_data.size()) }; 
    mean = std::reduce(copy_data.begin(), copy_data.end()) / _n; 

    var = 0.0; 
    for( const double& x : copy_data )
    {
        var += (x - mean) * (x - mean); 
    }
    var /= (_n - 1.0); 
    std = std::sqrt(var); 
}



auto LoadArray::print_vals() -> void
{
    std::cout 
    << "Array has " << data.size() << " values\n" 
    << "Mean = " << mean << '\n' 
    << "Variance = " << var << '\n' 
    << "Standard deviation = " << std << '\n' 
    ; 
}