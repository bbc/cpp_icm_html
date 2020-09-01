#include <memory>
#include <string>
#include <vector>
#include "common.hpp"

#ifndef CHUNKS_HPP
#define CHUNKS_HPP

namespace icm {

///Describes an audio track as in the CHNA
class AudioID {
public:
    /**
     * Constructor for AudioID.
     * @param[in] track_index the integer index of the track in the CHNA
     * @param[in] UID The track's UID
     * @param[in] format_ID_ref the Format ID Reference
     * @param[in] pack_format_id_ref the Pack Format ID Reference
     */
    AudioID(int track_index, std::string UID, std::string format_ID_ref, std::string pack_format_ID_ref = "");

    /**
     * Creates an AudioID and returns a shared pointer to it
     * @param[in] track_index the integer index of the track in the CHNA
     * @param[in] UID The track's UID
     * @param[in] format_ID_ref the Format ID Reference
     * @param[in] pack_format_id_ref the Pack Format ID Reference
     * @return A shared pointer to the created AudioID
     */
    static std::shared_ptr<AudioID> get_audioID(int track_index, std::string UID, std::string format_ID_ref, std::string pack_format_ID_ref = "");

    /**
     * Goes through a vector of supplied shared pointers adm::AudioObjects and adds a reference to that if this track is mentioned in
     * @param[in] aos A vector of shared pointers to audioObjects
     */
    void        resolve_aos(std::vector<std::shared_ptr<adm::AudioObject>>* aos);

//private:
    ///The numeric index of the track
    int         m_track_index;
    ///The track's UID
    std::string m_UID;
    ///The track's Format ID Reference
    std::string m_format_ID_ref;
    ///The pack Format ID Reference
    std::string m_pack_format_ID_ref;
    ///A vector of pairs: references to adm::AudioObject and their name in string format
    std::vector<std::pair<std::shared_ptr<adm::AudioObject>, std::string>> m_AOs;
};

///A class to describe the CHNA file
class Chna {
public:
    ///Chna default constructor
    Chna();

    /**
     * @param[in] filePath The file path of the CHNA file
     * @param[out] error_code The error code, if CHNA reading fails
     * @param[in] aos A vector of shared pointers to audioObjects
     * @return A shared pointer to the CHNA object
     */
    static std::shared_ptr<Chna> read_chna_file(std::string filePath, ICM_ERROR_CODE &error_code, std::vector<std::shared_ptr<adm::AudioObject>>* aos);

    /**
     * Returns the number of tracks.
     * @return The number of tracks.
     */
    int get_num_of_tracks();

    /**
     * Returns a pointer to the vector of AudioID entries.
     * @return A pointer to the vector of AudioID entries.
     */
    std::vector<std::shared_ptr<AudioID>>* get_entries(){
        return &m_chna_entries;
    }

private:
    /**
     * Pushes an AudioID object into the CHNA.
     * @return A pointer to the vector of AudioID entries.
     */
    ICM_ERROR_CODE                        add_audio_id(AudioID *audio_ID_to_add);

    /**
     * Converts a string into a vector of strings split on whitespace using RegEx
     * @param[in] string_in The string to split
     * @return A vector of strings
     */
    static std::vector<std::string>       split_string_on_whitespace(std::string string_in);

    int                                   m_num_of_tracks;
    int                                   m_num_of_uids;

    ///A vector of shared pointers to AudioIDs
    std::vector<std::shared_ptr<AudioID>> m_chna_entries;
};

} // namespace icm

#endif //CHNA_HPP