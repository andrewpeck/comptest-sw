#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <iterator>
#include <iostream>

template <class T>
void Parser<T>::parseDataField (const std::string* fieldline) {

    //std::cout << *fieldline << std::endl;

    int value = -1;

    boost::char_separator<char> sep("=");
    boost::tokenizer<boost::char_separator<char>> tokens(*fieldline, sep);

    Dict test_types;

    test_types ["OFFSET"] = test_offset;
    test_types ["THRESH"] = test_thresh;

    std::string field;

    int i = 0;

    for (const auto& token : tokens) {
        if (i==0)
            field = token;
        else if (i== 1) {
            if (token=="TEST")
                value = test_types[token];
            else
                value = strtol(token.c_str(), NULL, 10);
        }
        i++;
    }

    (*_params) [field] = value;
}

template <class T>
void Parser<T>::parseDataLine (const std::string* dataline)
{
    //std::cout << *dataline << std::endl;
    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char>> tokens(*dataline, sep);

    for (const auto& t : tokens) {
        const std::string* field = &t;
        parseDataField(field);
    }
}


template <class T>
void Parser<T>::parseBinaryLine (const std::string *line)
{
    for (int i=0; i<line->length()/4; i++) {
        _data [i] = (T) strtol (line->substr(i*4,4).c_str(), NULL, 16);
        // std::cout << std::hex << data[i] << std::endl;
    }
    (*_params) ["DATA_RX"] = 1;
    //printf("sys  :: loaded data to array\n");
}

template <class T>
void Parser<T>::parseBuffer (char* buffer, int size) {

    //std::cout << buffer << std::endl;

    std::string msg_type (buffer,6);
    std::string msg      (buffer+6);

    if ("INFO::"==msg_type) {
        std::cout << "info :: ";
        std::cout << msg << std::endl;
    }
    else if ("DATA::"==msg_type) {
        parseDataLine (&msg);
    }
    else if ("BIN:::"==msg_type) {
        parseBinaryLine (&msg);
    }
}
