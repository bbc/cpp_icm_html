#include "icm2html.hpp"
#include "icmelements.hpp"
#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include <stdbool.h>



namespace icm_html_cpp{

InteractiveValueSet::InteractiveValueSet(std::string ID) :
    m_IVS_ID(ID){}

std::vector<std::shared_ptr<InteractiveValueSet>>* InteractiveValueSet::read_IVSs_from_xml(rapidxml::xml_document<>* doc_in, std::shared_ptr<adm::Document> the_adm){
    bool ivs_has_been_found = false;

    


    auto first_ivs = doc_in->first_node("interactiveValueSet");
    if(first_ivs){
        std::vector<std::shared_ptr<InteractiveValueSet>>* the_IVSs = new std::vector<std::shared_ptr<InteractiveValueSet>>();
        auto attrib = first_ivs->first_attribute();
        std::string attrib_name(attrib->name());
        if(attrib_name != "interactiveValueSetID") ERROR("Unrecognised attribute in interactiveValueSetID: %s \n", attrib_name.c_str());
        else {

        }
        
        
        //while(attrib != 0)
        //    std::string attrib_name(attrib->name());
        //    if(name == )

    } else return 0;
 
    

    
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


Control::Control(CONTROL_TYPE control_type, std::string control_id, std::string control_name, bool control_is_conditional, std::string start_time, std::string end_time, adm::AudioProgramme* prog_ref) :
    m_control_type(control_type),
    m_control_ID(control_id),
    m_control_name(control_name),
    m_control_is_conditional(control_is_conditional),
    m_start_time(start_time),
    m_end_time(end_time),
    m_programme_ref(prog_ref){}


ContinuousControl::ContinuousControl(std::string control_label, std::string control_id, std::string control_name,
                                float min, float max, float step,
                                bool control_is_conditional, std::string start_time, std::string end_time, adm::AudioProgramme* prog_ref) :
    Control(CONTROL_CONTINUOUS, control_id, control_name, control_is_conditional, start_time, end_time, prog_ref),
    m_label(control_label){
        m_range.r_max = max;
        m_range.r_min = min;
        m_range.r_step = step;
    }

ICM_ERROR_CODE ContinuousControl::add_variable(adm::AudioObject* audio_object, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type){
    
    variable the_var{
        audio_object,
        param,
        min,
        max,
        scale_type,        
    };

    m_variables.push_back(the_var);
}

ToggleControl::ToggleControl(std::string control_label, std::string control_id, std::string control_name,
                            bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0) : 
     Control(CONTROL_TOGGLE, control_id, control_name, control_is_conditional, start_time, end_time, prog_ref),
     m_label(control_label){}


icm_html_cpp::ToggleControl::state* ToggleControl::add_state_info(std::string label, bool on, std::vector<adm::AudioObject*>* audio_objects, std::vector<adm::AudioObject*>* audio_objects_comp){
    auto state_to_edit = on ? &m_toggle_on : &m_toggle_off;

    state_to_edit->s_label=label;
    state_to_edit->s_    


}




}//namespace

