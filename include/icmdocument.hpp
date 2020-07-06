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


    //To hold the elements of an ICM file - similar to the ADM document.
    class ICMDocument{
        public:
            //ICMDocument();
            //Entry point - file supplied and an ICM is returned.
            static ICMDocument* parse_xml_from_file(char* file_in, std::shared_ptr<adm::Document> the_adm, ICM_ERROR_CODE& error);

            std::vector<std::shared_ptr<Preset>> m_presets;
            std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs;
            std::vector<std::shared_ptr<Control>> m_controls;

            
            void add_preset(rapidxml::xml_node<>* preset_in);
            void add_control(rapidxml::xml_node<>* control_in);
            void add_IVS(rapidxml::xml_node<>* ivs_in);

            void add_continuous_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name);
            void add_option_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name);
            void add_toggle_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name);

            void make_references(std::shared_ptr<adm::Document> the_adm);
            void print_info();
            std::shared_ptr<adm::AudioObject> lookup_audio_elem(std::shared_ptr<adm::Document> the_adm, std::string ao_name);
            std::shared_ptr<Control> lookup_control(std::string control_name);
            std::shared_ptr<InteractiveValueSet> lookup_IVS(std::string IVS_ID);


    };



}




#endif



