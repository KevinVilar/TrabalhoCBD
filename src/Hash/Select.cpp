#include <Hash.hpp>

std::vector<Registro> Hash::SELECT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params) {

    Schema<Registro> schema;
    vhdf::readBlock(mem.getDiskId(), 0, &schema);

    std::vector<Registro> ret_regs = std::vector<Registro>();

    std::vector<Target> targets = parseQuery(schema, params);

    Registro reg;
    int porChave = -1;

    // Verifica se a chave é um dos alvos
    for (int i = 0; i < targets.size(); i++) {
        if (strncmp(targets[i].campo.nm_campo, schema.campos[schema.chave].nm_campo, sizeof(schema.campos[i].nm_campo)) == 0) {
            porChave = i;
            break;
        }
    }

    if (porChave != -1) {
        switch (targets[porChave].tipo) {
            case VALUE: {
                // Funcao hash
                size_t bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[0], schema.blocos_hash);

                // Busca
                mem.loadBlock(bloco+schema.primeiro_bloco);
                while (true) {
                    for (int i = 0; i < schema.regs_por_bloco; i++) {
                        if (mem->isRegistroEscrito(i)) {
                            int match = 0;
                            reg = mem->getRegistro(i);
                            for (int j = 0; j < targets.size(); j++) matchQuery(targets[j], &reg) ? match++ : match;
                            if (match == targets.size()) {
                                ret_regs.push_back(reg);
                            }
                        }
                    }
                    if (mem->overflow != 0) mem.loadBlock(mem->overflow);
                    else break;
                }
                break;
            }
            case SET: {
                for (int k = 0; k < targets[porChave].valor.size(); k++) {
                    // Funcao hash
                    size_t bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[k], schema.blocos_hash);

                    // Busca
                    mem.loadBlock(bloco+schema.primeiro_bloco);
                    while (true) {
                        for (int i = 0; i < schema.regs_por_bloco; i++) {
                            if (mem->isRegistroEscrito(i)) {
                                int match = 0;
                                reg = mem->getRegistro(i);
                                for (int j = 0; j < targets.size(); j++) matchQuery(targets[j], &reg) ? match++ : match;
                                if (match == targets.size()) {
                                    ret_regs.push_back(reg);
                                }
                            }
                        }
                        if (mem->overflow != 0) mem.loadBlock(mem->overflow);
                        else break;
                    }
                }
                break;
            }
            case RANGE: {
                size_t primeiro_bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[0], schema.blocos_hash);
                size_t ultimo_bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[1], schema.blocos_hash);
                
                for (int k = primeiro_bloco; k < ultimo_bloco; k++) {
                    // Busca
                    mem.loadBlock(k+schema.primeiro_bloco);
                    while (true) {
                        for (int i = 0; i < schema.regs_por_bloco; i++) {
                            if (mem->isRegistroEscrito(i)) {
                                int match = 0;
                                reg = mem->getRegistro(i);
                                for (int j = 0; j < targets.size(); j++) matchQuery(targets[j], &reg) ? match++ : match;
                                if (match == targets.size()) {
                                    ret_regs.push_back(reg);
                                }
                            }
                        }
                        if (mem->overflow != 0) mem.loadBlock(mem->overflow);
                        else break;
                    }
                }
                break;
            }
        }

    } else {
        return Heap::SELECT(mem, params);
    }

    return ret_regs;
}
