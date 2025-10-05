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
        else if (activity == "SYSCALL") {
            //System call: CPU requests an I/O operation
            int device_num = duration_intr;  //duration_intr = device number here
            
            //run the boilerplate sequence (switch to kernel mode, save context, etc.)
            auto [boilerplate_str, new_time] = intr_boilerplate(current_time, device_num,context_save_time, vectors);
            execution += boilerplate_str;
            current_time = new_time;
            
            //Step 1: run the ISR (device driver)
            execution += std::to_string(current_time) + ", " + std::to_string(isr_activity_time) + ", SYSCALL: run the ISR (device driver)\n";
            current_time += isr_activity_time;
            
            //Step 2: transfer data between device and memory
            execution += std::to_string(current_time) + ", " + std::to_string(isr_activity_time) + ", transfer data from device to memory\n";
            current_time += isr_activity_time;
            
            //Step 3: checking for errors (whatever time is left from the device delay)
            int device_delay = delays.at(device_num);
            int remaining_time = device_delay - (2 * isr_activity_time);
            
            if (remaining_time > 0) {
                execution += std::to_string(current_time) + ", " + std::to_string(remaining_time) + ", check for errors\n";
                current_time += remaining_time;
            }

            /************************************************************************/
            else if (activity == "END_IO") {
            //End of I/O: device signals that it finished its operation
            int device_num = duration_intr;
            
            //runs the boilerplate (switch to kernel mode, save context, etc.)
            auto [boilerplate_str, new_time] = intr_boilerplate(current_time, device_num, context_save_time, vectors);
            execution += boilerplate_str;
            current_time = new_time;
            
            //Step 1: run the ISR for device completion
            execution += std::to_string(current_time) + ", " + std::to_string(isr_activity_time) + ", ENDIO: run the ISR (device driver)\n";
            current_time += isr_activity_time;
            
            //Step 2: check device status (whatever time is left from the device delay)
            int device_delay = delays.at(device_num);
            int remaining_time = device_delay - isr_activity_time;
            
            if (remaining_time > 0) {
                execution += std::to_string(current_time) + ", " + std::to_string(remaining_time) + ", check device status\n";
                current_time += remaining_time;
            }
            
        }

    }

    input_file.close();

    write_output(execution);

    return 0;
}
