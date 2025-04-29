#ifndef GEOCODER_H
#define GEOCODER_H

#include <memory>
#include <string>
#include "IGeocoder.h"
#include "CommonTypes.h"

class Geocoder
{
public:
    Geocoder(std::shared_ptr<IGeocoder> api);
    void displayCoordinates(const std::string &place) const;

private:
    std::shared_ptr<IGeocoder> geocoderAPI;
};

#endif
