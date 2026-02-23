#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <queue>
#include <stack>
#include <map>
#include <limits>
#include <algorithm>
#include <cstdlib>

using namespace std;

// ==================== GLOBAL COUNTERS ====================
int forestCount = 0, warehouseCount = 0, woodCount = 0, inventoryCount = 0, deliveryCount = 0, offerCount = 0;

// ==================== STRUCTURES ====================

// FOREST (Singly Linked List)
struct Forest {
    int id;
    string name;
    string city;
    string location;
    Forest* next;
};
Forest* forestHead = NULL;

// WAREHOUSE (Doubly Linked List)
struct Warehouse {
    int id;
    string name;
    string city;
    int totalCapacity;
    int usedCapacity;
    Warehouse* next;
    Warehouse* prev;
};
Warehouse* warehouseHead = NULL;
Warehouse* warehouseTail = NULL;

// STOCK IN WAREHOUSE (Circular Linked List)
struct Stock {
    int warehouseId;
    string woodType;
    int quantity;
    Stock* next;
};
Stock* stockHead = NULL;

// WOOD TYPE (Binary Search Tree)
struct WoodType {
    int id;
    string name;
    string category;
    string form;
    string quality;
    int forestId;
    double basePrice;
    WoodType* left;
    WoodType* right;
};
WoodType* woodRoot = NULL;

// INVENTORY (Array-based with Linear Search)
struct Inventory {
    int id;
    int woodTypeId;
    int warehouseId;
    int quantity;
    string lastUpdated;
};
const int MAX_INVENTORY = 1000;
Inventory inventoryArray[MAX_INVENTORY];
int inventorySize = 0;

// DELIVERY (Circular Queue)
struct Delivery {
    int id;
    int woodTypeId;
    string sourceCity;
    string destCity;
    string deliveryDate;
    string transportType;
    int quantity;
};
const int MAX_QUEUE = 100;
Delivery deliveryQueue[MAX_QUEUE];
int queueFront = -1, queueRear = -1;

// OFFER (Stack - Last In First Out)
struct Offer {
    int id;
    int woodTypeId;
    double discountPercent;
    string description;
    string validUntil;
};
const int MAX_STACK = 100;
Offer offerStack[MAX_STACK];
int stackTop = -1;

// ACTIVITY LOG (Circular Stack)
const int MAX_LOG = 50;
string activityLog[MAX_LOG];
int logTop = -1;

// CITY ROUTE STRUCTURE (Replacing vector with linked list)
struct CityRoute {
    string destCity;
    int distance;
    CityRoute* next;
};

struct CityNode {
    string cityName;
    CityRoute* routes;
    CityNode* next;
};
CityNode* cityGraphHead = NULL;

// SORTING HELPER
struct WoodSort {
    int id;
    string name;
    double price;
};

// ==================== UTILITY FUNCTIONS ====================
string getCurrentDateTime() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d  %H:%M:%S", localtime(&now));
    return string(buf);
}

string intToString(int val) {
    stringstream ss;
    ss << val;
    return ss.str();
}

