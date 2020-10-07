#include "../include/icmdocument.hpp"
#include <iostream>
#include <string>

#include "rapidxml_ext.h"

namespace icm {

//, adm::Document* the_adm
ICMDocument *ICMDocument::parse_xml_from_file(char *file_in, std::shared_ptr<adm::Document> the_adm, ICM_ERROR_CODE &error) {
    //Parse the ICM document string.
    rapidxml::xml_document<> *the_icm_xml = new rapidxml::xml_document<>();
    the_icm_xml->parse<0>(file_in);
    //scan for <audioInteraction>
    rapidxml::xml_node<> *current_node = the_icm_xml->first_node();

    while (std::string(current_node->name()) != "audioInteraction") {
        if (current_node == the_icm_xml->last_node()) {
            ERROR("Could not find audioInteraction node in ICM file!\n");
            return nullptr;
        }
        current_node = current_node->next_sibling();
    }
    auto root_node = current_node;

    //If AudioInteraction is found, create an empty ICMDocument, and then scan through all the notes, parse what they are, and run the appropriate adding function.
    ICMDocument *the_icm = new ICMDocument();

    for (rapidxml::xml_node<> *node = root_node->first_node(); node; node = node->next_sibling()) {
        std::string node_name = (std::string)(node->name());
        if (node_name == "control")
            the_icm->add_control(node);
        else if (node_name == "preset")
            the_icm->add_preset(node);
        else if (node_name == "interactiveValueSet")
            the_icm->add_IVS(node);
        else
            ERROR("Unknown node seen in AudioInteraction: %s\n", node_name.c_str());
    }

    //As the references are only stored as strings currently, this function will resolve those string identifiers into pointers and add them to the appropriate elements.
    the_icm->make_references(the_adm);

    //the_icm->print_info();

    return the_icm;
}

void ICMDocument::make_references(std::shared_ptr<adm::Document> the_adm) {
    //resolve IVS

    for (auto ivs : m_IVSs) {
        for (auto ao : ivs->m_AOs) {
            ao.first = lookup_audio_elem(the_adm, ao.second);
        }

        for (auto ivs_elem : ivs->m_elements) {
            if (ivs_elem->m_element_type == IVS_COMP_OBJECT) {
                IVSComplementaryAudioElement *cast_elem = reinterpret_cast<IVSComplementaryAudioElement *>(ivs_elem);
                cast_elem->m_comp_obj = lookup_audio_elem(the_adm, cast_elem->m_co_str);
            }
        }
    }

    //resolve presets

    for (auto preset : m_presets) {
        if (preset->get_audio_prog_str() != "")
            preset->set_audio_prog(the_adm->lookup(adm::parseAudioProgrammeId(preset->get_audio_prog_str())));
        if (preset->m_cond_controls.size() > 0) {
            for (auto cont : preset->m_cond_controls) {
                cont.first = lookup_control(cont.second);
            }
        }

        for (int b = 0; b < preset->m_interactive_value_sets.size(); b++) {
            preset->m_interactive_value_sets[b].first = lookup_IVS(preset->m_interactive_value_sets[b].second);
        }
    }

    //resolve controls

    for (auto control : m_controls) {
        if (control->m_control_type == Control::CONTROL_CONTINUOUS) {
            std::shared_ptr<ContinuousControl>          cast_control = std::static_pointer_cast<ContinuousControl>(control);
            std::vector<ContinuousControl::variable *> *cc_vars = cast_control->get_variables();
            for (auto &v : *cc_vars) {
                for (auto &ao_pair : v->v_audio_objects) {
                    ao_pair.first = lookup_audio_elem(the_adm, ao_pair.second);
                }
            }
        } else if (control->m_control_type == Control::CONTROL_OPTION) {
            std::shared_ptr<OptionControl> cast_control = std::static_pointer_cast<OptionControl>(control);
            for (auto &ao : cast_control->m_options_list) {
                ao.first = lookup_audio_elem(the_adm, ao.second);
            }
            for (auto opt : cast_control->m_options) {
                for (auto &ao : opt->o_audio_objects)
                    ao.first = lookup_audio_elem(the_adm, ao.second);

                //NEEDS IMPLEMENTING HA
                for (auto &cc : opt->o_cond_controls)
                    cc.first = lookup_control(cc.second);
            }
        } else if (control->m_control_type == Control::CONTROL_TOGGLE) {

            std::shared_ptr<ToggleControl> cast_control = std::static_pointer_cast<ToggleControl>(control);
            for (int b = 0; b < 2; b++) {
                auto the_state = cast_control->get_state(b);
                for (auto &ao : *(the_state->s_off->ss_audio_objects))
                    ao.first = lookup_audio_elem(the_adm, ao.second);
                for (auto &ao : *(the_state->s_on->ss_audio_objects))
                    ao.first = lookup_audio_elem(the_adm, ao.second);

                for (auto &cc : *(the_state->s_off->ss_comp_objects))
                    cc.first = lookup_audio_elem(the_adm, cc.second);
                for (auto &cc : *(the_state->s_on->ss_comp_objects))
                    cc.first = lookup_audio_elem(the_adm, cc.second);
            }
        }
    }
}

std::shared_ptr<adm::AudioObject> ICMDocument::lookup_audio_elem(std::shared_ptr<adm::Document> the_adm, std::string ao_name) {

    std::shared_ptr<adm::AudioObject> the_ao = the_adm->lookup(adm::parseAudioObjectId(ao_name));
    if (the_ao) {
        return the_ao;
    } else {
        ERROR("AO referenced in ICM not found in document! %s\n", ao_name.c_str());
        abort();
    }
}

std::shared_ptr<Control> ICMDocument::lookup_control(std::string control_name) {
    for (auto control : m_controls) {
        if (control->m_control_ID == control_name)
            return control;
    }
    return nullptr;
}

std::shared_ptr<InteractiveValueSet> ICMDocument::lookup_IVS(std::string IVS_ID) {
    for (auto ivs : m_IVSs) {
        if (ivs->m_IVS_ID == IVS_ID)
            return ivs;
    }
    return nullptr;
}

/*
    void ICMDocument::print_info(){
        std::stringstream str_to_print;

        str_to_print << "I have detected " << m_presets.size() << " presets, " << m_IVSs.size() << " interactiveValueSets, and " << m_controls.size() << " controls." << std::endl;

        if(m_presets.size() > 0 ){
            str_to_print << "PRESETS: " << std::endl << std::endl;

            for(auto preset_of_interest : m_presets){
                str_to_print << "Name: " << preset_of_interest->m_preset_name << ", ID: " << preset_of_interest->m_preset_ID << ". " << std::endl;
                str_to_print << "Label: " << preset_of_interest->m_label << ". " << std::endl ;
                
                str_to_print << "IVSs Referenced: ";
                for(auto IVS_ref : preset_of_interest->m_ivs_refs){
                    str_to_print << IVS_ref << ", ";
                }
                str_to_print << "." <<std::endl <<std::endl;

                if(preset_of_interest->m_cond_control_refs.size() > 0) {
                    str_to_print << "Conditional Controls Referenced: " << std::endl;
                    for(auto CC_ref : preset_of_interest->m_cond_control_refs){
                        str_to_print << CC_ref << ", ";
                    }
                    str_to_print << "." <<std::endl <<std::endl;
                }

                if(preset_of_interest->m_loudness.m_exists){
                    str_to_print << "Lcudness Metadata:" << std::endl;
                    str_to_print << "Loudness Method: " << preset_of_interest->m_loudness.m_loudness_method << std::endl;
                    str_to_print << "Loudness Rec Type: " << preset_of_interest->m_loudness.m_loudness_rec_type << std::endl;
                    str_to_print << "Loudness Correction Type: " << preset_of_interest->m_loudness.m_loudness_correction_type << std::endl;

                    str_to_print << "Integrated Loudness Value: " << std::to_string(preset_of_interest->m_loudness.m_integrated_loudness_value) << std::endl;
                    str_to_print << "Loudness Range: " << std::to_string(preset_of_interest->m_loudness.m_loudness_range) << std::endl;
                    str_to_print << "Max True Peak: " << std::to_string(preset_of_interest->m_loudness.m_max_true_peak) << std::endl;
                    str_to_print << "Max Momentary: " << std::to_string(preset_of_interest->m_loudness.m_max_momentary) << std::endl;
                    str_to_print << "Max Short Term: " << std::to_string(preset_of_interest->m_loudness.m_max_short_term) << std::endl;
                    str_to_print << "Dialogue Loudness: " << std::to_string(preset_of_interest->m_loudness.m_dialogue_loudness) << std::endl;
                    
                }
            }
        }

        if(m_IVSs.size() > 0){
            str_to_print << std::endl << "IVSs: " << std::endl << std::endl;
            for(auto IVS : m_IVSs){
                str_to_print << "\n\nID: " << IVS->m_IVS_ID << std::endl;
                str_to_print << "Audio Objects: ";

                for(auto AO : IVS->m_AO_references){
                    str_to_print << AO << ", ";
                }

                str_to_print << std::endl << "Values: " << std::endl;

                for(auto C : IVS->m_elements){
                    switch(C->m_element_type){
                        case IVS_GAIN: {
                            str_to_print << "Gain of ";
                            IVSGainElement* C_cast = reinterpret_cast<IVSGainElement*>(C);
                            str_to_print << std::to_string(C_cast->m_val) << std::endl;
                            break;
                        }
                        case IVS_POSITION: {
                            str_to_print << "Position ";
                            IVSPosElement* C_cast = reinterpret_cast<IVSPosElement*>(C);
                                switch(C_cast->m_coord_type){
                                    case IVS_X:
                                        str_to_print << "X - ";
                                        break;
                                    case IVS_Y:
                                        str_to_print << "Y - ";
                                        break; 
                                    case IVS_Z:
                                        str_to_print << "Z - ";
                                        break;
                                    case IVS_AZIMUTH:
                                        str_to_print << "Azimuth - ";
                                        break;
                                    case IVS_DISTANCE:
                                        str_to_print << "Distance - ";
                                        break;
                                    case IVS_ELEVATION:
                                        str_to_print << "Elevation - ";
                                        break;   
                                }
                            str_to_print << std::to_string(C_cast->m_val) << std::endl;
                            break;
                        }
                        case IVS_ONOFF: {
                            str_to_print << "On/Off ";
                            IVSOnOffElement* C_cast = reinterpret_cast<IVSOnOffElement*>(C);
                            str_to_print << (C_cast->m_is_on ? "On" : "Off") << std::endl;
                            break;
                        }
                        case IVS_COMP_OBJECT : {
                            str_to_print << "Complementary Object ";
                            IVSComplementaryAudioElement* C_cast = reinterpret_cast<IVSComplementaryAudioElement*>(C);
                            str_to_print << C_cast->m_co_str <<std::endl;
                            break;
                        }
                    }
                }

            }
        }

        str_to_print << std::endl << std::endl << "CONTROLS:" << std::endl << std::endl;

        for (auto ctl : m_controls){
            str_to_print << "\n\nControl Type: "; //<< ctl->m_control_is_conditional ? "Conditional " : "Unconditional ";
            switch(ctl->m_control_type){
                case Control::CONTROL_TYPE::CONTROL_CONTINUOUS: str_to_print << "Continuous"; break;
                case Control::CONTROL_TYPE::CONTROL_TOGGLE: str_to_print << "Toggle"; break;
                case Control::CONTROL_TYPE::CONTROL_OPTION: str_to_print << "Option"; break;
            }
            str_to_print << ", ID: " << ctl->m_control_ID << ", Name: " << ctl->m_control_name << std::endl;
            switch(ctl->m_control_type){
                case Control::CONTROL_TYPE::CONTROL_CONTINUOUS:{
                    ContinuousControl* c_ctl = reinterpret_cast<ContinuousControl*>(ctl.get());
                    str_to_print << "Range: " << std::to_string(c_ctl->m_range.r_min) << " to " << std::to_string(c_ctl->m_range.r_max);
                    if(c_ctl->m_range.r_step != 0) str_to_print << ", Step: " << std::to_string(c_ctl->m_range.r_step);
                    str_to_print << std::endl << "Variables: " << std::endl;
                    str_to_print << "AOs: ";  
                    for(auto var : c_ctl->m_variables){
                        for(std::string AOS : var->v_ao_stringnames) str_to_print << AOS << ", ";
                        switch(var->v_param){
                            case Control::CONTROL_VAR::CONTROL_GAIN: str_to_print << ". Gain: "; break;
                            case Control::CONTROL_VAR::CONTROL_POSN_AZIMUTH: str_to_print << ". Azimuth: "; break;
                            case Control::CONTROL_VAR::CONTROL_POSN_DISTANCE: str_to_print << ". Distance: "; break;
                            case Control::CONTROL_VAR::CONTROL_POSN_ELEVATION: str_to_print << ". Elevation: "; break;
                            case Control::CONTROL_VAR::CONTROL_POSN_X: str_to_print << ". X: "; break;
                            case Control::CONTROL_VAR::CONTROL_POSN_Y: str_to_print << ". Y: "; break;
                            case Control::CONTROL_VAR::CONTROL_POSN_Z: str_to_print << ". Z: "; break;
                        }
                        str_to_print << " Range: " << std::to_string(var->v_min) << " to " << std::to_string(var->v_max) << ". Scale type: ";
                        switch(var->v_scale_type){
                            case Control::CONTROL_VAR_SCALE_TYPE::CONTROL_LINEAR: str_to_print << "Linear." << std::endl; break;
                            case Control::CONTROL_VAR_SCALE_TYPE::CONTROL_LOG: str_to_print << "Log." << std::endl; break;
                            case Control::CONTROL_VAR_SCALE_TYPE::CONTROL_INV_LOG: str_to_print << "Inverse Log." << std::endl; break;
                        }
                    }
                    break;
                }
                case Control::CONTROL_TYPE::CONTROL_OPTION:{
                    OptionControl* c_ctl = reinterpret_cast<OptionControl*>(ctl.get());
                    for(auto opt : c_ctl->m_options){
                        str_to_print << "Index " << opt->o_index << ", Label : " << opt->o_label << std::endl << "Audio Objects on: ";
                        for(std::string ao : opt->o_ao_strings) str_to_print << ao << ", ";
                        //str_to_print << std::endl << opt->
                    }
                    break;
                }
                case Control::CONTROL_TYPE::CONTROL_TOGGLE:{
                    ToggleControl* c_ctl = reinterpret_cast<ToggleControl*>(ctl.get());
                    str_to_print << "Toggle On, Label: " << c_ctl->m_toggle_on->s_label << ":" << std::endl << "\t Objects On: ";

                    for(auto sss : *(c_ctl->m_toggle_on->s_on->ss_audio_objects_str)) str_to_print << sss << ", ";
                    
                    str_to_print << std::endl << "\t Objects Off:";

                    for(auto sss : *(c_ctl->m_toggle_on->s_off->ss_audio_objects_str)) str_to_print << sss << ", ";

                    str_to_print << std::endl;

                    str_to_print << "Toggle Off, Label: " << c_ctl->m_toggle_on->s_label << ":" << std::endl << "\t Objects On: ";

                    for(auto sss : *(c_ctl->m_toggle_off->s_on->ss_audio_objects_str)) str_to_print << sss << ", ";
                    
                    str_to_print << std::endl << "\t Objects Off:";

                    for(auto sss : *(c_ctl->m_toggle_off->s_off->ss_audio_objects_str)) str_to_print << sss << ", ";

                    str_to_print << std::endl;


                }
            }
        }

        std::cout << str_to_print.str();

    }
    */

void ICMDocument::add_control(rapidxml::xml_node<> *control_in) {

    enum CTYPE {
        CONT,
        OPT,
        TOG
    };

    std::string              c_name;
    std::string              c_ID;
    std::chrono::nanoseconds start_time = std::chrono::nanoseconds(0);
    std::chrono::nanoseconds end_time = std::chrono::nanoseconds(-1);
    std::chrono::nanoseconds dur = std::chrono::nanoseconds(-1);
    CTYPE                    c_type;
    bool                     is_cond = false;
    //scan attributes of control tag as they are the same across all types of control
    for (rapidxml::xml_attribute<> *attrib = control_in->first_attribute(); attrib; attrib = attrib->next_attribute()) {
        std::string attrib_name = (std::string)(attrib->name());
        if (attrib_name == "controlID")
            c_ID = (std::string)attrib->value();
        else if (attrib_name == "controlName")
            c_name = (std::string)attrib->value();
        else if (attrib_name == "type") {
            if ((std::string)attrib->value() == "continuous")
                c_type = CONT;
            else if ((std::string)attrib->value() == "option")
                c_type = OPT;
            else if ((std::string)attrib->value() == "toggle")
                c_type = TOG;
        } else if (attrib_name == "conditional") {
            if ((std::string)attrib->value() == "1")
                is_cond = true;
        } else if (attrib_name == "start") {
            start_time = adm::parseTimecode((std::string)attrib->value());
        } else if (attrib_name == "duration") {
            dur = adm::parseTimecode((std::string)attrib->value());
        }
    }

    if (dur != std::chrono::nanoseconds(-1)) {
        end_time = start_time + dur;
    }

    switch (c_type) {
        case CONT:
            add_continuous_control(control_in, c_ID, c_name, start_time, end_time, is_cond);
            break;
        case OPT:
            add_option_control(control_in, c_ID, c_name, start_time, end_time, is_cond);
            break;
        case TOG:
            add_toggle_control(control_in, c_ID, c_name, start_time, end_time, is_cond);
    }
}

std::shared_ptr<Control> ICMDocument::add_continuous_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond) {
    //step is optional
    float       min, max, step = 0.0;
    std::string label;

    auto the_control = std::shared_ptr<ContinuousControl>(new ContinuousControl(c_ID, c_name, start_time, end_time, is_cond));

    //Firstly, scan all nodes
    for (rapidxml::xml_node<> *node = control_in->first_node(); node; node = node->next_sibling()) {
        std::string node_name = (std::string)(node->name());
        if (node_name == "range") {
            for (rapidxml::xml_attribute<> *attrib = node->first_attribute(); attrib; attrib = attrib->next_attribute()) {
                std::string attrib_name = (std::string)(attrib->name());
                if (attrib_name == "min")
                    the_control->get_range()->r_min = std::stof(attrib->value());
                else if (attrib_name == "max")
                    the_control->get_range()->r_max = std::stof(attrib->value());
                else if (attrib_name == "step")
                    the_control->get_range()->r_step = std::stof(attrib->value());
            }
        } else if (node_name == "label")
            the_control->set_label((std::string)node->value());
        else if (node_name == "variable") {
            ContinuousControl::variable *the_var = new ContinuousControl::variable();
            the_var->v_scale_type = Control::CONTROL_LINEAR;
            for (rapidxml::xml_attribute<> *attrib = node->first_attribute(); attrib; attrib = attrib->next_attribute()) {
                std::string attrib_name = (std::string)(attrib->name());
                if (attrib_name == "param") {
                    std::string val = (std::string)attrib->value();
                    if (val == "gain")
                        the_var->v_param = Control::CONTROL_GAIN;
                    else if (val == "position.azimuth")
                        the_var->v_param = Control::CONTROL_POSN_AZIMUTH;
                    else if (val == "position.elevation")
                        the_var->v_param = Control::CONTROL_POSN_ELEVATION;
                    else if (val == "position.distance")
                        the_var->v_param = Control::CONTROL_POSN_DISTANCE;
                    else if (val == "position.X")
                        the_var->v_param = Control::CONTROL_POSN_X;
                    else if (val == "position.Y")
                        the_var->v_param = Control::CONTROL_POSN_Y;
                    else if (val == "position.Z")
                        the_var->v_param = Control::CONTROL_POSN_Z;
                } else if (attrib_name == "max")
                    the_var->v_max = std::stof(attrib->value());
                else if (attrib_name == "min")
                    the_var->v_min = std::stof(attrib->value());
                else if (attrib_name == "scale") {
                    std::string val = (std::string)attrib->value();
                    if (val == "linear")
                        the_var->v_scale_type = Control::CONTROL_LINEAR;
                    else if (val == "log")
                        the_var->v_scale_type = Control::CONTROL_LOG;
                    else if (val == "invlog")
                        the_var->v_scale_type = Control::CONTROL_INV_LOG;
                }
            }
            //grab strings of AO refs
            for (rapidxml::xml_node<> *var_child = node->first_node(); var_child; var_child = var_child->next_sibling()) {
                if ((std::string)var_child->name() == "audioObjectIDRef")
                    the_var->v_audio_objects.push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(nullptr, (std::string)var_child->value()));
            }
            the_control->add_variable(the_var);
        }
    }

    m_controls.push_back(std::dynamic_pointer_cast<icm::Control>(the_control));
}

