#ifndef _BENCHY_EXPORTER_H
#define _BENCHY_EXPORTER_H 

#include "benchmark.hpp"

#include <vector>

class Exporter {
public:
    virtual ~Exporter() = default;

    /* Virtual Methods */
    virtual void AddResult(BenchmarkResults &results) = 0;

    virtual void Export() = 0;

};


#define COLUMN_COUNT 6

class StdoutTableExporter : public Exporter {
public:

    void AddResult(BenchmarkResults &results) override;

    void Export() override;

    

private:
    struct TableRow {
        union {
            struct {
                std::string name;
                std::string result;
                std::string cputime;
                std::string realtime;
                std::string variance;
                std::string iterations;
            };
            std::string columns[COLUMN_COUNT];
        };

        ~TableRow() {
            for (int i = 0; i < COLUMN_COUNT; i++) {
                columns[i].~basic_string();
            }
        }
        
        TableRow(
            std::string &name,
            std::string &result,
            std::string &cputime,
            std::string &realtime,
            std::string &variance,
            std::string &iterations
        )
        : name(std::move(name)),
            result(std::move(result)),
            cputime(std::move(cputime)),
            realtime(std::move(realtime)),
            variance(std::move(variance)),
            iterations(std::move(iterations))
        { }


        TableRow(const TableRow &other) {
            for (int i = 0; i < COLUMN_COUNT; ++i)
                this->columns[i] = other.columns[i];
        }

        TableRow &operator =(const TableRow &other) {
            for (int i = 0; i < COLUMN_COUNT; ++i)
                this->columns[i] = other.columns[i];

            return *this;
        }
    };


private:
    std::vector<TableRow> m_tableRows;
    

};


#endif // _BENCHY_EXPORTER_H