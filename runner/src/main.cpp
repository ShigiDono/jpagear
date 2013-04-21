#include <iostream>
#include <fstream>
#include <spawner.h>
#include <Windows.h>
#include <json/json.h>
#include <vector>
#include <time.h>
using namespace std;

const int n = 20;


class settings_class: public Json::Value {
public:
    settings_class(): Json::Value(){}
    bool init(const std::string &config_file) {
        Json::Reader json_reader;
        if (!json_reader.parse(std::ifstream(config_file), *this)) {
            return 0;
        }
        return 1;
    }
};

class player_class {
private:
    std::string name;
public:
    player_class(const std::string &name_param, secure_runner *runner_param): name(name_param), 
        runner(runner_param) {
        output_buffer = new output_stream_buffer_class(8192);
        //output_stdout_buffer_class *output_buffer = new output_stdout_buffer_class(4096);
        runner->set_pipe(STD_OUTPUT_PIPE, new output_pipe_class(output_buffer));

        input_buffer = new input_stream_buffer_class();
        runner->set_pipe(STD_INPUT_PIPE, new input_pipe_class(input_buffer));

    }
    bool is_active() {
        return runner->is_running();
    }
    void wait_for_output() {
        while (is_active() && !output_buffer->ready())
            Sleep(5);
    }
    std::string get_name() {
        return name;
    }
    secure_runner *runner;// <-- must be some kind of abstract class
    input_stream_buffer_class *input_buffer;
    output_stream_buffer_class *output_buffer;
};

class game_options_class: public settings_class {
protected:
    options_class ai_options;
    restrictions_class ai_restrictions;
public:
    game_options_class(): settings_class() {}
    bool init_with_name(settings_class &settings_param, const std::string &game_name) {
        Json::Reader json_reader;
        std::string result = settings_param["games"][game_name]["config"].asString();
        if (result == "") {
            std::cout << "Unknown or unconfigured game \"" << game_name << "\"!" << endl;//#TODO make this as some kind of global log
            return 0;
        }
        if (!init("../../runner/" + result)) {
            std::cout << "Failed to load \"" << game_name << "\" game!" << endl;
            return 0;
        }
        Json::Value json_game_restrictions = (*this)["restrictions"]["default"];
        for (Json::ValueIterator i = json_game_restrictions.begin(); i != json_game_restrictions.end(); ++i) {
            std::string restriction = i.key().asString();
            ai_restrictions.set_restriction(restriction, json_game_restrictions[restriction].asLargestUInt());
        }
        return 1;

    }
    const options_class &get_ai_options() const {
        return ai_options;
    }
    const restrictions_class &get_ai_restrictions() const {
        return ai_restrictions;
    }
};

class game_class {
private:
    vector<player_class*> players;
    game_options_class game_options;
    int player_index;
    int c[n];
public:
    game_class(const game_options_class &game_options_param): game_options(game_options_param), player_index(0) {
    }
    void add_player(const std::string &name, const std::string &path) {
        secure_runner *runner = new secure_runner(path, game_options.get_ai_options(), game_options.get_ai_restrictions());
        player_class *player = new player_class(name, runner);
        players.push_back(player);
    }
    player_class *next_player() {
        player_index = (player_index + 1)%players.size();
        return players[player_index];
    }
    void init_player(player_class *player) {
        player->runner->run_process_async();
        player->runner->wait_for_init(infinite);
        //player specific init
        player->input_buffer->buffer << n << endl;
        player->input_buffer->set_ready();
    }
    //change to bool
    void init() {
        srand(time(NULL));
        for (int i = 0; i < n; ++i) {
            c[i] = 1 + rand()%n;
        }

        for (size_t i = 0; i < players.size(); ++i) {
            init_player(players[i]);
        }
    }
    bool active() {
        int total = 0;
        for (size_t i = 0; i < players.size(); ++i)
        {
            total += players[i]->is_active();
        }
        return total;
    }
    void start() {
        int j = 0;

        while (active()) {
            clock_t cl = clock();
            j++;
            player_class *player = next_player();
            player->wait_for_output();//specify timeout!!! if timeout 
            if (!player->is_active())
                continue;

            //check load ratio when program is not suspended(suspended <=> program written something)
            std::istringstream s(player->output_buffer->stock());
            int k = 0;
            for (int i = 0; i < n; ++i) {
                int p;
                s >> p;
                if (p == c[i])
                    k++;
            }
            std::cout << (clock()-cl) << ": " << (j/2) << "> " << player->get_name() << ": " << k << endl;
            player->input_buffer->buffer << k << endl;
            player->input_buffer->set_ready();
        }
    }
    void finish() {
    }
};

int main(int argc, char *argv[]) {
    std::string game_name = "alpha";
    settings_class program_settings;
    if (!program_settings.init("../../runner/config/config.json")) {
        std::cout << "Failed to read config!" << endl;
        return 0;
    }

    game_options_class game_options;

    if (!game_options.init_with_name(program_settings, game_name)) {
        return 0;
    }

    game_class game(game_options);

    game.add_player("player0", "empty.exe");
    game.add_player("player1", "empty.exe");

    game.init();
    game.start();


    game.finish();

    return 0;

}