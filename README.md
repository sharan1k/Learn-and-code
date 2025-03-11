# Learn-and-code

To run the code for checkout system

```
g++ -std=c++11 -o checkout main.cpp -lsqlite3
```

To view the database
```
sqlite3 orders.db
sqlite> SELECT * FROM orders;
```