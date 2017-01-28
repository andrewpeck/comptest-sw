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

#include "rx_parser.inc"

#endif /* RX_PARSER_H */
