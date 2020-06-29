#include "icmdocument.hpp"

namespace icm_html_cpp {

    rapidxml::xml_document<>* ICMDocument::parse_xml_from_file(char* file_in, ICM_ERROR_CODE& error){
        rapidxml::xml_document<>* the_icm_xml = new rapidxml::xml_document<>();
        the_icm->parse<0>(file_in);
        //maybe do some validity checking

        return the_icm;

    }

}