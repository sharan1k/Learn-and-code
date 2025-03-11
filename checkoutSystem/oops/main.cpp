#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sqlite3.h>
#include <json.hpp>

class Item {
public:
    std::string name;
    double price;
    int quantity;

    Item(const std::string& name, double price, int quantity)
        : name(name), price(price), quantity(quantity) {}

    double getTotalAmount() const {
        return price * quantity;
    }
};

class Order {
private:
    std::vector<Item> items;
    double shippingCost;
    double discount;

public:
    Order() : shippingCost(0), discount(0) {}

    void addItem(const Item& item) {
        items.push_back(item);
    }

    void setShippingCost(double cost) {
        shippingCost = cost;
    }

    void setDiscount(double discountAmount) {
        discount = discountAmount;
    }

    double getSubtotal() const {
        double subtotal = 0;
        for (const auto& item : items) {
            subtotal += item.getTotalAmount();
        }
        return subtotal;
    }

    double getTotalAmount() const {
        double subtotal = getSubtotal();
        return subtotal + shippingCost - discount;
    }

    double getShippingCost() const {
        return shippingCost;
    }

    double getDiscount() const {
        return discount;
    }

    double calculateGST() const {
        return getSubtotal() * 0.18;
    }

    nlohmann::json to_json() const {
        nlohmann::json order_json;
        order_json["items"] = nlohmann::json::array();
        
        for (const auto& item : items) {
            nlohmann::json item_json;
            item_json["name"] = item.name;
            item_json["price"] = item.price;
            item_json["quantity"] = item.quantity;
            item_json["total"] = item.getTotalAmount();
            order_json["items"].push_back(item_json);
        }

        order_json["shippingCost"] = shippingCost;
        order_json["discount"] = discount;
        order_json["subtotal"] = getSubtotal();
        order_json["gst"] = calculateGST();
        order_json["total"] = getTotal();
        return order_json;
    }
};

class Checkout {
private:
    Order order;
    sqlite3* db;

public:
    Checkout() : db(nullptr) {
        if (sqlite3_open("orders.db", &db)) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        const char* create_table_sql = R"(
            CREATE TABLE IF NOT EXISTS orders (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                order_data TEXT
            );
        )";
        char* err_msg = nullptr;
        if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
            std::cerr << "SQL error: " << err_msg << std::endl;
            sqlite3_free(err_msg);
        }
    }

    ~Checkout() {
        if (db) {
            sqlite3_close(db);
        }
    }

    void addItemToOrder(const Item& item) {
        order.addItem(item);
    }

    void setShippingCost(double cost) {
        order.setShippingCost(cost);
    }

    void setDiscount(double discountAmount) {
        order.setDiscount(discountAmount);
    }

    void processOrder() {
        double subtotal = order.getSubtotal();
        double shippingCost = order.getShippingCost();
        double discount = order.getDiscount();
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

    void saveOrderToDatabase() {
        nlohmann::json order_json = order.to_json();
        std::string json_data = order_json.dump();

        const char* insert_sql = "INSERT INTO orders (order_data) VALUES (?);";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        if (sqlite3_bind_text(stmt, 1, json_data.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
            std::cerr << "Failed to bind text: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return;
        }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << "Order saved to database!" << std::endl;
        }

        sqlite3_finalize(stmt);
    }
};

int main() {
    Checkout checkout;

    checkout.addItemToOrder(Item("Telivision", 25000, 2));
    checkout.addItemToOrder(Item("Remote", 200, 2));

    checkout.setShippingCost(200);
    checkout.setDiscount(1000);     

    checkout.processOrder();

    checkout.saveOrderToDatabase();

    return 0;
}
