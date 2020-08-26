#include "Preset.hpp"
#include "icmdocument.hpp"
#include "rapidxml.hpp"

using namespace icm;

Preset::Preset(std::string preset_ID, std::string preset_name, int preset_index) : m_preset_ID(preset_ID),
                                                                                   m_preset_name(preset_name),
                                                                                   m_index(preset_index) {}

Preset::Preset(std::string preset_ID, std::string preset_name, int preset_index, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time)
    : m_preset_ID(preset_ID),
      m_preset_name(preset_name),
      m_index(preset_index),
      m_start_time(start_time),
      m_end_time(end_time) {}

void Preset::add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) {
    rapidxml::xml_node<> *p_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("preset"));

    ICMDocument::add_attr_to_node(xml_in, p_node, "presetID", m_preset_ID);
    ICMDocument::add_attr_to_node(xml_in, p_node, "presetName", m_preset_name);
    ICMDocument::add_attr_to_node(xml_in, p_node, "presetIndex", m_index);

    if (get_duration() != std::chrono::nanoseconds(0)) {
        ICMDocument::add_attr_to_node(xml_in, p_node, "start", adm::formatTimecode(get_start_time()));
        ICMDocument::add_attr_to_node(xml_in, p_node, "duration", adm::formatTimecode(get_duration()));
    } else if (get_start_time() != std::chrono::nanoseconds(0)) {
        ICMDocument::add_attr_to_node(xml_in, p_node, "start", adm::formatTimecode(get_start_time()));
    }

    if (m_audio_prog.second != "") {
        rapidxml::xml_node<> *var_ap_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("audioProgrammeIDRef"), xml_in->allocate_string(m_audio_prog.second.c_str()));
        p_node->append_node(var_ap_node);
    }

    for (auto ivs : m_interactive_value_sets) {
        rapidxml::xml_node<> *var_ivs_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("interactiveValueSetIDRef"), xml_in->allocate_string(ivs.second.c_str()));
        p_node->append_node(var_ivs_node);
    }

    for (auto cc : m_cond_controls) {
        rapidxml::xml_node<> *var_cc_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("interactiveValueSetIDRef"), xml_in->allocate_string(cc.second.c_str()));
        p_node->append_node(var_cc_node);
    }

    if (m_loudness.m_exists)
        do_loudness(xml_in, p_node);

    if (m_label.first != "") {
        rapidxml::xml_node<> *label_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("label"), xml_in->allocate_string(m_label.first.c_str()));
        ICMDocument::add_attr_to_node(xml_in, label_node, "lang", m_label.second);
        p_node->append_node(label_node);
    }

    ai_node->append_node(p_node);
}

void Preset::do_loudness(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *p_node) {
    rapidxml::xml_node<> *l_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("loudnessMetadata"));

    if (m_loudness.m_loudness_method != "")
        ICMDocument::add_attr_to_node(xml_in, l_node, "loudnessMethod", m_loudness.m_loudness_method);
    if (m_loudness.m_loudness_rec_type != "")
        ICMDocument::add_attr_to_node(xml_in, l_node, "loudnessRecType", m_loudness.m_loudness_rec_type);
    if (m_loudness.m_loudness_correction_type != "")
        ICMDocument::add_attr_to_node(xml_in, l_node, "loudnessCorrectionType", m_loudness.m_loudness_correction_type);

    rapidxml::xml_node<> *add_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("integratedLoudness"), xml_in->allocate_string(std::to_string(m_loudness.m_integrated_loudness_value).c_str()));
    l_node->append_node(add_node);

    add_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("loudnessRange"), xml_in->allocate_string(std::to_string(m_loudness.m_loudness_range).c_str()));
    l_node->append_node(add_node);

    add_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("maxTruePeak"), xml_in->allocate_string(std::to_string(m_loudness.m_max_true_peak).c_str()));
    l_node->append_node(add_node);

    add_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("maxMomentary"), xml_in->allocate_string(std::to_string(m_loudness.m_max_momentary).c_str()));
    l_node->append_node(add_node);

    add_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("maxShortTerm"), xml_in->allocate_string(std::to_string(m_loudness.m_max_short_term).c_str()));
    l_node->append_node(add_node);

    add_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("dialogueLoudness"), xml_in->allocate_string(std::to_string(m_loudness.m_dialogue_loudness).c_str()));
    l_node->append_node(add_node);

    p_node->append_node(l_node);
}