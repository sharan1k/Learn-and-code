#pragma once

#include <vector>
#include <memory>
#include <string>
#include "../../../Common/Inc/Dto/Article.h"

class INewsSource {
public:
    virtual ~INewsSource() = default;
    virtual std::vector<Article> fetchNews() = 0;
    virtual bool initialize(const std::string& apiKey) = 0;
    virtual std::string getName() const = 0;
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;
};
