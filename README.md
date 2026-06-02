# QtCustomTree

[English](#english) | [Русский](#русский)

---

## English
🌲 QtCustomTree
QtCustomTree — custom hierarchical tree widget for Qt5 with Windows-style multi-selection, dark theme, lazy loading and Drag & Drop.

✨ Features
🔹 Multi-selection — Shift+Click and Shift+↑↓ like in Windows Explorer

🔹 Drag & Drop — Ctrl+Drag to move items

🔹 Status aggregation — parent nodes show worst child status (alarm→warning→normal)

🔹 Lazy loading — child nodes load in chunks of 100

🔹 Dark theme — out of the box, with custom SVG arrows

🔹 Context menu — copy ID and name

🔹 Flexible data sources — via ITreeDataSource and ITreeNodeData interfaces

🚀 Quick Start
cpp
#include <QtCustomTree/QtCustomTree.h>

QtCustomTree *tree = new QtCustomTree();
tree->setDataSource(new MyDataSource());
tree->rebuild();
📦 Installation
bash
git clone https://github.com/svetaileshab/QtCustomTree.git
cd QtCustomTree
mkdir build && cd build
cmake ..
make
🎮 Demo
bash
./examples/demo
4 built-in data sources:

🏭 Devices & Sensors

📁 File System

👥 Organization Chart

🌍 Geo Locations

📋 Requirements
Qt 5.12+

CMake 3.16+

C++17

📄 License
MIT

## Русский
🌲 QtCustomTree
QtCustomTree — кастомный виджет иерархического дерева для Qt5 с множественным выделением в стиле Windows, тёмной темой, ленивой загрузкой и Drag & Drop.

✨ Возможности
🔹 Множественное выделение — Shift+Click и Shift+↑↓ как в проводнике

🔹 Drag & Drop — Ctrl+Drag для перетаскивания элементов

🔹 Агрегация статусов — родители показывают наихудший статус детей (alarm→warning→normal)

🔹 Ленивая загрузка — дочерние узлы подгружаются чанками по 100

🔹 Тёмная тема — из коробки, с кастомными SVG-стрелками

🔹 Контекстное меню — копирование ID и имени

🔹 Гибкие источники данных — через интерфейсы ITreeDataSource и ITreeNodeData

🚀 Быстрый старт
cpp
#include <QtCustomTree/QtCustomTree.h>

QtCustomTree *tree = new QtCustomTree();
tree->setDataSource(new MyDataSource());
tree->rebuild();
📦 Установка
bash
git clone https://github.com/svetaileshab/QtCustomTree.git
cd QtCustomTree
mkdir build && cd build
cmake ..
make
🎮 Демо
bash
./examples/demo
4 встроенных источника данных:

🏭 Устройства и датчики

📁 Файловая система

👥 Оргструктура

🌍 Гео-локации

📋 Требования
Qt 5.12+

CMake 3.16+

C++17

📄 Лицензия
MIT


EOF
