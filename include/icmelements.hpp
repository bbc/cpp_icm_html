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
            ~InteractiveValueSet();

            static std::vector<std::shared_ptr<InteractiveValueSet>>* read_IVSs_from_xml(rapidxml::xml_document<>* doc_in, std::shared_ptr<adm::Document> the_adm);
            ICM_ERROR_CODE add_element(IVSElement* elem_in);

        private:
            std::string m_IVS_ID;
            std::vector<IVSElement> m_elements;


    };

    class Preset{
        public:

            Preset(std::string preset_ID, std::string preset_name, std::string label, std::string audio_prog_ref);
            ~Preset();

            static std::vector<std::shared_ptr<Preset>> read_presets_from_xml(std::stringstream xml_in, std::shared_ptr<adm::Document> the_adm, std::vector<std::shared_ptr<InteractiveValueSet>> IVSs_in);

            std::string generate_html(ICM_ERROR_CODE& err);

            ICM_ERROR_CODE add_IVS(InteractiveValueSet* IVS_in);

        private: 

            std::string m_preset_ID;
            std::string m_preset_name;
            std::string m_label;
            std::string m_audio_prog_ref;
            std::shared_ptr<adm::AudioProgramme> m_audio_prog;

            std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs_in_preset;
            
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
                CONTROL_POSN,
                CONTROL_ONOFF
            };

            enum CONTROL_VAR_SCALE_TYPE {
                CONTROL_LINEAR,
                CONTROL_LOG,
                CONTROL_INV_LOG
            };


        Control(CONTROL_TYPE control_type, std::string control_id, std::string control_name, bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);
        CONTROL_TYPE m_control_type;
        std::string m_control_ID;
        std::string m_control_name;
        bool m_control_is_conditional;
        std::string m_start_time;
        std::string m_end_time;
        adm::AudioProgramme* m_programme_ref;
    };

    class ContinuousControl : Control {
        public:

            struct variable{
                adm::AudioObject* v_audio_object;
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

            range m_range;
            std::vector<variable> m_variables;
            std::string m_label;

            ICM_ERROR_CODE add_variable(adm::AudioObject* audio_object, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type);

    };

    class OptionControl : Control {
        public:

        struct option{
            std::vector<adm::AudioObject*> o_audio_objects;
            int o_index;
            std::string o_label;
            std::vector<Control*> o_cond_controls;

        };



        std::string m_label;
        std::vector<option> m_options;
        
        OptionControl(std::string control_label, std::string control_id, std::string control_name,
                            bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);

        ICM_ERROR_CODE add_option(int index, std::string label);
        ICM_ERROR_CODE add_obj_to_option(int index, bool is_cond, adm::AudioObject* obj_to_add);

    };

    class ToggleControl : Control {
        public:
            
            struct substate{
                std::vector<adm::AudioObject*> ss_audio_objects;
                std::vector<adm::AudioObject*> ss_comp_objects;
            };

            struct state{
                std::string s_label;
                bool is_on;
                std::vector<substate> s_on;
                std::vector<substate> s_off;
            };
            ToggleControl(std::string control_label, std::string control_id, std::string control_name,
                            bool control_is_conditional = 0, std::string start_time = "", std::string end_time = "", adm::AudioProgramme* prog_ref = 0);

            state* add_state_info(std::string label, bool on, std::vector<adm::AudioObject*>* audio_objects, std::vector<adm::AudioObject*>* audio_objects_comp);
            //ICM_ERROR_CODE add_audio_obj_to_state(state the_state, std::string label, bool is_on‚ std::vector<adm::audioObject*>* audio_objects, std::vector<adm::audioObject*>* audio_objects);

            std::string m_label;
            state m_toggle_on;
            state m_toggle_off;
    };
}

#endif