void logActivity(string activity) {
    logTop = (logTop + 1) % MAX_LOG;
    activityLog[logTop] = "[" + getCurrentDateTime() + "] " + activity;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printTableLine(int width) {
    cout << "+";
    for(int i = 0; i < width; i++)
	cout << "-";
    cout << "+\n";
}

void printTableHeader(string title, int width) {
    printTableLine(width);
    int padding = (width - title.length()) / 2;
    cout << "|";
    for(int i = 0; i < padding; i++) cout << " ";
    cout << title;
    for(int i = 0; i < width - padding - title.length(); i++) cout << " ";
    cout << "|\n";
    printTableLine(width);
}

// ==================== GRAPH FUNCTIONS (Without Vector) ====================
CityNode* findCityNode(string cityName) {
    CityNode* temp = cityGraphHead;
    while (temp != NULL) {
        if (temp->cityName == cityName) 
		return temp;
        temp = temp->next;
    }
    return NULL;
}

void addCityRoute(string city1, string city2, int distance) {
    // Add city1 if not exists
    CityNode* node1 = findCityNode(city1);
    if (node1 == NULL) {
        node1 = new CityNode;
        node1->cityName = city1;
        node1->routes = NULL;
        node1->next = cityGraphHead;
        cityGraphHead = node1;
    }
    
    // Add city2 if not exists
    CityNode* node2 = findCityNode(city2);
    if (node2 == NULL) {
        node2 = new CityNode;
        node2->cityName = city2;
        node2->routes = NULL;
        node2->next = cityGraphHead;
        cityGraphHead = node2;
    }
    
    // Add route from city1 to city2
    CityRoute* route1 = new CityRoute;
    route1->destCity = city2;
    route1->distance = distance;
    route1->next = node1->routes;
    node1->routes = route1;
    
    // Add route from city2 to city1 (bidirectional)
    CityRoute* route2 = new CityRoute;
    route2->destCity = city1;
    route2->distance = distance;
    route2->next = node2->routes;
    node2->routes = route2;
}

void initializeCityGraph() {
    addCityRoute("Lahore", "Faisalabad", 120);
    addCityRoute("Lahore", "Multan", 340);
    addCityRoute("Faisalabad", "Multan", 250);
    addCityRoute("Lahore", "Islamabad", 375);
    addCityRoute("Karachi", "Hyderabad", 165);
    addCityRoute("Multan", "Bahawalpur", 105);
}

void displayAvailableRoutes() {
    printTableHeader("AVAILABLE TRANSPORT ROUTES", 70);
    cout << "| " << left << setw(15) << "From City" 
         << " | " << setw(15) << "To City" 
         << " | " << setw(12) << "Distance (km)" << "   |\n";
    printTableLine(70);
    
    CityNode* cityTemp = cityGraphHead;
    while (cityTemp != NULL) {
        CityRoute* routeTemp = cityTemp->routes;
        while (routeTemp != NULL) {
            cout << "| " << left << setw(15) << cityTemp->cityName 
                 << " | " << setw(15) << routeTemp->destCity 
                 << " | " << setw(12) << routeTemp->distance << "    |\n";
            routeTemp = routeTemp->next;
        }
        cityTemp = cityTemp->next;
    }
    printTableLine(70);
}

int findShortestDistance(string src, string dest) {
    // Simple implementation using array for distances
    const int MAX_CITIES = 50;
    string cities[MAX_CITIES];
    int dist[MAX_CITIES];
    bool visited[MAX_CITIES];
    int cityCount = 0;
    
    // Collect all cities
    CityNode* temp = cityGraphHead;
    while (temp != NULL) {
        cities[cityCount] = temp->cityName;
        dist[cityCount] = 999999;
        visited[cityCount] = false;
        cityCount++;
        temp = temp->next;
    }
    
    // Find source index
    int srcIndex = -1;
    for (int i = 0; i < cityCount; i++) {
        if (cities[i] == src) {
            srcIndex = i;
            dist[i] = 0;
            break;
        }
    }
    
    if (srcIndex == -1) return -1;
    
    // Dijkstra's algorithm
    for (int count = 0; count < cityCount - 1; count++) {
        int minDist = 999999;
        int minIndex = -1;
        
        for (int i = 0; i < cityCount; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                minIndex = i;
            }
        }
        
        if (minIndex == -1) break;
        
        visited[minIndex] = true;
        
        // Update distances
        CityNode* cityNode = findCityNode(cities[minIndex]);
        if (cityNode != NULL) {
            CityRoute* route = cityNode->routes;
            while (route != NULL) {
                for (int i = 0; i < cityCount; i++) {
                    if (cities[i] == route->destCity) {
                        if (!visited[i] && dist[minIndex] + route->distance < dist[i]) {
                            dist[i] = dist[minIndex] + route->distance;
                        }
                        break;
                    }
                }
                route = route->next;
            }
        }
    }
    
    // Find destination distance
    for (int i = 0; i < cityCount; i++) {
        if (cities[i] == dest) {
            return dist[i] == 999999 ? -1 : dist[i];
        }
    }
    
    return -1;
}

// ==================== FOREST FUNCTIONS (Singly Linked List) ====================
void addForest(int id, string name, string city, string location) {
    Forest* newForest = new Forest;
    newForest->id = id;
    newForest->name = name;
    newForest->city = city;
    newForest->location = location;
    newForest->next = NULL;
    
    if (forestHead == NULL) {
        forestHead = newForest;
    } else {
        Forest* temp = forestHead;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newForest;
    }
    
    logActivity("Forest registered: " + name);
}

void displayForests() {
    if (forestHead == NULL) {
        cout << "\nNo forests registered!\n";
        return;
    }
    
    printTableHeader("REGISTERED FORESTS", 80);
    cout << " | " << left << setw(5) << "ID" 
         << " | " << setw(20) << "Name" 
         << " | " << setw(15) << "City" 
         << " | " << setw(25) << "Location" << "        |\n";
    printTableLine(80);
    
    Forest* temp = forestHead;
    while (temp != NULL) {
        cout << "| " << left << setw(5) << temp->id 
             << " | " << setw(20) << temp->name 
             << " | " << setw(15) << temp->city 
             << " | " << setw(25) << temp->location << "        |\n";
        temp = temp->next;
    }
    printTableLine(80);
}

// ==================== WAREHOUSE FUNCTIONS (Doubly Linked List) ====================
void addWarehouse(int id, string name, string city, int capacity) {
    Warehouse* newWH = new Warehouse;
    newWH->id = id;
    newWH->name = name;
    newWH->city = city;
    newWH->totalCapacity = capacity;
    newWH->usedCapacity = 0;
    newWH->next = NULL;
    newWH->prev = NULL;
    
    if (warehouseHead == NULL) {
        warehouseHead = warehouseTail = newWH;
    } else {
        warehouseTail->next = newWH;
        newWH->prev = warehouseTail;
        warehouseTail = newWH;
    }
    
    logActivity("Warehouse added: " + name);
}