std::shared_ptr<Control> ICMDocument::add_option_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond) {

    std::string label;

    auto the_control = std::shared_ptr<OptionControl>(new OptionControl(c_ID, c_name, start_time, end_time, is_cond));

    for (rapidxml::xml_node<> *node = control_in->first_node(); node; node = node->next_sibling()) {
        std::string node_name = (std::string)(node->name());
        if (node_name == "optionObjectList") {
            for (rapidxml::xml_node<> *opt_child = node->first_node(); opt_child; opt_child = opt_child->next_sibling()) {
                if ((std::string)opt_child->name() == "audioObjectIDRef")
                    the_control->m_options_list.push_back(
                        std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(nullptr, (std::string)opt_child->value()));
            }

        } else if (node_name == "option") {
            OptionControl::option *the_opt = new OptionControl::option();
            for (rapidxml::xml_attribute<> *attrib = node->first_attribute(); attrib; attrib = attrib->next_attribute()) {
                std::string val = (std::string)attrib->name();
                if (val == "index")
                    the_opt->o_index = std::stoi((std::string)attrib->value());
            }

            for (rapidxml::xml_node<> *opt_child = node->first_node(); opt_child; opt_child = opt_child->next_sibling()) {
                if ((std::string)opt_child->name() == "label")
                    the_opt->o_label = (std::string)opt_child->value();
                else if ((std::string)opt_child->name() == "audioObjectIDRef")
                    the_opt->o_audio_objects.push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(nullptr, (std::string)opt_child->value()));
                else if ((std::string)opt_child->name() == "conditionalControlIDRef")
                    the_opt->o_cond_controls.push_back(std::make_pair<std::shared_ptr<Control>, std::string>(nullptr, (std::string)opt_child->value()));
            }
            the_control->m_options.push_back(the_opt);
        } else if (node_name == "label")
            the_control->set_label((std::string)node->value());
    }

    m_controls.push_back(std::dynamic_pointer_cast<Control>(the_control));
}

