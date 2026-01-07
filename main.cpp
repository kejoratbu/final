#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <limits>
#include <sstream>

using namespace std;

/**
 * @brief Represents an item in the inventory.
 */
struct Item {
    int id;                 ///< Unique ID of the item
    string name;            ///< Name of the item
    string size_color;      ///< Size or Color variant
    int quantity;           ///< Current stock quantity
    double purchase_price;  ///< Cost price
    double selling_price;   ///< Selling price
};

/**
 * @brief Represents a sales record.
 */
struct Sale {
    int id;                 ///< Unique ID of the sale
    int item_id;            ///< ID of the item sold
    string item_name;       ///< Name of the item sold (snapshot)
    int quantity_sold;      ///< Quantity sold
    double profit;          ///< Profit made from this sale
    string date_sold;       ///< Timestamp of the sale
};

// Global In-Memory Storage
vector<Item> items; ///< Global list of inventory items
vector<Sale> sales; ///< Global list of sales records
int nextItemId = 1; ///< Auto-increment counter for Item IDs
int nextSaleId = 1; ///< Auto-increment counter for Sale IDs

// Files
const string ITEMS_FILE = "items.csv";
const string SALES_FILE = "sales.csv";

// Helper utilities
static inline string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

static inline string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c); });
    return s;
}

static inline bool isCancel(const string &s) {
    string t = toLowerStr(trim(s));
    return (t == "cancel" || t == "c");
}

static inline bool toInt(const string &s, int &out) {
    try {
        size_t idx;
        long v = stol(trim(s), &idx);
        if (idx != trim(s).size()) return false;
        out = static_cast<int>(v);
        return true;
    } catch (...) { return false; }
}

static inline bool toDouble(const string &s, double &out) {
    try {
        size_t idx;
        double v = stod(trim(s), &idx);
        if (idx != trim(s).size()) return false;
        out = v;
        return true;
    } catch (...) { return false; }
}

static inline string promptLine(const string &msg) {
    string s;
    cout << msg;
    if (!getline(cin, s)) return string();
    return s;
}

static string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
       << setw(2) << ltm->tm_mday << " "
       << setw(2) << ltm->tm_hour << ":"
       << setw(2) << ltm->tm_min << ":"
       << setw(2) << ltm->tm_sec;
    return ss.str();
}

/* ================= CORE LOGIC FUNCTIONS (TESTABLE) ================= */

/**
 * @brief Adds a new item to the inventory.
 * 
 * @param name Name of the item.
 * @param size Size or color description.
 * @param qty Initial quantity.
 * @param buy Purchase price.
 * @param sell Selling price.
 * @return int The ID of the newly added item.
 */
int logic_addItem(string name, string size, int qty, double buy, double sell) {
    int id = nextItemId++;
    items.push_back({id, name, size, qty, buy, sell});
    return id;
}

/**
 * @brief Deletes an item by ID.
 * 
 * @param id The ID of the item to delete.
 * @return true If item was found and deleted.
 * @return false If item was not found.
 */
bool logic_deleteItem(int id) {
    auto it = find_if(items.begin(), items.end(), [id](const Item& item) { return item.id == id; });
    if (it != items.end()) {
        items.erase(it);
        return true;
    }
    return false;
}

/**
 * @brief Updates an existing item.
 * 
 * @param id The ID of the item to update.
 * @param qty New quantity.
 * @param buy New purchase price.
 * @param sell New selling price.
 * @return true If update was successful.
 * @return false If item was not found.
 */
bool logic_updateItem(int id, int qty, double buy, double sell) {
    auto it = find_if(items.begin(), items.end(), [id](const Item& item) { return item.id == id; });
    if (it != items.end()) {
        it->quantity = qty;
        it->purchase_price = buy;
        it->selling_price = sell;
        return true;
    }
    return false;
}

/**
 * @brief Processes a sale transaction.
 * 
 * @param id The ID of the item to sell.
 * @param qty The quantity to sell.
 * @param profitOut Output parameter to store the calculated profit.
 * @return int 0 = Success, 1 = Item not found, 2 = Not enough stock.
 */
