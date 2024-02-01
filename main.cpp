#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Node {
    string name; // Nama kategori atau item
    float price; // Harga item
    int stock; // Jumlah stok item
    Node* leftChild; // Anak kiri
    Node* rightChild; // Anak kanan

    explicit Node(const string& name) {
        this->name = name;
        this->price = 0;
        this->stock = 0;
        this->leftChild = nullptr;
        this->rightChild = nullptr;
    }

    Node(const string& name, float price, int stock) {
        this->name = name;
        this->price = price;
        this->stock = stock;
        this->leftChild = nullptr;
        this->rightChild = nullptr;
    }

    bool isItem() const {
        return this->price != 0 || this->stock != 0;
    }
};

static int nextEventId = 1; // ID untuk event

struct Event {
    int orderId;
    int status; // 0 = menunggu, 1 = selesai
    string menu;
    vector<pair<string, int>> items;
    string date; // Tanggal event
    float totalPrice; // Total harga event

    Event() : orderId(nextEventId++), status(0) {}
};

struct EventCategory {
    string name;
    vector<string> category;

    EventCategory() = default;

    EventCategory(const string& name, const vector<string>& category) {
        this->name = name;
        this->category = category;
    }
};

struct linkedList {
    Event data;
    linkedList* next;

    explicit linkedList(Event data) {
        this->data = data;
        this->next = nullptr;
    }
};

// Deklarasi fungsi
Node* addItem(Node* parent, const string& name, float price, int stock);
Node* addCategory(Node* parent, const string& name);
void displayTree(Node* root, int depth, vector<string>& availableCategories);
void displayItemsFromCategory(Node* root, const string& categoryName, int& itemNumber, vector<Node*>& items);
void displayItems(Node* node, int& itemNumber, vector<Node*>& items);
void deleteTree(Node* root);
void editNode(Node* root, const string& name, const string& newName, float newPrice, int newStock);
Node* findNode(Node* root, const string& name);
void fillMenu(Node* root, const vector<tuple<string, vector<pair<string, vector<tuple<string, float, int>>>>>>& menuData);
void insertAll(Node* root);
linkedList* addEvent(linkedList* head, const Event& data);
linkedList* findOne(linkedList* head, const int& id);
void displayStartingMenu();
void displayEvents(linkedList* head);
void chooseAndAddItemsToEvent(Node* root, Event& event, const vector<string>& categories);
void displayEventChoice();
void displayMenuChoice(const vector<EventCategory>& customMenus);
void handleMenuChoice(Node* root, linkedList*& eventList, const vector<EventCategory>& customMenus, int choice, const string& date);
EventCategory createNewMenu(Node* root);
void updateOrderStatus(linkedList*& eventList, vector<Event>& orderHistory);
string getCurrentDate();
void displayOrderHistory(const vector<Event>& orderHistory);
string nextDay(const string& currentDate);

// MENU
// Fungsi untuk menambahkan kategori baru
Node* addCategory(Node* parent, const string& name) {
    if (!parent) return nullptr;

    if (parent->leftChild == nullptr) {
        parent->leftChild = new Node(name);
        return parent->leftChild;
    } else {
        Node* sibling = parent->leftChild;
        while (sibling->rightChild) {
            sibling = sibling->rightChild;
        }
        sibling->rightChild = new Node(name);
        return sibling->rightChild;
    }
}

// Fungsi untuk menambahkan item baru
Node* addItem(Node* parent, const string& name, float price, int stock) {
    if (!parent) return nullptr;

    if (parent->leftChild == nullptr) {
        parent->leftChild = new Node(name, price, stock);
        return parent->leftChild;
    } else {
        Node* sibling = parent->leftChild;
        while (sibling->rightChild) {
            sibling = sibling->rightChild;
        }
        sibling->rightChild = new Node(name, price, stock);
        return sibling->rightChild;
    }
}

// Fungsi untuk mencari kategori atau item dengan nama tertentu
Node* findNode(Node* root, const string& name) {
    if (!root) return nullptr;

    if (root->name == name) return root;

    Node* foundNode = findNode(root->leftChild, name);
    if (foundNode) return foundNode;

    return findNode(root->rightChild, name);
}