std::shared_ptr<Control> ICMDocument::add_toggle_control(rapidxml::xml_node<> *control_in, std::string c_ID, std::string c_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond) {
    std::string label;

    auto the_control = std::shared_ptr<ToggleControl>(new ToggleControl(c_ID, c_name, start_time, end_time, is_cond));

    for (rapidxml::xml_node<> *node = control_in->first_node(); node; node = node->next_sibling()) {
        std::string node_name = (std::string)(node->name());
        if (node_name == "toggleOn" || node_name == "toggleOff") {

            //ToggleControl::state* the_state = new ToggleControl::state();

            bool                  is_on = (node_name == "toggleOn");
            ToggleControl::state *the_state = the_control->get_state((node_name == "toggleOn"));

            for (rapidxml::xml_node<> *tog_child = node->first_node(); tog_child; tog_child = tog_child->next_sibling()) {
                if ((std::string)tog_child->name() == "label")
                    the_state->s_label = (std::string)tog_child->value();
                if ((std::string)tog_child->name() == "on" || (std::string)tog_child->name() == "off") {
                    auto subst = (std::string)(tog_child->name()) == "on" ? (the_state->s_on) : (the_state->s_off);
                    if (subst == nullptr) {
                        subst = new ToggleControl::substate();
                    }

                    for (rapidxml::xml_node<> *on_child = tog_child->first_node(); on_child; on_child = on_child->next_sibling()) {
                        if ((std::string)on_child->name() == "audioObjectIDRef")
                            subst->ss_audio_objects->push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(nullptr, (std::string)on_child->value()));
                        else if ((std::string)on_child->name() == "conditionalControlIDRef")
                            subst->ss_comp_objects->push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(nullptr, (std::string)on_child->value()));
                    }
                }
            }
        } else if (node_name == "label")
            the_control->set_label((std::string)node->value());
    }

    m_controls.push_back(std::dynamic_pointer_cast<Control>(the_control));
}

