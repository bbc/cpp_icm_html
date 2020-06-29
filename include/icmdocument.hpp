#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include <fstream>
#include "rapidxml.hpp"
#include "icm2html.hpp"
#include "icmelements.hpp"

#ifndef ICMDOCUMENT_HPP
#define ICMDOCUMENT_HPP

namespace icm_html_cpp {

    class ICMDocument{
        public:
            ICMDocument();
            static rapidxml::xml_document<>* parse_xml_from_file(char* file_in, ICM_ERROR_CODE& error);

            std::vector<std::shared_ptr<Preset>> m_presets;
            std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs;
            std::vector<

    };



}




#endif



