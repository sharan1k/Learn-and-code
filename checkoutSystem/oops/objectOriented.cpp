#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sqlite3.h>
#include <json.hpp>
#include <fstream>
#include <memory>

class Item
{
public:
    virtual ~Item() = default;

    std::string name;
    double price;
    int quantity;

    Item(const std::string &name, double price, int quantity)
        : name(name), price(price), quantity(quantity) {}

    virtual double getTotalAmount() const = 0;

    virtual std::string getItemType() const = 0;
};

class PhysicalItem : public Item
{
private:
    double weight;

public:
    PhysicalItem(const std::string &name, double price, int quantity, double weight)
        : Item(name, price, quantity), weight(weight) {}

    double getShippingCost() const
    {
        return weight * 5.0;
    }

    double getTotalAmount() const override
    {
        return (price * quantity) + getShippingCost();
    }

    std::string getItemType() const override
    {
        return "Physical Item";
    }
};

class DigitalItem : public Item
{
private:
    double fileSize;

public:
    DigitalItem(const std::string &name, double price, int quantity, double fileSize)
        : Item(name, price, quantity), fileSize(fileSize) {}

    double getTotalAmount() const override
    {
        return price * quantity;
    }

    std::string getItemType() const override
    {
        return "Digital Item";
    }
};

class SubscriptionItem : public Item
{
private:
    int durationMonths;

public:
    SubscriptionItem(const std::string &name, double price, int quantity, int durationMonths)
        : Item(name, price, quantity), durationMonths(durationMonths) {}

    double getTotalAmount() const override
    {
        return price * quantity * durationMonths;
    }

    std::string getItemType() const override
    {
        return "Subscription Item";
    }
};

class Order
{
private:
    std::vector<std::shared_ptr<Item>> items;
    double shippingCost;
    double discount;

public:
    Order() : shippingCost(0), discount(0) {}

    void addItem(std::shared_ptr<Item> item)
    {
        items.push_back(item);
    }

    void setShippingCost(double cost)
    {
        shippingCost = cost;
    }

    void setDiscount(double discountAmount)
    {
        discount = discountAmount;
    }

    double getSubtotal() const
    {
        double subtotal = 0;

        for (const auto &item : items)
        {
            subtotal += item->getTotalAmount();
        }

        return subtotal;
    }

    double getTotalAmount() const
    {
        double subtotal = getSubtotal();
        return subtotal + shippingCost - discount;
    }

    double getShippingCost() const
    {
        return shippingCost;
    }

    double getDiscount() const
    {
        return discount;
    }

    double calculateGST() const
    {
        return getSubtotal() * 0.18;
    }

    nlohmann::json toJson() const
    {
        nlohmann::json orderJson;
        orderJson["items"] = nlohmann::json::array();

        for (const auto &item : items)
        {
            nlohmann::json itemJson;
            itemJson["name"] = item->name;
            itemJson["price"] = item->price;
            itemJson["quantity"] = item->quantity;
            itemJson["total"] = item->getTotalAmount();
            itemJson["type"] = item->getItemType();
            orderJson["items"].push_back(itemJson);
        }

        orderJson["shippingCost"] = shippingCost;
        orderJson["discount"] = discount;
        orderJson["subtotal"] = getSubtotal();
        orderJson["gst"] = calculateGST();
        orderJson["total"] = getTotalAmount();
        return orderJson;
    }
};

class Checkout
{
private:
    Order order;

public:
    void addItemToOrder(std::shared_ptr<Item> item)
    {
        order.addItem(item);
    }

    void setShippingCost(double cost)
    {
        order.setShippingCost(cost);
    }

    void setDiscount(double discountAmount)
    {
        order.setDiscount(discountAmount);
    }

    void processOrder()
    {
        double subtotal = order.getSubtotal();
        double shippingCost = order.getShippingCost();
        double discount = order.getDiscount;
        double gst = order.calculateGST();
        double total = order.getTotalAmount() + gst;

        std::cout << "Order Summary:" << std::endl;
        std::cout << "-------------------------------" << std::endl;
        std::cout << "Subtotal: " << std::fixed << std::setprecision(2) << subtotal << std::endl;
        std::cout << "Shipping Cost: " << std::fixed << std::setprecision(2) << shippingCost << std::endl;
        std::cout << "Discount: " << std::fixed << std::setprecision(2) << discount << std::endl;
        std::cout << "GST (18%): " << std::fixed << std::setprecision(2) << gst << std::endl;
        std::cout << "Total: " << std::fixed << std::setprecision(2) << total << std::endl;
    }

    void saveOrderToDatabase(sqlite3 *database)
    {
        nlohmann::json orderJson = order.toJson();
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

    void parseJsonFile(const std::string &filename)
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
            std::string name = itemJson["name"];
            double price = itemJson["price"];
            int quantity = itemJson["quantity"];
            std::string type = itemJson["type"];

            if (type == "physical")
            {
                double weight = itemJson["weight"];
                addItemToOrder(std::make_shared<PhysicalItem>(name, price, quantity, weight));
            }
            else if (type == "digital")
            {
                double fileSize = itemJson["fileSize"];
                addItemToOrder(std::make_shared<DigitalItem>(name, price, quantity, fileSize));
            }
            else if (type == "subscription")
            {
                int durationMonths = itemJson["durationMonths"];
                addItemToOrder(std::make_shared<SubscriptionItem>(name, price, quantity, durationMonths));
            }
        }

        setShippingCost(jsonData["shippingCost"]);
        setDiscount(jsonData["discount"]);
    }
};

int main()
{
    std::string jsonFile;
    std::cout << "Enter the JSON file name: ";
    std::cin >> jsonFile;

    Checkout checkout;
    checkout.parseJsonFile(jsonFile);
    checkout.processOrder();

    sqlite3 *database;
    if (sqlite3_open("orders.db", &database))
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(database) << std::endl;
        return 1;
    }

    checkout.saveOrderToDatabase(database);

    sqlite3_close(database);
    return 0;
}
