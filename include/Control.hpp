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

///Parent class for Control elements -- is extended by each type of control element
class Control {
public:
    friend class ICMDocument;

    ///An enum to represent the scale type used in Controls
    enum CONTROL_VAR_SCALE_TYPE {
        CONTROL_LINEAR,
        CONTROL_LOG,
        CONTROL_INV_LOG
    };

    ///An enum representing the control type (continuous, option, toggle)
    enum CONTROL_TYPE {
        CONTROL_CONTINUOUS,
        CONTROL_OPTION,
        CONTROL_TOGGLE
    };

    ///The type of parameter which the control will adjust
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

    ///The textual representation of the parameters as used in the XML and JS
    std::vector<std::string> m_VAR_STRINGS = {"gain", "position.X", "position.Y", "position.Z", "position.azimuth", "position.elevation", "position.distance", "onOff"};
    ///The textual representation of the scale types as used in the XML and JS
    std::vector<std::string> m_VAR_SCALES = {"linear", "log", "invlog"};


    /**
     * Constructor for the Control
     * @param[in] control_type The type of control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] control_is_conditional The control's conditionality (defaults to no)
     * @param[in] start_time The offset from the start of the media which this control becomes valid
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] prog_ref A reference to the audioProgramme which this control references
     */
    Control(CONTROL_TYPE control_type, std::string control_id, std::string control_name, bool control_is_conditional = 0, 
        std::chrono::nanoseconds start_time = std::chrono::nanoseconds(0), 
        std::chrono::nanoseconds end_time = std::chrono::nanoseconds(-1), adm::AudioProgramme *prog_ref = 0);
    
    ///Default constructor for Control
    Control();

    /**
     * Gets the control type
     * @return the control type
     */
    CONTROL_TYPE get_control_type() { return m_control_type; }
    /**
     * Sets the control type
     * @param[in] the control type
     */
    void         set_control_type(CONTROL_TYPE ct) { m_control_type = ct; }

    /**
     * Gets the control ID
     * @return the control ID
     */
    std::string get_control_ID() { return m_control_ID; }
    /**
     * Sets the control ID
     * @param[in] the control ID
     */
    void        set_control_ID(std::string ID) { m_control_ID = ID; }

    /**
     * Gets the control name
     * @return the control name
     */
    std::string get_control_name() { return m_control_name; }
    /**
     * Sets the control name
     * @param[in] the control name
     */
    void        set_control_name(std::string name) { m_control_name = name; }

    /**
     * Gets the control conditionality
     * @return the control conditionality
     */
    bool get_control_conditionality() { return m_control_is_conditional; }
    /**
     * Sets the control conditionality
     * @param[in] the control conditionality
     */
    void set_control_conditionality(bool is_cond) { m_control_is_conditional = is_cond; }

    /**
     * Gets the control's start time
     * @return the control start time
     */
    std::chrono::nanoseconds get_start_time() { return m_start_time; }
    /**
     * Sets the control's start time
     * @param[in] the control's start time
     */
    void        set_start_time(std::chrono::nanoseconds t) { m_start_time = t; }

    /**
     * Gets the control duration
     * @return the control duration
     */
    std::chrono::nanoseconds get_duration() { return m_end_time == MINUS1N ? MINUS1N : m_end_time - m_start_time; }
    /**
     * Sets the control duration
     * @param[in] the control duration
     */
    void        set_duration(std::chrono::nanoseconds t);

    /**
     * Gets the control's end time
     * @return the control's end time
     */
    std::chrono::nanoseconds get_end_time() { return m_end_time; }
    /**
     * Sets the control end time
     * @param[in] the control end time
     */
    void        set_end_time(std::chrono::nanoseconds t) { m_end_time = t; }

    /**
     * Gets the programme referenced by the control
     * @return the programme referenced by the control
     */
    adm::AudioProgramme *get_programme_ref() {
        return m_programme_ref;
    }
    /**
     * Sets the programme referenced by the control
     * @param[in] the programme referenced by the control
     */
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

