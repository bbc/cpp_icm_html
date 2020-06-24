#include "icm2html.hpp"
#include "icmelements.hpp"
#include <string>
#include <memory>
#include <vector>
#include <stdio.h>

namespace icm_html_cpp{

InteractiveValueSet::InteractiveValueSet(std::string ID) :
    m_IVS_ID(ID){}

std::vector<std::shared_ptr<InteractiveValueSet>> InteractiveValueSet::read_IVSs_from_xml(std::stringstream xml_in, std::shared_ptr<adm::Document> the_adm){

}

Preset::Preset(std::string preset_ID, std::string preset_name, std::string label, std::string audio_prog_ref) : 
    m_preset_ID(preset_ID),
    m_preset_name(preset_name),
    m_label(label),
    m_audio_prog_ref(audio_prog_ref){}

std::vector<std::shared_ptr<Preset>> Preset::read_presets_from_xml(std::stringstream xml_in, std::shared_ptr<adm::Document> the_adm, std::vector<std::shared_ptr<InteractiveValueSet>> IVSs_in){

}

std::string Preset::generate_html(ICM_ERROR_CODE& err){

}

ICM_ERROR_CODE Preset::add_IVS(InteractiveValueSet* IVS_in){

}

std::shared_ptr<Preset> Preset::parse_single_preset(std::stringstream preset_xml_in){
    
}



}