// Fungsi untuk mengganti isi dari kategori atau item
void editNode(Node* root, const string& name, const string& newName, float newPrice = 0, int newStock = 0) {
    Node* node = findNode(root, name);
    if (node != nullptr){
        if (node->isItem()) {
            node->name = newName;
            node->price = newPrice;
            node->stock = newStock;
        } else {
            node->name = newName;
        }
    } else {
        cout << "Node tidak ditemukan" << endl;
    }
}

// Fungsi untuk menampilkan tree
void displayTree(Node* root, int depth, vector<string>& availableCategories) {
    if (!root) return;

    string indent(depth * 2, ' '); // Membuat indentasi
    if (!root->isItem()) {
        availableCategories.push_back(root->name);
        cout << indent << root->name << endl; // Menampilkan nama kategori
    }

    if (root->leftChild) {
        displayTree(root->leftChild, depth + 1, availableCategories);
    }

    if (root->rightChild) {
        displayTree(root->rightChild, depth, availableCategories);
    }
}

// Fungsi untuk menampilkan item dari kategori tertentu
void displayItemsFromCategory(Node* root, const string& categoryName, int& itemNumber, vector<Node*>& items) {
    if (!root) return;

    if (root->name == categoryName) {
        // Si le noeud actuel est la catégorie recherchée, affichez tous les éléments sous celui-ci.
        displayItems(root->leftChild, itemNumber, items);
    } else {
        // Continuez la recherche dans les enfants gauche et droite.
        displayItemsFromCategory(root->leftChild, categoryName, itemNumber, items);
        displayItemsFromCategory(root->rightChild, categoryName, itemNumber, items);
    }
}

// Fungsi untuk menampilkan item
void displayItems(Node* node, int& itemNumber, vector<Node*>& items) {
    if (!node) return;

    if (node->isItem()) {
        items.push_back(node); // Ajouter le noeud à la liste.
        cout << itemNumber++ << ". " << node->name << " - Price: " << node->price << ", Stock: " << node->stock << endl;
    }

    // Continuer avec le frère et l'enfant gauche (pour les sous-catégories).
    displayItems(node->rightChild, itemNumber, items);
    displayItems(node->leftChild, itemNumber, items);
}

// Fungsi untuk menghapus kategori atau item
void deleteNode(Node* root, const string& name) {
    if (!root) return;

    if (root->leftChild) {
        if (root->leftChild->name == name) {
            Node* nodeToDelete = root->leftChild;
            root->leftChild = root->leftChild->rightChild;
            delete nodeToDelete;
            return;
        }
    }

    if (root->rightChild) {
        if (root->rightChild->name == name) {
            Node* nodeToDelete = root->rightChild;
            root->rightChild = root->rightChild->rightChild;
            delete nodeToDelete;
            return;
        }
    }

    deleteNode(root->leftChild, name);
    deleteNode(root->rightChild, name);
}

// Fungsi untuk menghapus tree
void deleteTree(Node* root) {
    if (!root) return;

    if (root->leftChild) {
        deleteTree(root->leftChild);
    }

    if (root->rightChild) {
        deleteTree(root->rightChild);
    }

    delete root;
}

// Fungsi untuk mengisi tree dengan data
void fillMenu(Node* root, const vector<tuple<string, vector<pair<string, vector<tuple<string, float, int>>>>>>& menuData) {
    for (const auto& categoryData : menuData) {
        const auto& [categoryName, subCategories] = categoryData;
        Node* categoryNode = addCategory(root, categoryName);

        for (const auto& subCategoryData : subCategories) {
            const auto& [subCategoryName, items] = subCategoryData;
            Node* subCategoryNode = addCategory(categoryNode, subCategoryName);

            for (const auto& itemData : items) {
                const auto& [itemName, itemPrice, itemStock] = itemData;
                addItem(subCategoryNode, itemName, itemPrice, itemStock);
            }
        }
    }
}