    /**
     * Adds the XML for the control into a RapidXML document.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ai_node The parent node to attach this control's XML to
     */
    virtual void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node) = 0;
};

///Represents a continuous control
class ContinuousControl : public Control {
public:

    /**
     * Constructor for the Control
     * @param[in] control_label The label to use for the control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] min minimum value for the control
     * @param[in] max maximum value for the control
     * @param[in] step the step size for the control
     * @param[in] control_is_conditional The control's conditionality (defaults to no)
     * @param[in] start_time The offset from the start of the media which this control becomes valid
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] prog_ref A reference to the audioProgramme which this control references
     */
    ContinuousControl(std::string control_label, std::string control_id, std::string control_name,
                      float min, float max, float step = 0,
                      bool control_is_conditional = 0, std::chrono::nanoseconds start_time = ZERON, std::chrono::nanoseconds end_time = MINUS1N, adm::AudioProgramme *prog_ref = 0);

    
     /**
     * Constructor for the Control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     */
    ContinuousControl(std::string control_id, std::string control_name);


    /**
     * Constructor for the Control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] start_time The offset from the start of the media which this control becomes valid
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] is_cond The control's conditionality
     */
    ContinuousControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);

    ///Describes a variable within this control
    struct variable {
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> v_audio_objects;
        CONTROL_VAR                                                            v_param;
        float                                                                  v_min;
        float                                                                  v_max;
        CONTROL_VAR_SCALE_TYPE                                                 v_scale_type;
    };

    ///Describes this control's range
    struct range {
        float r_min;
        float r_max;
        float r_step;
    };

    /**
     * Gets the control scale label
     * @return the control scale label
     */
    std::string get_label() {
        return m_label;
    }
    /**
     * Sets the control scale label
     * @param[in] the control scale label
     */
    void set_label(std::string label) {
        m_label = label;
    }

    /**
     * Returns a pointer to the vector of variables
     * @return a pointer to the vector of variables
     */
    std::vector<variable *> *get_variables() { return &m_variables; }

    /**
     * Adds a variable to the control.
     * @param[in] audio_object A pointer reference to the audioObject which this control's variable will control
     * @param[in] param The parameter this variable will control
     * @param[in] min the minimum value of the parameter to be varied
     * @param[in] max the maximum value of the parameter to be varied
     * @param[in] scale_type the type of scale (linear, log, invlog)
     * @return An ICM error code 
     */
    ICM_ERROR_CODE           add_variable(adm::AudioObject *audio_object, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type);
    /**
     * Adds a variable to the control.
     * @param[in] audio_object_string A string containing the ID of the AudioObject to be controlled (this will be resolved using the resolve_aos function)
     * @param[in] param The parameter this variable will control
     * @param[in] min the minimum value of the parameter to be varied
     * @param[in] max the maximum value of the parameter to be varied
     * @param[in] scale_type the type of scale (linear, log, invlog)
     * @return An ICM error code 
     */
    ICM_ERROR_CODE           add_variable(std::string audio_object_str, CONTROL_VAR param, float min, float max, CONTROL_VAR_SCALE_TYPE scale_type);
    /**
     * Takes a pointer to variable object and adds it to the control.
     * @param[in] var_in a pointer to a variable object
     * @return An ICM error code 
     */
    ICM_ERROR_CODE           add_variable(ContinuousControl::variable *var_in);


    /**
     * Returns the range of the control as a range object
     * @return the range of the control as a range object
     */
    range *get_range() { return &m_range; }

    /**
     * Adds the XML for the control into a RapidXML document.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ai_node The parent node to attach this control's XML to
     */
    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

private:
    range                   m_range;
    std::vector<variable *> m_variables;
    std::string             m_label;
};

