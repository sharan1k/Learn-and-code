#include "../Inc/ArticleHandler.h"
#include <nlohmann/json.hpp>
#include <iostream>

ArticleHandler::ArticleHandler(std::shared_ptr<HttpClient> httpClient)
    : httpClient(httpClient) {
}

std::string ArticleHandler::extractErrorMessage(
    const httplib::Result& result,
    const std::string& defaultMessage,
    const std::map<int, std::string>& statusCodes) {

    std::string errorMessage = defaultMessage;

    try {
        auto errorJson = nlohmann::json::parse(result->body);

        if (errorJson.contains("message") && errorJson["message"].is_string()) {
            errorMessage = errorJson["message"];
        }

        auto it = statusCodes.find(result->status);
        if (it != statusCodes.end()) {
            errorMessage = it->second;
        }
    } catch (...) {
        errorMessage = defaultMessage + " (status " + std::to_string(result->status) + ")";
    }

    return errorMessage;
}

nlohmann::json ArticleHandler::parseResponse(
    const httplib::Result& result,
    bool& success,
    std::string& errorMessage) {

    if (!result) {
        success = false;
        errorMessage = "Network error";
        return nlohmann::json();
    }

    try {
        nlohmann::json response = nlohmann::json::parse(result->body);
        if (response["status"] == "success") {
            success = true;
            return response;
        } else {
            success = false;
            errorMessage = response.value("message", "Unknown error");
            return nlohmann::json();
        }
    } catch (const std::exception& e) {
        success = false;
        errorMessage = "Error parsing server response: " + std::string(e.what());
        return nlohmann::json();
    }
}

void ArticleHandler::getTodayHeadlines(int limit, HeadlinesCallback callback) {
    std::string endpoint = "/api/articles/headlines/today";
    if (limit > 0) {
        endpoint += "?limit=" + std::to_string(limit);
    }

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Article*> articles;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    for (const auto& articleJson : responseJson["data"]) {
                        Article* article = new Article(Article::fromJson(articleJson));
                        articles.push_back(article);
                    }

                    message = responseJson["message"];
                    callback(true, message, articles);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch today's headlines: " + response;
        }

        callback(false, message, articles);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::getHeadlinesByDateRange(const std::string& startDate, const std::string& endDate, int limit, HeadlinesCallback callback) {
    std::string endpoint = "/api/articles/headlines/date-range?startDate=" + startDate + "&endDate=" + endDate;

    if (limit > 0) {
        endpoint += "&limit=" + std::to_string(limit);
    }

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Article*> articles;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    for (const auto& articleJson : responseJson["data"]) {
                        Article* article = new Article(Article::fromJson(articleJson));
                        articles.push_back(article);
                    }

                    message = responseJson["message"];
                    callback(true, message, articles);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch headlines by date range: " + response;
        }

        callback(false, message, articles);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::getHeadlinesByCategory(unsigned int categoryId, int limit, HeadlinesCallback callback) {
    std::string endpoint = "/api/articles/headlines/category/" + std::to_string(categoryId);

    if (limit > 0) {
        endpoint += "?limit=" + std::to_string(limit);
    }

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Article*> articles;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    for (const auto& articleJson : responseJson["data"]) {
                        Article* article = new Article(Article::fromJson(articleJson));
                        articles.push_back(article);
                    }

                    message = responseJson["message"];
                    callback(true, message, articles);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch headlines by category: " + response;
        }

        callback(false, message, articles);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::getHeadlinesByDateRangeAndCategory(const std::string& startDate, const std::string& endDate, unsigned int categoryId, int limit, HeadlinesCallback callback) {
    std::string endpoint = "/api/articles/headlines/date-range?startDate=" + startDate + "&endDate=" + endDate + "&categoryId=" + std::to_string(categoryId);

    if (limit > 0) {
        endpoint += "&limit=" + std::to_string(limit);
    }

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Article*> articles;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    for (const auto& articleJson : responseJson["data"]) {
                        Article* article = new Article(Article::fromJson(articleJson));
                        articles.push_back(article);
                    }

                    message = responseJson["message"];
                    callback(true, message, articles);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch headlines by date range and category: " + response;
        }

        callback(false, message, articles);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::getArticleDetails(unsigned int articleId, ArticleDetailsCallback callback) {
    std::string endpoint = "/api/articles/" + std::to_string(articleId);

    auto httpCallback = [callback](const httplib::Result& result) {
        Article* article = nullptr;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    article = new Article(Article::fromJson(responseJson["data"]));

                    message = responseJson["message"];
                    callback(true, message, article);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch article details: " + response;
        }

        callback(false, message, article);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::getCategories(CategoriesCallback callback) {
    std::string endpoint = "/api/categories";

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Category*> categories;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    for (const auto& categoryJson : responseJson["data"]) {
                        Category* category = new Category();
                        category->categoryName = categoryJson["categoryName"];
                        category->categoryId = categoryJson["categoryId"];
                        categories.push_back(category);
                    }

                    message = responseJson["message"];
                    callback(true, message, categories);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch categories: " + response;
        }

        callback(false, message, categories);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::saveArticle(unsigned int userId, unsigned int articleId, StatusCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/saved-articles";

    nlohmann::json requestBody = {
        {"articleId", articleId}
    };

    auto httpCallback = [callback](const httplib::Result& result) {
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    message = responseJson["message"];
                    callback(true, message);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to save article: " + response;
        }

        callback(false, message);
    };

    httpClient->post(endpoint, requestBody.dump(), httpCallback);
}

