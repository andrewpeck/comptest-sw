#ifndef RX_PARSER_H
#define RX_PARSER_H

#include <stdint.h>

#include "test_enums.h"
#include "dict.h"

template <class T>
class Parser {

    public:

        Parser (Dict* params, T* data) :
            _params(params), _data(data)
            {};

        void parseBuffer (char* buffer, int size);
        void parseBinaryLine (const std::string *line);
        void parseDataLine (const std::string *dataline);
        void parseDataField (const std::string *fieldline);

    private:

        Dict* _params;
        T* _data;
        char* read_data;

};

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
    test_types ["CURRENT"] = test_currents;

    std::string field;

    int i = 0;

    for (const auto& token : tokens) {
        if (i==0)
            field = token;
        else if (i== 1) {
            if (field=="TEST")
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
        uint16_t data = strtol (line->substr(i*4,4).c_str(), NULL, 16);
        _data [i] = (T) data;
        std::cout << std::hex << data << std::endl;
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
        //std::cout << "info :: ";
        //std::cout << msg << std::endl;
    }
    else if ("DATA::"==msg_type) {
        //std::cout << "data :: ";
        //std::cout << msg << std::endl;
        parseDataLine (&msg);
    }
    else if ("BIN:::"==msg_type) {
      //  std::cout << "bin :: ";
      //  std::cout << msg << std::endl;
        parseBinaryLine (&msg);
    }
}

#endif /* RX_PARSER_H */