void ICMDocument::add_preset(rapidxml::xml_node<> *preset_in) {

    std::string              pid, pname;
    int                      pindex;
    std::chrono::nanoseconds pstart = std::chrono::nanoseconds(0);
    std::chrono::nanoseconds pend = std::chrono::nanoseconds(-1);

    for (rapidxml::xml_attribute<> *attrib = preset_in->first_attribute(); attrib; attrib = attrib->next_attribute()) {
        std::string attrib_name = (std::string)(attrib->name());
        if (attrib_name == "presetID")
            pid = (std::string)attrib->value();
        else if (attrib_name == "presetName")
            pname = (std::string)attrib->value();
        else if (attrib_name == "presetIndex")
            pindex = std::stoi((std::string)attrib->value());
    }

    std::shared_ptr<Preset> the_preset = std::shared_ptr<Preset>(new Preset(pid, pname, pindex, pstart, pend));

    the_preset->m_loudness.m_exists = false; //loudness metadata is optional, so have a bool to indicate if it's there

    for (rapidxml::xml_node<> *node = preset_in->first_node(); node; node = node->next_sibling()) {
        if ((std::string)node->name() == "label") {
            std::string lang = "";
            for (rapidxml::xml_attribute<> *attrib = node->first_attribute(); attrib; attrib = attrib->next_attribute()) {
                //std::cout << (std::string)attrib->name() << std::endl;
                if ((std::string)attrib->name() == "lang")
                    lang = (std::string)attrib->value();
            }
            the_preset->set_preset_label((std::string)node->value(), lang);
        }

        else if ((std::string)node->name() == "audioProgrammeIDRef")
            the_preset->set_audio_prog((std::string)node->value());
        else if ((std::string)node->name() == "interactiveValueSetIDRef")
            the_preset->m_interactive_value_sets.push_back(std::make_pair<std::shared_ptr<InteractiveValueSet>, std::string>(nullptr, (std::string)node->value()));
        else if ((std::string)node->name() == "loudnessMetadata") {
            the_preset->m_loudness.m_exists = true;
            for (rapidxml::xml_attribute<> *lm_at = node->first_attribute(); lm_at; lm_at = lm_at->next_attribute()) {
                std::string la_name = (std::string)lm_at->name();
                if (la_name == "loudnessMethod")
                    the_preset->m_loudness.m_loudness_method = (std::string)lm_at->value();
                else if (la_name == "loudnessRecType")
                    the_preset->m_loudness.m_loudness_rec_type = (std::string)lm_at->value();
                else if (la_name == "loudnessCorrectionType")
                    the_preset->m_loudness.m_loudness_correction_type = (std::string)lm_at->value();
            }

            for (rapidxml::xml_node<> *lm_node = node->first_node(); lm_node; lm_node = lm_node->next_sibling()) {
                std::string lm_name = (std::string)lm_node->name();
                if (lm_name == "integratedLoudness")
                    the_preset->m_loudness.m_integrated_loudness_value = std::stof(lm_node->value());
                else if (lm_name == "loudnessRange")
                    the_preset->m_loudness.m_loudness_range = std::stof(lm_node->value());
                else if (lm_name == "maxTruePeak")
                    the_preset->m_loudness.m_max_true_peak = std::stof(lm_node->value());
                else if (lm_name == "maxMomentary")
                    the_preset->m_loudness.m_max_momentary = std::stof(lm_node->value());
                else if (lm_name == "maxShortTerm")
                    the_preset->m_loudness.m_max_short_term = std::stof(lm_node->value());
                else if (lm_name == "dialogueLoudness")
                    the_preset->m_loudness.m_dialogue_loudness = std::stof(lm_node->value());
            }
        }
    }

    m_presets.push_back(the_preset);
}

