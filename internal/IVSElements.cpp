#include "IVSElements.hpp"

using namespace icm_html_cpp;

IVSElement::IVSElement(IVS_ELEMENT_TYPE element_type) : 
    m_element_type(element_type){}

IVSGainElement::IVSGainElement(std::shared_ptr<adm::AudioObject> the_obj, float gain_val) :
    IVSElement(IVS_GAIN),
    m_obj(the_obj),
    m_val(gain_val){}

IVSOnOffElement::IVSOnOffElement(std::shared_ptr<adm::AudioObject> the_obj, bool is_on) :
    IVSElement(IVS_ONOFF),
    m_obj(the_obj),
    m_is_on(is_on){}

