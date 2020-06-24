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

IVSPosElement::IVSPosElement(std::shared_ptr<adm::AudioObject> the_obj, bool is_polar, float x_or_azimuth = 0, float y_or_elevation = 0, float z_or_distance = 0) :
    IVSElement(IVS_POSITION),
    m_obj(the_obj),
    m_is_polar(is_polar),
    m_x_or_azimuth(x_or_azimuth),
    m_y_or_elevation(y_or_elevation),
    m_z_or_distance(z_or_distance){}


IVSComplementaryAudioElement::IVSComplementaryAudioElement(std::shared_ptr<adm::AudioObject> the_obj, std::shared_ptr<adm::AudioObject> comp_obj) :
    IVSElement(IVS_COMP_OBJECT),
    m_obj(the_obj),
    m_comp_obj(comp_obj){}