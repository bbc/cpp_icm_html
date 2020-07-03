#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include "icm2html.hpp"
#include "IVSElements.hpp"
#include "rapidxml.hpp"

#ifndef ICMELEMENTS_HPP
#define ICMELEMENTS_HPP

namespace icm_html_cpp{

    

    

    class InteractiveValueSet{
        public:
            InteractiveValueSet(std::string ID);
            //~InteractiveValueSet();

            //ICM_ERROR_CODE add_element(IVSElement* elem_in);

        //private:
            std::string m_IVS_ID;
            std::vector<std::string> m_AO_references;
            std::vector<adm::AudioObject*> m_AOs;
            std::vector<IVSElement*> m_elements;


    };

    class Preset{
        public:

            Preset(std::string preset_ID, std::string preset_name, int preset_index);
            //~Preset();

            

            ICM_ERROR_CODE add_IVS(InteractiveValueSet* IVS_in, std::string IVS_id = "");
            ICM_ERROR_CODE add_ref_to_IVS(std::string IVS_id);

        

            std::string m_preset_ID;
            std::string m_preset_name;
            std::string m_label;
            int m_index;
            std::string m_audio_prog_ref;
            std::shared_ptr<adm::AudioProgramme> m_audio_prog;

            std::vector<std::shared_ptr<InteractiveValueSet>> m_interactive_value_sets;
            std::vector<std::string> m_ivs_refs;

            //std::vector<std::shared_ptr<Control>> m_cond_controls;
            std::vector<std::string> m_cond_control_refs;

            void* m_loudness;

            private: 
            
            static std::shared_ptr<Preset> parse_single_preset(std::stringstream preset_xml_in);




            

    };

    class Control{
        public:
            enum CONTROL_TYPE {
                CONTROL_CONTINUOUS,
                CONTROL_OPTION,
                CONTROL_TOGGLE
            };
            enum CONTROL_VAR {
                CONTROL_GAIN,
                CONTROL_POSN_X,
                CONTROL_POSN_Y,
                CONTROL_POSN_Z,
                CONTROL_POSN_AZIMUTH,
                CONTROL_POSN_ELEVATION,
                CONTROL_POSN_DISTANCE,
                CONTROL_ONOFF
            };

            enum CONTROL_VAR_SCALE_TYPE {
                CONTROL_LINEAR,
                CONTROL_LOG,
                CONTROL_INV_LOG
            };


        Control(CONTROL_TYPE control_type, std::string control_id, std::string control_name, bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);
        Control();
        CONTROL_TYPE m_control_type;
        std::string m_control_ID;
        std::string m_control_name;
        bool m_control_is_conditional;
        std::string m_start_time;
        std::string m_end_time;
        adm::AudioProgramme* m_programme_ref;
    };

    class ContinuousControl : public Control {
        public:

            struct variable{
                adm::AudioObject* v_audio_object;
                std::vector<std::string> v_ao_stringnames;
                CONTROL_VAR v_param;
                float v_min;
                float v_max;
                CONTROL_VAR_SCALE_TYPE v_scale_type;
            };

            struct range{
                float r_min;
                float r_max;
                float r_step;
            };

            ContinuousControl(std::string control_label, std::string control_id, std::string control_name,
                                float min, float max, float step = 0,
                                bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);

            ContinuousControl(std::string control_id, std::string control_name,
                                float min, float max, float step = 0,
                                bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);

            ContinuousControl(std::string control_id, std::string control_name);

            range m_range;
            std::vector<variable*> m_variables;
            std::string m_label;

            ICM_ERROR_CODE add_variable(adm::AudioObject* audio_object, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type);

    };

    class OptionControl : public Control {
        public:

        class option{
            public:
                std::vector<adm::AudioObject*> o_audio_objects;
                std::vector<std::string> o_ao_strings;
                int o_index;
                std::string o_label;
                std::vector<Control*> o_cond_controls;
                ICM_ERROR_CODE add_object(adm::AudioObject* obj_to_add);
                ICM_ERROR_CODE add_cond_control(Control* control_to_add);

        };



        std::string m_label;
        std::vector<option*> m_options;
        
        OptionControl(std::string control_label, std::string control_id, std::string control_name,
                            bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);

        OptionControl(std::string control_id, std::string control_name);

        option* add_option(int index, std::string label);
        //ICM_ERROR_CODE add_obj_to_option(int index, bool is_cond, adm::AudioObject* obj_to_add);

    };

    class ToggleControl : public Control {
        public:
            
            struct substate{
                std::vector<adm::AudioObject*>* ss_audio_objects;
                std::vector<std::string>* ss_audio_objects_str;
                std::vector<adm::AudioObject*>* ss_comp_objects;
                std::vector<std::string>* ss_comp_objects_str;
            };

            struct state{
                std::string s_label;
                bool is_on;
                substate s_on;
                substate s_off;
            };
            ToggleControl(std::string control_label, std::string control_id, std::string control_name,
                            bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);

            ToggleControl(std::string control_id, std::string control_name);

            ICM_ERROR_CODE add_state_info(std::string label, bool on, std::vector<adm::AudioObject*>* audio_objects_on, 
                                                                    std::vector<adm::AudioObject*>* audio_objects_off, std::vector<adm::AudioObject*>* audio_objects_comp_on,
                                                                    std::vector<adm::AudioObject*>* audio_objects_comp_off);
            //ICM_ERROR_CODE add_audio_obj_to_state(state the_state, std::string label, bool is_on‚ std::vector<adm::audioObject*>* audio_objects, std::vector<adm::audioObject*>* audio_objects);

            std::string m_label;
            state* m_toggle_on;
            state* m_toggle_off;
    };
}

#endif