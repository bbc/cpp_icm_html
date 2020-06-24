#include <string>
#include <memory>
#include <vector>
#include "icm2html.hpp"

#ifndef CHUNKS_HPP
#define CHUNKS_HPP

namespace icm_html_cpp{

   class AudioID{
        public:
            AudioID(int track_index, std::string UID, std::string format_ID_ref, std::string pack_format_ID_ref = "");
            static std::shared_ptr<AudioID> get_audioID(int track_index, std::string UID, std::string format_ID_ref, std::string pack_format_ID_ref = "");
        private:
            int m_track_index;
            std::string m_UID;
            std::string m_format_ID_ref;
            std::string m_pack_format_ID_ref;
    };


    class Chna{
        public: 
            Chna(int test = 0);
            static std::shared_ptr<Chna> read_chna_file(std::string filePath, ICM_ERROR_CODE& error_code);

            int get_num_of_tracks();
            int get_num_of_uids();

        
        private:
            ICM_ERROR_CODE add_audio_id(AudioID* audio_ID_to_add);
            static std::vector<std::string> split_string_on_whitespace(std::string string_in);
            int m_num_of_tracks;
            int m_num_of_uids;
            std::vector<std::shared_ptr<AudioID>> m_chna_entries;

    };

 

}

#endif //CHNA_HPP