#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sqlite3.h>
#include <json.hpp>

struct Item {
    std::string name;
    double price;
    int quantity;
};

double getItemTotal(const Item& item) {
    return item.price * item.quantity;
}

double getSubtotal(const std::vector<Item>& items) {
    double subtotal = 0;
    for (const auto& item : items) {
        subtotal += getItemTotal(item);
    }
    return subtotal;
}

double calculateGST(double total) {
    return total * 0.18;
}

double calculateTotal(double subtotal, double shippingCost, double discount) {
    return subtotal + shippingCost - discount;
}

std::string createOrderJson(const std::vector<Item>& items, double shippingCost, double discount, double subtotal, double gst, double total) {
    nlohmann::json orderJson;
    orderJson["items"] = nlohmann::json::array();

    for (const auto& item : items) {
        nlohmann::json itemJson;
        itemJson["name"] = item.name;
        itemJson["price"] = item.price;
        itemJson["quantity"] = item.quantity;
        itemJson["total"] = getItemTotal(item);
        orderJson["items"].push_back(itemJson);
    }

    orderJson["shippingCost"] = shippingCost;
    orderJson["discount"] = discount;
    orderJson["subtotal"] = subtotal;
    orderJson["gst"] = gst;
    orderJson["total"] = total;

    return orderJson.dump();  
}

sqlite3* initDatabase() {
    sqlite3* dataBase;
    if (sqlite3_open("orders.db", &dataBase)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(dataBase) << std::endl;
        return nullptr;
    }

    const char* createTableSqlQuery = R"(
        CREATE TABLE IF NOT EXISTS orders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            order_data TEXT
        );
    )";
    
    char* errMsg = nullptr;
    if (sqlite3_exec(dataBase, createTableSqlQuery, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(dataBase);
        return nullptr;
    }

    return dataBase;
}

void saveOrderToDatabase(sqlite3* dataBase, const std::string& jsonData) {
    const char* insertSqlQuery = "INSERT INTO orders (order_data) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(dataBase, insertSqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(dataBase) << std::endl;
        return;
    }

    if (sqlite3_bind_text(stmt, 1, jsonData.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind text: " << sqlite3_errmsg(dataBase) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(dataBase) << std::endl;
    } else {
        std::cout << "Order saved to database!" << std::endl;
    }

    sqlite3_finalize(stmt);
}

void displayOrderSummary(const std::vector<Item>& items, double shippingCost, double discount) {
    double subtotal = getSubtotal(items);
    double gst = calculateGST(subtotal);
    double total = calculateTotal(subtotal, shippingCost, discount);

    std::cout << "Order Summary:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Subtotal: " << std::fixed << std::setprecision(2) << subtotal << std::endl;
    std::cout << "Shipping Cost: " << std::fixed << std::setprecision(2) << shippingCost << std::endl;
    std::cout << "Discount: " << std::fixed << std::setprecision(2) << discount << std::endl;
    std::cout << "GST (18%): " << std::fixed << std::setprecision(2) << gst << std::endl;
    std::cout << "Total: " << std::fixed << std::setprecision(2) << total << std::endl;
}

int main() {
    sqlite3* dataBase = initDatabase();
    if (!dataBase) {
        return -1;  
    }

    std::vector<Item> items = {
        {"Telivision", 25000, 2},
        {"Remote", 200, 2}
    };

    double shippingCost = 200;  
    double discount = 1000;     

    double subtotal = getSubtotal(items);
    double gst = calculateGST(subtotal);
    double total = calculateTotal(subtotal, shippingCost, discount);

    displayOrderSummary(items, shippingCost, discount);

    std::string jsonData = createOrderJson(items, shippingCost, discount, subtotal, gst, total);
    saveOrderToDatabase(dataBase, jsonData);

    sqlite3_close(dataBase);

    return 0;
}
