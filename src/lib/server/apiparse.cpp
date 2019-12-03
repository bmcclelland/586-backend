#include "server/apiparse.h"

namespace mvc::api_parsers::detail
{
    State::State(ApiPath const& path)
        : State(path.begin(), path.end())
    {}

    State::State(ApiPath::Iter iter, ApiPath::Iter end)
        : _iter(iter)
        , _end(end)
    {}

    bool State::at_end() const
    {
        return _iter == _end;
    }

    ApiSegment const& State::segment() const
    {
        if (at_end())
        {
            throw 666;
        }
        return *_iter;
    }

    State State::next() const
    {
        return State(std::next(_iter), _end);
    }
}
