#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include "icm2html.hpp"

#ifndef ICMELEMENTS_HPP
#define ICMELEMENTS_HPP

namespace icm_html_cpp{

    

    class InteractiveValueSet{
        public:
            InteractiveValueSet();
            ~InteractiveValueSet();

            static std::vector<std::shared_ptr<InteractiveValueSet>> read_IVSs_from_xml(std::stringstream xml_in, std::shared_ptr<adm::Document> the_adm);

        private:
            ;


    };

    class Preset{
        public:

            Preset(std::string preset_ID, std::string preset_name, std::string label, std::string audio_prog_ref);
            ~Preset();

            static std::vector<std::shared_ptr<Preset>> read_presets_from_xml(std::stringstream xml_in, std::shared_ptr<adm::Document> the_adm, std::vector<std::shared_ptr<InteractiveValueSet>> IVSs_in);

            std::string generate_html(ICM_ERROR_CODE& err);

        private: 

            std::string m_preset_ID;
            std::string m_preset_name;
            std::string m_label;
            std::string m_audio_prog_ref;
            std::shared_ptr<adm::AudioProgramme> m_audio_prog;

            std::vector<std::shared_ptr<InteractiveValueSet>> m_IVSs_in_preset;
            
            static std::shared_ptr<Preset> parse_single_preset(std::stringstream preset_xml_in);

            ICM_ERROR_CODE add_IVS_to_preset(std::shared_ptr<InteractiveValueSet> IVS_ref);




            

    };
}

#endif