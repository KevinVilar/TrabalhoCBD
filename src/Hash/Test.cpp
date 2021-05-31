#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <exception>
#include <algorithm>
#include <vhdf.hpp>
#include <Hash.hpp>

namespace Hash
{
    MemoryWrapper<DataBlock<Registro>> initialize() {
        MemoryWrapper<DataBlock<Registro>> vhd(vhdf::openDisk("initialDisk.vhd"));
        remove("VHDHash.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDHash.vhd", sizeof(Registro)*20079, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 2000;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
        schema.chave = 11; //NM_CANDIDATO

        for (int i = 1; i < schema.blocos_hash+1; i++) {
            mem.loadBlock(i);
            mem->initialize();
            mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void cleanup(MemoryWrapper<DataBlock<Registro>> mem) {
        vhdf::closeDisk(mem.getDiskId());
    }

    size_t calculateHash(Campo campo, std::string chave, size_t tamanho) {
        switch (campo.tipo) {
            case INT:
                return hashingFunc(tamanho, std::stoi(chave));
            case BIGINT:
                return hashingFunc(tamanho, std::atoll(chave.c_str()));
            case CHAR:
                return hashingFunc(tamanho, chave);
            default:
                throw std::invalid_argument("O tipo de campo "+std::string(campo.nm_campo, sizeof(campo.nm_campo))+" nao e valido.");
        }
    }
    
    void runTests() {
        MemoryWrapper<DataBlock<Registro>> mem = initialize();
        
        std::cout << "==================================================================================" << std::endl;

        //INSERT UNICO TEST
        std::vector<Registro> vect;
        Registro reg;
        strncpy(reg.NM_CANDIDATO, "TESTE TESTADO", sizeof(reg.NM_CANDIDATO));
        INSERT(mem, {reg});
        std::cout << "INSERT NM_CANDIDATO=TESTE TESTADO\nNúmero de acessos a memória: " << mem.blockAccessCount << "(calcula hash dos blocos)" << std::endl;
        mem.blockAccessCount = 0;

        //INSERT UNICO TEST
        
        strncpy(reg.NM_CANDIDATO, "TESTE TESTADO 2", sizeof(reg.NM_CANDIDATO));
        INSERT(mem, {reg});
        std::cout << "INSERT NM_CANDIDATO=TESTE TESTADO 2\nNúmero de acessos a memória: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //INSERT MULTIPLO TEST
        std::vector<Registro> reglist(20);
        for(int i=0; i < 20; i++){
            reglist[i].NR_PARTIDO = i;
        }
        INSERT(mem, reglist);
        std::cout << "\nINSERT LISTA COM 20 REGISTROS\nQuantidade de acessos a blocos:" << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //SELECT UM VALOR TEST
        
        std::cout << "\nSelecionando um registro de teste\n";
        vect = SELECT(mem, {"NM_CANDIDATO=TESTE TESTADO"});
        std::cout << "SELECT NM_CANDIDATO=TESTE TESTADO\nNúmero de acessos a memória: "<< mem.blockAccessCount <<std::endl;

        vect = SELECT(mem, {"NM_CANDIDATO=AUGUSTO LUIZ DE LIMA"});
        std::cout << "\nSELECT NM_CANDIDATO=AUGUSTO LUIZ DE LIMA \nNúmero de acessos a memória: "<< mem.blockAccessCount  <<std::endl;

        vect = SELECT(mem, {"NM_CANDIDATO=RADAMES VIEIRA DA SILVA"});
        std::cout << "\nSELECT NM_CANDIDATO=RADAMES VIEIRA DA SILVA \nNúmero de acessos a memória: " << mem.blockAccessCount <<std::endl;

        vect = SELECT(mem, {"NM_CANDIDATO=JUPIRA MATTE"});
        std::cout << "\nSELECT NM_CANDIDATO=JUPIRA MATTE\nNúmero de acessos a memória: " << mem.blockAccessCount << std::endl;

        vect = SELECT(mem, {"NM_CANDIDATO=GESSÉ FREIRE PINHEIRO"});
        std::cout << "\nSELECT NM_CANDIDATO=GESSÉ FREIRE PINHEIRO \nNúmero de acessos a memória: " << mem.blockAccessCount << std::endl;

        vect = SELECT(mem, {"NM_CANDIDATO=OSMAR CHUKA"});
        std::cout << "\nSELECT NM_CANDIDATO=OSMAR CHUKA \nNúmero de acessos a memória: " << mem.blockAccessCount << std::endl;

        std::cout << "\nNúmero de acessos a memória médio: " << mem.blockAccessCount/6 << "\n"<< std::endl;
        mem.blockAccessCount = 0;


        //SELECT MULTIPLOS REGISTROS
        std::cout << "\nSelecionando múltiplos registros de teste\n";
        vect = SELECT(mem, {"NM_CANDIDATO={TESTE TESTADO,OSMAR CHUKA}"});
        std::cout << "SELECT NM_CANDIDATO={TESTE TESTADO,OSMAR CHUKA}\nNúmero de acessos a memória: " << mem.blockAccessCount  <<std::endl;
        mem.blockAccessCount = 0;

        //SELECT FAIXA
        std::cout << "\nSelecionando faixa de registros de teste\n";
        vect = SELECT(mem, {"NR_PARTIDO=[10:14]"});
        std::cout << "SELECT NR_PARTIDO=[10:14]\nNúmero de acessos a memória: " << mem.blockAccessCount<< std::endl;
        mem.blockAccessCount = 0;

        //SELECT 2 VALORES FIXOS
        std::cout << "\nSelecionando registros de teste por valores fixos\n";
        vect = SELECT(mem, {"NR_PARTIDO=12","NM_CANDIDATO=JUPIRA MATTE"});
        std::cout << "SELECT NR_PARTIDO=12,NM_CANDIDATO=JUPIRA MATTE\nNúmero de acessos a memória:  " <<  mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE UM REG
        std::cout << "\nDeletando um registro de teste\n";
        DELETE(mem, {"NM_CANDIDATO=TESTE TESTADO"});
        std::cout << "DELETE NM_CANDIDATO=TESTE TESTADO \nNúmero de acessos a memória:  " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE MULTIPLOS REG
        std::cout << "\nDeletando múltiplos registros de teste\n";
        DELETE(mem, {"NM_CANDIDATO={GESSÉ FREIRE PINHEIRO,OSMAR CHUKA}"});
        std::cout << "DELETE NM_CANDIDATO={GESSÉ FREIRE PINHEIRO,RADAMES VIEIRA DA SILVA} \nNúmero de acessos a memória:   " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE FAIXA
        std::cout << "\nDeletando faixa de registros de teste\n";
        DELETE(mem, {"NR_PARTIDO=[10:13]"});
        std::cout << "DELETE NR_PARTIDO=[10:13] \nNúmero de acessos a memória:  " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE 2 VALORES FIXOS
        std::cout << "\nDeletando dois registros de teste por valores fixos\n";
        DELETE(mem, {"NR_PARTIDO=12","NM_CANDIDATO=OSMAR CHUKA"});
        std::cout << "DELETE NR_PARTIDO=12,NM_CANDIDATO=OSMAR CHUKA \nNúmero de acessos a memória:  " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        std::cout << "==================================================================================" << std::endl;
        
        cleanup(mem);
    };

}