void displayWarehouses() {
    if (warehouseHead == NULL) {
        cout << "\nNo warehouses registered!\n";
        return;
    }
    
    printTableHeader("WAREHOUSE STATUS", 90);
    cout << "| " << left << setw(5) << "ID" 
         << " | " << setw(20) << "Name" 
         << " | " << setw(12) << "City" 
         << " | " << setw(12) << "Used/Total" 
         << " | " << setw(15) << "Available" << "        |\n";
    printTableLine(90);
    
    Warehouse* temp = warehouseHead;
    while (temp != NULL) {
        int available = temp->totalCapacity - temp->usedCapacity;
        cout << "| " << left << setw(5) << temp->id 
             << " | " << setw(20) << temp->name 
             << " | " << setw(12) << temp->city 
             << " | " << setw(12) << (intToString(temp->usedCapacity) + "/" + intToString(temp->totalCapacity))
             << " | " << setw(15) << available << "         |\n";
        temp = temp->next;
    }
    printTableLine(90);
}

Warehouse* findWarehouse(int id) {
    Warehouse* temp = warehouseHead;
    while (temp != NULL) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

// ==================== STOCK FUNCTIONS (Circular Linked List) ====================
void addStock(int warehouseId, string woodType, int quantity) {
    Stock* newStock = new Stock;
    newStock->warehouseId = warehouseId;
    newStock->woodType = woodType;
    newStock->quantity = quantity;
    
    if (stockHead == NULL) {
        stockHead = newStock;
        newStock->next = stockHead;
    } else {
        Stock* temp = stockHead;
        while (temp->next != stockHead) temp = temp->next;
        temp->next = newStock;
        newStock->next = stockHead;
    }
    
    Warehouse* wh = findWarehouse(warehouseId);
    if (wh != NULL) wh->usedCapacity += quantity;
    
    logActivity("Stock added: " + woodType + " x" + intToString(quantity));
}

void displayStockByWarehouse(int warehouseId) {
    if (stockHead == NULL) {
        cout << "\n>>> No stock in any warehouse!\n";
        return;
    }
    
    printTableHeader("WAREHOUSE STOCK DETAILS", 60);
    cout << "| " << left << setw(20) << "Wood Type" 
         << " | " << setw(15) << "Quantity" << "    |\n";
    printTableLine(60);
    
    Stock* temp = stockHead;
    bool found = false;
    do {
        if (temp->warehouseId == warehouseId) {
            cout << "| " << left << setw(20) << temp->woodType 
                 << " | " << setw(15) << temp->quantity << "    |\n";
            found = true;
        }
        temp = temp->next;
    } while (temp != stockHead);
    
    if (!found) {
        cout << "| " << setw(35) << "No stock in this warehouse" << "   |\n";
    }
    printTableLine(60);
}

// ==================== BST FUNCTIONS (Binary Search Tree) ====================
WoodType* createWoodNode(int id, string name, string cat, string form, string quality, int forestId, double price) {
    WoodType* newWood = new WoodType;
    newWood->id = id;
    newWood->name = name;
    newWood->category = cat;
    newWood->form = form;
    newWood->quality = quality;
    newWood->forestId = forestId;
    newWood->basePrice = price;
    newWood->left = NULL;
    newWood->right = NULL;
    return newWood;
}

WoodType* insertWood(WoodType* root, int id, string name, string cat, string form, string quality, int forestId, double price) {
    if (root == NULL) {
        logActivity("Wood type added: " + name);
        return createWoodNode(id, name, cat, form, quality, forestId, price);
    }
    
    if (id < root->id) {
        root->left = insertWood(root->left, id, name, cat, form, quality, forestId, price);
    } else if (id > root->id) {
        root->right = insertWood(root->right, id, name, cat, form, quality, forestId, price);
    }
    
    return root;
}

void inorderWood(WoodType* root) {
    if (root == NULL) return;
    
    inorderWood(root->left);
    cout << "| " << left << setw(5) << root->id 
         << " | " << setw(15) << root->name 
         << " | " << setw(12) << root->category 
         << " | " << setw(10) << root->form 
         << " | " << setw(10) << root->quality 
         << " | $" << setw(11) << fixed << setprecision(2) << root->basePrice << "     |\n";
    inorderWood(root->right);
}

void displayWoodTypes() {
    if (woodRoot == NULL) {
        cout << "\nNo wood types registered!\n";
        return;
    }
    
    printTableHeader("WOOD CATALOG - BST Inorder Traversal", 85);
    cout << "| " << left << setw(5) << "ID" 
         << " | " << setw(15) << "Name" 
         << " | " << setw(12) << "Category" 
         << " | " << setw(10) << "Form" 
         << " | " << setw(10) << "Quality" 
         << " | " << setw(12) << "Price" << "   |\n";
    printTableLine(85);
    
    inorderWood(woodRoot);
    printTableLine(85);
}

WoodType* searchWood(WoodType* root, int id) {
    if (root == NULL || root->id == id) return root;
    
    if (id < root->id) return searchWood(root->left, id);
    return searchWood(root->right, id);
}

// ==================== INVENTORY FUNCTIONS (Array + Linear Search) ====================
void addInventory(int id, int woodTypeId, int warehouseId, int quantity) {
    if (inventorySize >= MAX_INVENTORY) {
        cout << "\n>>> Inventory full!\n";
        return;
    }
    
    inventoryArray[inventorySize].id = id;
    inventoryArray[inventorySize].woodTypeId = woodTypeId;
    inventoryArray[inventorySize].warehouseId = warehouseId;
    inventoryArray[inventorySize].quantity = quantity;
    inventoryArray[inventorySize].lastUpdated = getCurrentDateTime();
    inventorySize++;
    
    logActivity("Inventory record added");
}

int linearSearchInventory(int woodTypeId) {
    for (int i = 0; i < inventorySize; i++) {
        if (inventoryArray[i].woodTypeId == woodTypeId) {
            return i;
        }
    }
    return -1;
}

void displayInventory() {
    if (inventorySize == 0) {
        cout << "\nNo inventory records!\n";
        return;
    }
    
    printTableHeader("INVENTORY RECORDS", 95);
    cout << "| " << left << setw(5) << "ID" 
         << " | " << setw(12) << "Wood ID" 
         << " | " << setw(14) << "Warehouse ID" 
         << " | " << setw(12) << "Quantity" 
         << " | " << setw(22) << "Last Updated" << "      |\n";
    printTableLine(95);
    
    for (int i = 0; i < inventorySize; i++) {
        cout << "| " << left << setw(5) << inventoryArray[i].id 
             << " | " << setw(12) << inventoryArray[i].woodTypeId 
             << " | " << setw(14) << inventoryArray[i].warehouseId 
             << " | " << setw(12) << inventoryArray[i].quantity 
             << " | " << setw(22) << inventoryArray[i].lastUpdated << "      |\n";
    }
    printTableLine(95);
}

// ==================== CIRCULAR QUEUE (Delivery) ====================
bool isQueueEmpty() {
    return queueFront == -1;
}

bool isQueueFull() {
    return (queueRear + 1) % MAX_QUEUE == queueFront;
}

void enqueueDelivery(Delivery d) {
    if (isQueueFull()) {
        cout << "\n>>> Delivery queue full!\n";
        return;
    }
    
    if (queueFront == -1) queueFront = 0;
    queueRear = (queueRear + 1) % MAX_QUEUE;
    deliveryQueue[queueRear] = d;
    
    logActivity("Delivery scheduled: " + d.sourceCity + " -> " + d.destCity);
}

Delivery dequeueDelivery() {
    Delivery d = deliveryQueue[queueFront];
    
    if (queueFront == queueRear) {
        queueFront = queueRear = -1;
    } else {
        queueFront = (queueFront + 1) % MAX_QUEUE;
    }
    
    return d;
}

void processNextDelivery() {
    if (isQueueEmpty()) {
        cout << "\n>>> No pending deliveries!\n";
        return;
    }
    
    Delivery d = dequeueDelivery();
    
    printTableHeader("PROCESSING DELIVERY", 70);
    cout << "Delivery ID    : " << d.id << "\n";
    cout << "Wood Type ID   : " << d.woodTypeId << "\n";
    cout << "Route          : " << d.sourceCity << " -> " << d.destCity << "\n";
    cout << "Date           : " << d.deliveryDate << "\n";
    cout << "Transport      : " << d.transportType << "\n";
    cout << "Quantity       : " << d.quantity << "\n";
    
    int distance = findShortestDistance(d.sourceCity, d.destCity);
    if (distance != -1) {
        cout << "Shortest Path  : " << distance << " km\n";
    }
    printTableLine(70);
    
    logActivity("Delivery processed: ID " + intToString(d.id));
}

void displayPendingDeliveries() {
    if (isQueueEmpty()) {
        cout << "\n>>> No pending deliveries!\n";
        return;
    }
    
    int i = queueFront;
    
    printTableHeader("PENDING DELIVERIES (Queue)", 70);
    cout << "| " << left << setw(5) << "ID" 
         << " | " << setw(15) << "Route" 
         << " | " << setw(12) << "Transport" 
         << " | " << setw(10) << "Quantity" << " |\n";
    printTableLine(70);
    
    while (true) {
        cout << "| " << left << setw(5) << deliveryQueue[i].id 
             << " | " << setw(15) << (deliveryQueue[i].sourceCity + "->" + deliveryQueue[i].destCity)
             << " | " << setw(12) << deliveryQueue[i].transportType 
             << " | " << setw(10) << deliveryQueue[i].quantity << " |\n";
        
        if (i == queueRear) break;
        i = (i + 1) % MAX_QUEUE;
    }
    printTableLine(70);
}

// ==================== STACK (Offers - LIFO) ====================
bool isStackEmpty() {
    return stackTop == -1;
}

bool isStackFull() {
    return stackTop == MAX_STACK - 1;
}

void pushOffer(Offer o) {
    if (isStackFull()) {
        cout << "\n>>> Offer stack full!\n";
        return;
    }
    
    offerStack[++stackTop] = o;
    logActivity("Offer added for Wood ID " + intToString(o.woodTypeId));
}

void displayOffers() {
    if (isStackEmpty()) {
        cout << "\nNo active offers!\n";
        return;
    }
    
    printTableHeader("ACTIVE OFFERS - Last In First Out", 95);
    cout << "| " << left << setw(8) << "Offer ID" 
         << " | " << setw(10) << "Wood ID" 
         << " | " << setw(12) << "Discount %" 
         << " | " << setw(25) << "Description" 
         << " | " << setw(15) << "Valid Until" << "  |\n";
    printTableLine(95);
    
    for (int i = stackTop; i >= 0; i--) {
        cout << "| " << left << setw(8) << offerStack[i].id 
             << " | " << setw(10) << offerStack[i].woodTypeId 
             << " | " << setw(12) << offerStack[i].discountPercent 
             << " | " << setw(25) << offerStack[i].description 
             << " | " << setw(15) << offerStack[i].validUntil << "  |\n";
        
        WoodType* wood = searchWood(woodRoot, offerStack[i].woodTypeId);
        if (wood != NULL) {
            double finalPrice = wood->basePrice * (1 - offerStack[i].discountPercent / 100);
            cout << "|   Base: $" << fixed << setprecision(2) << wood->basePrice 
                 << " -> Final: $" << finalPrice << "\n";
        }
    }
    printTableLine(95);
}

// ==================== ACTIVITY LOG (Circular Stack) ====================
void displayActivityLog() {
    if (logTop == -1) {
        cout << "\n>>> No activities logged!\n";
        return;
    }
    
    printTableHeader("RECENT ACTIVITIES (Last 10)", 80);
    
    int count = 0;
    int i = logTop;
    while (count < 10 && count <= logTop) {
        cout << "| " << activityLog[i] << "\n";
        i = (i - 1 + MAX_LOG) % MAX_LOG;
        count++;
        if (i == logTop) break;
    }
    printTableLine(80);
}

// ==================== SORTING - QUICK SORT (Best Algorithm) ====================
int partition(WoodSort arr[], int low, int high) {
    double pivot = arr[high].price;
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j].price < pivot) {
            i++;
            WoodSort temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    WoodSort temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;
}

