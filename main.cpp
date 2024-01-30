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

void insertall(Node* root){
    //Node* root = new Node("Menu");

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
                    {"Tarte Tatin", 3.99f, 4}
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
                    {"Chamomile", 2.99f, 30}
                }},
                {"Kopi", {
                    {"Long black", 2.49f, 25},
                    {"Cappuccino", 2.99f, 25}
                }}
            }},
            {"Makanan Utama", {
                {"Salad", {
                    {"Caesar Salad", 4.99f, 8},
                    {"Caprese Salad", 5.49f, 6}
                }},
                {"Sup", {
                    {"Pumpkin soup", 3.99f, 10},
                    {"Mushroom soup", 3.99f, 10}
                }},
                {"Pasta", {
                    {"Gnocchi", 6.99f, 5},
                    {"Aglio Olio", 5.99f, 7}
                }}
            }}
    };
     // Fill the menu tree with the data
    fillMenu(root, menuData);

}

void inputNewSubCategory(Node* root) {
    string category, itemName;
    float itemPrice;
    int itemStock;

    cout << "Category: ";
    getline(cin, category);

    Node* categoryNode = findNode(root, category);

    if (categoryNode) {
        cout << "SubCategory : ";
        getline(cin, itemName);

        Node* newSubCategory = addItem(categoryNode, itemName, 0, 0);
        cout << "Subcategory ditambahkan!" << endl;
    } else {
        cout << "Category tidak ditemukan!" << endl;
    }

    char choice;
    do {
        cout << "\nTambah subcategory lagi? (y/n): ";
        cin >> choice;
        cin.ignore();

        if (choice == 'y' || choice == 'Y') {
            inputNewSubCategory(root);
        }
    } while (choice == 'y' || choice == 'Y');
    }

void inputNewItem(Node* root) {
    string Category, SubCategory, itemName;
    float itemPrice;
    int itemStock;

    cout << "Category: ";
    getline(cin, Category);

    Node* CategoryNode = findNode(root, Category);

    if (CategoryNode) {

        cout << "SubCategory: ";
        getline(cin, SubCategory);

        Node* SubCategoryNode = findNode(root, SubCategory);

        if(SubCategoryNode){
        cout << "Nama Item : ";
        getline(cin, itemName);

        cout << "Harga Item : ";
        cin >> itemPrice;

        cout << "Stok Item : ";
        cin >> itemStock;

        addItem(SubCategoryNode, itemName, itemPrice, itemStock);
        cout << "Item ditambahkan!" << endl;
    } else {
        cout << "Sub Category Tidak ditemukan!" << endl;
    }
    } else {
        cout << "Category tidak ditemukan! " << endl;
    }

    char choice;
    do {
        cout << "\nTambah Item lagi ? (y/n): ";
        cin >> choice;
        cin.ignore();

        if (choice == 'y' || choice == 'Y') {
            inputNewItem(root);
        }
    } while (choice == 'y' || choice == 'Y');
}

int main() {
    Node* root = new Node("Menu");
    insertall(root);
    // Display the tree to verify
    displayTree(root);
    inputNewSubCategory(root);

    // inputNewItem(root);
    displayTree(root);

    return 0;
}
