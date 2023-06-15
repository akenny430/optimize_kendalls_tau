#include "./load_array.hpp" 
#include "./kendall_tau.hpp" 

#include <cmath> 
#include <cstddef> 
#include <iostream> 
#include <stdexcept> 
#include <unordered_map> 
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

// overloading to supply index to use
auto _get_permutation_index(const std::vector<double>& vals, const std::vector<std::size_t>& prev_index) -> std::vector<std::size_t>
{
    std::vector<std::size_t> index ( vals.size(), 0 );
    for (std::size_t i { 0 }; i < index.size() ; ++i) {
        index[i] = i;
        // index[i] = prev_index.at(i);
    }
    std::sort(
        index.begin(), 
        index.end(), 
        [&](const int& a, const int& b) {
            return (vals.at( prev_index.at(a) ) < vals.at( prev_index.at(b) )); 
        }
    ); 
    return index; 
}

auto _get_kendall_distance(const std::vector<int>& sorted_cum_sum, const std::vector<int>& unsorted_cum_sum, int sup) -> int
{
    std::vector<int> tally_arr ( sup, 0 ); // initializing size of sup with values of 0 
    std::size_t i { 0 }; 
    std::size_t k { 0 }; 
    int idx {  }; 

    int kendall_distance { 0 }; 

    while( i < sorted_cum_sum.size() )
    {
        while( k < sorted_cum_sum.size() && sorted_cum_sum.at(i) == sorted_cum_sum.at(k) )
        {
            kendall_distance += i; 
            idx = unsorted_cum_sum.at(k); 
            while( idx != 0 )
            {
                kendall_distance -= tally_arr.at(idx); 
                idx = idx & (idx - 1); 
            }
            ++k; 
        }
        while( i < k )
        {
            idx = unsorted_cum_sum.at(i); 
            while( idx < sup )
            {
                ++tally_arr.at(idx);
                idx += idx & (-idx); 
            }
            ++i; 
        }
    }

    return kendall_distance; 
}

auto _get_pair_ties(const std::vector<int>& sorted_cum_sum, const std::vector<int>& unsorted_cum_sum) -> int 
{
    std::vector<int> obs_diff_indices { 0 }; 
    for( std::size_t i { 1 }; i < sorted_cum_sum.size(); ++i )
    {
        if( sorted_cum_sum.at(i) != sorted_cum_sum.at(i - 1) || unsorted_cum_sum.at(i) != unsorted_cum_sum.at(i - 1) )
        {
            obs_diff_indices.push_back(i); 
        }
    }
    obs_diff_indices.push_back(sorted_cum_sum.size()); 

    int n_ties { 0 }; 
    int obs_space {  }; 
    for( std::size_t i { 1 }; i < obs_diff_indices.size(); ++i )
    {
        obs_space = obs_diff_indices.at(i) - obs_diff_indices.at(i - 1); 
        n_ties += obs_space * (obs_space - 1) / 2; 
    }
    return n_ties; 
}

auto _get_ind_ties_sorted(const std::vector<int>& sorted_cum_sum) -> int
{
    int prev_val { sorted_cum_sum.at(0) }; 
    int current_count { 1 }; 
    int n_ties { 0 }; 
    for( std::size_t i { 1 }; i < sorted_cum_sum.size(); ++i )
    {
        if( sorted_cum_sum.at(i) == prev_val ) // increase count 
        {
            ++current_count; 
        }
        else 
        {
            if( current_count > 1 ) // we've seen it more than twice, add the difference 
            {
                n_ties += current_count * (current_count - 1) / 2; 
            }
            current_count = 1; 
            prev_val = sorted_cum_sum.at(i); 
        }
    }
    if( current_count > 1 ) // have to potentially add values to end 
    {
        n_ties += current_count * (current_count - 1) / 2; 
    }
    return n_ties; 
}

