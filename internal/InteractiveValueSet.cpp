#include "InteractiveValueSet.hpp"
#include "ICMElements.hpp"
#include "icmdocument.hpp"
#include "rapidxml.hpp"

using namespace icm_html_cpp;

InteractiveValueSet::InteractiveValueSet(std::string ID) : m_IVS_ID(ID) {}

void InteractiveValueSet::add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) {
    rapidxml::xml_node<> *ivs_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("interactiveValueSet"));
    for (auto ao : m_AOs) {
        rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("audioObjectIDRef"), xml_in->allocate_string(ao.second.c_str()));
        ivs_node->append_node(var_ao_node);
    }

    for (auto elem : m_elements)
        elem->add_xml_to_node(xml_in, ivs_node);
    ai_node->append_node(ivs_node);
}

IVSElement::IVSElement(IVS_ELEMENT_TYPE element_type) : m_element_type(element_type) {}

IVSGainElement::IVSGainElement(float gain_val) : IVSElement(IVS_GAIN),
                                                 m_val(gain_val) {}

void IVSGainElement::add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node) {
    rapidxml::xml_node<> *gain_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("gain"), xml_in->allocate_string(std::to_string(m_val).c_str()));
    ivs_node->append_node(gain_node);
}

IVSOnOffElement::IVSOnOffElement(bool is_on) : IVSElement(IVS_ONOFF),
                                               m_is_on(is_on) {}

void IVSOnOffElement::add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node) {
    rapidxml::xml_node<> *onoff_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("onOff"), xml_in->allocate_string(std::to_string((int)m_is_on).c_str()));
    ivs_node->append_node(onoff_node);
}
IVSPosElement::IVSPosElement(IVS_COORD_TYPE coord_type, float val) : IVSElement(IVS_POSITION),
                                                                     m_coord_type(coord_type),
                                                                     m_val(val) {}

void IVSPosElement::add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node) {
    std::vector<std::string> postypes = {"X", "Y", "Z", "azimuth", "elevation", "distance"};

    rapidxml::xml_node<> *pos_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("positionOffset"), xml_in->allocate_string(std::to_string(m_val).c_str()));

    ICMDocument::add_attr_to_node(xml_in, pos_node, "coordinate", postypes[m_coord_type]);

    ivs_node->append_node(pos_node);
}

IVSComplementaryAudioElement::IVSComplementaryAudioElement(std::shared_ptr<adm::AudioObject> comp_obj) : IVSElement(IVS_COMP_OBJECT),
                                                                                                         m_comp_obj(comp_obj) {}

IVSComplementaryAudioElement::IVSComplementaryAudioElement(std::string comp_obj_str) : IVSElement(IVS_COMP_OBJECT),
                                                                                       m_co_str(comp_obj_str) {}

void IVSComplementaryAudioElement::add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node) {
    rapidxml::xml_node<> *cc_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("onOff"), xml_in->allocate_string(m_co_str.c_str()));
    ivs_node->append_node(cc_node);
}