#include "./load_array.hpp" 
#include "./kendall_tau.hpp" 

#include <cmath> 
#include <cstddef> 
#include <iostream> 
#include <stdexcept> 
#include <vector> 



auto _compute_kendall_tau_with_full(const std::vector<double>& x, const std::vector<double>& y) -> float
{
    int size { x.size() }; 
    int v_total { size * (size - 1) / 2 }; 


    return 0.0; 
}



auto compute_kendall_tau(const LoadArray& x, const LoadArray& y) -> double
{
    // if( x.m_size != y.m_size ) { throw std::invalid_argument("Passed in LoadArray's do not have same size!"); }
    if( x.m_data.size() != y.m_data.size() ) { throw std::invalid_argument("Passed in LoadArray's do not have same size!"); }

    // if we don't have any NaN data points, compute right away without any pre-processing 
    if( !(x.has_nan() || y.has_nan()) )
    {
        return _compute_kendall_tau_with_full(x.m_data, y.m_data); 
    }

    // otherwise, have to create new vectors that omit NaN 
    std::vector<double> altx {  }; 
    std::vector<double> alty {  }; 
    for( std::size_t i { 0 }; i < x.m_size; ++i )
    {
        if( std::isnan( x.m_data.at(i) ) || std::isnan( y.m_data.at(i) ) ) { continue; } // skip if either row has NaN 
        altx.push_back( x.m_data.at(i) ); 
        alty.push_back( y.m_data.at(i) ); 
    }
    return _compute_kendall_tau_with_full(altx, alty); 
}