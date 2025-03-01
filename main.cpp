#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <string>
#include "traypp//tray/include/tray.hpp"
#include <toml.hpp>
class TrayItem {
    public:
        std::string type;
        std::string name;
        std::map<std::string,std::string> opt_params;
        Tray::Tray &tray;

        TrayItem(std::string type, std::string name, std::map<std::string,std::string> opt_params,Tray::Tray &tray): tray(tray), name(name), opt_params(opt_params), type(type) {
            populate();
        }

    private:
        void populate() {
            if (this->type == "Button") {
                auto v  = parse_params<std::string,std::string>(opt_params,{"callback"});
                    tray.addEntry(Tray::Button(this->name, [v, this]{
                        system( v.at("callback").c_str());
                    }));
            }else if (this->type == "ImageButton") {
                auto v =this->parse_params<std::string,std::string>(this->opt_params,{"callback","image"});
                tray.addEntry(Tray::ImageButton(this->name, v["image"],[v, this] {
                    system( v.at("callback").c_str());
                }));
            }
            else{
                throw_error("Could not find type");
            }
        }

        template <typename T, typename C>
        std::map<T,C> parse_params(std::map<T,std::string> data, std::list<T> args) {
            std::map<T, C> params;
            for (auto i : args) {
                try {
                    params[i] = data.at(i);
                }catch (std::out_of_range &e) {
                    throw_error("Could not find required parameter " + i + " in opt_params");
                    params[i] = C();
                }
            }
            return params;
        }
    void throw_error(std::string error) {
            std::cerr << std::format("{} for name: {} of type {}",error ,this->name, this->type) << std::endl;
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
    trayApp app("yomper","network-wireless");
    return 1;
}