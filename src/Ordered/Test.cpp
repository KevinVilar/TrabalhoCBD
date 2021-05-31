#include <string>
#include <cstring>
#include <map>
#include <exception>
#include <algorithm>
#include <vhdf.hpp>
#include <Ordered.hpp>

namespace Ordered
{
    MemoryWrapper<DataBlock<Registro>> initialize() {
        MemoryWrapper<DataBlock<Registro>> vhd(vhdf::openDisk("initialDisk.vhd"));
        remove("VHDOrdenado.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDOrdenado.vhd", sizeof(Registro)*25000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = ORDERED;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 2000;
        schema.chave = 11; //NM_CANDIDATO
        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void cleanup(MemoryWrapper<DataBlock<Registro>> mem) {
        vhdf::closeDisk(mem.getDiskId());
    }


    void runTests() {
        MemoryWrapper<DataBlock<Registro>> mem = initialize();

        std::cout << "==================================================================================" << std::endl;

        //INSERT UNICO TEST
        std::vector<Registro> vect;
        Registro reg;
        strncpy(reg.NM_CANDIDATO, "TESTE TESTADO", sizeof(reg.NM_CANDIDATO));
        std::cout << "INSERT NM_CANDIDATO=TESTE TESTADO\nNúmero de acessos a memória: " << std::endl;
        INSERT(mem, {reg});
        mem.blockAccessCount = 0;

        //INSERT MULTIPLO TEST
        std::vector<Registro> reglist(20);
        for(int i=0; i < 20; i++){
            reglist[i].NR_PARTIDO = i;
        }
        std::cout << "\nINSERT LISTA COM 20 REGISTROS\nQuantidade de acessos a blocos:" << std::endl;
        INSERT(mem, reglist);
        mem.blockAccessCount = 0;

        //SELECT UM VALOR TEST        
        std::cout << "\nSelecionando um registro de teste\n";
        std::cout << "SELECT NM_CANDIDATO=TESTE TESTADO\nNúmero de acessos a memória: " <<std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=TESTE TESTADO"});

        std::cout << "\nSELECT NM_CANDIDATO=AUGUSTO LUIZ DE LIMA \nNúmero de acessos a memória: " <<std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=AUGUSTO LUIZ DE LIMA"});

        std::cout << "\nSELECT NM_CANDIDATO=RADAMES VIEIRA DA SILVA \nNúmero de acessos a memória: " <<std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=RADAMES VIEIRA DA SILVA"});

        std::cout << "\nSELECT NM_CANDIDATO=JUPIRA MATTE\nNúmero de acessos a memória: "  << std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=JUPIRA MATTE"});

        std::cout << "\nSELECT NM_CANDIDATO=GESSÉ FREIRE PINHEIRO \nNúmero de acessos a memória: "  << std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=GESSÉ FREIRE PINHEIRO"});

        std::cout << "\nSELECT NM_CANDIDATO=OSMAR CHUKA \nNúmero de acessos a memória: "  << std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=OSMAR CHUKA"});

        mem.blockAccessCount = 0;


        //SELECT MULTIPLOS REGISTROS
        std::cout << "\nSelecionando múltiplos registros de teste\n";
        std::cout << "SELECT NM_CANDIDATO={TESTE TESTADO,OSMAR CHUKA}\nNúmero de acessos a memória: "   <<std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO={TESTE TESTADO,OSMAR CHUKA}"});
        mem.blockAccessCount = 0;

        //SELECT FAIXA
        std::cout << "\nSelecionando faixa de registros de teste\n";
        std::cout << "SELECT NR_PARTIDO=[10:14]\nNúmero de acessos a memória: " << std::endl;
        vect = SELECT(mem, {"NR_PARTIDO=[10:14]"});
        mem.blockAccessCount = 0;

        //SELECT 2 VALORES FIXOS
        std::cout << "\nSelecionando registros de teste por valores fixos\n";
        std::cout << "SELECT NR_PARTIDO=12,NM_CANDIDATO=JUPIRA MATTE\nNúmero de acessos a memória:  " << std::endl;
        vect = SELECT(mem, {"NR_PARTIDO=12","NM_CANDIDATO=JUPIRA MATTE"});
        mem.blockAccessCount = 0;

        //DELETE UM REG
        std::cout << "\nDeletando um registro de teste\n";
        std::cout << "DELETE NM_CANDIDATO=TESTE TESTADO \nNúmero de acessos a memória:  "  << std::endl;
        DELETE(mem, {"NM_CANDIDATO=TESTE TESTADO"});
        mem.blockAccessCount = 0;

        //DELETE MULTIPLOS REG
        std::cout << "\nDeletando múltiplos registros de teste\n";
        std::cout << "DELETE NM_CANDIDATO={GESSÉ FREIRE PINHEIRO,RADAMES VIEIRA DA SILVA} \nNúmero de acessos a memória:   "  << std::endl;
        DELETE(mem, {"NM_CANDIDATO={GESSÉ FREIRE PINHEIRO,OSMAR CHUKA}"});
        mem.blockAccessCount = 0;

        //DELETE FAIXA
        std::cout << "\nDeletando faixa de registros de teste\n";
        std::cout << "DELETE NR_PARTIDO=[10:13] \nNúmero de acessos a memória:  "  << std::endl;
        DELETE(mem, {"NR_PARTIDO=[10:13]"});
        mem.blockAccessCount = 0;

        //DELETE 2 VALORES FIXOS
        std::cout << "\nDeletando dois registros de teste por valores fixos\n";
        std::cout << "DELETE NR_PARTIDO=12,NM_CANDIDATO=OSMAR CHUKA \nNúmero de acessos a memória:  "  << std::endl;
        DELETE(mem, {"NR_PARTIDO=12","NM_CANDIDATO=OSMAR CHUKA"});
        mem.blockAccessCount = 0;

        std::cout << "==================================================================================" << std::endl;

        //vhdf::closeDisk(vhd.getDiskId());
        cleanup(mem);
    };

}