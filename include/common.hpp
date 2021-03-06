#include "adm.hpp"

#ifndef COMMON_HPP
#define COMMON_HPP

namespace icm {

#define DEBUGOUT
//Some useful debug and error reporting functions.
#ifdef DEBUGOUT
#define DEBUG(fmt, ...) fprintf(stdout, "%s:%d (DBG:%s): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define ERROR(fmt, ...) fprintf(stderr, "%s:%d (ERR:%s): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define DEBUG(...) \
    do {           \
    } while (0)
#define ERROR(...) \
    do {           \
    } while (0)
#endif

///Types of error in ICM
enum ICM_ERROR_CODE {
    ICM_OK,
    ICM_COULD_NOT_OPEN_FILE,
    ICM_COULD_NOT_PARSE_DATA
};

/**
 * Reads the ADM file using the libADM library and returns a shared pointer to an adm::Document
 * @param[in] filePath The path of the file
 * @param[out] er Error code
 * @return A shared pointer to an adm::Document
 */
std::shared_ptr<adm::Document> read_adm_xml_file(std::string filePath, ICM_ERROR_CODE &er);


} // namespace icm

#endif