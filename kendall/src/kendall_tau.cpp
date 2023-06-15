#include "./load_array.hpp" 
#include "./kendall_tau.hpp" 

#include <cmath> 
#include <cstddef> 
#include <iostream> 
#include <stdexcept> 
#include <vector> 



auto _get_permutation_index(const std::vector<double>& vals) -> std::vector<std::size_t>
{
    std::vector<std::size_t> index ( vals.size(), 0 );
    for (std::size_t i { 0 }; i < index.size() ; ++i) {
        index[i] = i;
    }
    std::sort(
        index.begin(), 
        index.end(), 
        [&](const int& a, const int& b) {
            return (vals[a] < vals[b]); 
        }
    ); 
    return index; 
}


auto _compute_kendall_tau_with_full(const std::vector<double>& x, const std::vector<double>& y) -> float
{
    int size { static_cast<int>(x.size()) }; 
    int v_total { size * (size - 1) / 2 }; 

    // first permutations of y 
    std::vector<std::size_t> y_sort_indices = _get_permutation_index(y); 

    std::vector<int> y_cum_sum {  }; 
    int current_cum_sum { 1 }; // always starts with 1 
    int times_seen { 1 }; 
    // for( const std::size_t index : y_sort_indices ) 
    std::size_t _prev_val { static_cast<std::size_t>( y.at( y_sort_indices.at(0) ) ) }; 
    for( std::size_t _i { 1 }; _i < y_sort_indices.size(); ++_i ) 
    {
        if( y.at( y_sort_indices.at(_i) ) == _prev_val )
        {
            ++times_seen; 
            continue; 
        }
        while( times_seen > 0 )
        {
            y_cum_sum.push_back( current_cum_sum ); 
            --times_seen; 
        }
        times_seen = 1; 
        ++current_cum_sum; 
        _prev_val = y.at( y_sort_indices.at(_i) ); 
    }

    // test printing 
    for( int j = 0; j < 20; ++j )
    {
        std::cout << "(" << y[y_sort_indices[j]] << ", " << y_cum_sum[j] << ")\n"; 
    }


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