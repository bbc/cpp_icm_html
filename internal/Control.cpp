//VSCODE STOPPED DETECTING THINGS EVEN THOUGH THEY ARE CLEARLY IN THE INCLUDE PATH SO I'M GOING TO HAVE TO 
#include "Control.hpp"
#include "common.hpp"
#include "icmdocument.hpp"
#include <memory>
#include <stdbool.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace icm {

//Constructors for elements.

Control::Control(CONTROL_TYPE control_type, std::string control_id, std::string control_name, bool control_is_conditional,
                 std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, adm::AudioProgramme *prog_ref)
    : m_control_type(control_type),
      m_control_ID(control_id),
      m_control_name(control_name),
      m_control_is_conditional(control_is_conditional),
      m_start_time(start_time),
      m_end_time(end_time),
      m_programme_ref(prog_ref) {}

ContinuousControl::ContinuousControl(std::string control_label, std::string control_id, std::string control_name,
                                     float min, float max, float step,
                                     bool control_is_conditional, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, adm::AudioProgramme *prog_ref)
    : Control(CONTROL_CONTINUOUS, control_id, control_name, control_is_conditional, start_time, end_time, prog_ref),
      m_label(control_label) {
    m_range.r_max = max;
    m_range.r_min = min;
    m_range.r_step = step;
}

ContinuousControl::ContinuousControl(std::string control_id, std::string control_name) : Control(CONTROL_CONTINUOUS, control_id, control_name) {}

ContinuousControl::ContinuousControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond) : Control(CONTROL_CONTINUOUS, control_id, control_name, is_cond, start_time, end_time) {}

ICM_ERROR_CODE ContinuousControl::add_variable(ContinuousControl::variable *var_in) {
    m_variables.push_back(var_in);
}

void ContinuousControl::add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) {
    rapidxml::xml_node<> *c_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("control"));

    ICMDocument::add_attr_to_node(xml_in, c_node, "controlID", get_control_ID());
    ICMDocument::add_attr_to_node(xml_in, c_node, "controlName", get_control_name());
    ICMDocument::add_attr_to_node(xml_in, c_node, "type", std::string("continuous"));
    if(get_control_conditionality()) ICMDocument::add_attr_to_node(xml_in, c_node, "conditional", std::string("1"));

    if(get_duration() != std::chrono::nanoseconds(0)){
        ICMDocument::add_attr_to_node(xml_in, c_node, "start", adm::formatTimecode(get_start_time()));
        ICMDocument::add_attr_to_node(xml_in, c_node, "duration", adm::formatTimecode(get_duration()));
    }
    else if(get_start_time() != std::chrono::nanoseconds(0)){
        ICMDocument::add_attr_to_node(xml_in, c_node, "start", adm::formatTimecode(get_start_time()));
    }


    rapidxml::xml_node<> *range_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("range"));

    ICMDocument::ICMDocument::add_attr_to_node(xml_in, range_node, "min", m_range.r_min);
    ICMDocument::add_attr_to_node(xml_in, range_node, "max", m_range.r_max);
    if (m_range.r_step != 0)
        ICMDocument::add_attr_to_node(xml_in, range_node, "step", m_range.r_step);

    c_node->append_node(range_node);

    for (auto var : m_variables) {
        rapidxml::xml_node<> *var_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("variable"));

        ICMDocument::add_attr_to_node(xml_in, range_node, "param", m_VAR_STRINGS[var->v_scale_type]);
        ICMDocument::add_attr_to_node(xml_in, var_node, "max", var->v_max);
        ICMDocument::add_attr_to_node(xml_in, var_node, "min", var->v_min);

        if (var->v_scale_type != Control::CONTROL_VAR_SCALE_TYPE::CONTROL_LINEAR)
            ICMDocument::add_attr_to_node(xml_in, var_node, "scale", m_VAR_SCALES[var->v_scale_type]);

        for (auto ao : var->v_audio_objects) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("audioObjectIDRef"), xml_in->allocate_string(refstr.c_str()));
            var_node->append_node(var_ao_node);
        }
    }

    if (m_label != "") {
        rapidxml::xml_node<> *label_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("label"), xml_in->allocate_string(m_label.c_str()));
        c_node->append_node(label_node);
    }

    ai_node->append_node(c_node);
}