auto _get_ind_ties_unsorted(const std::vector<int>& unsorted_cum_sum) -> int
{
    int n_ties { 0 }; 
    std::unordered_map<int, int> digit_tracker {  }; 
    for( const int& x : unsorted_cum_sum )
    {
        if( digit_tracker.find(x) == digit_tracker.end() ){
            digit_tracker[x] = 1; 
        }
        else 
        {
            ++digit_tracker[x]; 
        }
    }
    for( const auto& [digit, count] : digit_tracker )
    {
        if( count > 1 ) 
        {
            n_ties += count * (count - 1) / 2; 
        }
    }
    return n_ties; 
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
    // adding last bit of values 
    while( times_seen > 0 )
    {
        y_cum_sum.push_back( current_cum_sum ); 
        --times_seen; 
    }
    // while we are here, getting max value of y and then adding 1 to get sup for kendall distance 
    int sup { current_cum_sum + 1 }; 


    // now permuting the x 
    std::vector<std::size_t> x_sort_indices = _get_permutation_index(x, y_sort_indices);  
    std::vector<int> x_cum_sum {  }; 
    current_cum_sum = 1; 
    times_seen = 1; 
    _prev_val = static_cast<std::size_t>( x.at( x_sort_indices.at(y_sort_indices.at(0)) ) ); // have to apply indexing twice, but now we dont have to sort 
    for( std::size_t _i { 1 }; _i < x_sort_indices.size(); ++_i ) 
    {
        // if( x.at( x_sort_indices.at(_i) ) == _prev_val )
        if( x.at( x_sort_indices.at(y_sort_indices.at(_i)) ) == _prev_val )
        {
            ++times_seen; 
            continue; 
        }
        while( times_seen > 0 )
        {
            x_cum_sum.push_back( current_cum_sum ); 
            --times_seen; 
        }
        times_seen = 1; 
        ++current_cum_sum; 
        // _prev_val = x.at( x_sort_indices.at(_i) ); 
        _prev_val = x.at( x_sort_indices.at(y_sort_indices.at(_i)) ); 
    }
    // adding last bit of values 
    while( times_seen > 0 )
    {
        x_cum_sum.push_back( current_cum_sum ); 
        --times_seen; 
    }

    // finally sorting y_cum_sum with respect to the x permutation 
    const std::vector<int> _copy_y_cum_cum( y_cum_sum ); 
    for( std::size_t i { 0 }; i < y_cum_sum.size(); ++i )
    {
        y_cum_sum.at(i) = _copy_y_cum_cum.at( x_sort_indices.at(i) ); 
    }

    // now will be working with x_cum_sum and y_cum_sum, both are std::vector<int>, from here on 
    int kendall_distance = _get_kendall_distance(x_cum_sum, y_cum_sum, sup); 

    // getting number of ties 
    // std::vector<int> obs_diff_indices { 0 }; 
    // for( std::size_t i { 1 }; i < x_cum_sum.size(); ++i )
    // {
    //     if( x_cum_sum.at(i) != x_cum_sum.at(i - 1) || y_cum_sum.at(i) != y_cum_sum.at(i - 1) )
    //     {
    //         obs_diff_indices.push_back(i); 
    //     }
    // }
    // obs_diff_indices.push_back(x_cum_sum.size()); 

    // int n_ties { 0 }; 
    // int obs_space {  }; 
    // for( std::size_t i { 1 }; i < obs_diff_indices.size(); ++i )
    // {
    //     obs_space = obs_diff_indices.at(i) - obs_diff_indices.at(i - 1); 
    //     n_ties += obs_space * (obs_space - 1) / 2; 
    // }
    int n_ties = _get_pair_ties(x_cum_sum, y_cum_sum); 

    // x and y ties 
    int x_ties = _get_ind_ties_sorted(x_cum_sum); 
    int y_ties = _get_ind_ties_unsorted(y_cum_sum); 
    
    if( x_ties == v_total || y_ties == v_total ) // have to return NaN here 
    {
        return std::numeric_limits<double>::quiet_NaN(); 
    }

    // final part, constructing values from what we have saved along the way 
    double tau { static_cast<double>( v_total - x_ties - y_ties + n_ties - 2 * kendall_distance ) }; 
    tau /= std::sqrt( static_cast<double>( v_total - x_ties ) ); 
    tau /= std::sqrt( static_cast<double>( v_total - y_ties ) ); 

    return tau; 
}



auto compute_kendall_tau(const LoadArray& x, const LoadArray& y) -> double
{
    // if( x.m_size != y.m_size ) { throw std::invalid_argument("Passed in LoadArray's do not have same size!"); }
    if( x.m_data.size() != y.m_data.size() ) { throw std::invalid_argument("Passed in LoadArray's do not have same size!"); }

    // std::cout << x.m_data[x.m_data.size() - 1] << ", " << y.m_data[y.m_data.size() - 1] << '\n'; 

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