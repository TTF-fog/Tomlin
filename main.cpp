#include <iostream>
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
        toml::table opt_params;
        Tray::Tray &tray;

        TrayItem(std::string type, std::string name,toml::table opt_params, Tray::Tray &tray): tray(tray) {
            this->name = std::move(name);
            this->callback = std::move(callback);
            this->opt_params = std::move(opt_params);
            this->type = std::move(type);
            this->tray = tray;
            populate();
        }

    private:
        void populate() {
            if (this->type == "button") {
                tray.addEntry(Tray::Button(this->name, [&]{
                    exec(opt_params["callback"]);

                  }));

            }else{
                std::cerr << "Error: " << type << "is not a type"<< std::endl;
            }
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
                toml::find<toml::table>(data, entry, "opt_params"),
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