#ifndef IVSELEMENTS_HPP
#define IVSELEMENTS_HPP

#include "common.hpp"
#include "rapidxml.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

namespace icm {

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

///Represents an element within an IVS
class IVSElement {
public:
    /**IVSElement Constructor.
     * @param[in] element_type An enumerated IVS_ELEMENT_TYPE.
     */
    IVSElement(IVS_ELEMENT_TYPE element_type);
    //~IVSElement();
    IVS_ELEMENT_TYPE m_element_type;

    /**
     * Adds the XML for the control into a RapidXML parent XML node.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ivs_node The parent InteractiveValueSet node to attach this control's XML to
     */
    virtual void add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node) = 0;
};

class IVSGainElement : public IVSElement {
public:
    /**
     * IVSGainElement Constructor
     * @param[in] gain_val the value to set the gain of this element to.
     */
    IVSGainElement(float gain_val);
    float m_val;

    /**
     * Adds the XML for the control into a RapidXML parent XML node.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ivs_node The parent InteractiveValueSet node to attach this control's XML to
     */
    void add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

class IVSOnOffElement : public IVSElement {
public:
    /**
     * IVSOnOffElement Constructor
     * @param[in] is_on The state to set this element to.
     */
    IVSOnOffElement(bool is_on);
    bool m_is_on;

    /**
     * Adds the XML for the control into a RapidXML parent XML node.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ivs_node The parent InteractiveValueSet node to attach this control's XML to
     */
    void add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

class IVSPosElement : public IVSElement {
public:
    /**
     * IVSOnOffElement Constructor
     * @param[in] coord_type An enumerated representation of the co-ordinate type.
     * @param[in] val The value of the position.
     */
    IVSPosElement(IVS_COORD_TYPE coord_type, float val);
    IVS_COORD_TYPE m_coord_type;
    float          m_val;

    /**
     * Adds the XML for the control into a RapidXML parent XML node.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ivs_node The parent InteractiveValueSet node to attach this control's XML to
     */
    void add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

class IVSComplementaryAudioElement : public IVSElement {
public:

    /**
     * IVSComplementaryAudioElement Constructor
     * @param[in] comp_obj A pointer to the complementary object.
     */
    IVSComplementaryAudioElement(std::shared_ptr<adm::AudioObject> comp_obj);

    /**
     * IVSComplementaryAudioElement Constructor
     * @param[in] comp_obj_str A string containing the complementary object's name.
     */
    IVSComplementaryAudioElement(std::string comp_obj_str);
    std::shared_ptr<adm::AudioObject> m_comp_obj;
    std::string                       m_co_str;

    /**
     * Adds the XML for the control into a RapidXML parent XML node.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ivs_node The parent InteractiveValueSet node to attach this control's XML to
     */
    void add_xml_to_node(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ivs_node);
};

//Represents an InteractiveValueSet element from the ICM
class InteractiveValueSet {
public:
    InteractiveValueSet(std::string ID);

    /**
     * Adds the XML for the control into a RapidXML parent XML node.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ai_node The parent node to attach this control's XML to
     */
    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    std::string                                                            m_IVS_ID;
    std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> m_AOs;
    std::vector<IVSElement *>                                              m_elements;
};

} // namespace icm

#endif