// Fungsi untuk mengisi tree dengan data
void insertAll(Node* root){
    // Node* root = new Node("Menu");
    vector<tuple<string, vector<pair<string, vector<tuple<string, float, int>>>>>> menuData = {
            {"Roti dan kue", {
                {"Viennoiseries", {
                    {"Croissant", 1.99f, 10},
                    {"Pain au Chocolat", 2.49f, 2},
                    {"Brioche", 2.99f, 7}
                }},
                {"Pastry", {
                    {"Éclair", 3.49f, 10},
                    {"Mille-feuille", 4.49f, 6},
                    {"Tarte Tatin", 3.99f, 4},
                    {"Tartlette", 3.99f, 4}
                }},
                {"Bread", {
                    {"Baguette", 1.99f, 15},
                    {"Sourdough", 2.99f, 10}
                }},
                {"Bun", {
                    {"Bagel", 1.49f, 8},
                    {"Burger", 1.99f, 12},
                    {"Hotdog", 1.99f, 14}
                }}
            }},
            {"Minuman", {
                {"Juice", {
                    {"Apple Juice", 3.49f, 20},
                    {"Orange Juice", 3.49f, 20},
                    {"Strawberry Juice", 3.99f, 15}
                }},
                {"Teh", {
                    {"Earl Grey", 2.99f, 30},
                    {"Chamomile", 2.99f, 30},
                    {"English Breakfast", 2.99f, 30},
                    {"Darjeeling", 2.99f, 30},
                    {"Oolong", 2.99f, 30},
                    {"Jasmine", 2.99f, 30},
                    {"Matcha", 2.99f, 30},
                }},
                {"Kopi", {
                    {"Long black", 2.49f, 25},
                    {"Cappuccino", 2.99f, 25},
                    {"Latte", 2.99f, 25},
                    {"Espresso", 2.49f, 25},
                    {"Macchiato", 2.99f, 25},
                    {"Piccolo", 2.99f, 25},
                }}
            }},
            {"Makanan Utama", {
                {"Salad", {
                    {"Caesar Salad", 4.99f, 8},
                    {"Caprese Salad", 5.49f, 6},
                    {"Nicoise Salad", 5.99f, 5},
                    {"Waldorf Salad", 5.99f, 5},
                }},
                {"Sup", {
                    {"Pumpkin soup", 3.99f, 10},
                    {"Mushroom soup", 3.99f, 10},
                    {"Corn soup", 3.99f, 10}
                }},
                {"Sandwich", {
                     {"Club Sandwich", 4.99f, 10},
                     {"Tuna Sandwich", 4.29, 10}
                }},
                {"Pasta", {
                    {"Gnocchi", 6.99f, 5},
                    {"Aglio Olio", 5.99f, 7},
                    {"Carbonara", 6.99f, 5},
                    {"Lasagna", 7.99f, 5},
                    {"Fettuccine", 5.99f, 7}
                }}
            }}
    };
     // Fill the menu tree with the data
    fillMenu(root, menuData);
}

// EVENT
linkedList* addEvent(linkedList* head, const Event& data) {
    if (head == nullptr) {
        // Si la tête est nulle, créez un nouveau nœud et retournez-le comme nouvelle tête de la liste
        return new linkedList(data);
    } else {
        // Sinon, ajoutez le nouvel événement à la fin de la liste
        linkedList* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = new linkedList(data);
        return head; // La tête ne change pas
    }
}

// Fungsi untuk mencari event dengan ID tertentu
linkedList* findOne(linkedList* head, const int& id) {
    if (!head) return nullptr;

    if (head->data.orderId == id) return head;

    linkedList* foundNode = findOne(head->next, id);
    if (foundNode) return foundNode;

    return findOne(head->next, id);
}

// Menampilkan menu awal
void displayStartingMenu() {
    cout << "========================" << endl;
    cout << "1. Buka kafe" << endl;
    cout << "2. Tutup kafe (Pindah ke hari berikutnya)" << endl;
    cout << "3. Menampilkan riwayat pesanan" << endl;
    cout << "4. Exit code" << endl;
    cout << "========================" << endl;
}