void ArticleHandler::removeSavedArticle(unsigned int userId, unsigned int articleId, StatusCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/saved-articles/" + std::to_string(articleId);

    auto httpCallback = [callback](const httplib::Result& result) {
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    message = responseJson["message"];
                    callback(true, message);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to remove saved article: " + response;
        }

        callback(false, message);
    };

    httpClient->del(endpoint, httpCallback);
}

void ArticleHandler::getSavedArticles(unsigned int userId, SavedArticlesCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/saved-articles";

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Article*> articles;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";

        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);

                if (responseJson["status"] == "success") {
                    for (const auto& articleJson : responseJson["data"]) {
                        Article* article = new Article(Article::fromJson(articleJson));
                        articles.push_back(article);
                    }

                    message = responseJson["message"];
                    callback(true, message, articles);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch saved articles: " + response;
        }

        callback(false, message, articles);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::searchArticles(const std::string& query, int limit,
                               const std::string& startDate, const std::string& endDate,
                               const std::string& sortBy, bool isDescending,
                               HeadlinesCallback callback) {
    std::string encodedQuery = httpClient->urlEncode(query);

    std::string endpoint = "/api/search?query=" + encodedQuery;

    if (limit > 0) {
        endpoint += "&limit=" + std::to_string(limit);
    }

    if (!startDate.empty() && !endDate.empty()) {
        endpoint += "&startDate=" + startDate + "&endDate=" + endDate;
    }

    if (!sortBy.empty()) {
        endpoint += "&sortBy=" + sortBy;
        endpoint += "&order=" + std::string(isDescending ? "desc" : "asc");
    }

    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Article*> articles;
        std::string message;
        bool success = result && result->status == 200;

        if (result) {
            std::cout << "Search request received response with status: " << result->status << std::endl;

            if (result->status == 200) {
                try {
                    nlohmann::json responseJson = nlohmann::json::parse(result->body);

                    if (responseJson["status"] == "success") {
                        for (const auto& articleJson : responseJson["data"]) {
                            Article* article = new Article(Article::fromJson(articleJson));
                            articles.push_back(article);
                        }

                        message = responseJson["message"];
                        callback(true, message, articles);
                        return;
                    } else {
                        message = responseJson["message"];
                    }
                } catch (const std::exception& e) {
                    message = "Failed to parse response: " + std::string(e.what());
                }
            } else {
                message = "Server returned error code: " + std::to_string(result->status);
                nlohmann::json responseJson = nlohmann::json::parse(result->body);
                if (responseJson.contains("message")) {
                    message += " - " + responseJson["message"].get<std::string>();
                }
            }
        } else {
            message = "Failed to connect to server";
            if (result.error() != httplib::Error::Success) {
                message += " (Error code: " + std::to_string(static_cast<int>(result.error())) + ")";
            }
        }

        callback(false, message, articles);
    };

    httpClient->get(endpoint, httpCallback);
}

void ArticleHandler::likeArticle(unsigned int userId, unsigned int articleId, StatusCallback callback) {
    std::string endpoint = "/api/articles/" + std::to_string(articleId) + "/like";

    nlohmann::json requestBody;
    requestBody["userId"] = userId;

    auto httpCallback = [callback](const httplib::Result& result) {
        bool success = result && result->status == 200;
        std::string message;

        try {
            if (success) {
                nlohmann::json jsonResponse = nlohmann::json::parse(result->body);
                message = jsonResponse.contains("message") ? jsonResponse["message"].get<std::string>() : "Article liked successfully";
            } else {
                message = result ? "Failed with status: " + std::to_string(result->status) : "Failed to connect to server";

                if (result && !result->body.empty()) {
                    nlohmann::json errorResponse = nlohmann::json::parse(result->body);
                    if (errorResponse.contains("message")) {
                        message = errorResponse["message"].get<std::string>();
                    }
                }
            }
        } catch (const std::exception& e) {
            success = false;
            message = "Error processing response: " + std::string(e.what());
        }

        if (callback) {
            callback(success, message);
        }
    };

    httpClient->post(endpoint, requestBody.dump(), httpCallback);
}

void ArticleHandler::dislikeArticle(unsigned int userId, unsigned int articleId, StatusCallback callback) {
    std::string endpoint = "/api/articles/" + std::to_string(articleId) + "/dislike";

    nlohmann::json requestBody;
    requestBody["userId"] = userId;

    auto httpCallback = [callback](const httplib::Result& result) {
        bool success = result && result->status == 200;
        std::string message;

        try {
            if (success) {
                nlohmann::json jsonResponse = nlohmann::json::parse(result->body);
                message = jsonResponse.contains("message") ? jsonResponse["message"].get<std::string>() : "Article disliked successfully";
            } else {
                message = result ? "Failed with status: " + std::to_string(result->status) : "Failed to connect to server";

                if (result && !result->body.empty()) {
                    nlohmann::json errorResponse = nlohmann::json::parse(result->body);
                    if (errorResponse.contains("message")) {
                        message = errorResponse["message"].get<std::string>();
                    }
                }
            }
        } catch (const std::exception& e) {
            success = false;
            message = "Error processing response: " + std::string(e.what());
        }

        if (callback) {
            callback(success, message);
        }
    };

    httpClient->post(endpoint, requestBody.dump(), httpCallback);
}



