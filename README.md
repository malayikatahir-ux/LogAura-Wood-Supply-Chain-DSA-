# 🌲 LogAura – Wood Supply Chain & Inventory Management System (DSA Project)

LogAura is a complete Data Structures based Wood Supply Chain Management System implemented in C++.  
It simulates real-world operations including forest registration, warehouse storage, wood catalog, inventory tracking, delivery routing, pricing offers, and activity logs.

The system is fully modular and uses multiple data structures to handle different components efficiently.

---

## 📌 Core Modules

### 🌳 Forest Management
- Stores forest data using **Singly Linked List**
- Tracks forest ID, name, city, and location

### 🏬 Warehouse Management
- Implemented using **Doubly Linked List**
- Tracks:
  - Total capacity
  - Used capacity
  - Available space

### 📦 Stock Management
- Uses **Circular Linked List**
- Stores warehouse-wise wood stock

### 🪵 Wood Catalog
- Implemented using **Binary Search Tree (BST)**
- Stores:
  - Wood type
  - Category (Hardwood / Softwood)
  - Form (Logs / Planks / Sheets)
  - Quality grade
  - Base price
  - Source forest

### 📊 Inventory System
- Implemented using **Static Array**
- Includes:
  - Wood ID
  - Warehouse ID
  - Quantity
  - Last updated timestamp
- Supports **Linear Search**

### 🚚 Delivery Management
- Implemented using **Circular Queue**
- Features:
  - Delivery scheduling
  - Route display
  - Delivery processing
  - Shortest path calculation

### 🎯 Offers & Discounts
- Implemented using **Stack**
- Shows:
  - Discount %
  - Validity date
  - Final price calculation

### 🧾 Activity Log
- Implemented using **Circular Array**
- Stores last 50 system activities with timestamps

### 🗺️ City Transport Graph
- Implemented using **Adjacency List (Graph)**
- Supports:
  - Route storage
  - Shortest distance calculation (Dijkstra-style logic)

### 🔃 Sorting
- **Quick Sort** used to sort wood types by price

### 💾 File Handling
Data persistence using:
- `forests.txt`
- `warehouses.txt`
- `inventory.txt`

---

## 🧠 Data Structures Used

| Module            | Data Structure Used        |
|-------------------|---------------------------|
Forest              | Singly Linked List         |
Warehouse           | Doubly Linked List         |
Stock               | Circular Linked List       |
Wood Types          | Binary Search Tree (BST)   |
Inventory           | Array + Linear Search      |
Deliveries          | Circular Queue             |
Offers              | Stack                      |
Activity Log        | Circular Array             |
City Routes         | Graph (Adjacency List)     |
Sorting             | Quick Sort                 |

---

## 🖥️ Features
- Menu driven console interface
- Table formatted output
- Real-time timestamps
- Capacity tracking
- Shortest delivery route calculation
- Price after discount calculation
- File save & load functionality

---
