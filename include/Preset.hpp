
#ifndef PRESET_HPP
#define PRESET_HPP

#include "ICMElements.hpp"
#include "icm2html.hpp"
#include "rapidxml.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

namespace icm_html_cpp {

//Represents the loudness metadata element
class LoudnessMetadata {
public:
    bool        m_exists;
    std::string m_loudness_method;
    std::string m_loudness_rec_type;
    std::string m_loudness_correction_type;
    float       m_integrated_loudness_value;
    float       m_max_true_peak;
    float       m_loudness_range;
    float       m_max_momentary;
    float       m_max_short_term;
    float       m_dialogue_loudness;
};

//Represents a Preset element
class Preset {
public:
    Preset(std::string preset_ID, std::string preset_name, int preset_index);

    std::vector<std::pair<std::shared_ptr<InteractiveValueSet>, std::string>>   m_interactive_value_sets;
    std::vector<std::pair<std::shared_ptr<icm_html_cpp::Control>, std::string>> m_cond_controls;

    LoudnessMetadata m_loudness;

    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);
    void do_loudness(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *p_node);

    std::string get_preset_ID() {
        return m_preset_ID;
    }
    void set_preset_ID(std::string preset_ID) {
        m_preset_ID = preset_ID;
    }

    std::string get_preset_name() {
        return m_preset_ID;
    }
    void set_preset_name(std::string preset_name) {
        m_preset_name = preset_name;
    }

    std::string get_preset_label() {
        return m_label.first;
    }

    std::string get_preset_label_lang() {
        return m_label.second;
    }
    void set_preset_label(std::string label, std::string lang = "") {
        m_label.first = label;
        m_label.second = lang;
    }

    std::string get_audio_prog_str() { return m_audio_prog.second; }

    std::shared_ptr<adm::AudioProgramme> get_audio_prog_ref() { return m_audio_prog.first; }

    void set_audio_prog(std::string programme) { m_audio_prog.second = programme; }
    void set_audio_prog(std::shared_ptr<adm::AudioProgramme> refin) { m_audio_prog.first = refin; }

private:
    std::string                                                  m_preset_ID;
    std::string                                                  m_preset_name;
    std::pair<std::string, std::string>                          m_label;
    int                                                          m_index;
    std::pair<std::shared_ptr<adm::AudioProgramme>, std::string> m_audio_prog;
};
} // namespace icm_html_cpp

#endif