void quickSortWood(WoodSort arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortWood(arr, low, pi - 1);
        quickSortWood(arr, pi + 1, high);
    }
}

void mergeSortWood(WoodSort arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        mergeSortWood(arr, left, mid);
        mergeSortWood(arr, mid + 1, right);
        
        // Merge
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        WoodSort L[500], R[500];
        
        for (int i = 0; i < n1; i++)
            L[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        
        while (i < n1 && j < n2) {
            if (L[i].price <= R[j].price) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }
        
        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }
        
        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }
}

void collectWoodForSorting(WoodType* root, WoodSort arr[], int &index) {
    if (root == NULL) return;
    
    collectWoodForSorting(root->left, arr, index);
    arr[index].id = root->id;
    arr[index].name = root->name;
    arr[index].price = root->basePrice;
    index++;
    collectWoodForSorting(root->right, arr, index);
}

void displaySortedWood(int sortType) {
    if (woodRoot == NULL) {
        cout << "\nNo wood types to sort!\n";
        return;
    }
    
    WoodSort arr[1000];
    int size = 0;
    collectWoodForSorting(woodRoot, arr, size);
    
    string method;
    if (sortType == 1) {
        quickSortWood(arr, 0, size - 1);
        method = "Quick Sort";
    } else {
        mergeSortWood(arr, 0, size - 1);
        method = "Merge Sort";
    }
    
    printTableHeader("WOOD SORTED BY PRICE - " + method, 70);
    cout << "| " << left << setw(5) << "ID" 
         << " | " << setw(25) << "Name" 
         << " | " << setw(15) << "Price" << "        |\n";
    printTableLine(70);
    
    for (int i = 0; i < size; i++) {
        cout << "| " << left << setw(5) << arr[i].id 
             << " | " << setw(25) << arr[i].name 
             << " | $" << setw(14) << fixed << setprecision(2) << arr[i].price << "        |\n";
    }
    printTableLine(70);
}