// Menampilkan event
void displayEvents(linkedList* head) {
    linkedList* current = head;
    while (current != nullptr) {
        cout << endl << "========================" << endl;
        cout << "Order ID: " << current->data.orderId << endl;
        cout << "Date: " << current->data.date << endl;
        cout << "Menu: " << current->data.menu << endl;
        cout << "Status: " << (current->data.status == 0 ? "Pending" : "Done") << endl;
        cout << "Items: " << endl;
        for (const auto& item : current->data.items) {
            cout << "\t* " << item.first << " - " << item.second << endl;
        }
        cout << endl << "TOTAL PRICE: " << current->data.totalPrice << endl;
        cout << endl;
        current = current->next;
    }
}

// Fungsi untuk memilih dan menambahkan item ke event
void chooseAndAddItemsToEvent(Node* root, Event& event, const vector<string>& categories) {
    if (!root) return;

    event.totalPrice = 0.0f;

    for (const auto& category : categories) {
        int itemNumber = 1;
        vector<Node*> itemsVector; // Vector untuk menyimpan item dari kategori tertentu

        cout << "Pick items from " << category << endl;
        displayItemsFromCategory(root, category, itemNumber, itemsVector);

        int selectedItemNumber = 0;
        int quantity = 0;

        while (true) {
            cout << "Pilih item: ";
            cin >> selectedItemNumber;

            if (cin.fail() || selectedItemNumber < 1 || selectedItemNumber > itemsVector.size()) {
                // Jika input bukan angka atau angka yang tidak valid, hapus dan coba lagi
                cin.clear(); // Hapus flag kesalahan dari cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Abaikan sisa baris
                cout << "Nomor item tidak valid, silakan coba lagi." << endl;
            } else {
                // Jika nomor item valid, hentikan loop
                break;
            }
        }

        while (true) {
            cout << "Masukkan jumlah: " << itemsVector[selectedItemNumber - 1]->name << ": ";
            cin >> quantity;

            if (cin.fail() || quantity < 1 || quantity > itemsVector[selectedItemNumber - 1]->stock) {
                // Jika jumlah tidak valid, hapus dan coba lagi
                cin.clear(); // Hapus flag kesalahan dari cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Abaikan sisa baris
                cout << "Jumlah tidak valid, silakan coba lagi." << endl;
            } else {
                // Jika jumlah valid, hentikan loop
                break;
            }
        }

        Node* selectedItem = itemsVector[selectedItemNumber - 1];
        event.items.push_back(make_pair(selectedItem->name, quantity));
        event.totalPrice += selectedItem->price * float(quantity);

        // Mengurangi stok item
        selectedItem->stock -= quantity;
        if (selectedItem->stock <= 0) {
            deleteNode(root, selectedItem->name);
        }
    }
}

// Menampilkan pilihan event
void displayEventChoice() {
    cout << "1. Tambah event" << endl;
    cout << "2. Display event" << endl;
}

// Menampilkan pilihan menu
void displayMenuChoice(const vector<EventCategory>& customMenus) {
    cout << "------------------------" << endl;
    cout << "1. Breakfast" << endl;
    cout << "2. Morning Coffee / Tea Time / Afternoon Coffee" << endl;
    cout << "3. Brunch" << endl;
    cout << "4. Lunch" << endl;
    cout << "5. Dinner" << endl;

    // Menampilkan menu-menu yang telah dibuat
    int customMenuStartIndex = 6; // Commence après les choix de menu prédéfinis
    for (int i = 0; i < customMenus.size(); i++) {
        cout << customMenuStartIndex + i << ". " << customMenus[i].name << endl;
    }

    cout << customMenuStartIndex + customMenus.size() << ". Tambah new Event" << endl;
    cout << customMenuStartIndex + customMenus.size() + 1 << ". Display Orders" << endl;
    cout << customMenuStartIndex + customMenus.size() + 2 << ". Menyajikan Event" << endl;
    cout << customMenuStartIndex + customMenus.size() + 3 << ". Back" << endl;
    cout << "------------------------" << endl;
}

