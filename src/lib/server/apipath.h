#pragma once

#include "util/newtype.h"
#include "util/string.h"
#include "util/containers.h"
#include "http.h"

namespace mvc 
{
    NEW_TYPE(String, ApiSegment);

    class ApiPath
    {
        public:
            using Iter = Vec<ApiSegment>::const_iterator;

            ApiPath(HttpRequest const&);
            ApiPath(std::initializer_list<ApiSegment> const&);

            size_t size() const;
            Iter begin() const;
            Iter end() const;

        private:
            Vec<ApiSegment> _segments;
    };
}
