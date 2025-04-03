#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sqlite3.h>
#include <json.hpp>
#include <fstream>

struct Item
{
    std::string name;
    double price;
    int quantity;
    std::string type;
    double weight = 0;
    double fileSize = 0;
};

struct Order
{
    std::vector<Item> items;
    double shippingCost = 0;
    double discount = 0;
};

double getTotalAmount(const Item &item)
{
    if (item.type == "physical")
    {
        double shippingCost = item.weight * 5.0;
        return (item.price * item.quantity) + shippingCost;
    }
    else if (item.type == "digital")
    {
        return item.price * item.quantity;
    }
    else
    {
        std::cerr << "Unknown item type: " << item.type << std::endl;
        return 0;
    }
}

double getSubtotal(const Order &order)
{
    double subtotal = 0;

    for (const auto &item : order.items)
    {
        subtotal += getTotalAmount(item);
    }
    
    return subtotal;
}

double calculateGST(const Order &order)
{
    return getSubtotal(order) * 0.18;
}

double calculateTotal(const Order &order)
{
    return getSubtotal(order) + calculateGST(order) + order.shippingCost - order.discount;
}

void saveOrderToDatabase(sqlite3 *database, const Order &order)
{
    nlohmann::json orderJson;
    orderJson["items"] = nlohmann::json::array();

    for (const auto &item : order.items)
    {
        nlohmann::json itemJson;
        itemJson["name"] = item.name;
        itemJson["price"] = item.price;
        itemJson["quantity"] = item.quantity;
        itemJson["total"] = getTotalAmount(item);
        itemJson["type"] = item.type;
        orderJson["items"].push_back(itemJson);
    }

    orderJson["shippingCost"] = order.shippingCost;
    orderJson["discount"] = order.discount;
    orderJson["subtotal"] = getSubtotal(order);
    orderJson["gst"] = calculateGST(order);
    orderJson["total"] = calculateTotal(order);

    std::string jsonData = orderJson.dump();

    const char *insertSql = "INSERT INTO orders (order_data) VALUES (?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(database, insertSql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    if (sqlite3_bind_text(stmt, 1, jsonData.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        std::cerr << "Failed to bind text: " << sqlite3_errmsg(database) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "Execution failed: " << sqlite3_errmsg(database) << std::endl;
    }
    else
    {
        std::cout << "Order saved to database!" << std::endl;
    }

    sqlite3_finalize(stmt);
}

void processOrder(const Order &order)
{
    double subtotal = getSubtotal(order);
    double gst = calculateGST(order);
    double total = calculateTotal(order);

    std::cout << "Order Summary:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Subtotal: " << std::fixed << std::setprecision(2) << subtotal << std::endl;
    std::cout << "Shipping Cost: " << std::fixed << std::setprecision(2) << order.shippingCost << std::endl;
    std::cout << "Discount: " << std::fixed << std::setprecision(2) << order.discount << std::endl;
    std::cout << "GST (18%): " << std::fixed << std::setprecision(2) << gst << std::endl;
    std::cout << "Total: " << std::fixed << std::setprecision(2) << total << std::endl;
}

void parseJsonFile(const std::string &filename, Order &order)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;

    for (const auto &itemJson : jsonData["items"])
    {
        Item item;
        item.name = itemJson["name"];
        item.price = itemJson["price"];
        item.quantity = itemJson["quantity"];
        item.type = itemJson["type"];

        if (item.type == "physical")
        {
            item.weight = itemJson["weight"];
        }
        else if (item.type == "digital")
        {
            item.fileSize = itemJson["fileSize"];
        }

        order.items.push_back(item);
    }

    order.shippingCost = jsonData["shippingCost"];
    order.discount = jsonData["discount"];
}

void cancelItem(const Item &item)
{
    if (item.type == "physical")
    {
        std::cout << "The physical item \"" << item.name << "\" has been cancelled." << std::endl;
    }
    else if (item.type == "digital")
    {
        std::cout << "The digital item \"" << item.name << "\" has been cancelled." << std::endl;
    }
    else
    {
        std::cerr << "Unknown item type: " << item.type << std::endl;
    }
}

int main()
{
    std::string jsonFile;
    std::cout << "Enter the JSON file name: ";
    std::cin >> jsonFile;

    Order order;
    parseJsonFile(jsonFile, order);
    processOrder(order);

    if (!order.items.empty())
    {
        cancelItem(order.items[0]);
    }

    sqlite3 *database;
    if (sqlite3_open("orders.db", &database))
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(database) << std::endl;
        return 1;
    }

    saveOrderToDatabase(database, order);

    sqlite3_close(database);
    return 0;
}
