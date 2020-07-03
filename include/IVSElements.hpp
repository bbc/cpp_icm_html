#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include "icm2html.hpp"

#ifndef IVSELEMENTS_HPP
#define IVSELEMENTS_HPP

namespace icm_html_cpp{

enum IVS_ELEMENT_TYPE{
    IVS_GAIN,
    IVS_ONOFF,
    IVS_COMP_OBJECT,
    IVS_POSITION
};

enum IVS_COORD_TYPE{
    IVS_X,
    IVS_Y,
    IVS_Z,
    IVS_AZIMUTH,
    IVS_ELEVATION,
    IVS_DISTANCE
};

class IVSElement{
    public:
        IVSElement(IVS_ELEMENT_TYPE element_type);
        //~IVSElement();
        IVS_ELEMENT_TYPE m_element_type;

};

class IVSGainElement : public IVSElement {
    public:
        IVSGainElement(float gain_val);
        float m_val;
};

class IVSOnOffElement : public IVSElement{
    public:
        IVSOnOffElement(bool is_on);
        bool m_is_on;

};

class IVSPosElement : public IVSElement {
    public:
        IVSPosElement(IVS_COORD_TYPE coord_type, float val);
        IVS_COORD_TYPE m_coord_type;
        float m_val;

};

class IVSComplementaryAudioElement : public IVSElement {
    public:
        IVSComplementaryAudioElement(std::shared_ptr<adm::AudioObject> comp_obj);
        IVSComplementaryAudioElement(std::string comp_obj_str);
        std::shared_ptr<adm::AudioObject> m_comp_obj;
        std::string m_co_str;
};


}

#endif