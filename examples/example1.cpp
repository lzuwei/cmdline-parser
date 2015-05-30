#include "../include/cmdline_parser.hpp"

int main(int argc, char* argv[]) {    

    value_option<std::string> host_option("H", "host", "server host name", false, "127.0.0.1");
    switch_option long_option("abc", "long option", true);
    switch_option unused_option("unused", "unused option");
    positional_arg<std::string> arg1;
    positional_arg<std::string> arg2;

    cmdline_parser c;
    c.add(host_option);
    c.add(long_option);
    c.add(unused_option);
    c.add(arg1);
    c.add(arg2);

    try {
        c.parseopt(argc, argv);
    }
    catch(OptionParsingError e) {
        std::cerr << e.what() << std::endl;
        std::cin.get();
        return 1;
    }   

    if(c.is_help_selected()) {
        std::cout << c;
    }
    std::cout << "host: " << host_option.value() << std::endl;
    std::cout << "abc: " << long_option.isset() << std::endl;
    std::cout << "unused: " << unused_option.isset() << std::endl;
    std::cout << "arg1: " << arg1.value() << std::endl;
    std::cout << "arg2: " << arg2.value() << std::endl;

    std::cin.get();
    return 0;
}