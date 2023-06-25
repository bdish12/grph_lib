#ifndef GRPH_EXCEPTIONS_H
#define GRPH_EXCEPTIONS_H

#include <exception>
#include <stdexcept>

//#include "../../graph/common/types.h"

namespace grph::exceptions {

    class invalid_edge : public std::invalid_argument {
    public:
        explicit invalid_edge(const std::string &error_message) : invalid_argument(error_message) {}
    };

}

#endif //GRPH_EXCEPTIONS_H
