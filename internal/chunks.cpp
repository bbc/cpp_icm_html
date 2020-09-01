#include "chunks.hpp"
#include "common.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

using namespace icm;

AudioID::AudioID(int track_index, std::string UID, std::string format_ID_ref, std::string pack_format_ID_ref) : m_track_index(track_index),
                                                                                                                m_UID(UID),
                                                                                                                m_format_ID_ref(format_ID_ref),
                                                                                                                m_pack_format_ID_ref(pack_format_ID_ref) {}

std::shared_ptr<AudioID> AudioID::get_audioID(int track_index, std::string UID, std::string format_ID_ref, std::string pack_format_ID_ref) {
    return std::shared_ptr<AudioID>(new AudioID(track_index, UID, format_ID_ref, pack_format_ID_ref));
}

Chna::Chna(int test) {
    ;
}

std::shared_ptr<Chna> Chna::read_chna_file(std::string filePath, ICM_ERROR_CODE &error_code, std::vector<std::shared_ptr<adm::AudioObject>> *aos) {
    std::ifstream chna_file(filePath);
    if (!chna_file.is_open()) {
        ERROR("Could not open CHNA file %s\n", filePath.c_str());
        error_code = ICM_COULD_NOT_OPEN_FILE;
        return nullptr;
    } else {
        std::string chna_line;
        int         dbg_line_counter = 0;

        std::shared_ptr<Chna> the_chna(new Chna());

        while (std::getline(chna_file, chna_line)) {
            if (chna_line == "")
                break;
            dbg_line_counter++;
            //DEBUG("CHNA Line %d Seen: %s\n", dbg_line_counter, chna_line.c_str());
            std::vector<std::string> split_line = Chna::split_string_on_whitespace(chna_line);
            int                      track_index;
            std::string              UID, format_id_ref, pack_format_ID_ref;
            try {
                track_index = stoi(split_line[0]);
            } catch (const std::exception &e) {
                ERROR("Could not parse CHNA line %d: %s\n", dbg_line_counter, e.what());
                error_code = ICM_COULD_NOT_PARSE_DATA;
                the_chna = nullptr;
                return nullptr;
            }
            UID = split_line[1];
            format_id_ref = split_line[2];

            if (split_line.size() == 4)
                pack_format_ID_ref = split_line[3];
            else
                pack_format_ID_ref = "";

            auto the_id = AudioID::get_audioID(track_index, UID, format_id_ref, pack_format_ID_ref);

            the_id->resolve_aos(aos);

            the_chna->m_chna_entries.push_back(the_id);
        }
        return the_chna;
    }
}
std::vector<std::string> Chna::split_string_on_whitespace(std::string string_in) {
    std::regex regex_whitespace("\\s+");

    std::vector<std::string> split_string{
        std::sregex_token_iterator(string_in.begin(), string_in.end(), regex_whitespace, -1), {}};

    return split_string;
}

int Chna::get_num_of_tracks() {
    return m_num_of_tracks;
}

int Chna::get_num_of_uids() {
    return m_num_of_uids;
}

void AudioID::resolve_aos(std::vector<std::shared_ptr<adm::AudioObject>> *aos) {
    for (auto ao : *aos) {
        auto ao_uids = ao->getReferences<adm::AudioTrackUid>();
        for (auto uid : ao_uids) {
            auto id = uid->get<adm::AudioTrackUidId>();
            auto id_string = adm::formatId(id);

            if (strcmp(m_UID.c_str(), id_string.c_str()) == 0) {
                std::shared_ptr<adm::AudioObject> the_sp = ao;
                std::string                       ao_name = adm::formatId(ao->get<adm::AudioObjectId>());
                m_AOs.push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(std::move(the_sp), std::move(ao_name)));
            }
        }
    }

    int ao_size = m_AOs.size(); 

    for(int ao_pair = 0; ao_pair < ao_size; ao_pair++){
        std::string name_to_check = m_AOs[ao_pair].second;
        for(auto ao : *aos){
            std::string ao_name = adm::formatId(ao->get<adm::AudioObjectId>());
            if(ao_name != name_to_check){
                auto aos_referenced = ao->getReferences<adm::AudioObject>();
                for(auto ao_ref : aos_referenced){
                    std::string ref_name = adm::formatId(ao_ref->get<adm::AudioObjectId>());
                    if(ref_name == name_to_check){ //if the AudioObject reference is the same as the one we're working on, then push back another
                        m_AOs.push_back(std::make_pair<std::shared_ptr<adm::AudioObject>, std::string>(std::move(ao), std::move(ao_name)));
                    }
                }
            }
        }
    }
}
