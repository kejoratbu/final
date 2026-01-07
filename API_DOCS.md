# Inventory Manager API Documentation

This document lists all functions available in the **Inventory Manager** application.

## Core Logic Functions
These functions handle the business logic and data manipulation. They are decoupled from `cin`/`cout` to enable unit testing.

### `int logic_addItem(string name, string size, int qty, double buy, double sell)`
**Description**: Adds a new item to the inventory.
- **Parameters**:
  - `name`: Name of the item.
  - `size`: Size or color description.
  - `qty`: Initial quantity.
  - `buy`: Purchase price.
  - `sell`: Selling price.
- **Returns**: The ID of the newly added item.

### `bool logic_deleteItem(int id)`
**Description**: Deletes an item by ID.
- **Parameters**:
  - `id`: The ID of the item to delete.
- **Returns**: `true` if item was found and deleted, `false` otherwise.

### `bool logic_updateItem(int id, int qty, double buy, double sell)`
**Description**: Updates an existing item.
- **Parameters**:
  - `id`: The ID of the item to update.
  - `qty`: New quantity.
  - `buy`: New purchase price.
  - `sell`: New selling price.
- **Returns**: `true` if update was successful, `false` if item was not found.

### `int logic_sellItem(int id, int qty, double& profitOut)`
**Description**: Processes a sale transaction.
- **Parameters**:
  - `id`: The ID of the item to sell.
  - `qty`: The quantity to sell.
  - `profitOut`: Output parameter to store the calculated profit.
- **Returns**: 
  - `0` = Success
  - `1` = Item not found
  - `2` = Not enough stock

---

## Persistence Functions
Functions responsible for saving and loading data to CSV files.

### `void saveData()`
**Description**: Saves all items and sales to `items.csv` and `sales.csv`.

### `void loadData()`
**Description**: Loads data from CSV files into memory on startup.

### `void seedData()`
**Description**: Seeds the database with default data if no files are found.

---

## UI Functions
Functions that handle user interaction (printing to console, reading input).

- **`void ui_addItem()`**: Prompts user for details and calls `logic_addItem`.
- **`void ui_updateItem()`**: Prompts for ID and new details, calls `logic_updateItem`.
- **`void ui_deleteItem()`**: Prompts for ID, confirms action, and calls `logic_deleteItem`.
- **`void ui_searchItem()`**: Prompts for keyword and displays matching items.
- **`void ui_lowStock()`**: Displays items with quantity <= 5.
- **`void ui_sellItem()`**: Prompts for sale details and calls `logic_sellItem`.
- **`void ui_salesHistory()`**: Displays all recorded sales.
- **`void ui_listItems()`**: Displays all items in inventory.
- **`void ui_checkConnection()`**: Displays system status and memory statistics.

---

## Helper Utilities
Internal utility functions.

- `string trim(const string &s)`: Removes whitespace from ends of string.
- `string toLowerStr(string s)`: Converts string to lowercase.
- `bool isCancel(const string &s)`: Checks if input is "cancel".
- `bool toInt(const string &s, int &out)`: Safely converts string to int.
- `bool toDouble(const string &s, double &out)`: Safely converts string to double.
- `string promptLine(const string &msg)`: Helper to print message and get line input.
- `string getCurrentDate()`: Returns current date/time as string.
