#include "icmdocument.hpp"
#include <string>

namespace icm_html_cpp {



    ICMDocument* ICMDocument::parse_xml_from_file(char* file_in, ICM_ERROR_CODE& error, adm::Document* the_adm){
        rapidxml::xml_document<>* the_icm_xml = new rapidxml::xml_document<>();
        the_icm_xml->parse<0>(file_in);
        //scan for <audioInteraction>
        rapidxml::xml_node<>* current_node = the_icm_xml->first_node();
        
        while(std::string(current_node->name()) != "audioInteraction"){
            if(current_node == the_icm_xml->last_node()){
                ERROR("Could not find audioInteraction node in ICM file!\n");
                return nullptr;
            }
            current_node = current_node->next_sibling();
        }
        auto root_node = current_node;

        ICMDocument* the_icm = new ICMDocument();
        
        for (rapidxml::xml_node<>* node = root_node->first_node(); node; node = node->next_sibling()){
            std::string node_name = (std::string)(node->name());
            if(node_name == "control") the_icm->add_control(node);
            else if(node_name == "preset") the_icm->add_preset(node);
            else if(node_name == "interactiveValueSet") the_icm->add_IVS(node);
            else ERROR("Unknown node seen in AudioInteraction: %s\n", node_name.c_str());
        }

        the_icm->make_references(the_adm);

        return the_icm;

    }

    void ICMDocument::add_control(rapidxml::xml_node<>* control_in){

        std::string c_name;
        std::string c_ID;

        for(rapidxml::xml_attribute<>* attrib = control_in->first_attribute(); attrib; attrib->next_attribute()){
            std::string attrib_name = (std::string)(attrib->name());
            if(attrib_name == "controlID") c_ID = (std::string)attrib->value();
            else if(attrib_name == "controlName") c_name = (std::string)attrib->value();
            else if(attrib_name == "type"){
                if((std::string)attrib->value() == "continuous") add_continuous_control(control_in, c_ID, c_name);
                else if((std::string)attrib->value() == "option") add_option_control(control_in, c_ID, c_name);
                else if((std::string)attrib->value() == "toggle") add_toggle_control(control_in, c_ID, c_name);
            }
        }
       
    }