void ICMDocument::add_IVS(rapidxml::xml_node<> *IVS_in) {
    std::string iid;
    for (rapidxml::xml_attribute<> *attrib = IVS_in->first_attribute(); attrib; attrib = attrib->next_attribute()) {
        std::string attrib_name = (std::string)(attrib->name());
        if (attrib_name == "interactiveValueSetID")
            iid = (std::string)attrib->value();
    }
    std::shared_ptr<InteractiveValueSet> the_IVS = std::shared_ptr<InteractiveValueSet>(new InteractiveValueSet(iid));

    for (rapidxml::xml_node<> *node = IVS_in->first_node(); node; node = node->next_sibling()) {
        if ((std::string)node->name() == "audioObjectIDRef")
            the_IVS->m_AOs.push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(nullptr, (std::string)node->value()));
        else if ((std::string)node->name() == "gain") {
            the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSGainElement(std::stof((std::string)node->value()))));
        } else if ((std::string)node->name() == "onOff") {
            bool on;
            if ((std::string)node->value() == "1")
                on = true;
            else if ((std::string)node->value() == "0")
                on = false;
            the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSOnOffElement(on)));
        } else if ((std::string)node->name() == "complementaryAudioObjectIDRef") {
            the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSComplementaryAudioElement((std::string)node->value())));
        } else if ((std::string)node->name() == "positionOffset") {
            for (rapidxml::xml_attribute<> *attrib = node->first_attribute(); attrib; attrib = attrib->next_attribute()) {
                if ((std::string)attrib->name() == "coordinate") {
                    std::string coord_type = (std::string)attrib->value();
                    float       coord_val = std::stof((std::string)node->value());

                    if (coord_type == "X")
                        the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSPosElement(IVS_X, coord_val)));
                    else if (coord_type == "Y")
                        the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSPosElement(IVS_Y, coord_val)));
                    else if (coord_type == "Z")
                        the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSPosElement(IVS_Z, coord_val)));
                    else if (coord_type == "azimuth")
                        the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSPosElement(IVS_AZIMUTH, coord_val)));
                    else if (coord_type == "elevation")
                        the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSPosElement(IVS_ELEVATION, coord_val)));
                    else if (coord_type == "distance")
                        the_IVS->m_elements.push_back(reinterpret_cast<IVSElement *>(new IVSPosElement(IVS_DISTANCE, coord_val)));
                }
            }
        }
    }

    m_IVSs.push_back(the_IVS);
}

