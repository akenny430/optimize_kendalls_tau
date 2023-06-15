#pragma once 

#include <string> 
#include <vector> 

class LoadArray
{
private: 
    std::vector<double> data; 
    int m_size; 

    double mean; 
    double median; 
    double var; 
    double std; 
public: 
    LoadArray(); 
    LoadArray(const std::vector<double>& copy_data); 
    LoadArray(const std::string& file_path, const std::string& column); 

    auto print_vals() -> void; 
}; 