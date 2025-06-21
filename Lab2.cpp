#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <list>
#include <set>
#include <cctype>

struct Child {
    int year;
    int groupNumber;
    std::string fullName;
    std::string birthDate;
    int shiftNumber;

    bool operator<(const Child& other) const {
        if (year != other.year)
            return year < other.year;
        if (groupNumber != other.groupNumber)
            return groupNumber < other.groupNumber;
        if (shiftNumber != other.shiftNumber)
            return shiftNumber < other.shiftNumber;
        return fullName < other.fullName;
    }

    bool operator>(const Child& other) const {
        return other < *this;
    }

    bool operator<=(const Child& other) const {
        return !(other < *this);
    }

    bool operator>=(const Child& other) const {
        return !(*this < other);
    }
};

// Функция для чтения строки из CSV файла и преобразования в объект child
Child parseCSVLine(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    Child child;

    std::getline(ss, token, ',');
    child.year = std::stoi(token);
    std::getline(ss, token, ',');
    child.groupNumber = std::stoi(token);
    std::getline(ss, token, ',');
    child.fullName = token;
    std::getline(ss, token, ',');
    child.birthDate = token;
    std::getline(ss, token, ',');
    child.shiftNumber = std::stoi(token);

    return child;
}

// Функция для чтения из файла CSV и создания вектора объектов child
std::vector<Child> readChildrenFromFile(const std::string& filename) {
    std::vector<Child> children;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            children.push_back(parseCSVLine(line));
        }
    }
    return children;
}

// Функция для записи в файл CSV вектора объектов child
void writeChildrenToFile(const std::string& filename, const std::vector<Child>& children) {
    std::ofstream file(filename);
    for (const Child& child : children) {
        file << child.year << ','
             << child.groupNumber << ','
             << child.fullName << ','
             << child.birthDate << ','
             << child.shiftNumber << '\n';
    }
}

// Функция для записи в файл CSV времени, затраченного на сортировку 
void writeTimesToFile(const std::string& filename, const std::vector<std::pair<std::string, long long>>& times) {
    std::ofstream file(filename);
    for (const auto& [name, time] : times) {
        file << name << ',' << time << '\n';
    }
}

// Функция для создания директории (если она не существует)
void createDirectoryIfNotExists(const std::string& dirname) {
    struct stat st{};
    if (stat(dirname.c_str(), &st) != 0) {
        mkdir(dirname.c_str(), 0755);
    }
}

// Функция для проверки того, что строка заканчивается на ".csv"
bool hasCSVExtension(const std::string& filename) {
    if (filename.size() < 4) return false;
    std::string ext = filename.substr(filename.size() - 4);
    for (auto &c : ext) c = std::tolower(c);
    return ext == ".csv";
}

// Функция для линейного поиска
std::vector<Child> linearSearch(const std::vector<Child>& children, const std::string& fullName) {
    std::vector<Child> found;
    for (const auto& child : children) {
        if (child.fullName == fullName) {
            found.push_back(child);
        }
    }
    return found;
}

// Класс для бинарного дерева поиска
class BST {
    struct Node {
        std::string key;
        std::vector<Child> childrenList;
        Node *left;
        Node *right;
        Node(const Child& child) : key(child.fullName), left(nullptr), right(nullptr) {
            childrenList.push_back(child);
        }
    };
    Node* root;

    void insertNode(Node*& node, const Child& child) {
        if (node == nullptr) {
            node = new Node(child);
        } else if (child.fullName < node->key) {
            insertNode(node->left, child);
        } else if (child.fullName > node->key) {
            insertNode(node->right, child);
        } else {
            node->childrenList.push_back(child);
        }
    }

    Node* findNode(Node* node, const std::string& key) const {
        if (node == nullptr) return nullptr;
        if (key < node->key) return findNode(node->left, key);
        if (key > node->key) return findNode(node->right, key);
        return node;
    }

public:
    BST() : root(nullptr) {}

    void insert(const Child& child) {
        insertNode(root, child);
    }

    std::vector<Child> search(const std::string& key) const {
        std::vector<Child> result;
        Node* node = findNode(root, key);
        if (node != nullptr) {
            result = node->childrenList;
        }
        return result;
    }
};

