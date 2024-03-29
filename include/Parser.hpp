#pragma once
#include <fstream>
#include <string>
#include <vhdf.hpp>
#include <cmath>
#include <cstring>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <Schema.hpp>
#include <Registro.hpp>
#include <DataBlock.hpp>

template <typename T>
void parseStream(std::ifstream& ifs, int vhd, int blockoffset, int numRegistros) {
    std::string str;
    size_t blocksprocessed = blockoffset;
    size_t regsprocessed = 0;
    size_t totalregsprocessed = 0;

    MemoryWrapper<DataBlock<T>> mem(vhd);
    mem.loadBlock(blocksprocessed);

    Schema<T> schema = Schema<T>();
    schema.primeiro_bloco = blockoffset;

    while(getline(ifs, str) && totalregsprocessed < numRegistros) {
        //Comitta no vhd caso o numero de registros por bloco seja atingido
        if (regsprocessed == T::nPorBloco()) {
            mem.commitBlock();
            regsprocessed = 0;
            blocksprocessed++;
            mem.loadBlock(blocksprocessed);
        }
        T reg(str);
        mem->setRegistro(regsprocessed, reg);
        regsprocessed++;
        totalregsprocessed++;
    }
    mem.commitBlock();
    blocksprocessed++;

    schema.ultimo_bloco = blocksprocessed;
    vhdf::writeBlock(vhd, 0, &schema);
}



