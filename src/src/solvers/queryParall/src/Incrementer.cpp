/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "../Incrementer.h"

namespace mt::qpar {
    Incrementer::Incrementer(const mt::Nodes& nodes, const Nodes::const_reverse_iterator& delimiter, const std::size_t& startPos, const std::size_t& delta)
        : end(nodes.crend())
        , cursor(delimiter)
        , delta(startPos) {
        ++(*this);
        this->delta = delta;
    };

    Incrementer& Incrementer::operator++() {
        std::size_t k = 0;
        while ((k < this->delta) && (this->cursor != this->end)) {
            ++this->cursor;
            ++k;
        }
        return *this;
    };
}