#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>
#include <future>
#include <chrono>
#include <vector>
#include <cstdlib>

namespace DataView {

template<typename T>
class DataViewClient
{
public:
    DataViewClient();
    ~DataViewClient() = default;
    
    DataViewClient(const DataViewClient&) = delete;
    DataViewClient(DataViewClient&&) = delete;
    DataViewClient& operator=(const DataViewClient& rhs) = delete;
    DataViewClient& operator=(DataViewClient&& rhs) = delete;

    template<typename U> 
    friend std::ostream& operator<<(std::ostream& ostr, DataViewClient<U>& d);

private:    
    T mVal;
};

template<typename T>
DataViewClient<T>::DataViewClient()
    : mVal(0)
{
}

template<typename U>
std::ostream& operator<<(std::ostream& ostr, DataView::DataViewClient<U> & d)
{
    auto tmp = d.mVal;
    ostr << tmp; 
    return ostr;
}

}; // namespace DataView


