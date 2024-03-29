#include <Campo.hpp>
#include <Util.hpp>
#include <Registro.hpp>
#include <exception>
#include <stdexcept>

bool comparaCampo( Campo campo, const void* reg, std::string valor, const std::string &opr) {
    const void* cmp_ptr = nullptr;

    enum OP { IGUAL, DIFF, MENOR, MAIOR, MEIG, MAIG };

    OP operacao;
    if (opr == "==") operacao = IGUAL;
    else if (opr == "!=") operacao = DIFF;
    else if (opr == "<") operacao = MENOR;
    else if (opr == ">") operacao = MAIOR;
    else if (opr == "<=") operacao = MEIG;
    else if (opr == ">=") operacao = MAIG;
    else throw std::invalid_argument("Operation not supported: '"+opr+"'");

    switch (campo.tipo) {
        case INT:
            cmp_ptr = static_cast<const char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if (*reinterpret_cast<const int*>(cmp_ptr) == std::stoi(valor)) return true;
                    break;
                case DIFF:
                    if (*reinterpret_cast<const int*>(cmp_ptr) == std::stoi(valor)) return false;
                    break;
                case MENOR:
                    if (*reinterpret_cast<const int*>(cmp_ptr) < std::stoi(valor)) return true;
                    break;
                case MAIOR:
                    if (*reinterpret_cast<const int*>(cmp_ptr) > std::stoi(valor)) return true;
                    break;
                case MEIG:
                    if (*reinterpret_cast<const int*>(cmp_ptr) <= std::stoi(valor)) return true;
                    break;
                case MAIG:
                    if (*reinterpret_cast<const int*>(cmp_ptr) >= std::stoi(valor)) return true;
                    break;
            }
            break;
        case BIGINT:
            cmp_ptr = static_cast<const char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if (*reinterpret_cast<const size_t*>(cmp_ptr) == std::atoll(valor.c_str())) return true;
                    break;
                case DIFF:
                    if (*reinterpret_cast<const size_t*>(cmp_ptr) == std::atoll(valor.c_str())) return false;
                    break;
                case MENOR:
                    if (*reinterpret_cast<const size_t*>(cmp_ptr) < std::atoll(valor.c_str())) return true;
                    break;
                case MAIOR:
                    if (*reinterpret_cast<const size_t*>(cmp_ptr) > std::atoll(valor.c_str())) return true;
                    break;
                case MEIG:
                    if (*reinterpret_cast<const size_t*>(cmp_ptr) <= std::atoll(valor.c_str())) return true;
                    break;
                case MAIG:
                    if (*reinterpret_cast<const size_t*>(cmp_ptr) >= std::atoll(valor.c_str())) return true;
                    break;
            }
            break;
        case CHAR:
            cmp_ptr = static_cast<const char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if ( strncmp(reinterpret_cast<const char*>(cmp_ptr), valor.c_str(), campo.tamanho) == 0) return true;
                    break;
                case DIFF:
                    if ( strncmp(reinterpret_cast<const char*>(cmp_ptr), valor.c_str(), campo.tamanho) == 0) return false;
                    break;
                case MENOR:
                    if ( strncmp(reinterpret_cast<const char*>(cmp_ptr), valor.c_str(), campo.tamanho) < 0) return true;
                    break;
                case MAIOR:
                    if ( strncmp(reinterpret_cast<const char*>(cmp_ptr), valor.c_str(), campo.tamanho) > 0) return true;
                    break;
                case MEIG:
                    if ( strncmp(reinterpret_cast<const char*>(cmp_ptr), valor.c_str(), campo.tamanho) <= 0) return true;
                    break;
                case MAIG:
                    if ( strncmp(reinterpret_cast<const char*>(cmp_ptr), valor.c_str(), campo.tamanho) >= 0) return true;
                    break;
            }
            break;
            
        case BOOL:
            cmp_ptr = static_cast<const char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if (*reinterpret_cast<const bool*>(cmp_ptr) == SNtoBool(valor)) return true;
                    break;
                case DIFF:
                    if (*reinterpret_cast<const bool*>(cmp_ptr) == SNtoBool(valor)) return false;
                    break;
                default:
                    throw std::invalid_argument("Operacao invalida para campo de tipo BOOL ('"+opr+")");
            }
            break;
        default:
            return false;
    }
    return false;
}

std::string getValorCampo(Campo campo, const void* reg) {
    std::string valor = "";
    const void* val_ptr = reinterpret_cast<const char*>(reg) + campo.pos_relativa;
    
    switch (campo.tipo) {
        case INT:
            valor = std::to_string(*(int*)val_ptr);
            break;
        case BIGINT:
            valor = std::to_string(*(size_t*)val_ptr);
            break;
        case CHAR: {
            int len = strnlen((char*)val_ptr, campo.tamanho);
            valor = std::string((char*)val_ptr, len);
            }
            break;
        case BOOL:
            *(bool*)val_ptr == true? valor = "S" : valor = "N";
            break;
        default:
            throw std::runtime_error("NotImplementedException");
    }
    return valor;
}