// ==================== FILE HANDLING ====================
void saveToFiles() {
    // Save Forests
    ofstream fFile("forests.txt");
    if (fFile.is_open()) {
        Forest* f = forestHead;
        while (f != NULL) {
            fFile << f->id << "|" << f->name << "|" << f->city << "|" << f->location << "\n";
            f = f->next;
        }
        fFile.close();
    }
    
    // Save Warehouses
    ofstream wFile("warehouses.txt");
    if (wFile.is_open()) {
        Warehouse* w = warehouseHead;
        while (w != NULL) {
            wFile << w->id << "|" << w->name << "|" << w->city << "|" 
                  << w->totalCapacity << "|" << w->usedCapacity << "\n";
            w = w->next;
        }
        wFile.close();
    }
    
    // Save Inventory
    ofstream iFile("inventory.txt");
    if (iFile.is_open()) {
        for (int i = 0; i < inventorySize; i++) {
            iFile << inventoryArray[i].id << "|" << inventoryArray[i].woodTypeId << "|" 
                  << inventoryArray[i].warehouseId << "|" << inventoryArray[i].quantity 
                  << "|" << inventoryArray[i].lastUpdated << "\n";
        }
        iFile.close();
    }
    
    cout << "\n>>> Data saved successfully!\n";
    logActivity("Data saved to files");
}

