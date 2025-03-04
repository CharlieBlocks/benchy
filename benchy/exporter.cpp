#include "exporter.hpp"
#include <sstream>

void StdoutTableExporter::AddResult(BenchmarkResults &results) {

    std::string state = BenchmarkStateToString(results.state);
    std::string meanCpuTime = std::to_string(static_cast<long long>(results.cpuData.mean()));
    std::string meanRealTime = std::to_string(static_cast<long long>(results.realData.mean()));
    std::string variance = std::to_string(static_cast<long long>(results.cpuData.variance()));
    std::string iterationCount = std::to_string(results.iterationCount);

    m_tableRows.emplace(m_tableRows.end(), TableRow {
        results.benchmarkName,
        state,
        meanCpuTime,
        meanRealTime,
        variance,
        iterationCount
    });
}


void StdoutTableExporter::Export() {
    const std::string columnHeaders[COLUMN_COUNT] = {
        "Name",
        "State",
        "CPU Time (ns)",
        "Real Time (ns)",
        "Variance",
        "Iterations"
    };
    int columnWidths[COLUMN_COUNT] = { 
        4,
        5,
        13,
        14,
        8,
        10
    };

    for (size_t i = 0; i < m_tableRows.size(); ++i) {
        for (int j = 0; j < COLUMN_COUNT; ++j) {
            int size = static_cast<int>(m_tableRows[i].columns[j].size());
            if (size > columnWidths[j])
                columnWidths[j] = size;
        }
    }
    for (int i = 0; i < COLUMN_COUNT; i++)
        columnWidths[i] += 2;

    
    std::stringstream ss;
    for (int i = 0; i < COLUMN_COUNT; ++i) {
        ss << ' ' << columnHeaders[i];
        ss << std::string(columnWidths[i] - columnHeaders[i].size() - 1, ' ');
        if (i != COLUMN_COUNT - 1)
            ss << '|';
    }
    ss << '\n';


    for (int i = 0; i < COLUMN_COUNT; ++i) {
        ss << std::string(columnWidths[i], '-');
        if (i != COLUMN_COUNT - 1)
            ss << "|";
    }
    ss << '\n';


    for (size_t i = 0; i < m_tableRows.size(); ++i) {
        for (int j = 0; j < COLUMN_COUNT; ++j) {
            ss << ' ' << m_tableRows[i].columns[j];
            ss << std::string(columnWidths[j] - m_tableRows[i].columns[j].size() - 1, ' ');
            if (j != COLUMN_COUNT - 1)
                ss << "|";
        }
        ss << '\n';
    }

    std::cout << ss.str() << std::flush;
}