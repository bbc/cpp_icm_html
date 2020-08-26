#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "InteractiveValueSet.hpp"
#include "adm.hpp"
#include "rapidxml.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#define ZERON   std::chrono::nanoseconds(0)
#define MINUS1N std::chrono::nanoseconds(-1)

namespace icm {

//Parent class for Control elements -- is extended by each type of control element
class Control {
public:
    friend class ICMDocument;

    enum CONTROL_VAR_SCALE_TYPE {
        CONTROL_LINEAR,
        CONTROL_LOG,
        CONTROL_INV_LOG
    };

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

    std::vector<std::string> m_VAR_STRINGS = {"gain", "position.X", "position.Y", "position.Z", "position.azimuth", "position.elevation", "position.distance", "onOff"};

    std::vector<std::string> m_VAR_SCALES = {"linear", "log", "invlog"};

    Control(CONTROL_TYPE control_type, std::string control_id, std::string control_name, bool control_is_conditional = 0, 
        std::chrono::nanoseconds start_time = std::chrono::nanoseconds(0), 
        std::chrono::nanoseconds end_time = std::chrono::nanoseconds(-1), adm::AudioProgramme *prog_ref = 0);
    Control();

    CONTROL_TYPE get_control_type() { return m_control_type; }
    void         set_control_type(CONTROL_TYPE ct) { m_control_type = ct; }

    std::string get_control_ID() { return m_control_ID; }
    void        set_control_ID(std::string ID) { m_control_ID = ID; }

    std::string get_control_name() { return m_control_name; }
    void        set_control_name(std::string name) { m_control_name = name; }

    bool get_control_conditionality() { return m_control_is_conditional; }
    void set_control_conditionality(bool is_cond) { m_control_is_conditional = is_cond; }

    std::chrono::nanoseconds get_start_time() { return m_start_time; }
    void        set_start_time(std::chrono::nanoseconds t) { m_start_time = t; }

    std::chrono::nanoseconds get_duration() { return m_end_time == MINUS1N ? MINUS1N : m_end_time - m_start_time; }
    void        set_duration(std::chrono::nanoseconds t);

    std::chrono::nanoseconds get_end_time() { return m_end_time; }
    void        set_end_time(std::chrono::nanoseconds t) { m_end_time = t; }

    adm::AudioProgramme *get_programme_ref() {
        return m_programme_ref;
    }
    void set_programme_ref(adm::AudioProgramme *programme_ref) {
        m_programme_ref = programme_ref;
    }

private:
    CONTROL_TYPE             m_control_type;
    std::string              m_control_ID;
    std::string              m_control_name;
    bool                     m_control_is_conditional;
    std::chrono::nanoseconds m_start_time;
    std::chrono::nanoseconds m_end_time;
    adm::AudioProgramme *    m_programme_ref;

    virtual void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) = 0;
};

class ContinuousControl : public Control {
public:
    ContinuousControl(std::string control_label, std::string control_id, std::string control_name,
                      float min, float max, float step = 0,
                      bool control_is_conditional = 0, std::chrono::nanoseconds start_time = ZERON, std::chrono::nanoseconds end_time = MINUS1N, adm::AudioProgramme *prog_ref = 0);

    ContinuousControl(std::string control_id, std::string control_name);

    ContinuousControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);

    struct variable {
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> v_audio_objects;
        CONTROL_VAR                                                            v_param;
        float                                                                  v_min;
        float                                                                  v_max;
        CONTROL_VAR_SCALE_TYPE                                                 v_scale_type;
    };

    struct range {
        float r_min;
        float r_max;
        float r_step;
    };

    std::string get_label() {
        return m_label;
    }
    void set_label(std::string label) {
        m_label = label;
    }

    std::vector<variable *> *get_variables() { return &m_variables; }
    ICM_ERROR_CODE           add_variable(adm::AudioObject *audio_object, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type);
    ICM_ERROR_CODE           add_variable(std::string audio_object_str, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type);
    ICM_ERROR_CODE           add_variable(ContinuousControl::variable *var_in);

    range *get_range() { return &m_range; }

    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

private:
    range                   m_range;
    std::vector<variable *> m_variables;
    std::string             m_label;
};

class OptionControl : public Control {
public:
    class option {
    public:
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> o_audio_objects;
        int                                                                    o_index;
        std::string                                                            o_label;
        std::vector<std::pair<std::shared_ptr<Control>, std::string>>          o_cond_controls;
    };

    OptionControl(std::string control_label, std::string control_id, std::string control_name,
                  bool control_is_conditional = 0, std::chrono::nanoseconds start_time = ZERON, std::chrono::nanoseconds end_time = MINUS1N, adm::AudioProgramme *prog_ref = 0);

    OptionControl(std::string control_id, std::string control_name);

    OptionControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);

    option *add_option(int index, std::string label);

    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    std::string get_label() {
        return m_label;
    }
    void set_label(std::string label) {
        m_label = label;
    }

    std::vector<option *> m_options;

private:
    std::string m_label;
};
/** The ToggleControl class tries to mirror the structure of the Toggle option in the ICM using as few objects as possible.
    * The toggle option is structured in the following way:
    * Toggle{
    *   OnPosition<label>{
    *       Objects_ON;
    *       Objects_OFF};
    *   OffPosition<label>{
    *       Objects_ON;
    *       Objects_OFF};
    *   }
    * 
    *  Each position is represented inside the ToggleControl clasas as a 'state' struct,
    *  and each set of on and off objects is represented as a 'substate' struct.
    */
class ToggleControl : public Control {
public:
    struct substate {
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> *ss_audio_objects;
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> *ss_comp_objects;
    };

    struct state {
        std::string s_label;
        bool        is_on;
        substate *  s_on;
        substate *  s_off;
    };

    ToggleControl(std::string control_label, std::string control_id, std::string control_name,
                  bool control_is_conditional = 0, std::chrono::nanoseconds start_time = ZERON, std::chrono::nanoseconds end_time = MINUS1N, adm::AudioProgramme *prog_ref = 0);

    ToggleControl(std::string control_id, std::string control_name);

    ToggleControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);


    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    std::string get_label() {
        return m_label;
    }
    void set_label(std::string label) {
        m_label = label;
    }

    state *get_state(bool is_on) {
        return is_on ? m_toggle_on : m_toggle_off;
    }

private:
    void        do_toggle_state(bool isOn, rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *tog_node, rapidxml::xml_node<> *t_node);
    std::string m_label;
    state *     m_toggle_on;
    state *     m_toggle_off;
};

} // namespace icm

#endif