ToggleControl::ToggleControl(std::string control_label, std::string control_id, std::string control_name,
                             bool control_is_conditional, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time,
                             adm::AudioProgramme *prog_ref)
    : Control(CONTROL_TOGGLE, control_id, control_name, control_is_conditional, start_time, end_time, prog_ref),
      m_label(control_label) {
    m_toggle_on->s_off = new ToggleControl::substate();
    m_toggle_on->s_on = new ToggleControl::substate();

    m_toggle_off->s_off = new ToggleControl::substate();
    m_toggle_off->s_on = new ToggleControl::substate();

    m_toggle_off->s_off->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_off->s_off->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
}

ToggleControl::ToggleControl(std::string control_id, std::string control_name) : Control(CONTROL_TOGGLE, control_id, control_name) {
    m_toggle_on = new ToggleControl::state();
    m_toggle_off = new ToggleControl::state();

    m_toggle_on->s_off = new ToggleControl::substate();
    m_toggle_on->s_on = new ToggleControl::substate();

    m_toggle_off->s_off = new ToggleControl::substate();
    m_toggle_off->s_on = new ToggleControl::substate();

    m_toggle_off->s_off->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_off->s_off->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();

    m_toggle_off->s_off->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_off->s_off->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
}

ToggleControl::ToggleControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond)
    : Control(CONTROL_TOGGLE, control_id, control_name, is_cond, start_time, end_time){
    m_toggle_on = new ToggleControl::state();
    m_toggle_off = new ToggleControl::state();

    m_toggle_on->s_off = new ToggleControl::substate();
    m_toggle_on->s_on = new ToggleControl::substate();

    m_toggle_off->s_off = new ToggleControl::substate();
    m_toggle_off->s_on = new ToggleControl::substate();

    m_toggle_off->s_on->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_off->s_off->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_off->ss_audio_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();

    m_toggle_off->s_on->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_on->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_off->s_off->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
    m_toggle_on->s_off->ss_comp_objects = new std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>>();
}

void ToggleControl::add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) {
    rapidxml::xml_node<> *t_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("control"));

    ICMDocument::add_attr_to_node(xml_in, t_node, "controlID", get_control_ID());
    ICMDocument::add_attr_to_node(xml_in, t_node, "controlName", get_control_name());
    ICMDocument::add_attr_to_node(xml_in, t_node, "type", std::string("toggle"));
    if(get_control_conditionality()) ICMDocument::add_attr_to_node(xml_in, t_node, "conditional", std::string("1"));

    if(get_duration() != std::chrono::nanoseconds(0)){
        ICMDocument::add_attr_to_node(xml_in, t_node, "start", adm::formatTimecode(get_start_time()));
        ICMDocument::add_attr_to_node(xml_in, t_node, "duration", adm::formatTimecode(get_duration()));
    }
    else if(get_start_time() != std::chrono::nanoseconds(0)){
        ICMDocument::add_attr_to_node(xml_in, t_node, "start", adm::formatTimecode(get_start_time()));
    }


    rapidxml::xml_node<> *togon_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("toggleOn"));
    do_toggle_state(true, xml_in, togon_node, t_node);
    t_node->append_node(togon_node);

    rapidxml::xml_node<> *togoff_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("toggleOn"));
    do_toggle_state(false, xml_in, togoff_node, t_node);
    t_node->append_node(togoff_node);

    if (m_label != "") {
        rapidxml::xml_node<> *label_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("label"), xml_in->allocate_string(m_label.c_str()));
        t_node->append_node(label_node);
    }

    ai_node->append_node(t_node);
}

