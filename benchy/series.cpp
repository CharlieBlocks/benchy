#include "series.hpp"

#include <iostream>


template<typename _Storage>
DataSeries<_Storage>::DataSeries() 
: m_isSorted(false) { }

template<typename _Storage>
DataSeries<_Storage>::DataSeries(int reserveSize) 
: m_isSorted(false) {
    m_dataPoints.reserve(reserveSize);
}

template<typename _Storage>
DataSeries<_Storage>::DataSeries(const DataSeries &other) 
: m_isSorted(other.m_isSorted) {
    m_dataPoints = other.m_dataPoints; // Copy data poitns
}

template<typename _Storage>
DataSeries<_Storage>::DataSeries(DataSeries &&other)
: m_isSorted(other.m_isSorted) {
    m_dataPoints = std::move(other.m_dataPoints);
}

template<typename _Storage>
void DataSeries<_Storage>::add_point(_Storage point) {
    m_dataPoints.push_back(point);
    m_isSorted = false;
}

template<typename _Storage>
_Storage DataSeries<_Storage>::median() {
    if (!m_isSorted) {
        std::sort(m_dataPoints.begin(), m_dataPoints.end(), [](_Storage a, _Storage b) { return a < b; });
        m_isSorted = true;
    }

    return m_dataPoints.at(m_dataPoints.size() / 2);
}

template<typename _Storage>
_Storage DataSeries<_Storage>::mean() {
    return sum() / m_dataPoints.size();
}

template<typename _Storage>
_Storage DataSeries<_Storage>::variance() {
    _Storage meanSquared = this->mean();
    meanSquared *= meanSquared;

    _Storage squaredSum = 0;
    for (size_t i = 0; i < m_dataPoints.size(); i++)
        squaredSum += m_dataPoints[i]*m_dataPoints[i];
    
    return (squaredSum / m_dataPoints.size()) - meanSquared;
}

template<typename _Storage>
_Storage DataSeries<_Storage>::range() {
    if (!m_isSorted) {
        std::sort(m_dataPoints.begin(), m_dataPoints.end(), [](_Storage a, _Storage b) { return a < b; });
        m_isSorted = true;
    }

    return m_dataPoints[m_dataPoints.size() - 1] - m_dataPoints[0];
} 

template<typename _Storage>
_Storage DataSeries<_Storage>::sum() {
    _Storage sum = 0;
    for (size_t i = 0; i < m_dataPoints.size(); i++)
        sum += m_dataPoints[i];
    
    return sum;
}


template class DataSeries<float>;
template class DataSeries<double>;