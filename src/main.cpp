#include <iostream>
#include <spawner.h>
#include <Windows.h>
using namespace std;

const int n = 4;

int main(int argc, char *argv[]) {
    options_class options;
    restrictions_class restrictions;
    int c[n];
    srand(time(NULL));
    for (int i = 0; i < n; ++i) {
        c[i] = 1 + rand()%n;
    }

    restrictions.set_restriction(restriction_memory_limit, 2048*1024);
    options.use_cmd = true;
    secure_runner *test_runner = new secure_runner("empty.exe", options, restrictions);

    output_stream_buffer_class *output_buffer = new output_stream_buffer_class(8192);
    //output_stdout_buffer_class *output_buffer = new output_stdout_buffer_class(4096);
    test_runner->set_pipe(STD_OUTPUT_PIPE, new output_pipe_class(output_buffer));

    input_stream_buffer_class *input_buffer = new input_stream_buffer_class();
    test_runner->set_pipe(STD_INPUT_PIPE, new input_pipe_class(input_buffer));

    test_runner->run_process_async();

    input_buffer->buffer << n << endl;
    input_buffer->set_ready();

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

    }
    return 0;

}