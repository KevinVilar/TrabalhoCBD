#include <vhdf.hpp>
#include <iostream>
#include <Registro.hpp>
#include <Parser.hpp>
#include <Util.hpp>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>
#include <Schema.hpp>
#include <Hash.hpp>
#include <Ordered.hpp>
#include <Heap.hpp>
#include <Index.hpp>

int main(int argc, char const *argv[])
{
    if (Registro::nPorBloco() < 0) throw std::runtime_error("Erro: Tamanho do registro é grande de mais (>1 bloco)");
    int nreg = 20079;
    std::cout << "Tamanho do registro: " << sizeof(Registro) << " bytes." << std::endl;
    std::cout << "Tamanho do bloco: " << vhdf::BLOCK_SIZE << " bytes (" << Registro::nPorBloco() << " registros por bloco)." << std::endl;
    std::cout << "Quantidade de blocos: " <<  std::floor(sizeof(Registro)*nreg/4096) << std::endl;

    std::cout << "\nParseando csv...";
    std::ifstream ifs;
    ifs.open("consulta_cand_2020_BRASIL.csv", std::ifstream::in);
    std::string str;
    std::getline(ifs, str);

    std::cout << "\nInicializando vhd...";
    remove("testdisk.vhd");
    int vhd = vhdf::openDisk("initialDisk.vhd", sizeof(Registro)*nreg, true);
    if (vhd == -1) {
        std::cout << "\nErro ao iniciar vhd." << std::endl;
        return -1;
    }
    std::cout << "\nIncicializacao feita com sucesso!" << std::endl;

    parseStream<Registro>(ifs, vhd, 1, nreg);
    ifs.clear();
    ifs.seekg(0, std::ios::beg);

    std::cout << "\nExecutando testes\n" << std::endl;

    std::cout << "Armazenamento utilizando heap" << std::endl; 
    Heap::runTests();

    std::cout << "Armazenamento ordenado" << std::endl;
    Ordered::runTests();

    std::cout << "Armazenamento utilizando hash" << std::endl;
    Hash::runTests();

    
    ifs.close();
    vhdf::closeDisk(vhd);

    return 0;
}