int logic_sellItem(int id, int qty, double& profitOut) {
    auto it = find_if(items.begin(), items.end(), [id](const Item& item) { return item.id == id; });
    if (it == items.end()) return 1; // Not found

    if (qty > it->quantity) return 2; // Not enough stock

    double profit = (it->selling_price - it->purchase_price) * qty;
    it->quantity -= qty;
    
    // Record sale
    sales.push_back({nextSaleId++, it->id, it->name, qty, profit, getCurrentDate()});
    
    profitOut = profit;
    return 0; // Success
}

/* ================= FILE PERSISTENCE ================= */

/**
 * @brief Saves all items and sales to CSV files.
 */
void saveData() {
    // Save Items
    ofstream itemFile(ITEMS_FILE);
    if (itemFile.is_open()) {
        for (const auto& item : items) {
            itemFile << item.id << "," 
                     << item.name << "," 
                     << item.size_color << "," 
                     << item.quantity << "," 
                     << item.purchase_price << "," 
                     << item.selling_price << "\n";
        }
        itemFile.close();
        cout << " [Saved] Items to " << ITEMS_FILE << endl;
    } else {
        cout << " [Error] Could not save items!\n";
    }

    // Save Sales
    ofstream saleFile(SALES_FILE);
    if (saleFile.is_open()) {
        for (const auto& sale : sales) {
            saleFile << sale.id << "," 
                     << sale.item_id << "," 
                     << sale.item_name << "," 
                     << sale.quantity_sold << "," 
                     << sale.profit << "," 
                     << sale.date_sold << "\n";
        }
        saleFile.close();
        cout << " [Saved] Sales to " << SALES_FILE << endl;
    } else {
        cout << " [Error] Could not save sales!\n";
    }
}

// Helper to parse CSV line
vector<string> parseCSV(string line) {
    vector<string> result;
    stringstream ss(line);
    string item;
    while (getline(ss, item, ',')) {
        result.push_back(item);
    }
    return result;
}

/**
 * @brief Seeds the database with default data if empty.
 */
void seedData() {
    items.push_back({nextItemId++, "Widget", "Small", 10, 5.0, 8.0});
    items.push_back({nextItemId++, "Bolt", "Red", 3, 0.5, 1.0});
    items.push_back({nextItemId++, "Gadget", "Blue", 20, 10.0, 15.0});
    cout << " [Info] No previous data found. Seeded default items.\n";
}

/**
 * @brief Loads data from CSV files into memory.
 */
void loadData() {
    items.clear();
    sales.clear();
    nextItemId = 1;
    nextSaleId = 1;

    // Load Items
    ifstream itemFile(ITEMS_FILE);
    if (itemFile.is_open()) {
        string line;
        while (getline(itemFile, line)) {
            if (trim(line).empty()) continue;
            vector<string> data = parseCSV(line);
            if (data.size() >= 6) {
                Item it;
                it.id = stoi(data[0]);
                it.name = data[1];
                it.size_color = data[2];
                it.quantity = stoi(data[3]);
                it.purchase_price = stod(data[4]);
                it.selling_price = stod(data[5]);
                items.push_back(it);
                if (it.id >= nextItemId) nextItemId = it.id + 1;
            }
        }
        itemFile.close();
        cout << " [Loaded] " << items.size() << " items.\n";
    }

    // Load Sales
    ifstream saleFile(SALES_FILE);
    if (saleFile.is_open()) {
        string line;
        while (getline(saleFile, line)) {
            if (trim(line).empty()) continue;
            vector<string> data = parseCSV(line);
            if (data.size() >= 6) {
                Sale s;
                s.id = stoi(data[0]);
                s.item_id = stoi(data[1]);
                s.item_name = data[2];
                s.quantity_sold = stoi(data[3]);
                s.profit = stod(data[4]);
                s.date_sold = data[5]; 
                sales.push_back(s);
                if (s.id >= nextSaleId) nextSaleId = s.id + 1;
            }
        }
        saleFile.close();
        cout << " [Loaded] " << sales.size() << " sales records.\n";
    }

    if (items.empty() && sales.empty()) {
        seedData();
    }
}

