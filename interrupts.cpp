/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /****************** VARIABLES ******************/
    
    int current_time = 0;           // keeps track of simulation time (in ms)
    int context_save_time = 10;     // time to save/restore CPU context
    int isr_activity_time = 40;     // time for each ISR step (as given in spec)

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /****************** MAIN SIMULATION ******************/

        if (activity == "CPU") {
            // CPU burst: program is running on the CPU normally
            execution += std::to_string(current_time) + ", "+ std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        }

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
