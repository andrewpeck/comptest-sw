#ifndef RX_PARSER_H
#define RX_PARSER_H

#include <stdint.h>

#include "test_enums.h"
#include "dict.h"
#include <vector>

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
        std::vector<uint8_t>*  getDeltas();

        void parseTimeLine (const std::string* dataline);
    private:

        Dict* _params;
        T* _data;
        char* read_data;
        std::vector<uint8_t> deltas [8];

};

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <iterator>
#include <iostream>

template <class T>
void Parser<T>::parseDataField (const std::string* fieldline) {

    //------------------------------------------------------------------------------------------------------------------
    const bool debug=0;
    //------------------------------------------------------------------------------------------------------------------

    if (debug) std::cout << "fieldline: " << *fieldline << std::endl;

    int value = -1;

    boost::char_separator<char> sep("=");
    boost::tokenizer<boost::char_separator<char>> tokens(*fieldline, sep);

    Dict test_types;

    test_types ["TIMING"]    = test_timing;
    test_types ["MODE"]    = test_mode;
    test_types ["OFFSET"]  = test_offset;
    test_types ["THRESH"]  = test_thresh;
    test_types ["CURRENT"] = test_currents;
    test_types ["COMPOUT"] = test_compout;

    std::string field;

    int i = 0;

    for (const auto& token : tokens) {
        if (i==0) {
            field = token;
        }
        else if (i== 1) {

            if (field=="TEST")
                value = test_types[token];
            else
                value = strtol(token.c_str(), NULL, 10);

        }
        i++;
    }

    if (debug) std::cout << "    field = " << field << "  value = " << value << std::endl;
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
void Parser<T>::parseTimeLine (const std::string* dataline)
{
    const bool debug = 0;

    if (debug) std::cout << "parseTimeLine" << std::endl;
    if (debug) std::cout << *dataline << std::endl;

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char>> tokens(*dataline, sep);

    uint8_t time=0;

    int i = 0;
    for (const auto& token : tokens) {

        if (i==0) {

            i++;

            const std::string* field = &token;
            parseDataField(field);

            time = (*_params)["pktime"];

            if (debug) printf ("pktime=%i ", time);

            deltas[time].clear();

        }
        else {
            uint16_t value = strtol(token.c_str(), NULL, 10);
            deltas[time].push_back(value);

            if (debug) printf (" %2i ", deltas[time].back());
        }
    }

    if (debug) printf("\n");

}


template <class T>
void Parser<T>::parseBinaryLine (const std::string *line)
{
    const bool debug=0;
    // parse line of data, taking each 4 hex characters as a 16 bit number
    for (int i=0; i<line->length()/4; i++) {
        uint16_t data = strtol (line->substr(i*4,4).c_str(), NULL, 16);
        _data [i] = (T) data;
        if (debug) std::cout << std::hex << data << std::endl;
    }
    (*_params) ["DATA_RX"] = 1;

    if (debug) printf("sys  :: loaded data to array\n");
}

template <class T>
void Parser<T>::parseBuffer (char* buffer, int size) {

    const bool debug = 0;

    if (debug) std::cout<<buffer;
    // cut the first 6 characters from the string as the header e.g. "DATA::"
    std::string msg_type (buffer,6);
    std::string msg      (buffer+6);

    if ("INFO::"==msg_type) {
    }
    else if ("DATA::"==msg_type) {
        parseDataLine (&msg);
    }
    else if ("BIN:::"==msg_type) {
        parseBinaryLine (&msg);
    }
    else if ("TIME::"==msg_type) {
        parseTimeLine (&msg);
    }
}

template <class T>
std::vector<uint8_t>*  Parser<T>::getDeltas()
{
    return deltas;
}

#endif /* RX_PARSER_H */
