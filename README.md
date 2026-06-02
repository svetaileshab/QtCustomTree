# QtCustomTree

[English](#english) | [Русский](#русский)

---

## Русский

**QtCustomTree** — кастомное иерархическое дерево для Qt5 с Windows-подобным множественным выделением, тёмной темой, ленивой загрузкой и Drag & Drop.

### Возможности

- **Множественное выделение** — Shift+Click и Shift+↑↓ как в проводнике Windows
- **Drag & Drop** — Ctrl+Drag для перетаскивания элементов
- **Агрегация статусов** — родительские узлы показывают наихудший статус дочерних (alarm → warning → normal)
- **Ленивая загрузка** — дочерние узлы подгружаются чанками по 100 штук
- **Тёмная тема** — из коробки, с кастомными SVG-стрелками
- **Контекстное меню** — копирование ID и имени элемента
- **Гибкие источники данных** — через интерфейсы `ITreeDataSource` и `ITreeNodeData`

### Быстрый старт

```cpp
#include <QtCustomTree/QtCustomTree.h>

// 1. Создайте источник данных
class MyDataSource : public ITreeDataSource {
    // Реализуйте getRootNodes(), getChildren(), getItemData()
};

// 2. Создайте виджет
QtCustomTree *tree = new QtCustomTree();
tree->setDataSource(new MyDataSource());
tree->rebuild();
Демо
Соберите пример:

bash
mkdir build && cd build
cmake ..
make
./examples/demo
В демо доступно 4 тестовых источника данных:

🏭 Devices & Sensors

📁 File System

👥 Organization Structure

🌍 Geo Locations

Требования
Qt 5.12+

CMake 3.16+

C++17

Лицензия
MIT

English
QtCustomTree — a custom hierarchical tree widget for Qt5 with Windows-like multi-selection, dark theme, lazy loading and Drag & Drop.

Features
Multi-selection — Shift+Click and Shift+↑↓ like in Windows Explorer

Drag & Drop — Ctrl+Drag to move items

Status aggregation — parent nodes show the worst child status (alarm → warning → normal)

Lazy loading — child nodes load in chunks of 100

Dark theme — out of the box, with custom SVG arrows

Context menu — copy item ID and name

Flexible data sources — via ITreeDataSource and ITreeNodeData interfaces

Quick Start
cpp
#include <QtCustomTree/QtCustomTree.h>

// 1. Create a data source
class MyDataSource : public ITreeDataSource {
    // Implement getRootNodes(), getChildren(), getItemData()
};

// 2. Create the widget
QtCustomTree *tree = new QtCustomTree();
tree->setDataSource(new MyDataSource());
tree->rebuild();
Demo
Build the example:

bash
mkdir build && cd build
cmake ..
make
./examples/demo
The demo includes 4 test data sources:

🏭 Devices & Sensors

📁 File System

👥 Organization Structure

🌍 Geo Locations

Requirements
Qt 5.12+

CMake 3.16+

C++17

License
MIT