/* ================= UI FUNCTIONS ================= */

void ui_addItem() {
    string name, size, line;
    int qty;
    double buy, sell;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    name = promptLine("Item name (or type 'cancel' to return): ");
    if (isCancel(name) || trim(name).empty()) { cout << "Cancelled.\n"; return; }
    replace(name.begin(), name.end(), ',', ' ');

    size = promptLine("Size/Color (or type 'cancel' to return): ");
    if (isCancel(size)) { cout << "Cancelled.\n"; return; }
    replace(size.begin(), size.end(), ',', ' ');

    line = promptLine("Quantity (or type 'cancel' to return): ");
    if (isCancel(line) || !toInt(line, qty)) { cout << "Cancelled or invalid quantity.\n"; return; }

    line = promptLine("Purchase price (or type 'cancel' to return): ");
    if (isCancel(line) || !toDouble(line, buy)) { cout << "Cancelled or invalid purchase price.\n"; return; }

    line = promptLine("Selling price (or type 'cancel' to return): ");
    if (isCancel(line) || !toDouble(line, sell)) { cout << "Cancelled or invalid selling price.\n"; return; }

    int newId = logic_addItem(name, size, qty, buy, sell);
    cout << "Item added successfully! Assigned ID: " << newId << "\n";
}

void ui_updateItem() {
    string line;
    int id, qty;
    double buy, sell;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    line = promptLine("Item ID (or type 'cancel' to return): ");
    if (isCancel(line) || !toInt(line, id)) { cout << "Cancelled or invalid ID.\n"; return; }

    // Check existence visually first (optional, logic handles it too)
    // Here we just ask for data then call logic
    
    line = promptLine("New quantity (or type 'cancel' to return): ");
    if (isCancel(line) || !toInt(line, qty)) { cout << "Cancelled or invalid quantity.\n"; return; }

    line = promptLine("New purchase price (or type 'cancel' to return): ");
    if (isCancel(line) || !toDouble(line, buy)) { cout << "Cancelled or invalid purchase price.\n"; return; }

    line = promptLine("New selling price (or type 'cancel' to return): ");
    if (isCancel(line) || !toDouble(line, sell)) { cout << "Cancelled or invalid selling price.\n"; return; }

    if (logic_updateItem(id, qty, buy, sell)) {
        cout << "Item updated!\n";
    } else {
        cout << "Item not found.\n";
    }
}

void ui_searchItem() {
    string key;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    key = promptLine("Search name (or type 'cancel' to return): ");
    if (isCancel(key) || trim(key).empty()) { cout << "Cancelled.\n"; return; }

    string lowerKey = toLowerStr(key);
    cout << "\n--- SEARCH RESULTS ---\n";
    bool found = false;
    for (const auto& item : items) {
        if (toLowerStr(item.name).find(lowerKey) != string::npos) {
            cout << "ID: " << item.id
                 << " | " << item.name
                 << " | " << item.size_color
                 << " | Qty: " << item.quantity
                 << " | Buy: " << item.purchase_price
                 << " | Sell: " << item.selling_price << endl;
            found = true;
        }
    }
    if (!found) cout << "No matches found.\n";
}

void ui_lowStock() {
    string line = promptLine("Show low stock items? Press Enter to continue or type 'cancel' to return: ");
    if (isCancel(line)) { cout << "Cancelled.\n"; return; }

    cout << "\n--- LOW STOCK ITEMS ---\n";
    bool found = false;
    for (const auto& item : items) {
        if (item.quantity <= 5) {
            cout << item.name << " | Qty: " << item.quantity << " ⚠️\n";
            found = true;
        }
    }
    if (!found) cout << "No low stock items.\n";

    promptLine("Press Enter to return to menu...");
}

void ui_sellItem() {
    string line;
    int id, qty;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    line = promptLine("Item ID (or type 'cancel' to return): ");
    if (isCancel(line) || !toInt(line, id)) { cout << "Cancelled or invalid ID.\n"; return; }

    line = promptLine("Quantity sold (or type 'cancel' to return): ");
    if (isCancel(line) || !toInt(line, qty)) { cout << "Cancelled or invalid quantity.\n"; return; }

    double profit = 0.0;
    int result = logic_sellItem(id, qty, profit);

    if (result == 0) {
        cout << "Item sold! Profit: " << profit << endl;
    } else if (result == 1) {
        cout << "Item not found!\n";
    } else if (result == 2) {
        cout << "Not enough stock!\n";
    }
}

