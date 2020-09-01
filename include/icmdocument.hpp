#include "adm/parse.hpp"
#include "common.hpp"
#include "rapidxml.hpp"
#include <fstream>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#ifndef ICMDOCUMENT_HPP
#define ICMDOCUMENT_HPP

#include "Control.hpp"
#include "InteractiveValueSet.hpp"
#include "Preset.hpp"

namespace icm {

//To hold the elements of an ICM file - similar to the ADM document.
class ICMDocument {
public:
    //ICMDocument();
    /**
     * Entry point - file supplied and an ICM is returned.
     * @param[in] file_in The ICM XML file in a char* format.
     * @param[in] the_adm A shared pointer to the adm::document
     * @param[out] error ICM error code
     * @return a pointer to the ICMDocument
     */
    static ICMDocument *parse_xml_from_file(char *file_in, std::shared_ptr<adm::Document> the_adm, ICM_ERROR_CODE &error);

    std::vector<std::shared_ptr<Preset>>              m_presets;
    std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs;
    std::vector<std::shared_ptr<Control>>             m_controls;

    /**
     * Helper for RapidXML - adds an attribute to a RapidXML node, automatically stringifying and creating memory for that string.
     * @param[in] doc_in A pointer to the rapidxml::xml_document
     * @param[in] node_in A pointer to the rapidxml::xml_node
     * @param[in] attr_name The name of the attribute
     * @param[in] attr_value The value of the attribute (string, int, float, or bool)
     */
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, std::string attr_value);
    /**
     * Helper for RapidXML - adds an attribute to a RapidXML node, automatically stringifying and creating memory for that string.
     * @param[in] doc_in A pointer to the rapidxml::xml_document
     * @param[in] node_in A pointer to the rapidxml::xml_node
     * @param[in] attr_name The name of the attribute
     * @param[in] attr_value The value of the attribute (string, int, float, or bool)
     */
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, int attr_value);
    /**
     * Helper for RapidXML - adds an attribute to a RapidXML node, automatically stringifying and creating memory for that string.
     * @param[in] doc_in A pointer to the rapidxml::xml_document
     * @param[in] node_in A pointer to the rapidxml::xml_node
     * @param[in] attr_name The name of the attribute
     * @param[in] attr_value The value of the attribute (string, int, float, or bool)
     */
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, float attr_value);
    /**
     * Helper for RapidXML - adds an attribute to a RapidXML node, automatically stringifying and creating memory for that string.
     * @param[in] doc_in A pointer to the rapidxml::xml_document
     * @param[in] node_in A pointer to the rapidxml::xml_node
     * @param[in] attr_name The name of the attribute
     * @param[in] attr_value The value of the attribute (string, int, float, or bool)
     */
    static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, bool attr_value);
    //static void add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, const char* attr_value);

    /**
     * Takes an XML node containing a preset and parses into a preset object in the ICMDocument
     * @param[in] preset_in A pointer to a rapidxml node containing the preset
     */
    void add_preset(rapidxml::xml_node<> *preset_in);
    /**
     * Takes an XML node containing a control and parses into a Control object in the ICMDocument
     * @param[in] preset_in A pointer to a rapidxml node containing the control
     */
    void add_control(rapidxml::xml_node<> *control_in);
    /**
     * Takes an XML node containing an InteractiveValueSet and parses into an IVS object in the ICMDocument
     * @param[in] preset_in A pointer to a rapidxml node containing the IVS
     */
    void add_IVS(rapidxml::xml_node<> *ivs_in);

    /**
     * Converts all string references to AudioObjects, IVS, Controls, etc into pointer references.
     * @param[in] the_adm A shared pointer to the ADM document
     */
    void make_references(std::shared_ptr<adm::Document> the_adm);
    /**
     * Writes the ICMDocument to an XML file at the path specified.
     * @param[in] file_path the file path to write to.
     */
    void write_xml_file(std::string file_path);
    //void                                 print_info();

private:
    std::shared_ptr<adm::AudioObject>    lookup_audio_elem(std::shared_ptr<adm::Document> the_adm, std::string ao_name);
    std::shared_ptr<Control>             lookup_control(std::string control_name);
    std::shared_ptr<InteractiveValueSet> lookup_IVS(std::string IVS_ID);

    std::shared_ptr<Control> add_continuous_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);
    std::shared_ptr<Control> add_option_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);
    std::shared_ptr<Control> add_toggle_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);
};

} // namespace icm

#endif
