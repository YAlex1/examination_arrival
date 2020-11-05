#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>
#include <future>
#include <chrono>
#include <vector>
#include <cstdlib>

namespace DataGenerator {

const int cMaxSpeedValue = 180;		// in km / h
const int cWaitingTimePeriodValue = 800; // in milliseconds

/*
 *@brief This class generates numerical data values, which are calculated
 *       by precompiled mathematical function 
 * 	 f(x) = 200sin(0.05x) + 100sin(0.04x) + 120sin(0.03x) + 80sin(0.02x) + 40sin(0.01x) 
 *
 */
template<typename T>
class DataGenEngine
{
public:
    DataGenEngine() = default;
    ~DataGenEngine() = default;
    
    DataGenEngine(const DataGenEngine&) = delete;
    DataGenEngine(DataGenEngine&&) = delete;
    DataGenEngine& operator=(const DataGenEngine& rhs) = delete;
    DataGenEngine& operator=(DataGenEngine&& rhs) = delete;

    template<typename U> 
    friend std::ostream& operator<<(std::ostream& ostr, DataGenEngine<U>& d);
    template<typename U>
    friend std::stringstream& operator<<(std::stringstream& sstr, DataGenEngine<U>& d);

    void trigger();

private:    
    std::future<T> mRes;
};

/*
 *@brief This method launches background async task 
 *	 for a calculation  of the next numerical value
 */
template<typename T>
void DataGenEngine<T>::trigger()
{
    try {
    	mRes = std::async(std::launch::async,
                      []{
                          auto t = std::chrono::system_clock::now();
                          auto x = t.time_since_epoch().count()/1000050000 % (360);
                          std::vector<std::pair<int, double>> m = {{200,0.05},{160,0.04},{120,0.03},{80,0.02},{40,0.01}};
                          long res(0);
                          
                          for (auto const & el : m){
                              res += el.first * sin(el.second * x);
                          }

                          return static_cast<T>(abs(res % cMaxSpeedValue));
                      });
    } catch (...){
    	std::cerr << "Method DataGenEngine<T>::trigger() is failed !" << std::endl;
    }
}

/*
 *@brief Overwrite operator for outputting data to std::cout object
 */
template<typename U>
std::ostream& operator<<(std::ostream& ostr, DataGenerator::DataGenEngine<U> & d)
{
    auto tmp = d.mRes.get();
    ostr << tmp; 
    return ostr;
}


/*
 *@brief Overwrite operator for outputing data to std::stringstream object
 */
template<typename U>
std::stringstream& operator<<(std::stringstream& sstr, DataGenerator::DataGenEngine<U>& d)
{
    auto tmp = d.mRes.get();
    sstr << tmp;
    return sstr;
}

}; // namespace DataGenerator