    void ICMDocument::add_continuous_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name){
        float min, max, step=0.0;
        std::string label;

         auto the_control = std::shared_ptr<ContinuousControl>(new ContinuousControl(c_ID, c_name));

        for(rapidxml::xml_node<>* node = control_in->first_node(); node; node->next_sibling()){
            std::string node_name = (std::string)(node->name());
            if(node_name == "range"){
                for(rapidxml::xml_attribute<>* attrib = node->first_attribute(); attrib; attrib->next_attribute()){
                    std::string attrib_name = (std::string)(attrib->name());
                    if(attrib_name == "min") the_control->m_range.r_min = std::stof(attrib->value());
                    else if (attrib_name == "max") the_control->m_range.r_max = std::stof(attrib->value());
                    else if (attrib_name == "step") the_control->m_range.r_step = std::stof(attrib->value());
                }
            }
            else if(node_name == "label") the_control->m_label = (std::string) node->value();
            else if(node_name == "variable"){
                ContinuousControl::variable* the_var = new ContinuousControl::variable();
                the_var->v_scale_type = Control::CONTROL_LINEAR;
                for(rapidxml::xml_attribute<>* attrib = node->first_attribute(); attrib; attrib->next_attribute()){
                    std::string attrib_name = (std::string)(attrib->name());
                    if(attrib_name == "param"){
                        std::string val = (std::string)attrib->value();
                        if(val == "gain") the_var->v_param = Control::CONTROL_GAIN;
                        else if(val == "position.azimuth") the_var->v_param = Control::CONTROL_POSN_AZIMUTH;
                        else if(val == "position.elevation") the_var->v_param = Control::CONTROL_POSN_ELEVATION;
                        else if(val == "position.distance") the_var->v_param = Control::CONTROL_POSN_DISTANCE;
                        else if(val == "position.X") the_var->v_param = Control::CONTROL_POSN_X;
                        else if(val == "position.Y") the_var->v_param = Control::CONTROL_POSN_Y;
                        else if(val == "position.Z") the_var->v_param = Control::CONTROL_POSN_Z;
                    } else if (attrib_name == "max") the_var->v_max= std::stof(attrib->value());
                    else if (attrib_name == "min") the_var->v_min= std::stof(attrib->value());
                    else if(attrib_name == "scale") {
                        std::string val = (std::string)attrib->value();
                        if(val == "linear") the_var->v_scale_type = Control::CONTROL_LINEAR;
                        else if(val == "log") the_var->v_scale_type = Control::CONTROL_LOG;
                        else if(val == "invlog") the_var->v_scale_type = Control::CONTROL_INV_LOG;
                    }
                }
                for(rapidxml::xml_node<>* var_child = node->first_node(); var_child; var_child->next_sibling()){
                    if((std::string) var_child->name() == "audioObjectIDRef") the_var->v_ao_stringnames.push_back((std::string)var_child->value());
                }
            }
        }

        m_controls.push_back(std::dynamic_pointer_cast<Control>(the_control));

    }


    void ICMDocument::add_option_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name){

        std::string label;

        auto the_control = std::shared_ptr<OptionControl>(new OptionControl(c_ID, c_name));

          for(rapidxml::xml_node<>* node = control_in->first_node(); node; node->next_sibling()){
            std::string node_name = (std::string)(node->name());
            if(node_name == "option"){
                OptionControl::option* the_opt = new OptionControl::option();
                for(rapidxml::xml_attribute<>* attrib = node->first_attribute(); attrib; attrib->next_attribute()){
                    std::string val = (std::string)attrib->value();
                    if(val == "index") the_opt->o_index = std::stoi((std::string)attrib->value());
                }

                for(rapidxml::xml_node<>* opt_child = node->first_node(); opt_child; opt_child->next_sibling()){
                    if((std::string) opt_child->name() == "label") the_opt->o_label = (std::string)opt_child->value();
                    else if((std::string) opt_child->name() == "audioObjectIDRef") the_opt->o_ao_strings.push_back((std::string)opt_child->value());
                    else if((std::string) opt_child->name() == "conditionalControlIDRef") the_opt->o_ao_strings.push_back((std::string)opt_child->value());
                }
                the_control->m_options.push_back(the_opt);
            }
            else if(node_name == "label") the_control->m_label = (std::string) node->value();
        }

        m_controls.push_back(std::dynamic_pointer_cast<Control>(the_control));



    }


    void ICMDocument::add_toggle_control(rapidxml::xml_node<>* control_in, std::string c_ID, std::string c_name){
        std::string label;

        auto the_control = std::shared_ptr<ToggleControl>(new ToggleControl(c_ID, c_name));

          for(rapidxml::xml_node<>* node = control_in->first_node(); node; node->next_sibling()){
            std::string node_name = (std::string)(node->name());
            if(node_name == "toggleOn" || node_name == "toggleOff"){
                ToggleControl::state* the_state = new ToggleControl::state();
                the_state->is_on = (node_name == "toggleOn");

                for(rapidxml::xml_node<>* tog_child = node->first_node(); tog_child; tog_child->next_sibling()){
                     if((std::string) tog_child->name() == "label") the_state->s_label = (std::string)tog_child->value();
                     if((std::string) tog_child->name() == "on" || (std::string) tog_child->name() == "off"){
                        auto subst = (std::string) tog_child->name() == "on" ? &(the_state->s_on) : &(the_state->s_off);

                        for(rapidxml::xml_node<>* on_child = node->first_node(); on_child; on_child->next_sibling()){
                                 if((std::string) on_child->name() == "audioObjectIDRef") subst->ss_audio_objects_str->push_back((std::string)on_child->value());
                                 else if((std::string) on_child->name() == "conditionalControlIDRef") subst->ss_comp_objects_str->push_back((std::string)on_child->value());
                        }
                     }

                }
                if(node_name == "toggleOn") the_control->m_toggle_on = the_state;
                else the_control->m_toggle_off = the_state;
            }
            else if(node_name == "label") the_control->m_label = (std::string) node->value();
        }

        m_controls.push_back(std::dynamic_pointer_cast<Control>(the_control));
    }


    void ICMDocument::add_preset(rapidxml::xml_node<>* preset_in){

        std::string pid, pname;
        int pindex;
        
        for(rapidxml::xml_attribute<>* attrib = preset_in->first_attribute(); attrib; attrib->next_attribute()){
            std::string attrib_name = (std::string)(attrib->name());
            if(attrib_name == "presetID") pid = (std::string)attrib->value();
            else if(attrib_name == "presetName") pname = (std::string)attrib->value();
            else if(attrib_name == "presetIndex") pindex = std::stoi((std::string)attrib->value());
        }


        std::shared_ptr<Preset> the_preset = std::shared_ptr<Preset>(new Preset(pid, pname, pindex));

        for(rapidxml::xml_node<>* node = preset_in->first_node(); node; node->next_sibling()){
            if((std::string) node->name() == "label") the_preset->m_label = (std::string)node->value(); 
            else if((std::string) node->name() == "audioProgrammeIDRef") the_preset->m_audio_prog_ref = (std::string)node->value(); 
            else if((std::string) node->name() == "interactiveValueSetIDRef") the_preset->m_ivs_refs.push_back((std::string)node->value());
        }

        m_presets.push_back(the_preset);

    }

    void ICMDocument::add_IVS(rapidxml::xml_node<>* IVS_in){
        std::string iid;
        for(rapidxml::xml_attribute<>* attrib = IVS_in->first_attribute(); attrib; attrib->next_attribute()){
        std::string attrib_name = (std::string)(attrib->name());
            if(attrib_name == "interactiveValueSetID") iid = (std::string)attrib->value();
        }
        std::shared_ptr<InteractiveValueSet> the_IVS = std::shared_ptr<InteractiveValueSet>(new InteractiveValueSet(iid));

         for(rapidxml::xml_node<>* node = IVS_in->first_node(); node; node->next_sibling()){
              if((std::string) node->name() == "audioObjectIDRef") the_IVS->m_AO_references.push_back((std::string) node->value());
              else if((std::string) node->name() == "gain"){
                  the_IVS->m_elements.push_back(reinterpret_cast<IVSElement*>(new IVSGainElement(std::stof((std::string)node->value()))));
              } else if((std::string) node->name() == "onOff"){
                  bool on;
                  if((std::string)node->value() == "1") on = true;
                  else if ((std::string)node->value() == "0") on = false;
                  the_IVS->m_elements.push_back(reinterpret_cast<IVSElement*>(new IVSOnOffElement(on)));
              }
         
         }


    }

}

