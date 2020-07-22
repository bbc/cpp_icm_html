#include "adm.hpp"
#include "parse.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include "Preset.hpp"
#include "Control.hpp"
#include "chunks.hpp"
#include "InteractiveValueSet.hpp"

#pragma GCC system_header

#ifndef LIBICM_HPP
#define LIBICM_HPP

namespace libicm {

//For reading in ADM files.
std::shared_ptr<adm::Document> read_adm_xml_file(std::string filePath, ICM_ERROR_CODE &er);

} // namespace libicm
#endif