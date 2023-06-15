#include "./load_array.hpp"

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
: m_data{  }, m_size { 0 }, m_mean{ 0.0 }, m_var{ 0.0 }, m_std{ 0.0 }
{}

LoadArray::LoadArray(const std::vector<double>& copy_data)
: m_data( copy_data )
, m_size { static_cast<int>(copy_data.size()) } 
, m_mean { 0.0 } 
, m_var { 0.0 } 
, m_std { 0.0 } 
{
    if( copy_data.empty() ) { throw std::invalid_argument("Passed in copy array cannot be empty"); }

    double _n { static_cast<double>(copy_data.size()) }; 
    m_mean = std::reduce(copy_data.begin(), copy_data.end()) / _n; 

    for( const double& x : copy_data )
    {
        m_var += (x - m_mean) * (x - m_mean); 
    }
    m_var /= (_n - 1.0); 
    m_std = std::sqrt(m_var); 
}

LoadArray::LoadArray(const std::string& file_path, const std::string& column)
: m_data {  } // empty initialization  
, m_size { 0 } 
, m_mean { 0.0 } 
, m_var { 0.0 } 
{
    std::ifstream file_in_stream ( file_path );
    std::string current_line; 

    // reading first row to get correct location of headers 
    std::getline(file_in_stream, current_line); // TODO: check if input is valid 
    std::stringstream current_line_stream ( current_line ); 
    std::string _buffer_str; // used to store temp variables 
    int column_index { -1 }; 
    int curr_index { 0 }; 
    while( std::getline(current_line_stream, _buffer_str, ',') )
    {
        std::cout << _buffer_str << '\n'; 
        if( _buffer_str == column )
        {
            column_index = curr_index; 
            break; 
        }
        ++curr_index; 
    }
    if( curr_index == 0 ) { throw std::invalid_argument("Supplied file not found!"); } // never read in anything, means wrong file name passed 
    else if( column_index < 0 ) { throw std::domain_error("Passed invalid column!"); } // could not find column we wanted 

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

        // if it cannot convert to double, set value to NaN 
        try 
        { 
            _data_double = std::stod(_data_str); 
            m_mean += _data_double; 
            ++m_size; 
        }
        catch( const std::invalid_argument& e ) 
        { 
            _data_double = std::numeric_limits<double>::quiet_NaN(); 
        }
        m_data.push_back(_data_double); 
    }

    // now have data loaded, need to compute mean, var, std 
    double _n { static_cast<double>(m_size) }; 
    m_mean /= _n; 
    for( const double& x : m_data )
    {
        if( std::isnan(x) ) { continue; }
        m_var += (x - m_mean) * (x - m_mean); 
    }
    m_var /= (_n - 1.0); 
    m_std = std::sqrt(m_var); 
}



auto LoadArray::print_vals() -> void
{
    std::cout 
    << "Array has " << m_size << " values\n" 
    << "Mean = " << m_mean << '\n' 
    << "Variance = " << m_var << '\n' 
    << "Standard deviation = " << m_std << '\n' 
    ; 
}