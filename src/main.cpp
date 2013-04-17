#include <iostream>
#include <spawner.h>
#include <Windows.h>

#include <vector>
using namespace std;

const int n = 4;


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
            Sleep(10);
    }
    std::string get_name() {
        return name;
    }
    secure_runner *runner;// <-- must be some kind of abstract class
    input_stream_buffer_class *input_buffer;
    output_stream_buffer_class *output_buffer;
};

class game_options_class {
public:
    options_class ai_options;
    restrictions_class ai_restrictions;
};
class game_class {
private:
    vector<player_class*> players;
    game_options_class game_options;
    int player_index;
public:
    game_class(const game_options_class &game_options_param): game_options(game_options_param), player_index(0) {
    }
    void add_player(const std::string &name, const std::string &path) {
        secure_runner *runner = new secure_runner(path, game_options.ai_options, game_options.ai_restrictions);
        player_class *player = new player_class(name, runner);
        players.push_back(player);
    }
    player_class *next_player() {
        player_index = (player_index + 1)%players.size();
        return players[player_index];
    }
    void start() {
        for (int i = 0; i < players.size(); ++i)
        {
            players[i]->runner->run_process_async();
            players[i]->input_buffer->buffer << n << endl;
            players[i]->input_buffer->set_ready();

        }
    }
    bool active() {
        int total = 0;
        for (int i = 0; i < players.size(); ++i)
        {
            total += players[i]->is_active();
        }
        return total;
    }
    void finish() {
    }
};

int main(int argc, char *argv[]) {
    int c[n];
    srand(time(NULL));
    for (int i = 0; i < n; ++i) {
        c[i] = 1 + rand()%n;
    }

    game_options_class game_options;


    game_options.ai_restrictions.set_restriction(restriction_memory_limit, 20480*1024);
    game_options.ai_options.use_cmd = true;

    game_class game(game_options);

    game.add_player("player0", "empty.exe");
    game.add_player("player1", "empty.exe");
    //secure_runner *test_runner = new secure_runner("empty.exe", options, restrictions);
    game.start();
    int j = 0;

    while (game.active()) {
        j++;
        player_class *player = game.next_player();
        player->wait_for_output();//specify timeout!!!
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
            //cout << p << " ";
        }
        cout << (j/2) << "> " << player->get_name() << ": " << k << endl;
        player->input_buffer->buffer << k << endl;
        player->input_buffer->set_ready();
    }

    game.finish();

    /*test_runner->run_process_async();

    while (test_runner->is_running()) {
        while (!output_buffer->ready())
            Sleep(10);
        int k = 0;
        std::istringstream s(output_buffer->stock());
        for (int i = 0; i < n; ++i) {
            int p;
            s >> p;
            if (p == c[i])
                k++;
            cout << p << " ";
        }
        cout << endl << k << endl;
        input_buffer->buffer << k << endl;
        input_buffer->set_ready();

    }*/
    return 0;

}