// Класс для хэш-таблицы
class HashTable {
    std::vector<std::list<Child>> table;
    int capacity;
    long long collisions;

    // Хэш-функция
    size_t betterHash(const std::string& key) const {
        size_t hash = 0;
        for (char c : key) {
            hash = hash * 131 + static_cast<unsigned char>(c);
        }
        return hash % capacity;
    }

public:
    HashTable(int size) : table(size), capacity(size), collisions(0) {}

    // Функция помещения объекта child в хэш-таблицу
    void insert(const Child& child) {
        std::string key = child.fullName;
        size_t hashValue = betterHash(key);
        if (!table[hashValue].empty()) {
            collisions++;
        }
        table[hashValue].push_back(child);
    }

    // Функция поиска всех объектов child с соответствующим ключом
    std::vector<Child> search(const std::string& key) const {
        std::vector<Child> result;
        size_t hashValue = betterHash(key);
        for (const auto& child : table[hashValue]) {
            if (child.fullName == key) {
                result.push_back(child);
            }
        }
        return result;
    }

    long long getCollisions() const {
        return collisions;
    }
};

// Класс для красно-черного дерева
class RBT {
    enum Color { RED, BLACK };

    // Структура для узла красно-черного
    struct RBTNode {
        std::string key;
        std::vector<Child> childrenList;
        Color color;
        RBTNode* parent;
        RBTNode* left;
        RBTNode* right;

        RBTNode(const std::string& k, const Child& c)
            : key(k), color(RED), parent(nullptr), left(nullptr), right(nullptr) {
            childrenList.push_back(c);
        }
    };

    RBTNode* root;

