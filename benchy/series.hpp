#ifndef _BENCHY_SERIES_H
#define _BENCHY_SERIES_H

#include <vector>

template<typename _Storage>
class DataSeries {
public:
    DataSeries();
    DataSeries(int reserveSize);

    // Copy Constructor
    DataSeries(const DataSeries &other);

    // Move Constructor
    DataSeries(DataSeries &&other);

    void add_point(_Storage point);

    inline int size() { return static_cast<int>(m_dataPoints.size());}


    _Storage median();
    _Storage mean();
    _Storage variance();
    _Storage range();
    _Storage sum();

    inline _Storage stdev() { return sqrt(variance()); }
    inline _Storage uncertainty() { return range() / 2; }


private:
    std::vector<_Storage> m_dataPoints;
    bool m_isSorted;
};


#endif // _BENCHY_SERIES_H