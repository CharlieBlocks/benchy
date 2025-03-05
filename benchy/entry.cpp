#include "benchy.hpp"

int main(int argc, char **argv) {

    Benchy benchy(4);

    benchy.ExecuteMicroBenchmarks();
    
    std::unique_ptr<Exporter> exporter = std::make_unique<StdoutTableExporter>();
    benchy.ExportTo(exporter);

    return 0;
}