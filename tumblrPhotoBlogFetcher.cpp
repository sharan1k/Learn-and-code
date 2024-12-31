#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "httplib.h"
#include <iostream>
#include <string>
#include "json.hpp"
#include <sstream>

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

void printPhotoURL(const nlohmann::json &blogData, int startPost)
{
    if (blogData.contains("posts"))
    {
        const auto &posts = blogData["posts"];
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
}

void processBlogPosts(const std::string &jsonResponse, int startPost)
{
    nlohmann::json blogData;
    try
    {
        blogData = nlohmann::json::parse(jsonResponse);
    }
    catch (nlohmann::json::parse_error &error)
    {
        std::cerr << "Failed to parse JSON: " << error.what() << std::endl;
        return;
    }

    displayBlogInfo(blogData);
    printPhotoURL(blogData, startPost);
}

std::string buildTumblrApiUrl(const std::string &blogName, int startPost, int postCount)
{
    return "/api/read/json?type=photo&num=" + std::to_string(postCount) + "&start=" + std::to_string(startPost - 1);
}

std::string fetchBlogPosts(const std::string &blogName, int startPost, int postCount)
{
    httplib::Client client("https://" + blogName + ".tumblr.com");
    std::string apiUrl = buildTumblrApiUrl(blogName, startPost, postCount);

    auto response = client.Get(apiUrl.c_str());
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
    std::string postRange;

    std::cout << "Enter the Tumblr blog name: ";
    std::cin >> blogName;
    std::cout << "Enter the post range: ";
    std::cin >> postRange;

    int startPost, endPost;
    std::stringstream rangeStream(postRange);
    char dash;
    rangeStream >> startPost >> dash >> endPost;

    std::string responseBody = fetchBlogPosts(blogName, startPost, endPost - startPost + 1);
    if (responseBody.empty())
    {
        std::cerr << "Failed to fetch or retrieve valid response." << std::endl;
        return 1;
    }

    std::string cleanResponse = extractJson(responseBody);
    if (!cleanResponse.empty())
    {
        processBlogPosts(cleanResponse, startPost);
    }
    else
    {
        std::cerr << "Invalid JSON response from Tumblr API" << std::endl;
    }

    return 0;
}