// Fungsi untuk menangani pilihan menu
void handleMenuChoice(Node* root, linkedList*& eventList, const vector<EventCategory>& customMenus, int choice, const string& date) {
    Event eventNew;
    eventNew.date = date;

    // Pilihan menu awal
    switch (choice) {
        case 1:
            cout << "Breakfast" << endl;
            eventNew.menu = "Breakfast";
            chooseAndAddItemsToEvent(root, eventNew, {"Sandwich", "Bread", "Bun", "Minuman"});
            break;
        case 2:
            cout << "Morning Coffee / Tea Time / Afternoon Coffee" << endl;
            eventNew.menu = "Morning Coffee / Tea Time / Afternoon Coffee";
            chooseAndAddItemsToEvent(root, eventNew, {"Viennoiseries", "Pastry", "Minuman"});
            break;
        case 3:
            cout << "Brunch" << endl;
            eventNew.menu = "Brunch";
            chooseAndAddItemsToEvent(root, eventNew, {"Sandwich", "Bread", "Bun", "Salad", "Sup", "Minuman"});
            break;
        case 4:
            cout << "Lunch" << endl;
            eventNew.menu = "Lunch";
            chooseAndAddItemsToEvent(root, eventNew, {"Makanan Utama", "Pastry", "Minuman"});
            break;
        case 5:
            cout << "Dinner" << endl;
            eventNew.menu = "Dinner";
            chooseAndAddItemsToEvent(root, eventNew, {"Makanan Utama", "Pastry", "Minuman"});
            break;
    }

    // Pilihan menu yang telah dibuat
    if (choice >= 6 && choice < 6 + customMenus.size()) {
        int menuIndex = choice - 6; // Indeks menu yang dipilih
        eventNew.menu = customMenus[menuIndex].name;
        chooseAndAddItemsToEvent(root, eventNew, customMenus[menuIndex].category);
    }

    eventList = addEvent(eventList, eventNew);
}

// Fungsi untuk membuat menu baru
EventCategory createNewMenu(Node* root) {
    EventCategory newMenu;

    if (!root) {
        cout << "Menu tidak tersedia." << endl;
        return newMenu;
    }

    cout << "Nama event: ";
    cin.ignore(); // Ignore le '\n' restant dans le buffer
    getline(cin, newMenu.name);

    string categoryName;
    vector<string> availableCategories;

    cout << "Pilih kategori yang tersedia:" << endl;
    displayTree(root, 0, availableCategories);

    while (true) {
        cout << "Pilih kategori: (masukkan 'sudah' untuk selesai)" << endl;
        getline(cin, categoryName);
        if (categoryName == "sudah") {
            break;
        }
        if (find(availableCategories.begin(), availableCategories.end(), categoryName) != availableCategories.end()) {
            newMenu.category.push_back(categoryName);
        } else {
            cout << "Kategori tidak tersedia." << endl;
        }
    }

    return newMenu;
}

// Fungsi untuk mengubah status order
void updateOrderStatus(linkedList*& eventList, vector<Event>& orderHistory) {
    int orderId;
    cout << "Masukkan ID order yang ingin diubah: ";
    cin >> orderId;

    linkedList* current = eventList;
    linkedList* prev = nullptr;

    while (current != nullptr && current->data.orderId != orderId) {
        prev = current;
        current = current->next;
    }

    if (current == nullptr) {
        cout << "Order tidak ditemukan" << endl;
        return;
    }

    // Mengubah status order menjadi 'selesai' dan memindahkannya ke orderHistory
    current->data.status = 1;
    orderHistory.push_back(current->data);

    // Menghapus order dari eventList
    if (prev == nullptr) {
        eventList = current->next;
    } else {
        prev->next = current->next;
    }
    delete current;
}