void ui_salesHistory() {
    string line = promptLine("Show sales history? Press Enter to continue or type 'cancel' to return: ");
    if (isCancel(line)) { cout << "Cancelled.\n"; return; }

    cout << "\n--- SALES HISTORY ---\n";
    if (sales.empty()) {
        cout << "No sales recorded yet.\n";
    } else {
        for (auto it = sales.rbegin(); it != sales.rend(); ++it) {
            cout << "SaleID: " << it->id
                 << " | " << it->item_name
                 << " | Qty: " << it->quantity_sold
                 << " | Profit: " << it->profit
                 << " | Date: " << it->date_sold << endl;
        }
    }

    promptLine("Press Enter to return to menu...");
}

void ui_listItems() {
    cout << "\n--- ITEM LIST ---\n";
    if (items.empty()) {
        cout << "No items in inventory.\n";
    } else {
        for (const auto& item : items) {
            cout << "ID: " << item.id
                 << " | " << item.name
                 << " | " << item.size_color
                 << " | Qty: " << item.quantity
                 << " | Buy: " << item.purchase_price
                 << " | Sell: " << item.selling_price << endl;
        }
    }
    promptLine("Press Enter to return to menu...");
}

void ui_checkConnection() {
    cout << "\nChecking database connection...\n";
    cout << " [OK] Application memory initialized.\n";
    cout << " [OK] Item storage active (" << items.size() << " items).\n";
    cout << " [OK] Sales storage active (" << sales.size() << " records).\n";
    cout << "Database connection is HEALTHY (Local Mode).\n";
    promptLine("Press Enter to return to menu...");
}

void ui_deleteItem() {
    string line;
    int id;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    line = promptLine("Item ID to DELETE (or type 'cancel' to return): ");
    if (isCancel(line) || !toInt(line, id)) { cout << "Cancelled or invalid ID.\n"; return; }

    // Check existence first to show details before deleting
    auto it = find_if(items.begin(), items.end(), [id](const Item& item) { return item.id == id; });
    if (it == items.end()) {
        cout << "Item not found.\n";
        return;
    }

    cout << "Deleting Item: " << it->name << " (Qty: " << it->quantity << ")\n";
    string confirm = promptLine("Are you sure? (y/n): ");
    if (toLowerStr(trim(confirm)) != "y") {
        cout << "Deletion cancelled.\n";
        return;
    }

    if (logic_deleteItem(id)) {
        cout << "Item deleted successfully.\n";
    } else {
        cout << "Error deleting item.\n";
    }
}

/* ================= MAIN MENU ================= */

#ifndef UNIT_TEST
int main() {
    cout << "Running in STANDALONE mode (In-Memory + CSV Persistence)\n";
    loadData();

    int choice;
    do {
        cout << "\n===== INVENTORY MANAGER (Local Storage) =====\n";
        cout << "1. Add Item\n";
        cout << "2. Update Item\n";
        cout << "3. Delete Item\n";
        cout << "4. Search Item\n";
        cout << "5. Low Stock Alert\n";
        cout << "6. Sell Item\n";
        cout << "7. Sales History\n";
        cout << "8. List All Items\n";
        cout << "9. Check System Status\n";
        cout << "10. Save & Exit\n";
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        switch (choice) {
        case 1: ui_addItem(); break;
        case 2: ui_updateItem(); break;
        case 3: ui_deleteItem(); break;
        case 4: ui_searchItem(); break;
        case 5: ui_lowStock(); break;
        case 6: ui_sellItem(); break;
        case 7: ui_salesHistory(); break;
        case 8: ui_listItems(); break;
        case 9: ui_checkConnection(); break;
        case 10: saveData(); break;
        }
    } while (choice != 10);

    return 0;
}
#endif