    void leftRotate(RBTNode* x) {
        if (!x || !x->right) return;
        RBTNode* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBTNode* x) {
        if (!x || !x->left) return;
        RBTNode* y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void fixInsert(RBTNode* node) {
        while (node != root && node->parent && node->parent->color == RED) {
            RBTNode* parent = node->parent;
            RBTNode* grand = parent->parent;
            if (!grand) break;

            if (parent == grand->left) {
                RBTNode* uncle = grand->right;
                if (uncle && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grand->color = RED;
                    node = grand;
                } else {
                    if (node == parent->right) {
                        node = parent;
                        leftRotate(node);
                    }
                    parent->color = BLACK;
                    grand->color = RED;
                    rightRotate(grand);
                }
            } else {
                RBTNode* uncle = grand->left;
                if (uncle && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grand->color = RED;
                    node = grand;
                } else {
                    if (node == parent->left) {
                        node = parent;
                        rightRotate(node);
                    }
                    parent->color = BLACK;
                    grand->color = RED;
                    leftRotate(grand);
                }
            }
        }
        root->color = BLACK;
    }

public:
    RBT() : root(nullptr) {}

    void insert(const Child& c) {
        RBTNode* node = root;
        RBTNode* parent = nullptr;

        while (node) {
            parent = node;
            if (c.fullName == node->key) {
                node->childrenList.push_back(c);
                return;
            } else if (c.fullName < node->key) {
                node = node->left;
            } else {
                node = node->right;
            }
        }

        RBTNode* newNode = new RBTNode(c.fullName, c);
        newNode->parent = parent;
        if (!parent)
            root = newNode;
        else if (c.fullName < parent->key)
            parent->left = newNode;
        else
            parent->right = newNode;

        fixInsert(newNode);
    }

    std::vector<Child> search(const std::string& key) const {
        RBTNode* node = root;
        while (node) {
            if (key == node->key) return node->childrenList;
            else if (key < node->key) node = node->left;
            else node = node->right;
        }
        return {};
    }
};

int main() {
    const std::string inputDir = "children-unsorted";
    const std::string outputDir = "found-children";
    const std::string searchName = "Ковалев Иван Иванович";

    createDirectoryIfNotExists(outputDir);
    createDirectoryIfNotExists(outputDir + "/LinearSearch");
    createDirectoryIfNotExists(outputDir + "/BST");
    createDirectoryIfNotExists(outputDir + "/RBT");
    createDirectoryIfNotExists(outputDir + "/HashTable");
    createDirectoryIfNotExists(outputDir + "/HashTable/collision");
    createDirectoryIfNotExists(outputDir + "/MultimapSearch");
    createDirectoryIfNotExists(outputDir + "/times");

    DIR* dir = opendir(inputDir.c_str());
    if (!dir) {
        std::cerr << "Не удалось открыть директорию: " << inputDir << "\n";
        return 1;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        
        // Проверяем, что entry это файл, и что он имеет расширение CSV
        if (entry->d_type == DT_REG && hasCSVExtension(filename)) {
            if (hasCSVExtension(filename) && filename.rfind("children_", 0) == 0) {
                std::string inputPath = inputDir + "/" + filename;
                std::vector<Child> children = readChildrenFromFile(inputPath);
                int size = children.size();
                std::string sizeStr = std::to_string(size);

                std::string baseFilename = "children_" + sizeStr + ".csv";
                std::vector<std::pair<std::string,long long>> times;

                // Линейный поиск
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    std::vector<Child> found = linearSearch(children, searchName);
                    auto end = std::chrono::high_resolution_clock::now();
                    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    times.emplace_back("LinearSearch", duration);
                    // Write found children to file
                    std::string outPath = outputDir + "/LinearSearch/" + baseFilename;
                    writeChildrenToFile(outPath, found);
                }

                // Поиск бинарным деревом поиска
                {
                    BST bst;
                    for (const Child& child : children) {
                        bst.insert(child);
                    }
                    auto start = std::chrono::high_resolution_clock::now();
                    std::vector<Child> found = bst.search(searchName);
                    auto end = std::chrono::high_resolution_clock::now();
                    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    times.emplace_back("BST", duration);
                    std::string outPath = outputDir + "/BST/" + baseFilename;
                    writeChildrenToFile(outPath, found);
                }
                
                // Поиск красно-черным деревом поиска
                {
                    RBT rbt;
                    for (const Child& child : children) {
                        rbt.insert(child);
                    }
                    auto start = std::chrono::high_resolution_clock::now();
                    std::vector<Child> found = rbt.search(searchName);
                    auto end = std::chrono::high_resolution_clock::now();
                    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    times.emplace_back("RBT", duration);
                    std::string outPath = outputDir + "/RBT/" + baseFilename;
                    writeChildrenToFile(outPath, found);
                }

                // Поиск через хэш-таблицу
                {
                    const int tableSize = 300007;
                    HashTable ht(tableSize);
                    for (const Child& child : children) {
                        ht.insert(child);
                    }
                    auto start = std::chrono::high_resolution_clock::now();
                    std::vector<Child> found = ht.search(searchName);
                    auto end = std::chrono::high_resolution_clock::now();
                    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    times.emplace_back("HashTable", duration);
                    std::string outPath = outputDir + "/HashTable/" + baseFilename;
                    writeChildrenToFile(outPath, found);
                    // Записываем количесов коллизий
                    long long collisions = ht.getCollisions();
                    std::string collisionPath = outputDir + "/HashTable/collision/" + baseFilename;
                    std::ofstream collFile(collisionPath);
                    if (collFile.is_open()) {
                        collFile << collisions << '\n';
                        collFile.close();
                    }
                }

                // Поиск через multimap
                {
                    std::multimap<std::string, Child> mmap;
                    for (const Child& child : children) {
                        mmap.insert({child.fullName, child});
                    }
                    auto start = std::chrono::high_resolution_clock::now();
                    auto range = mmap.equal_range(searchName);
                    std::vector<Child> found;
                    for (auto it = range.first; it != range.second; ++it) {
                        found.push_back(it->second);
                    }
                    auto end = std::chrono::high_resolution_clock::now();
                    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    times.emplace_back("MultimapSearch", duration);
                    std::string outPath = outputDir + "/MultimapSearch/" + baseFilename;
                    writeChildrenToFile(outPath, found);
                }

                // Сохраняем файл с результатами времени
                std::string timesPath = outputDir + "/times/" + baseFilename;
                writeTimesToFile(timesPath, times);
            }
        }
    }
    return 0;
}
