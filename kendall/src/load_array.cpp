#include "./load_array.hpp"

// #include <algorithm> 
#include <cmath> 
#include <fstream> 
#include <iostream> 
#include <numeric> 
#include <limits> 
#include <string> 
#include <sstream> 
#include <stdexcept> 
#include <vector> 

LoadArray::LoadArray()
: data{}, mean{ 0.0 }, median{ 0.0 }, var{ 0.0 }, std{ 0.0 }
{}

LoadArray::LoadArray(const std::vector<double>& copy_data)
: data( copy_data )
// having mean and var initialized to 0 
, mean { 0.0 } 
, median { 0.0 } 
, var { 0.0 } 
, std { 0.0 } 
{
    if( copy_data.empty() ) { throw std::invalid_argument("Passed in copy array cannot be empty"); }

    double _n { static_cast<double>(copy_data.size()) }; 
    mean = std::reduce(copy_data.begin(), copy_data.end()) / _n; 

    for( const double& x : copy_data )
    {
        var += (x - mean) * (x - mean); 
    }
    var /= (_n - 1.0); 
    std = std::sqrt(var); 
}

LoadArray::LoadArray(const std::string& file_path, const std::string& column)
// : data ( 1'500 ) // initializing this to have 1,500 values 
: data {  } // empty initialization  
, mean { 0.0 } 
, var { 0.0 } 
{
    std::ifstream file_in_stream ( file_path );
    std::string current_line;
    const int _original_size = data.size(); 

    // reading first row to get correct location of headers 
    std::getline(file_in_stream, current_line);  
    std::stringstream current_line_stream ( current_line ); 
    std::string _buffer_str; // used to store temp variables 
    int column_index { -1 }; 
    int curr_index { 0 }; 
    while( std::getline(current_line_stream, _buffer_str, ',') )
    {
        if( _buffer_str == column )
        {
            column_index = curr_index; 
            break; 
        }
        ++curr_index; 
    }
    if( column_index < 0 ) { throw std::domain_error("Passed invalid column!"); }

    // moving through remaining rows collecting data 
    std::string _data_str; 
    double _data_double; 
    while( std::getline(file_in_stream, current_line) )
    {
        // getting individual value from each line 
        std::stringstream current_line_stream ( current_line ); // TODO: do we have to initialize this every time? 
        curr_index = 0; 
        while (curr_index < column_index)
        {
            ++curr_index; 
            std::getline(current_line_stream, _buffer_str, ','); 
        }
        std::getline(current_line_stream, _data_str, ','); 

        // TODO: handle empty values 
        _data_double = std::stod(_data_str); 
        mean += _data_double; 
        data.push_back(_data_double); 
    }

    // now have data loaded, need to compute mean, var, std 
    double _n { static_cast<double>(data.size()) }; 
    mean /= _n; 
    for( const double& x : data )
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