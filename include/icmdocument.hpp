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
            static ICMDocument* parse_xml_from_file(char* file_in, ICM_ERROR_CODE& error);

            std::vector<std::shared_ptr<Preset>> m_presets;
            std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs;
            std::vector<std::shared_ptr<Control>> m_controls;

            void add_preset(rapidxml::xml_node<>* preset_in);
            void add_control(rapidxml::xml_node<>* control_in);
            void add_IVS(rapidxml::xml_node<>* ivs_in);

            void add_continuous_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name);
            void add_option_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name);
            void add_toggle_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name);



    };



}




#endif



