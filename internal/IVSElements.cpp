#include "IVSElements.hpp"

using namespace icm_html_cpp;

IVSElement::IVSElement(IVS_ELEMENT_TYPE element_type) : 
    m_element_type(element_type){}

IVSGainElement::IVSGainElement(float gain_val) :
    IVSElement(IVS_GAIN),
    m_val(gain_val){}

IVSOnOffElement::IVSOnOffElement(bool is_on) :
    IVSElement(IVS_ONOFF),
    m_is_on(is_on){}

IVSPosElement::IVSPosElement(IVS_COORD_TYPE coord_type, float val) :
    IVSElement(IVS_POSITION),
    m_coord_type(coord_type),
    m_val(val){}


IVSComplementaryAudioElement::IVSComplementaryAudioElement(std::shared_ptr<adm::AudioObject> comp_obj) :
    IVSElement(IVS_COMP_OBJECT),
    m_comp_obj(comp_obj){}

IVSComplementaryAudioElement::IVSComplementaryAudioElement(std::string comp_obj_str) :
    IVSElement(IVS_COMP_OBJECT),
    m_co_str(comp_obj_str){}