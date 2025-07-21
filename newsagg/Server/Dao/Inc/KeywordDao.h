#pragma once

#include <string>
#include <vector>
#include <map>

class KeywordDao {
public:
    std::vector<std::string> getByUser(unsigned int userId);
    std::map<unsigned int, std::vector<std::string>> getAll();
    bool add(unsigned int userId, const std::string& keyword);
    bool remove(unsigned int userId, const std::string& keyword);
};
