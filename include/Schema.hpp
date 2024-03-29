#pragma once
#include <Util.hpp>
#include <Data.hpp>
#include <Campo.hpp>
#include <Timestamp.hpp>
#include <DataBlock.hpp>

enum Organizacao { HEAP, ORDERED, HASH, BTREE };    

template <typename T>
class Schema
{
//private:
    // Vetor de tamanhos e localizações relativas dos campos e do registro (usar função initRegistro abaixo)
    // Ponteiros para estruturas secundárias como o identificador de registros deletados
    // Timestamps relevantes à base
public:
    size_t primeiro_bloco, ultimo_bloco, blocos_hash;
    unsigned int  tamanho, regs_por_bloco, offset, chave;
    size_t ptr_registros_deletados;
    Timestamp criacao, alteracao;
    int nCampos;
    Campo campos[T::nCampos()];
    Organizacao org;

    Schema() {
        std::vector<Campo> cps = T::initHEAD();
        memcpy(campos, &cps[0], T::nCampos()*sizeof(Campo));
        nCampos = cps.size();
        tamanho = sizeof(T);
        regs_por_bloco = vhdf::BLOCK_SIZE/tamanho;
    };
private:
    char padding[vhdf::BLOCK_SIZE - (sizeof(size_t)*3 + sizeof(int)*5 + sizeof(Organizacao) + sizeof(Timestamp)*2 + sizeof(Campo)*T::nCampos())];
};


