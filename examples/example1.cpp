#include "../include/cmdline_parser.hpp"

int main(int argc, char* argv[]) {

    //value options captures options that takes in arguments with argument format checking
    value_option<std::string> host_option("H", "host", "server host name", false, "127.0.0.1");

    //switch option captures flag options
    switch_option long_option("abc", "long option", true);
    switch_option unused_option("unused", "unused option");

    //positional arguments are arguments after options
    positional_arg<std::string> arg1("argument 1");
    positional_arg<std::string> arg2("argument 2");

    cmdline_parser c;
    c.add(host_option);
    c.add(long_option);
    c.add(unused_option);
    c.add(arg1);
    c.add(arg2);

    try {
        //parse the command line arguments
        c.parseopt(argc, argv);
    }
    catch (OptionParsingError e) {
        //capture and print any parsing errors
        std::cerr << e.what() << std::endl;
        return 1;
    }

    //default help option is added when cmdline_parser is created
    //check if the switch is set by calling is_help_selected()
    if (c.is_help_selected()) {
        //display options by calling output operator
        std::cout << c;
        return 0;
    }

    //print out options
    std::cout << "host: " << host_option.value() << std::endl;
    std::cout << "abc: " << long_option.isset() << std::endl;
    std::cout << "unused: " << unused_option.isset() << std::endl;
    std::cout << "arg1: " << arg1.value() << std::endl;
    std::cout << "arg2: " << arg2.value() << std::endl;

    return 0;
}
