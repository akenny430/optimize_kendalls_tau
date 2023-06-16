#include "../kendall/src/load_array.hpp" 
#include "../kendall/src/kendall_tau.hpp" 

#include <chrono> 
#include <cstddef> 
#include <fstream> 
#include <iostream> 
#include <sstream> 
#include <string> 
#include <vector> 

// constexpr std::string_view _FILE_PATH_BEGINNING { "../data/data/" }; 
// constexpr std::string_view _CSV { ".csv" }; 
const std::string _FILE_PATH_BEGINNING { "../data/data/" }; 
const std::string _CSV { ".csv" }; 

LoadArray x; 
LoadArray y; 
std::vector<std::string> ids_to_test { "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15" }; 

// struct KendallResult
// {
//     std::string X; 
//     std::string Y; 
//     double Tau; 
//     std::size_t N; 
//     double Time; 
// }; 
class KendallResult
{
public: 
    std::string m_X; 
    std::string m_Y; 
    double m_Tau; 
    std::size_t m_N; 
    double m_Time; 

    KendallResult(const std::string& X, const std::string& Y, double Tau, std::size_t N, double Time)
    : m_X { X } 
    , m_Y { Y } 
    , m_Tau { Tau } 
    , m_N { N } 
    , m_Time { Time } 
    {}
}; 

auto main() -> int 
{
    std::size_t i {  }; 
    std::size_t j {  }; 
    double tau {  }; 
    double time {  }; 

    std::vector<KendallResult> results {  }; 
    for( i = 0; i < ids_to_test.size() - 1; ++i )
    {
        x = LoadArray(_FILE_PATH_BEGINNING + ids_to_test.at(i) + _CSV, "X"); 
        for( j = i + 1; j < ids_to_test.size(); ++j ) 
        {
            y = LoadArray(_FILE_PATH_BEGINNING + ids_to_test.at(j) + _CSV, "X"); 

            auto t0 = std::chrono::high_resolution_clock::now(); 
            tau = compute_kendall_tau(x, y); 
            auto tf = std::chrono::high_resolution_clock::now(); 

            time = static_cast<double>( std::chrono::duration_cast<std::chrono::nanoseconds>(tf - t0).count() ); 
            // std::cout << "Computed in " << std::chrono::duration_cast<std::chrono::nanoseconds>(tf - t0).count() << "ns\n"; 

            results.push_back( KendallResult(ids_to_test.at(i), ids_to_test.at(j), tau, x.m_data.size(), time) ); 
        }
    }

    std::ofstream output_results; 
    output_results.open( "../results/cpp.csv" ); 
    output_results << "X,Y,Tau,N,Time\n"; 
    for( const KendallResult& res : results )
    {
        output_results 
        << res.m_X << ',' 
        << res.m_Y << ',' 
        << res.m_Tau << ',' 
        << res.m_N << ',' 
        << res.m_Time << '\n' 
        ; 
    }
    output_results.close(); 

    return 0; 
}