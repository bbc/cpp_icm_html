
#ifndef PRESET_HPP
#define PRESET_HPP

#include "Control.hpp"
#include "InteractiveValueSet.hpp"
#include "common.hpp"
#include "rapidxml.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

namespace icm {

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
    /**
     * Constructor for Preset
     * @param[in] preset_ID the preset's ID
     * @param[in] preset_name the name of the preset
     * @param[in] preset_index the position in the list this preset should show up in
     */
    Preset(std::string preset_ID, std::string preset_name, int preset_index);

    /**
     * Constructor for Preset
     * @param[in] preset_ID the preset's ID
     * @param[in] preset_name the name of the preset
     * @param[in] preset_index the position in the list this preset should show up in
     * @param[in] start_time The offset from the start of the media which this control becomes valid
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     */
    Preset(std::string preset_ID, std::string preset_name, int preset_index, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time);

    std::vector<std::pair<std::shared_ptr<InteractiveValueSet>, std::string>> m_interactive_value_sets;
    std::vector<std::pair<std::shared_ptr<icm::Control>, std::string>>        m_cond_controls;

    LoudnessMetadata m_loudness;

    /**
     * Adds the XML for the preset into a RapidXML document.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ai_node The parent node to attach this control's XML to
     */
    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    /** 
     * Parses loudness metadata and adds to the preset.
     * @param[in] xml_in A pointer to the XML to parse
     * @param[in] p_node the node containing the preset
     */
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

    std::chrono::nanoseconds get_start_time() { return m_start_time; }
    void                     set_start_time(std::chrono::nanoseconds t) { m_start_time = t; }

    std::chrono::nanoseconds get_duration() { return m_end_time == MINUS1N ? MINUS1N : m_end_time - m_start_time; }
    void                     set_duration(std::chrono::nanoseconds t);

    std::chrono::nanoseconds get_end_time() { return m_end_time; }
    void                     set_end_time(std::chrono::nanoseconds t) { m_end_time = t; }

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
    std::chrono::nanoseconds                                     m_start_time;
    std::chrono::nanoseconds                                     m_end_time;
};
} // namespace icm

#endif