void ICMDocument::add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, std::string attr_value) {

    rapidxml::xml_attribute<> *attr = doc_in->allocate_attribute(doc_in->allocate_string(attr_name.c_str()), doc_in->allocate_string(attr_value.c_str()));
    node_in->append_attribute(attr);
}
void ICMDocument::add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, int attr_value) {
    rapidxml::xml_attribute<> *attr = doc_in->allocate_attribute(doc_in->allocate_string(attr_name.c_str()), doc_in->allocate_string(std::to_string(attr_value).c_str()));
    node_in->append_attribute(attr);
}
void ICMDocument::add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, float attr_value) {
    rapidxml::xml_attribute<> *attr = doc_in->allocate_attribute(doc_in->allocate_string(attr_name.c_str()), doc_in->allocate_string(std::to_string(attr_value).c_str()));
    node_in->append_attribute(attr);
}
void ICMDocument::add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, bool attr_value) {
    if (attr_value) {
        rapidxml::xml_attribute<> *attr = doc_in->allocate_attribute(doc_in->allocate_string(attr_name.c_str()), doc_in->allocate_string("1"));
        node_in->append_attribute(attr);
    }
}

/*
void ICMDocument::add_attr_to_node(rapidxml::xml_document<> *doc_in, rapidxml::xml_node<> *node_in, std::string attr_name, const char* attr_value) {
    ICMDocument::add_attr_to_node(doc_in, node_in, attr_name, (std::string) attr_value);
}*/

