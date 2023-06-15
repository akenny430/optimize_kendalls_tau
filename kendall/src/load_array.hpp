#pragma once 

#include <string> 
#include <vector> 

class LoadArray
{
private: 
    std::vector<double> m_data; 
    int m_size; 

    int m_n_nan; 

    double m_mean; 
    double m_var; 
    double m_std; 
public: 
    LoadArray(); 
    LoadArray(const std::vector<double>& copy_data); 
    LoadArray(const std::string& file_path, const std::string& column); 

    auto print_vals() -> void; 
}; 