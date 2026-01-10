#include <iostream>
#include <memory>
#include "Geocoder.h"
#include "GoogleGeocodingAPI.h"

int main()
{
    std::string place;
    std::cout << "Enter a place name: ";
    std::getline(std::cin, place);

    std::shared_ptr<IGeocoder> googleGeocodingApi = std::make_shared<GoogleGeocodingAPI>();
    Geocoder geocoder(googleGeocodingApi);

    geocoder.displayCoordinates(place);

    return 0;
}
