#pragma once

#include <string>
#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

// Parsing Exceptions
class OptionParsingError : public std::exception {
public:
    OptionParsingError(const std::string& message) :
        std::exception(message.c_str()) {

    }
};

class InvalidOptionFormatError : public OptionParsingError {
public:
    InvalidOptionFormatError(const std::string& message) :
        OptionParsingError(message) {

    }
};

class InvalidOptionError : public OptionParsingError {
public:
    InvalidOptionError(const std::string& message) :
        OptionParsingError(message) {

    }
};

class InvalidArgumentFormatError : public OptionParsingError {
public:
    InvalidArgumentFormatError(const std::string& message) :
        OptionParsingError(message) {

    }
};

class MissingArgumentError : public OptionParsingError {
public:
    MissingArgumentError(const std::string& message) :
        OptionParsingError(message) {

    }
};

// Template specializations for common argument formats
template<typename T>
T parse_value(const std::string& value) {
    T t(value);
    return t;
}

template<>
int parse_value(const std::string& value) {
    return stoi(value);
}

template<>
short parse_value(const std::string& value) {
    return (short)stoi(value);
}

template<>
long parse_value(const std::string& value) {
    return stol(value);
}

template<>
long long parse_value(const std::string& value) {
    return stoll(value);
}

template<>
unsigned long parse_value(const std::string& value) {
    return stoul(value);
}

template<>
unsigned long long parse_value(const std::string& value) {
    return stoull(value);
}

template<>
float parse_value(const std::string& value) {
    return stof(value);
}

template<>
double parse_value(const std::string& value) {
    return stod(value);
}

class option {
protected:
    std::string m_short_name;
    std::string m_long_name;
    std::string m_description;
    bool m_required;
    bool m_isset;

public:
    option(const std::string& short_name, const std::string& long_name, const std::string& description, bool required = false) :
        m_description(description),
        m_required(required),
        m_isset(false) {
            if(!is_valid_option_format(short_name) || !is_valid_option_format(long_name)) {
                throw InvalidOptionFormatError("invalid option format, option cannot start with '-'");
            }
            else {
                if(!is_short_name(short_name)) {
                    throw InvalidOptionFormatError("short option can only have 1 character");
                }
                if(!is_long_name(long_name)) {
                    throw InvalidOptionFormatError("long option must be longer than 1 character");
                }
            }
            m_short_name.assign(short_name);
            m_long_name.assign(long_name);
    }

    option(const std::string& name,  const std::string& description, bool required = false) :
        m_description(description),
        m_required(required),
        m_isset(false) {
            if(!is_valid_option_format(name)) {
                throw InvalidOptionFormatError("option name cannot start with '-'");
            }
            else {
                if(!is_long_name(name)) {
                    throw InvalidOptionFormatError("long option name required");
                }
            }
            m_long_name = name;
    }

    bool isset() {
        return m_isset;
    }

    bool required() {
        return m_required;
    }

    std::string description() {
        return m_description;
    }

    std::string name() {
        return m_long_name;
    }

    std::string long_name() {
        return m_long_name;
    }

    std::string short_name() {
        return m_short_name;
    }

public:
    virtual int parse_arguments(int argc, char* argv[], int cur_pos) = 0;

    friend std::ostream& operator<< (std::ostream& out, const option& opt);

private:
    bool is_valid_option_format(const std::string& opt) {
        if(opt.find("-") == 0)
            return false;
        return true;
    }
    bool is_long_name(const std::string& opt) {
        return opt.length() > 1;
    }
    bool is_short_name(const std::string& opt) {
        return opt.length() == 1;
    }
};

class switch_option : public option {
public:
    switch_option(const std::string& short_name, const std::string& long_name, const std::string& description, bool default_isset = false) :
        option(short_name, long_name, description, false) {
            m_isset = default_isset;
    }

    switch_option(const std::string& name, const std::string& description, bool default_isset = false) :
        option(name, description, false) {
            m_isset = default_isset;
    }

    int parse_arguments(int argc, char* argv[], int cur_pos) {
        m_isset = true;
        return cur_pos;
    }
};

template<typename T>
class value_option : public option {
public:
    value_option(const std::string& short_name, const std::string& long_name, const std::string& description, bool required, T default_value) :
        option(short_name, long_name, description, required),
        m_value(default_value) {

    }

    value_option(const std::string& name, const std::string& description, bool required, T default_value) :
        option(name, description, required),
        m_value(default_value) {

    }

