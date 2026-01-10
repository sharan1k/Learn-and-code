#ifndef IGEOCODER_H
#define IGEOCODER_H

#include <string>
#include "CommonTypes.h"

class IGeocoder
{
public:
    virtual ~IGeocoder() = default;
    virtual Coordinates getCoordinates(const std::string &place) = 0;
};

#endif