void loadFromFiles() {
    // Load Forests
    ifstream fFile("forests.txt");
    if (fFile.is_open()) {
        string line;
        while (getline(fFile, line)) {
            stringstream ss(line);
            string id, name, city, location;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, city, '|');
            getline(ss, location, '|');
            addForest(atoi(id.c_str()), name, city, location);
            forestCount = max(forestCount, atoi(id.c_str()));
        }
        fFile.close();
    }
    
    // Load Warehouses
    ifstream wFile("warehouses.txt");
    if (wFile.is_open()) {
        string line;
        while (getline(wFile, line)) {
            stringstream ss(line);
            string id, name, city, cap, used;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, city, '|');
            getline(ss, cap, '|');
            getline(ss, used, '|');
            addWarehouse(atoi(id.c_str()), name, city, atoi(cap.c_str()));
            Warehouse* w = findWarehouse(atoi(id.c_str()));
            if (w != NULL) w->usedCapacity = atoi(used.c_str());
            warehouseCount = max(warehouseCount, atoi(id.c_str()));
        }
        wFile.close();
    }
    
    // Load Inventory
    ifstream iFile("inventory.txt");
    if (iFile.is_open()) {
        string line;
        while (getline(iFile, line)) {
            stringstream ss(line);
            string id, wid, whid, qty, date;
            getline(ss, id, '|');
            getline(ss, wid, '|');
            getline(ss, whid, '|');
            getline(ss, qty, '|');
            getline(ss, date, '|');
            
            if (inventorySize < MAX_INVENTORY) {
                inventoryArray[inventorySize].id = atoi(id.c_str());
                inventoryArray[inventorySize].woodTypeId = atoi(wid.c_str());
                inventoryArray[inventorySize].warehouseId = atoi(whid.c_str());
                inventoryArray[inventorySize].quantity = atoi(qty.c_str());
                inventoryArray[inventorySize].lastUpdated = date;
                inventorySize++;
                inventoryCount = max(inventoryCount, atoi(id.c_str()));
            }
        }
        iFile.close();
    }
}

// ==================== MENU FUNCTIONS ====================

