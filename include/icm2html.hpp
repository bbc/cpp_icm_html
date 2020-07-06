#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include "adm.hpp"
#include "parse.hpp"

#pragma GCC system_header

#ifndef ICM2HTMP_HPP
#define ICM2HTMP_HPP

namespace icm_html_cpp{

    #define DEBUGOUT
    //Some useful debug and error reporting functions.
    #ifdef DEBUGOUT
        #define DEBUG(fmt, ...) fprintf(stdout, "%s:%d (DBG:%s): " fmt, __FILE__, __LINE__, __func__, ## __VA_ARGS__)
        #define ERROR(fmt, ...) fprintf(stderr, "%s:%d (ERR:%s): " fmt, __FILE__, __LINE__, __func__, ## __VA_ARGS__)
    #else
        #define DEBUG(...) do {} while (0)
        #define ERROR(...) do {} while (0)
    #endif 

    
    enum ICM_ERROR_CODE{
        ICM_OK,
        ICM_COULD_NOT_OPEN_FILE,
        ICM_COULD_NOT_PARSE_DATA
    };

    //For reading in ADM files.
    std::shared_ptr<adm::Document> read_adm_xml_file(std::string filePath, ICM_ERROR_CODE& er);


}
#endif