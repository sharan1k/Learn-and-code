#ifndef GOOGLE_GEOCODING_API_H
#define GOOGLE_GEOCODING_API_H

#include "IGeocoder.h"
#include "CommonTypes.h"

class GoogleGeocodingAPI : public IGeocoder
{
public:
    Coordinates getCoordinates(const std::string &place) override;

private:
    Coordinates parseCoordinates(const std::string &response);
};

#endif
