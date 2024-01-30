#include <iostream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

struct Node {
    string name; // Nama kategori atau item
    float price; // Harga item
    int stock; // Jumlah stok item
    Node* leftChild; // Anak kiri
    Node* rightChild; // Anak kanan

    // Konstruktor untuk kategori (tidak ada harga dan stok)
    explicit Node(const string& name) {
        this->name = name;
        this->price = 0;
        this->stock = 0;
        this->leftChild = nullptr;
        this->rightChild = nullptr;
    }

    // Konstruktor untuk item (ada harga dan stok)
    Node(const string& name, float price, int stock) {
        this->name = name;
        this->price = price;
        this->stock = stock;
        this->leftChild = nullptr;
        this->rightChild = nullptr;
    }

    // Memeriksa apakah node merupakan kategori atau item
    bool isItem() const {
        return this->price != 0 || this->stock != 0;
    }
};

static int nextEventId = 1; // ID untuk event

struct Event {
    int orderId;
    int status; // 0 = pending, 1 = done
    string menu;
    vector<pair<string, int>> items;

    Event() : orderId(nextEventId++), status(0) {}
};

struct linkedList {
    Event data;
    linkedList* next;

    explicit linkedList(Event data) {
        this->data = data;
        this->next = nullptr;
    }
};

// Menu function
Node* addItem(Node* parent, const string& name, float price, int stock);
Node* addCategory(Node* parent, const string& name);
void displayTree(Node* root, int depth);
void displayItemsFromCategory(Node* root, const string& categoryName, int& itemNumber);
void displayItems(Node* node, int& itemNumber, vector<Node*>& items);
void deleteTree(Node* root);
void editNode(Node* root, const string& name, const string& newName, float newPrice, int newStock);
Node* findNode(Node* root, const string& name);
void fillMenu(Node* root, const vector<tuple<string, vector<pair<string, vector<tuple<string, float, int>>>>>>& menuData);
void insertAll(Node* root);

// Event function
linkedList* addEvent(linkedList* head, const Event& data);
linkedList* findOne(linkedList* head, const int& id);
void displayStartingMenu();
void displayEvents(linkedList* head);

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
void displayTree(Node* root, int depth = 0) {
    if (!root) return;

    string indent(depth * 2, ' '); // Membuat indentasi
    cout << indent << root->name << endl; // Menampilkan nama kategori atau item

    if (root->leftChild) {
        displayTree(root->leftChild, depth + 1);
    }

    if (root->rightChild) {
        displayTree(root->rightChild, depth);
    }
}

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
                    {"Pain au Chocolat", 2.49f, 5},
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


linkedList* findOne(linkedList* head, const int& id) {
    if (!head) return nullptr;

    if (head->data.orderId == id) return head;

    linkedList* foundNode = findOne(head->next, id);
    if (foundNode) return foundNode;

    return findOne(head->next, id);
}

void displayStartingMenu() {
    cout << "1. Buka cafe" << endl;
    cout << "2. Keluar" << endl;
    cout << "3. Exit code" << endl;
}

void displayEvents(linkedList* head) {
    linkedList* current = head;
    while (current != nullptr) {
        cout << "Order ID: " << current->data.orderId << endl;
        cout << "Menu: " << current->data.menu << endl;
        cout << "Status: " << (current->data.status == 0 ? "Pending" : "Done") << endl;
        cout << "Items: " << endl;
        for (const auto& item : current->data.items) {
            cout << item.first << " - " << item.second << endl;
        }
        cout << endl;
        current = current->next;
    }
}