///Describes the Option control type
class OptionControl : public Control {
public:
    ///A sub-class (basically a struct) that describes an individual option
    class option {
    public:
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> o_audio_objects;
        int                                                                    o_index;
        std::string                                                            o_label;
        std::vector<std::pair<std::shared_ptr<Control>, std::string>>          o_cond_controls;
    };

    /**
     * Constructor for the Option Control
     * @param[in] control_label The label to use for the control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] control_is_conditional The control's conditionality (defaults to no)
     * @param[in] start_time The offset from the start of the media which this control becomes valid
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] prog_ref A reference to the audioProgramme which this control references
     */
    OptionControl(std::string control_label, std::string control_id, std::string control_name,
                  bool control_is_conditional = 0, std::chrono::nanoseconds start_time = ZERON, std::chrono::nanoseconds end_time = MINUS1N, adm::AudioProgramme *prog_ref = 0);

    
    /**
     * Constructor for the Option Control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     */
    OptionControl(std::string control_id, std::string control_name);

    /**
     * Constructor for the Option Control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] prog_ref A reference to the audioProgramme which this control references
     * @param[in] is_cond The control's conditionality (defaults to no)
     */
    OptionControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);


    /**
     * Creates an option object, adds it to the vector of options, and returns a pointer to the option object.
     * @param[in] index The index of the option in the list.
     * @param[in] label The option's label
     * @return a pointer to the option object in the vector
     */
    option *add_option(int index, std::string label);

    /**
     * Adds the XML for the control into a RapidXML document.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ai_node The parent node to attach this control's XML to
     */
    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    /**
     * Gets the label.
     * @return the Control's label.
     */
    std::string get_label() {
        return m_label;
    }

    /**
     * Sets the label.
     * @param[in] label the Control's new label.
     */
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

    ///A substate can either describe the 'on' or 'off' group of objects for a given toggle position.
    struct substate {
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> *ss_audio_objects;
        std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> *ss_comp_objects;
    };

    ///Represents a given toggle position.
    struct state {
        std::string s_label;
        bool        is_on;
        substate *  s_on;
        substate *  s_off;
    };

    /**
     * Constructor for the Toggle Control
     * @param[in] control_label The label to use for the control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] control_is_conditional The control's conditionality (defaults to no)
     * @param[in] start_time The offset from the start of the media which this control becomes valid
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] prog_ref A reference to the audioProgramme which this control references
     */
    ToggleControl(std::string control_label, std::string control_id, std::string control_name,
                  bool control_is_conditional = 0, std::chrono::nanoseconds start_time = ZERON, std::chrono::nanoseconds end_time = MINUS1N, adm::AudioProgramme *prog_ref = 0);

    
    /**
     * Constructor for the Toggle Control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     */
    ToggleControl(std::string control_id, std::string control_name);


    /**
     * Constructor for the Toggle Control
     * @param[in] control_id The ID of the control (C_...)
     * @param[in] control_name The name of the control (e.g. AwayCommentary)
     * @param[in] end_time The offset from the start of the media which this control ceases to be valid
     * @param[in] prog_ref A reference to the audioProgramme which this control references
     * @param[in] is_cond The control's conditionality (defaults to no)
     */
    ToggleControl(std::string control_id, std::string control_name, std::chrono::nanoseconds start_time, std::chrono::nanoseconds end_time, bool is_cond);

    /**
     * Adds the XML for the control into a RapidXML document.
     * @param[in] xml_in Reference to the XML document
     * @param[in] ai_node The parent node to attach this control's XML to
     */
    void add_xml_to_doc(rapidxml::xml_document<> *xml_in, rapidxml::xml_node<> *ai_node);

    /**
     * Gets the label.
     * @return the Control's label.
     */
    std::string get_label() {
        return m_label;
    }

    /**
     * Sets the label.
     * @param[in] label the Control's new label.
     */
    void set_label(std::string label) {
        m_label = label;
    }

    /**
     * Returns a pointer to the requested state
     * @param[in] is_on True if you want the 'on' state, false if you want 'off'.
     * @return A pointer to the requested state.
     */
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