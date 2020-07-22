#include "common.hpp"
#include "rapidxml.hpp"
#include <fstream>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include "Preset.hpp"
#include "Control.hpp"
#include "InteractiveValueSet.hpp"


#ifndef ICMDOCUMENT_HPP
#define ICMDOCUMENT_HPP

namespace libicm {

//To hold the elements of an ICM file - similar to the ADM document.
class ICMDocument {
public:
    //ICMDocument();
    //Entry point - file supplied and an ICM is returned.
    static ICMDocument *parse_xml_from_file(char *file_in, std::shared_ptr<adm::Document> the_adm, ICM_ERROR_CODE &error);

    std::vector<std::shared_ptr<Preset>>              m_presets;
    std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs;
    std::vector<std::shared_ptr<Control>>             m_controls;

    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, std::string attr_value);
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, int attr_value);
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, float attr_value);
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, bool attr_value);

    void add_preset(rapidxml::xml_node<> *preset_in);
    void add_control(rapidxml::xml_node<> *control_in);
    void add_IVS(rapidxml::xml_node<> *ivs_in);

    

    void                                 make_references(std::shared_ptr<adm::Document> the_adm);
    void                                 write_xml_file(std::string file_path);
    void                                 print_info();
    std::shared_ptr<adm::AudioObject>    lookup_audio_elem(std::shared_ptr<adm::Document> the_adm, std::string ao_name);
    std::shared_ptr<Control>             lookup_control(std::string control_name);
    std::shared_ptr<InteractiveValueSet> lookup_IVS(std::string IVS_ID);

private:
    void add_continuous_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name);
    void add_option_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name);
    void add_toggle_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name);

};

} // namespace libicm

#endif