// Fungsi untuk mendapatkan tanggal saat ini
string getCurrentDate() {
    auto now = system_clock::now();
    auto in_time_t = system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

// Menampilkan order history
void displayOrderHistory(const vector<Event>& orderHistory) {
    if (orderHistory.empty()) {
        cout << "Tidak ada riwayat pesanan." << endl;
        return;
    }

    for (const auto& event : orderHistory) {
        cout << "Order ID: " << event.orderId << endl;
        cout << "Date: " << event.date << endl;
        cout << "Menu: " << event.menu << endl;
        cout << "Status: " << (event.status == 0 ? "Menunggu" : "Selesai") << endl;
        cout << "Items: " << endl;
        for (const auto& item : event.items) {
            cout << "\t* " << item.first << " - " << item.second << endl;
        }
        cout << "--------------------------------" << endl;
    }
}

// Fungsi untuk mendapatkan tanggal berikutnya
string nextDay(const string& currentDate) {
    tm tmDate = {};
    stringstream ss(currentDate);
    ss >> get_time(&tmDate, "%Y-%m-%d"); // Assurez-vous que le format correspond

    // Conversion de tm en time_t, puis en time_point
    time_t tt = mktime(&tmDate);
    auto tp = system_clock::from_time_t(tt);

    // Ajout d'un jour
    tp += hours(24);

    // Conversion en time_t, puis en tm, et finalement en string
    tt = system_clock::to_time_t(tp);
    tmDate = *localtime(&tt);
    stringstream ssOut;
    ssOut << put_time(&tmDate, "%Y-%m-%d");

    return ssOut.str();
}

int main() {
    int choice = 0, statusCafe = 0, choiceOpen = 0; // Status: 0 close ; 1 open ; 2 exit
    Node* root = new Node("Menu");
    insertAll(root);
    linkedList* eventList = nullptr;
    vector<EventCategory> customMenus;
    vector<Event> orderHistory;
    string currentDate = getCurrentDate();

    while(statusCafe != 4) {
        displayStartingMenu();
        cout << "Masukkan pilihan: ";
        cin >> statusCafe;

        switch (statusCafe) {
            case 1: // Ouverture du café
                choice = 0;
                // Dans main(), section de gestion des choix de menu
                while(choice != customMenus.size() + 9) {
                    displayMenuChoice(customMenus);
                    cout << "Masukkan pilihan: ";
                    cin >> choice;

                    if (choice == customMenus.size() + 6) {
                        EventCategory newMenu = createNewMenu(root);
                        customMenus.push_back(newMenu);
                    } else if (choice > 0 && choice <= 5) {
                        handleMenuChoice(root, eventList, customMenus, choice, currentDate);
                    } else if (choice >= 6 && choice < 6 + customMenus.size()) {
                        handleMenuChoice(root, eventList, customMenus, choice, currentDate);
                    } else if (choice == customMenus.size() + 7) {
                        displayEvents(eventList); // Menampilkan event
                    } else if (choice == customMenus.size() + 8) {
                        updateOrderStatus(eventList, orderHistory); // Menyajikan event
                    } else if (choice == customMenus.size() + 9) {
                        cout << "Kembali ke menu utama" << endl;
                    } else {
                        cout << "Pilihan tidak tersedia" << endl;
                    }
                }
                break;
            case 2:
                cout << "Pindah ke hari berikutnya (dan reset menu)" << endl;
                currentDate = nextDay(currentDate);
                deleteTree(root);
                root = new Node("Menu");
                insertAll(root);
                cout << "Tanggal hari ini: " << currentDate << endl;
                break;
            case 3:
                if (orderHistory.empty()) {
                    cout << "Tidak ada riwayat pesanan." << endl;
                    break;
                }
                cout << endl << "Menampilkan riwayat pesanan" << endl;
                displayOrderHistory(orderHistory);
                break;
            case 4:
                cout << "Exit code" << endl;
                while (eventList) {
                    linkedList* nodeToDelete = eventList;
                    eventList = eventList->next;
                    delete nodeToDelete;
                }
                deleteTree(root);
                break;
            default:
                cout << "Pilihan tidak tersedia" << endl;
                break;
        }
    }
    return 0;
}
