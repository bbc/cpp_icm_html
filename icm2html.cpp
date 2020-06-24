#include "chunks.hpp"
#include "icm2html.hpp"
#include <getopt.h>
#include "adm.hpp"
#include "parse.hpp"
#include <fstream>


using namespace icm_html_cpp;

int main(int argc, char* argv[]){
    static const char short_opts[] = "hc:a:";
    const struct option long_opts[] = {
        {"help", 0, NULL, 'h'},
        {"chnafile", 1, NULL, 'c'},
        {"admfile", 1, NULL, 'a'},
        {NULL, 0, NULL, 0}
    };

    int help = 0;
    int usage = 0;
    int err_out = 0;
    int opt;
    int option_index = 0;

    std::string chna_file;
    std::string adm_file;


    while ((opt = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
        switch (opt) {
        case 'h':
            help = 1;
            usage = 1;
            break;
        
        case 'c':
            chna_file = optarg;
            break;

        case 'a':
            adm_file = optarg;
            break;
        
        default:
            usage = 1;
            err_out = 1;
            break;
        }
    }

    ICM_ERROR_CODE error;
    std::shared_ptr<Chna> the_chna_data = Chna::read_chna_file(chna_file, error);

    if(the_chna_data == nullptr) return 1;

    std::shared_ptr<adm::Document> the_adm = read_adm_xml_file(adm_file, error);

    if(the_adm == nullptr) return 1;
    

    





    printf("blah");


}

std::shared_ptr<adm::Document> icm_html_cpp::read_adm_xml_file(std::string filePath, ICM_ERROR_CODE &err){
    std::ifstream adm_file(filePath);
    if(!adm_file.is_open()){
        ERROR("Could not open ADM file %s\n", filePath.c_str());
        err = ICM_COULD_NOT_OPEN_FILE;
        return nullptr;
    } else {
        try{
            return adm::parseXml(adm_file);
        } catch (std::exception e){
            ERROR("Could not create ADM object: %s", e.what());
            return nullptr;
        }
    }
}