void ICMDocument::write_xml_file(std::string file_path) {
    rapidxml::xml_document<> *the_xml = new rapidxml::xml_document<>;
    rapidxml::xml_node<> *    ai_node = the_xml->allocate_node(rapidxml::node_element, the_xml->allocate_string("audioInteraction"));

    for (auto pr : m_presets)
        pr->add_xml_to_doc(the_xml, ai_node);
    for (auto ivs : m_IVSs)
        ivs->add_xml_to_doc(the_xml, ai_node);
    for (auto c : m_controls)
        c->add_xml_to_doc(the_xml, ai_node);

    the_xml->append_node(ai_node);

    std::ofstream xml_to_write;
    xml_to_write.open(file_path, std::ios_base::app);

    xml_to_write << *the_xml;
}

std::shared_ptr<adm::Document> read_adm_xml_file(std::string filePath, ICM_ERROR_CODE &err) {
    std::ifstream adm_file(filePath);
    if (!adm_file.is_open()) {
        ERROR("Could not open ADM file %s\n", filePath.c_str());
        err = ICM_COULD_NOT_OPEN_FILE;
        return nullptr;
    } else {
        std::shared_ptr<adm::Document> the_adm;
        //try{
        the_adm = adm::parseXml(adm_file, adm::xml::ParserOptions::recursive_node_search);
        // } catch (std::exception e){
        //ERROR("Could not create ADM object: %s", e.what());
        //return nullptr;
        //}
        return the_adm;
    }
}

} // namespace icm
