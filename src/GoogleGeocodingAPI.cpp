#include "GoogleGeocodingAPI.h"
#include "config.h"
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

#define CPPHTTPLIB_OPENSSL_SUPPORT

Coordinates GoogleGeocodingAPI::getCoordinates(const std::string &place)
{
    std::string encodedPlace = httplib::detail::encode_url(place);
    std::string url = "/maps/api/geocode/json?address=" + encodedPlace + "&key=" + GOOGLE_API_KEY;

    httplib::SSLClient client("maps.googleapis.com");
    auto res = client.Get(url.c_str());

    if (!res || res->status != 200)
    {
        throw std::runtime_error("Failed to get valid response from Google API");
    }

    return parseCoordinates(res->body);
}

Coordinates GoogleGeocodingAPI::parseCoordinates(const std::string &response)
{
    auto jsonResponse = json::parse(response);

    if (jsonResponse["status"] == "OK")
    {
        double latitude = jsonResponse["results"][0]["geometry"]["location"]["lat"];
        double longitude = jsonResponse["results"][0]["geometry"]["location"]["lng"];
        return {latitude, longitude};
    }
    else
    {
        throw std::runtime_error("API Error: " + jsonResponse["status"].get<std::string>());
    }
}