void chooseAndAddItemsToEvent(Node* root, Event& event, const vector<string>& categories) {
    for (const auto& category : categories) {
        int itemNumber = 1;
        vector<Node*> itemsVector; // Ce vecteur va garder la trace des pointeurs vers les items affichés

        cout << "Choisissez un item de la catégorie " << category << ":" << endl;
        displayItemsFromCategory(root, category, itemNumber, itemsVector);

        int selectedItemNumber = 0;
        int quantity = 0;

        while (true) {
            cout << "Entrez le numéro de l'item: ";
            cin >> selectedItemNumber;

            if (cin.fail() || selectedItemNumber < 1 || selectedItemNumber > itemsVector.size()) {
                // Si l'entrée n'est pas valide ou hors de l'intervalle, on efface et on réessaye
                cin.clear(); // Effacer le flag d'erreur de cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorer le reste de la ligne
                cout << "Numéro d'item invalide, veuillez réessayer." << endl;
            } else {
                // Si le numéro est valide, on casse la boucle
                break;
            }
        }

        while (true) {
            cout << "Entrez la quantité pour " << itemsVector[selectedItemNumber - 1]->name << ": ";
            cin >> quantity;

            if (cin.fail() || quantity < 1) {
                // Si l'entrée n'est pas un nombre ou est inférieur à 1, on efface et on réessaye
                cin.clear(); // Effacer le flag d'erreur de cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorer le reste de la ligne
                cout << "Quantité invalide, veuillez réessayer." << endl;
            } else {
                // Si la quantité est valide, on casse la boucle
                break;
            }
        }

        Node* selectedItem = itemsVector[selectedItemNumber - 1];
        event.items.push_back(make_pair(selectedItem->name, quantity));
    }
}


void displayEventChoice() {
    cout << "1. Tambah event" << endl;
    cout << "2. Display event" << endl;
}

void displayMenuChoice() {
    cout << "1. Breakfast" << endl;
    cout << "2. Morning Coffee / Tea Time / Afternoon Coffee" << endl;
    cout << "3. Brunch" << endl;
    cout << "4. Lunch" << endl;
    cout << "5. Dinner" << endl;
    cout << "6. Back" << endl;
}

int main() {
    int choice = 0, statusCafe = 0; // Status: 0 close ;  1 open ; 2 exit
    Node* root = new Node("Menu");
    insertAll(root);
    linkedList* eventList = nullptr;

    while(statusCafe != 3) {
        displayStartingMenu();
        cout << "Masukkan pilihan: ";
        cin >> statusCafe;
        switch (statusCafe) {
            case 1:
                cout << "Cafe dibuka" << endl;
                while(choice != 6) {
                    displayMenuChoice();
                    Event eventNew;
                    cout << "Masukkan pilihan: ";
                    cin >> choice;
                    switch (choice) {
                        case 1:
                            cout << "Breakfast" << endl;
                            eventNew.menu = "Breakfast";
                            chooseAndAddItemsToEvent(root, eventNew, {"Sandwich", "Bread", "Bun", "Minuman"});
                            eventList = addEvent(eventList, eventNew);
                            break;
                        case 2:
                            cout << "Morning Coffee / Tea Time / Afternoon Coffee" << endl;
                            eventNew.menu = "Morning Coffee / Tea Time / Afternoon Coffee";
                            chooseAndAddItemsToEvent(root, eventNew, {"Viennoiseries", "Pastry", "Minuman"});
                            eventList = addEvent(eventList, eventNew);
                            break;
                        case 3:
                            cout << "Brunch" << endl;
                            eventNew.menu = "Brunch";
                            chooseAndAddItemsToEvent(root, eventNew, {"Sandwich", "Bread", "Bun", "Salad", "Sup", "Minuman"});
                            eventList = addEvent(eventList, eventNew);
                            break;
                        case 4:
                            cout << "Lunch" << endl;
                            eventNew.menu = "Lunch";
                            chooseAndAddItemsToEvent(root, eventNew, {"Makanan Utama", "Pastry", "Minuman"});
                            eventList = addEvent(eventList, eventNew);
                            break;
                        case 5:
                            cout << "Dinner" << endl;
                            eventNew.menu = "Dinner";
                            chooseAndAddItemsToEvent(root, eventNew, {"Makanan Utama", "Pastry", "Minuman"});
                            eventList = addEvent(eventList, eventNew);
                            break;
                        case 6:
                            cout << "Back" << endl;
                            break;
                        default:
                            cout << "Pilihan tidak tersedia" << endl;
                            break;
                    }
                    cout << "Current event" << endl;
                    displayEvents(eventList);
//                    currentEvent.push_back(eventNew);
//                    for (const Event& event : currentEvent) {
//                        cout << endl << "============" << event.orderId << "============" << endl;
//                        cout << event.menu << endl;
//                        cout << event.status << endl;
//                        for (const auto& item : event.items) {
//                            cout << item.first << " - " << item.second << endl;
//                        }
//                    }
                }
                break;
            case 2:
                cout << "Cafe ditutup" << endl;
                break;
            case 3:
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