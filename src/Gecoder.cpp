#include "Geocoder.h"
#include <iostream>
#include <exception>

Geocoder::Geocoder(std::shared_ptr<IGeocoder> api)
    : geocoderAPI(std::move(api)) {}

void Geocoder::displayCoordinates(const std::string &place) const
{
    try
    {
        Coordinates coordinatess = geocoderAPI->getCoordinates(place);
        std::cout << "Latitude: " << coordinatess.latitude << "\nLongitude: " << coordinatess.longitude << std::endl;
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Error retrieving coordinates: " << exception.what() << std::endl;
    }
}