    int parse_arguments(int argc, char* argv[], int cur_pos) {
        try {
            return consume_n_arguments(argc, argv, cur_pos + 1, 1); //capture all arguments after option;
        }
        catch(std::invalid_argument e) {
            throw InvalidArgumentFormatError("invalid argument format: " + std::string(argv[cur_pos + 1])); 
        }
    }

    T value() {
        return m_value;
    }

private:

    bool is_argument(const std::string& s) {
        return s.find("-") != 0;
    }

    int consume_n_arguments(int argc, char* argv[], int cur_pos, int num_consume) {
        //while not end of all arguments, consume all arguments that are not options
        //stop when you encounter a new option
        int left = num_consume;
        while(cur_pos < argc && left > 0) {
            if(is_argument(std::string(argv[cur_pos]))) {
                //std::cout << "parameter: " << argv[cur_pos] << std::endl;
                m_value = parse_value<T>(argv[cur_pos]);
                left--;
                cur_pos++;
            }
            else {
                std::ostringstream oss;
                oss << "missing option arguments, expected: " << num_consume << " got: " << num_consume - left << std::endl;
                throw MissingArgumentError(oss.str());                
            }        
        }
        return cur_pos - 1;   
    }

    T m_value;
};

class arg {
public:
    virtual void parse_arguments(const std::string& arguments) = 0;
};

template<typename T>
class positional_arg : public arg {
public:
    void parse_arguments(const std::string& arguments) {
        try {
            m_value = parse_value<T>(arguments);
        }
        catch(std::invalid_argument e) {
            throw InvalidArgumentFormatError("invalid argument format: " + arguments);
        }        
    }

    T value() {
        return m_value;
    }
private:
    T m_value;
};

class cmdline_parser {
public:
    void add(option* opt) {
        m_optlist.push_back(opt);
    }

    void add(option& opt) {
        add(&opt);
    }

    void add(arg* argument) {
        m_posargs.push_back(argument);
    }

    void add(arg& argument) {
        m_posargs.push_back(&argument);
    }

    void parseopt(int argc, char* argv[]) {
        for(int i = 1; i < argc; ++i) {
            std::string argument(argv[i]);
            if(is_long_option(argument)) {
                std::string option_name = argument.substr(2);
                option* opt = NULL;
                if(option_exists(option_name, m_optlist, &opt)) {                    
                    i = opt->parse_arguments(argc, argv, i);
                }
                else {
                    throw InvalidOptionError("invalid option " + argument);
                }
            }         
            else if(is_short_option(argument)) {
                std::string option_name = argument.substr(1);
                option* opt = NULL;
                if(option_exists(option_name, m_optlist, &opt)) {
                    i = opt->parse_arguments(argc, argv, i);
                }
                else {
                    throw InvalidOptionError("invalid option " + argument);
                }
            }
            else {
                std::cout << "positional args: " << argv[i] << std::endl;
                if(!m_posargs.empty()) {
                    arg* a = m_posargs.front();
                    a->parse_arguments(argument);
                    m_posargs.pop_front();
                }
                else {
                    throw InvalidOptionError("unexpected positional arguments found: " + argument);
                }
            }
        }
    }

    friend std::ostream& operator<< (std::ostream& out, const cmdline_parser& c);

private:

    bool is_short_option(const std::string& s) {    
        return s.find("-") == 0;   
    }

    bool is_long_option(const std::string& s) {
        return s.find("--") == 0;
    }

    bool option_exists(const std::string& s, const std::list<option*>& e, option** o) {
        std::list<option*>::const_iterator it = e.begin();
        std::list<option*>::const_iterator end = e.end();
        while(it != end) {
            if((*it)->long_name() == s || (*it)->short_name() == s) {
                *o = *it;
                return true;
            }
            ++it;
        }
        return false;
    }

    std::list<option*> m_optlist;
    std::list<arg*> m_posargs;
};

// Overloaded Output Stream Operators
std::ostream& operator<< (std::ostream& out, const option& opt) {
    if(opt.m_short_name.empty()) {
        std::string uppercase(opt.m_long_name);
        std::transform(opt.m_long_name.begin(), opt.m_long_name.end(), uppercase.begin(), ::toupper);
        out << "--" << opt.m_long_name << ", " << uppercase << "\t" << opt.m_description;
    }
    else
        out << "-" << opt.m_short_name <<", --" << opt.m_long_name << "\t" << opt.m_description;

    return out;
}

std::ostream& operator<< (std::ostream& out, const cmdline_parser& c) {
    out << "optional arguments: " << std::endl;
    for(option* o : c.m_optlist) {
        out << *o << std::endl;
    }
    return out;
}
