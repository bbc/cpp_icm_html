#ifndef IVSELEMENTS_HPP
#define IVSELEMENTS_HPP

#include "icm2html.hpp"
#include "rapidxml.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

namespace icm_html_cpp {

enum IVS_ELEMENT_TYPE {
    IVS_GAIN,
    IVS_ONOFF,
    IVS_COMP_OBJECT,
    IVS_POSITION
};

enum IVS_COORD_TYPE {
    IVS_X,
    IVS_Y,
    IVS_Z,
    IVS_AZIMUTH,
    IVS_ELEVATION,
    IVS_DISTANCE
};

class IVSElement {
public:
    IVSElement(IVS_ELEMENT_TYPE element_type);
    //~IVSElement();
    IVS_ELEMENT_TYPE m_element_type;
    virtual void     add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node) = 0;
};

class IVSGainElement : public IVSElement {
public:
    IVSGainElement(float gain_val);
    float m_val;
    void  add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

class IVSOnOffElement : public IVSElement {
public:
    IVSOnOffElement(bool is_on);
    bool m_is_on;
    void add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

class IVSPosElement : public IVSElement {
public:
    IVSPosElement(IVS_COORD_TYPE coord_type, float val);
    IVS_COORD_TYPE m_coord_type;
    float          m_val;
    void           add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

class IVSComplementaryAudioElement : public IVSElement {
public:
    IVSComplementaryAudioElement(std::shared_ptr<adm::AudioObject> comp_obj);
    IVSComplementaryAudioElement(std::string comp_obj_str);
    std::shared_ptr<adm::AudioObject> m_comp_obj;
    std::string                       m_co_str;
    void                              add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

//Represents an InteractiveValueSet element from the ICM
class InteractiveValueSet {
public:
    InteractiveValueSet(std::string ID);
    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    std::string                                                            m_IVS_ID;
    std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> m_AOs;
    std::vector<IVSElement *>                                              m_elements;
};

} // namespace icm_html_cpp

#endif