void ToggleControl::do_toggle_state(bool isOn, rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *tog_node, rapidxml::xml_node<> *t_node) {
    state *the_state = isOn ? m_toggle_on : m_toggle_off;
    if (the_state->s_label != "") {
        rapidxml::xml_node<> *label_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("label"), xml_in->allocate_string(m_label.c_str()));
        tog_node->append_node(label_node);
    }

    rapidxml::xml_node<> *on_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("on"));
    if (the_state->s_on->ss_audio_objects)
        for (auto ao : *(the_state->s_on->ss_audio_objects)) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("audioObjectIDRef"), xml_in->allocate_string(refstr.c_str()));
            on_node->append_node(var_ao_node);
        }
    if (the_state->s_on->ss_comp_objects)
        for (auto ao : *(the_state->s_on->ss_comp_objects)) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("conditionalControlIDRef"), xml_in->allocate_string(refstr.c_str()));
            on_node->append_node(var_ao_node);
        }

    tog_node->append_node(on_node);

    rapidxml::xml_node<> *off_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("off"));
    if (the_state->s_off->ss_audio_objects)
        for (auto ao : *(the_state->s_off->ss_audio_objects)) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("audioObjectIDRef"), xml_in->allocate_string(refstr.c_str()));
            off_node->append_node(var_ao_node);
        }
    if (the_state->s_off->ss_comp_objects)
        for (auto ao : *(the_state->s_off->ss_comp_objects)) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("conditionalControlIDRef"), xml_in->allocate_string(refstr.c_str()));
            off_node->append_node(var_ao_node);
        }

    tog_node->append_node(off_node);
}

OptionControl::OptionControl(std::string control_label, std::string control_id, std::string control_name,
                             bool control_is_conditional, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, adm::AudioProgramme *prog_ref) : Control(CONTROL_OPTION, control_id, control_name, control_is_conditional, start_time, end_time, prog_ref),
                                                                                                                                         m_label(control_label) {}

OptionControl::OptionControl(std::string control_id, std::string control_name) : Control(CONTROL_OPTION, control_id, control_name) {}

OptionControl::OptionControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond) : 
     Control(CONTROL_OPTION, control_id, control_name, is_cond, start_time, end_time) {}

icm::OptionControl::option *OptionControl::add_option(int index, std::string label) {
    option *the_option = new option();

    the_option->o_index = index;
    the_option->o_label = label;

    m_options.push_back(the_option);

    return the_option;
}

void OptionControl::add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) {
    rapidxml::xml_node<> *o_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("control"));
    ICMDocument::add_attr_to_node(xml_in, o_node, "controlID", get_control_ID());
    ICMDocument::add_attr_to_node(xml_in, o_node, "controlName", get_control_name());
    ICMDocument::add_attr_to_node(xml_in, o_node, "type", std::string("option"));
    if(get_control_conditionality()) ICMDocument::add_attr_to_node(xml_in, o_node, "conditional", std::string("1"));

    if(get_duration() != std::chrono::nanoseconds(0)){
        ICMDocument::add_attr_to_node(xml_in, o_node, "start", adm::formatTimecode(get_start_time()));
        ICMDocument::add_attr_to_node(xml_in, o_node, "duration", adm::formatTimecode(get_duration()));
    }
    else if(get_start_time() != std::chrono::nanoseconds(0)){
        ICMDocument::add_attr_to_node(xml_in, o_node, "start", adm::formatTimecode(get_start_time()));
    }

    for (auto opt : m_options) {
        rapidxml::xml_node<> *opt_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("option"));
        ICMDocument::add_attr_to_node(xml_in, opt_node, "index", opt->o_index);

        for (auto ao : opt->o_audio_objects) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("audioObjectIDRef"), xml_in->allocate_string(refstr.c_str()));
            opt_node->append_node(var_ao_node);
        }
        for (auto ao : opt->o_cond_controls) {
            std::string           refstr = std::get<1>(ao);
            rapidxml::xml_node<> *var_ao_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("conditionalControlIDRef"), xml_in->allocate_string(refstr.c_str()));
            opt_node->append_node(var_ao_node);
        }

        if (opt->o_label != "") {
            rapidxml::xml_node<> *label_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("label"), xml_in->allocate_string(m_label.c_str()));
            opt_node->append_node(label_node);
        }
    }

    if (m_label != "") {
        rapidxml::xml_node<> *label_node = xml_in->allocate_node(rapidxml::node_element, xml_in->allocate_string("label"), xml_in->allocate_string(m_label.c_str()));
        o_node->append_node(label_node);
    }

    ai_node->append_node(o_node);
}

} // namespace icm
