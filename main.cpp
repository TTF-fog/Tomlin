#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include "traypp//tray/include/tray.hpp"
#include <toml.hpp>
class TrayItem {
    public:
        std::string type;
        std::string name;
        std::string callback;
        std::map<std::string,std::string> opt_params;
        Tray::Tray &tray;

        TrayItem(std::string type, std::string name, std::map<std::string,std::string> opt_params,Tray::Tray &tray): tray(tray), name(name), opt_params(opt_params), type(type) {
            populate();
        }

    private:
        void populate() {
            if (this->type == "button") {
                try {
                    std::string v = this->opt_params.at("callback");
                    tray.addEntry(Tray::Button(this->name, [v, this]{
                        exec( v.c_str() );

                  }));
                }
                catch (std::exception e) {
                    std::cerr << "Could not find callback skipping " << this->name << std::endl;
                }


            }else{
                std::cerr << "Error: " << type << "is not a type"<< std::endl;
            }
        }
    std::string exec(const char* cmd) {
            std::array<char, 128> buffer;
            std::string result;
            std::unique_ptr<FILE, void(*)(FILE*)> pipe(popen(cmd, "r"),
            [](FILE * f) -> void
            {
                // wrapper to ignore the return value from pclose() is needed with newer versions of gnu g++
                std::ignore = pclose(f);
            });
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            return result;
        }


};
class trayApp {
public:
    std::vector<std::string> entries;
    std::vector<TrayItem> items;
    trayApp(const std::string &name, const std::string &icon){
        Tray::Tray tray (name,icon);

        auto data = toml::parse("config.toml");
        this->entries = toml::find<std::vector<std::string>>(data, "main", "entry_points");
        for (const auto& entry : this->entries) {
            this->items.push_back(TrayItem(
                toml::find<std::string>(data, entry, "type"),
                toml::find<std::string>(data, entry, "name"),
                toml::find<std::map<std::string, std::string>>(data, entry, "opt_params"),
                tray

                ));
        }
        tray.run();
        }



};



int main() {
    trayApp app("yomper","acroread");
    return 1;
}