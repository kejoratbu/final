# Standalone Inventory Manager

A robust, offline-capable CLI inventory management system written in C++. 
It requires **no external database** (like MySQL) and persists all data to local CSV files.

## Features 🚀
- **Full Inventory Control**: Add, Update, **Delete**, and Search items.
- **Sales Tracking**: Record sales and view sales history with profit calculation.
- **Low Stock Alerts**: Instantly identify items running low (qty <= 5).
- **Persistent Storage**: Data is automatically saved to `items.csv` and `sales.csv` on exit.
- **Zero Dependencies**: Runs as a single portable `.exe` file.

## Quick Start
### 1. Build
You need a C++ compiler (like `g++`).

```bash
# Using Make (if available)
make

# Manual compilation
g++ main.cpp -o inventory.exe -std=c++17
```

### 2. Run
```bash
./inventory.exe
```

The application will launch in the terminal. Use the number keys to navigate the menu.

## Data Persistence 💾
Data is stored in plain text CSV files in the same directory:
- `items.csv`: Stores ID, Name, Size, Quantity, BuyPrice, SellPrice.
- `sales.csv`: Stores SaleID, ItemID, ItemName, QtySold, Profit, Date.

*Note: If these files don't exist, the app will start with a fresh (seeded) database.*

## Testing 🧪
The project includes a suite of unit tests to verify core logic (adding, updating, deleting, selling).

```bash
# Compile and Run Tests
g++ tests/unit_tests.cpp -o tests/runner.exe -std=c++17
./tests/runner.exe
```

## Documentation �
For detailed developer documentation, see [API_DOCS.md](API_DOCS.md).
