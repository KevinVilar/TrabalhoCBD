#include <Heap.hpp>

bool Heap::DELETE(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params) {
    Schema<Registro> schema;
    vhdf::readBlock(mem.getDiskId(), 0, &schema);

    std::vector<Target> targets = parseQuery(schema, params);

    Registro reg;
    
    for (size_t i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
        mem.loadBlock(i);
        bool changed = false;
        for (int j = 0; j < mem->registrosEscritos.size(); j++) {
            reg = mem->getRegistro(j);

            int match = 0;
            for (int k = 0; k < targets.size(); k++) matchQuery(targets[k], &reg) ? match++ : match;
            if (match == targets.size()) {
                mem->deleteRegistro(j);
                changed = true;
            }
        }
        if (changed) mem.commitBlock();
    }
    return true;
}
