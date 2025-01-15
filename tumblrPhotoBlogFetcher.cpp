#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>
#include <string>
#include <sstream>
#include "httplib.h" //HTTP/HTTPS client-server library for sending requests and handling responses
#include "json.hpp" // JSON library (nlohmann) for parsing and handling JSON data

struct PostRange
{
    int startPost;
    int endPost;

    int getPostCount() const
    {
        return endPost - startPost + 1;
    }
};

std::string extractJson(const std::string &rawResponse)
{
    size_t jsonStart = rawResponse.find("{");
    size_t jsonEnd = rawResponse.rfind("}");

    if (jsonStart != std::string::npos && jsonEnd != std::string::npos)
    {
        return rawResponse.substr(jsonStart, jsonEnd - jsonStart + 1);
    }
    return "";
}

void displayBlogInfo(const nlohmann::json &blogData)
{
    auto blogInfo = blogData["tumblelog"];

    std::cout << "Title: " << blogInfo["title"] << std::endl;
    std::cout << "Name: " << blogInfo["name"] << std::endl;
    std::cout << "Description: " << blogInfo["description"] << std::endl;

    int totalPosts = blogData["posts-total"];
    std::cout << "Total number of posts: " << totalPosts << std::endl;
    std::cout << std::endl;
}

void displayPhotoURL(const nlohmann::json &posts, int startPost)
{
    int postCount = posts.size();
    int postSerialNumber = startPost;

    for (int index = 0; index < postCount; index++)
    {
        const auto &post = posts[index];
        std::cout << postSerialNumber << ". ";

        if (post.contains("photo-url-1280"))
        {
            auto &photo = post["photo-url-1280"];
            if (photo.is_string())
            {
                std::cout << photo.get<std::string>() << std::endl;
            }
        }
        postSerialNumber++;
    }
}

nlohmann::json parseJsonResponse(const std::string &jsonResponse)
{
    try
    {
        return nlohmann::json::parse(jsonResponse);
    }
    catch (nlohmann::json::parse_error &error)
    {
        std::cerr << "Failed to parse JSON: " << error.what() << std::endl;
        return nullptr;
    }
}

void handleBlogPosts(const nlohmann::json &blogData, const PostRange &range)
{
    if (blogData.is_null())
    {
        std::cerr << "Invalid JSON data" << std::endl;
        return;
    }

    displayBlogInfo(blogData);

    if (blogData.contains("posts"))
    {
        const auto &posts = blogData["posts"];
        displayPhotoURL(posts, range.startPost);
    }
}

std::string buildTumblrApiUrl(const PostRange &range)
{
    // Builds the Tumblr API URL for fetching photo posts within the specified range
    return "/api/read/json?type=photo&num=" + std::to_string(range.getPostCount()) + "&start=" + std::to_string(range.startPost - 1);
}

std::string fetchBlogPosts(const std::string &blogName, const PostRange &range)
{
    httplib::Client tumblrClient("https://" + blogName + ".tumblr.com");
    std::string apiUrl = buildTumblrApiUrl(range);

    // Send a GET request to the Tumblr API using the constructed URL
    auto response = tumblrClient.Get(apiUrl.c_str());
    if (response && response->status == 200)
    {
        return response->body;
    }
    else if (response->status == 404)
    {
        std::cout << "Blog with the specified name does not exit" << std::endl;
        return "";
    }
    else
    {
        std::cerr << "Request failed with status: " << (response ? response->status : -1) << std::endl;
        return "";
    }
}

int main()
{
    std::string blogName;
    std::string postRangeInput;

    std::cout << "Enter the Tumblr blog name: ";
    std::cin >> blogName;

    std::cout << "Enter the post range: ";
    std::cin >> postRangeInput;

    PostRange range;
    std::stringstream rangeStream(postRangeInput);
    char dash;
    rangeStream >> range.startPost >> dash >> range.endPost;

    std::string responseBody = fetchBlogPosts(blogName, range);
    if (responseBody.empty())
    {
        std::cerr << "Failed to fetch or retrieve valid response." << std::endl;
        return 1;
    }

    std::string cleanResponse = extractJson(responseBody);
    if (!cleanResponse.empty())
    {
        auto blogData = parseJsonResponse(cleanResponse);
        handleBlogPosts(blogData, range);
    }
    else
    {
        std::cerr << "Invalid JSON response from Tumblr API" << std::endl;
    }

    return 0;
}