void forestManagementMenu() {
    while (true) {
        clearScreen();
        printTableHeader("FOREST MANAGEMENT", 60);
        cout << "| [1] Register New Forest                               |\n";
        cout << "| [2] View All Forests                                  |\n";
        cout << "| [3] Search Forest (Linear Search)                     |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 1) {
            string name, city, location;
            cout << "\nForest Name: "; getline(cin, name);
            cout << "City: "; getline(cin, city);
            cout << "Location: "; getline(cin, location);
            addForest(++forestCount, name, city, location);
            cout << "\n>>> Forest registered successfully!\n";
            pauseScreen();
        } else if (choice == 2) {
            displayForests();
            pauseScreen();
        } else if (choice == 3) {
            cout << "\nEnter Forest ID to search: ";
            int searchId;
            cin >> searchId;
            Forest* temp = forestHead;
            bool found = false;
            while (temp != NULL) {
                if (temp->id == searchId) {
                    cout << "\n>>> Forest Found!\n";
                    cout << "ID: " << temp->id << " | Name: " << temp->name 
                         << "\nCity: " << temp->city << " | Location: " << temp->location << "\n";
                    found = true;
                    break;
                }
                temp = temp->next;
            }
            if (!found) cout << "\n>>> Forest not found!\n";
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

void warehouseManagementMenu() {
    while (true) {
        clearScreen();
        printTableHeader("WAREHOUSE MANAGEMENT", 60);
        cout << "| [1] Add New Warehouse                                 |\n";
        cout << "| [2] View All Warehouses (Capacity Status)            |\n";
        cout << "| [3] Add Stock to Warehouse                            |\n";
        cout << "| [4] View Stock in Warehouse                           |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 1) {
            string name, city;
            int capacity;
            cout << "\nWarehouse Name: "; getline(cin, name);
            cout << "City: "; getline(cin, city);
            cout << "Total Capacity: "; cin >> capacity;
            addWarehouse(++warehouseCount, name, city, capacity);
            cout << "\n>>> Warehouse added successfully!\n";
            pauseScreen();
        } else if (choice == 2) {
            displayWarehouses();
            pauseScreen();
        } else if (choice == 3) {
            displayWarehouses();
            int whId, qty;
            string woodType;
            cout << "\nEnter Warehouse ID: "; cin >> whId;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Wood Type: "; getline(cin, woodType);
            cout << "Quantity: "; cin >> qty;
            
            Warehouse* wh = findWarehouse(whId);
            if (wh != NULL) {
                if (wh->usedCapacity + qty <= wh->totalCapacity) {
                    addStock(whId, woodType, qty);
                    cout << "\n>>> Stock added successfully!\n";
                } else {
                    cout << "\n>>> Not enough capacity! Available: " 
                         << (wh->totalCapacity - wh->usedCapacity) << "\n";
                }
            } else {
                cout << "\n>>> Warehouse not found!\n";
            }
            pauseScreen();
        } else if (choice == 4) {
            displayWarehouses();
            int whId;
            cout << "\nEnter Warehouse ID: "; cin >> whId;
            displayStockByWarehouse(whId);
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

void woodTypeMenu() {
    while (true) {
        clearScreen();
        printTableHeader("WOOD TYPE MANAGEMENT", 60);
        cout << "| [1] Add Wood Type (BST)                               |\n";
        cout << "| [2] View All Wood Types (Inorder)                     |\n";
        cout << "| [3] Search Wood Type (Binary Search)                  |\n";
        cout << "| [4] Sort by Price (Quick Sort - Best)                 |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 1) {
            string name, category, form, quality;
            int forestId;
            double price;
            cout << "\nWood Name: "; getline(cin, name);
            cout << "Category (Hardwood/Softwood): "; getline(cin, category);
            cout << "Form (Logs/Planks/Sheets): "; getline(cin, form);
            cout << "Quality Grade (Premium/Standard/Basic): "; getline(cin, quality);
            cout << "Forest ID: "; cin >> forestId;
            cout << "Base Price: "; cin >> price;
            woodRoot = insertWood(woodRoot, ++woodCount, name, category, form, quality, forestId, price);
            cout << "\nWood type added successfully!\n";
            pauseScreen();
        } else if (choice == 2) {
            displayWoodTypes();
            pauseScreen();
        } else if (choice == 3) {
            int searchId;
            cout << "\nEnter Wood Type ID: "; cin >> searchId;
            WoodType* result = searchWood(woodRoot, searchId);
            if (result != NULL) {
                cout << "\nWood Found!\n";
                cout << "ID: " << result->id << " | Name: " << result->name 
                     << "\nCategory: " << result->category << " | Price: $" << result->basePrice << "\n";
            } else {
                cout << "\nWood type not found!\n";
            }
            pauseScreen();
        } else if (choice == 4) {
            displaySortedWood(1);
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

void inventoryMenu() {
    while (true) {
        clearScreen();
        printTableHeader("INVENTORY MANAGEMENT", 60);
        cout << "| [1] Add Inventory Record                              |\n";
        cout << "| [2] View All Inventory                                |\n";
        cout << "| [3] Search Inventory (Linear Search)                  |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        
        if (choice == 1) {
            int woodId, whId, qty;
            cout << "\nWood Type ID: "; cin >> woodId;
            cout << "Warehouse ID: "; cin >> whId;
            cout << "Quantity: "; cin >> qty;
            addInventory(++inventoryCount, woodId, whId, qty);
            cout << "\n>>> Inventory record added!\n";
            pauseScreen();
        } else if (choice == 2) {
            displayInventory();
            pauseScreen();
        } else if (choice == 3) {
            int woodId;
            cout << "\nEnter Wood Type ID to search: "; cin >> woodId;
            int index = linearSearchInventory(woodId);
            if (index != -1) {
                cout << "\n>>> Inventory Found!\n";
                cout << "Wood ID: " << inventoryArray[index].woodTypeId 
                     << " | Warehouse: " << inventoryArray[index].warehouseId
                     << " | Quantity: " << inventoryArray[index].quantity << "\n";
            } else {
                cout << "\n>>> Inventory not found!\n";
            }
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

void pricingMenu() {
    while (true) {
        clearScreen();
        printTableHeader("PRICING & OFFERS (OWNER ONLY)", 60);
        cout << "| [1] View All Active Offers                            |\n";
        cout << "| [2] Add New Offer (Owner Only)                        |\n";
        cout << "| [3] Remove Latest Offer (Stack Pop)                   |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 1) {
            displayOffers();
            pauseScreen();
        } else if (choice == 2) {
            displayWoodTypes();
            int woodId;
            double discount;
            string desc, validUntil;
            cout << "\nWood Type ID: "; cin >> woodId;
            cout << "Discount Percentage: "; cin >> discount;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Description: "; getline(cin, desc);
            cout << "Valid Until (YYYY-MM-DD): "; getline(cin, validUntil);
            
            Offer newOffer;
            newOffer.id = ++offerCount;
            newOffer.woodTypeId = woodId;
            newOffer.discountPercent = discount;
            newOffer.description = desc;
            newOffer.validUntil = validUntil;
            
            pushOffer(newOffer);
            cout << "\n>>> Offer added successfully!\n";
            pauseScreen();
        } else if (choice == 3) {
            if (!isStackEmpty()) {
                stackTop--;
                cout << "\n>>> Latest offer removed!\n";
                logActivity("Offer removed from stack");
            } else {
                cout << "\n>>> No offers to remove!\n";
            }
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

void deliveryMenu() {
    while (true) {
        clearScreen();
        printTableHeader("DELIVERY & TRANSPORT SYSTEM", 60);
        cout << "| [1] View Available Routes                             |\n";
        cout << "| [2] Schedule Delivery (Enqueue)                       |\n";
        cout << "| [3] Process Next Delivery (Dequeue)                   |\n";
        cout << "| [4] View Pending Deliveries                           |\n";
        cout << "| [5] Calculate Shortest Path (Dijkstra)                |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 1) {
            displayAvailableRoutes();
            pauseScreen();
        } else if (choice == 2) {
            displayAvailableRoutes();
            int woodId, qty;
            string src, dest, date, transport;
            cout << "\nWood Type ID: "; cin >> woodId;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Source City: "; getline(cin, src);
            cout << "Destination City: "; getline(cin, dest);
            cout << "Delivery Date (YYYY-MM-DD): "; getline(cin, date);
            cout << "Transport Type (Truck/Train): "; getline(cin, transport);
            cout << "Quantity: "; cin >> qty;
            
            Delivery newDelivery;
            newDelivery.id = ++deliveryCount;
            newDelivery.woodTypeId = woodId;
            newDelivery.sourceCity = src;
            newDelivery.destCity = dest;
            newDelivery.deliveryDate = date;
            newDelivery.transportType = transport;
            newDelivery.quantity = qty;
            
            enqueueDelivery(newDelivery);
            cout << "\n>>> Delivery scheduled successfully!\n";
            pauseScreen();
        } else if (choice == 3) {
            processNextDelivery();
            pauseScreen();
        } else if (choice == 4) {
            displayPendingDeliveries();
            pauseScreen();
        } else if (choice == 5) {
            string src, dest;
            cout << "\nSource City: "; getline(cin, src);
            cout << "Destination City: "; getline(cin, dest);
            int distance = findShortestDistance(src, dest);
            if (distance != -1) {
                cout << "\n>>> Shortest Path Distance: " << distance << " km\n";
            } else {
                cout << "\n>>> Route not found!\n";
            }
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

void reportsMenu() {
    while (true) {
        clearScreen();
        printTableHeader("REPORTS & ANALYTICS", 60);
        cout << "| [1] View Activity Log (Circular Stack)               |\n";
        cout << "| [2] System Summary                                    |\n";
        cout << "| [3] Data Structure Statistics                         |\n";
        cout << "| [0] Back to Main Menu                                 |\n";
        printTableLine(60);
        
        int choice;
        cout << "Select option: ";
        cin >> choice;
        
        if (choice == 1) {
            displayActivityLog();
            pauseScreen();
        } else if (choice == 2) {
            printTableHeader("SYSTEM SUMMARY", 60);
            cout << "Total Forests          : " << forestCount << "\n";
            cout << "Total Warehouses       : " << warehouseCount << "\n";
            cout << "Total Wood Types       : " << woodCount << "\n";
            cout << "Total Inventory Records: " << inventorySize << "\n";
            cout << "Pending Deliveries     : ";
            if (isQueueEmpty()) cout << "0\n";
            else cout << ((queueRear - queueFront + MAX_QUEUE) % MAX_QUEUE + 1) << "\n";
            cout << "Active Offers          : " << (stackTop + 1) << "\n";
            printTableLine(60);
            pauseScreen();
        } else if (choice == 3) {
            printTableHeader("DATA STRUCTURE USAGE", 70);
            cout << "| Singly Linked List    | Forests                      |\n";
            cout << "| Doubly Linked List    | Warehouses                   |\n";
            cout << "| Circular Linked List  | Stock Management             |\n";
            cout << "| Binary Search Tree    | Wood Types                   |\n";
            cout << "| Array (Linear Search) | Inventory                    |\n";
            cout << "| Circular Queue        | Delivery System              |\n";
            cout << "| Stack (LIFO)          | Offers Management            |\n";
            cout << "| Circular Stack        | Activity Log                 |\n";
            cout << "| Graph (Dijkstra)      | City Routes                  |\n";
            cout << "| Bubble/Selection/     | Wood Sorting                 |\n";
            cout << "| Insertion Sort        |                              |\n";
            printTableLine(70);
            pauseScreen();
        } else if (choice == 0) {
            break;
        }
    }
}

// ==================== MAIN MENU ====================
void displayMainMenu() {
    printTableHeader("LOGAURA - WOOD SUPPLY CHAIN SYSTEM", 70);
    cout << "|                                                                      |\n";
    cout << "|            Smart | Organized | Reliable Wood Management              |\n";
    cout << "|                                                                      |\n";
    printTableLine(70);
    cout << "| [1] Forest Management         | [5] Pricing & Offers                 |\n";
    cout << "| [2] Warehouse Management      | [6] Delivery & Transport             |\n";
    cout << "| [3] Wood Types                | [7] Reports & Analytics              |\n";
    cout << "| [4] Inventory Management      | [8] Save All Data                    |\n";
    cout << "|                               | [0] Exit System                      |\n";
    printTableLine(70);
}

// ==================== MAIN FUNCTION ====================
int main() {
    initializeCityGraph();
    loadFromFiles();
    
    int choice;
    
    while (true) {
        clearScreen();
        displayMainMenu();
        cout << "\nSelect option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                forestManagementMenu();
                break;
            case 2:
                warehouseManagementMenu();
                break;
            case 3:
                woodTypeMenu();
                break;
            case 4:
                inventoryMenu();
                break;
            case 5:
                pricingMenu();
                break;
            case 6:
                deliveryMenu();
                break;
            case 7:
                reportsMenu();
                break;
            case 8:
                saveToFiles();
                pauseScreen();
                break;
            case 0:
                printTableHeader("THANK YOU FOR USING LOGAURA", 60);
                cout << "|           Smart Wood Management System                     |\n";
                printTableLine(60);
                return 0;
            default:
                cout << "\n>>> Invalid choice! Please try again.\n";
                pauseScreen();
        }
    }
    
    return 0;
}
