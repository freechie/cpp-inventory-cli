// courseID: CIS165-001
// name: John Molina
// Prof. Wang
// Assignment #12 - Textbook Chapter 12,
// Programming Challenge #13 Inventory Program
// Due by 10/20/2021 11:55 PM

/* Steps
Write a program that uses a structure to store
the following inventory data in a file:

    Item Description
    Quantity on Hand
    Wholesale Cost
    Retail Cost
    Date Added to Inventory

The program should have a menu that allows
the user to perform the following tasks:

    • Add new records to the file.
    • Display any record in the file.
    • Change any record in the file.

Input Validation:
    The program should not accept quantities,
    or wholesale or retail costs, less than 0.
    The program should not accept dates
    that the programmer determines are unreasonable.
 */

#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <vector>

struct Date {
  int month, day, year;
};

struct Item {
  std::string ItemDescription;
  int quantityCost;
  float wholesale;
  float retailCost;
  Date dateAdded;
};

struct LoadResult {
  std::size_t loaded = 0;
  std::size_t skipped = 0;
};

bool isLeapYear(int year);
bool isValidDate(const Date &date);
std::optional<Date> parseDate(const std::string &text);
Item inputItem();
void outputItem(Item);
LoadResult readInventory(const std::string &fileName,
                         std::vector<Item> &inventory);
void writeInventory(const std::string &, const std::vector<Item> &);
int displayMenu();
void displayRecords(std::vector<Item>);
std::optional<int> readBoundedInteger(const std::string &prompt, int minimum,
                                      int maximum);
std::optional<std::size_t> selectRecordIndex(const std::vector<Item> &inventory,
                                             const std::string &prompt);
bool readConfirmation(const std::string &prompt) {
  while (true) {
    std::cout << prompt << " [y/n]: ";
    std::string text;

    if (!std::getline(std::cin, text)) {
      return false;
    }
    std::istringstream input(text);
    char response{};

    if (input >> response) {
      input >> std::ws;

      if (input.eof()) {
        if (response == 'y' || response == 'Y') {
          return true;
        }
        if (response == 'n' || response == 'N') {
          return false;
        }
      }
    }
    std::cout << "Invalid input. Enter y or n.\n";
  }
}

int main() {
  std::vector<Item> inventory;
  int menuOption;
  std::string fileName;
  const LoadResult loadResult = readInventory("Inventory.csv", inventory);

  std::cout << "Loaded " << loadResult.loaded << " items; skipped "
            << loadResult.skipped << ".\n";
  try {
    do {
      menuOption = displayMenu();
      switch (menuOption) {
      case 1: {
        inventory.push_back(inputItem());
      } break;

      case 2: {
        if (inventory.empty()) {
          std::cout << "No inventory records are available.\n";
        } else {
          displayRecords(inventory);
        }
      } break;

      case 3: {
        const std::optional<std::size_t> selectedIndex =
            selectRecordIndex(inventory, "Enter record number to view");

        if (selectedIndex.has_value()) {
          outputItem(inventory[selectedIndex.value()]);
        }
      } break;

      // Displays records to change if needed
      case 4: {
        const std::optional<std::size_t> selectedIndex =
            selectRecordIndex(inventory, "Enter record number to change");
        if (selectedIndex.has_value()) {
          inventory[selectedIndex.value()] = inputItem();
        }
      } break;

      case 5: {
        const std::optional<std::size_t> selectedIndex =
            selectRecordIndex(inventory, "Enter the record number to delete");

        if (selectedIndex.has_value()) {
          const std::size_t index = selectedIndex.value();
          outputItem(inventory[index]);

          if (readConfirmation("Delete this record?")) {
            inventory.erase(
                inventory.begin() +
                // std::ptrdiff_t is the signed type used for iterator distances
                // the index is safe to convert because selectRecordIndex()
                // already proved it is within the vector.
                static_cast<std::ptrdiff_t>(index));

            std::cout << "Record deleted.\n";
          } else {
            std::cout << "Record was not deleted.\n";
          }
        }
      } break;
      case 6: {
        writeInventory("Inventory.csv", inventory);
        std::cout << "Saved. Goodbye!" << std::endl;
      } break;
      }
    } while (menuOption != 6);
  } catch (const std::runtime_error &error) {
    std::cout << '\n' << error.what() << '\n';
    writeInventory("Inventory.csv", inventory);
    std::cout << "Saved. Goodbye!\n";
  }
  return 0;
}

bool isLeapYear(int year) {
  return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

bool isValidDate(const Date &date) {
  if (date.year < 1000 || date.year > 9999) {
    return false;
  }

  if (date.month < 1 || date.month > 12) {
    return false;
  }

  constexpr int daysPerMonth[] = {31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31};

  int maximumDay = daysPerMonth[date.month - 1];

  if (date.month == 2 && isLeapYear(date.year)) {
    maximumDay = 29;
  }

  return date.day >= 1 && date.day <= maximumDay;
}

std::optional<Date> parseDate(const std::string &text) {
  std::istringstream input(text);

  Date date{};
  char firstSlash{};
  char secondSlash{};

  if (!(input >> date.month >> firstSlash >> date.day >> secondSlash >>
        date.year)) {
    return std::nullopt;
  }

  input >> std::ws;

  if (!input.eof()) {
    return std::nullopt;
  }

  if (!isValidDate(date)) {
    return std::nullopt;
  }

  if (firstSlash != '/' || secondSlash != '/') {
    return std::nullopt;
  }
  return date;
}

void outputItem(Item output) {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "\nItemDescription :\t" << output.ItemDescription;
  std::cout << "\nDate            :\t";
  std::cout << output.dateAdded.month << "/" << output.dateAdded.day << "/"
            << output.dateAdded.year;
  std::cout << "\nQuantity on Hand:\t" << output.quantityCost;
  std::cout << "\nWholesale Cost  :\t$" << output.wholesale;
  std::cout << "\nRetail Cost     :\t$" << output.retailCost << std::endl
            << std::endl;
}

Item inputItem() {
  Item temp{};

  const auto readNonNegativeInteger = [](const std::string &prompt) -> int {
    while (true) {
      std::cout << prompt;

      std::string text;

      if (!std::getline(std::cin, text)) {
        throw std::runtime_error("Input ended while entering quantity");
      }

      std::istringstream input(text);
      int value{};

      if (input >> value) {
        input >> std::ws;

        if (input.eof() && value >= 0) {
          return value;
        }
      }

      std::cout << "Invalid input. Enter a non-negative number.\n";
    }
  };
  const auto readNonNegativePrice = [](const std::string &prompt) -> float {
    while (true) {
      std::cout << prompt;

      std::string text;

      if (!std::getline(std::cin, text)) {
        throw std::runtime_error("Input ended while entering price.");
      }

      std::istringstream input(text);
      float value{};

      if (input >> value) {
        input >> std::ws;
        if (input.eof() && value >= 0) {
          return value;
        }
      }
      std::cout << "invalid input. Enter a non-negative whole number.";
    }
  };

  std::cout << "Item Description: ";

  if (!std::getline(std::cin >> std::ws, temp.ItemDescription)) {
    throw std::runtime_error("Input ended while entering item description.");
  }

  while (temp.ItemDescription.empty()) {
    std::cout << "Description cannot be empty. Enter item description: ";

    if (!std::getline(std::cin, temp.ItemDescription)) {
      throw std::runtime_error("Input ended while entering item description.");
    }
  }

  while (true) {
    std::cout << "Date Added (MM/DD/YYYY): ";

    std::string dateText;

    if (!std::getline(std::cin, dateText)) {
      throw std::runtime_error("Input ended while entering date.");
    }

    std::optional<Date> parsedDate = parseDate(dateText);

    if (parsedDate.has_value()) {
      temp.dateAdded = parsedDate.value();
      break;
    }
    std::cout << "Invalid Date. Enter a real date such as 07/18/2026.\n";
  }

  temp.quantityCost = readNonNegativeInteger("Quantity on hand: ");
  temp.wholesale = readNonNegativePrice("Wholesale cost: $");
  temp.retailCost = readNonNegativePrice("Retail cost: $");

  return temp;
}

LoadResult readInventory(const std::string &fileName,
                         std::vector<Item> &inventory) {
  LoadResult result{};
  std::ifstream file(fileName);

  if (!file) {
    std::cout << "No existing inventory file found.\n";
    return result;
  }

  std::string line;
  getline(file, line);

  while (getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream row(line);
    Item item{};
    char separator;

    auto readComma = [&]() {
      return static_cast<bool>(row >> separator) && separator == ',';
    };

    if (!(row >> quoted(item.ItemDescription)) || !readComma() ||
        !(row >> item.quantityCost) || !readComma() ||
        !(row >> item.wholesale) || !readComma() || !(row >> item.retailCost) ||
        !readComma() || !(row >> item.dateAdded.month) || !readComma() ||
        !(row >> item.dateAdded.day) || !readComma() ||
        !(row >> item.dateAdded.year)) {
      std::cerr << "Skipping malformed inventory row: " << line << '\n';
      ++result.skipped;
      continue;
    }

    row >> std::ws;

    if (!row.eof() || item.ItemDescription.empty() || item.quantityCost < 0 ||
        !std::isfinite(item.wholesale) || item.wholesale < 0 ||
        !std::isfinite(item.retailCost) || item.retailCost < 0 ||
        !isValidDate(item.dateAdded)) {
      std::cerr << "Skipping invalid inventory row: " << line << '\n';
      ++result.skipped;
      continue;
    }

    inventory.push_back(item);
    ++result.loaded;
  }
  return result;
}

void writeInventory(const std::string &fileName,
                    const std::vector<Item> &inventory) {
  std::ofstream file(fileName, std::ios::trunc);

  if (!file) {
    std::cerr << "Unable to open " << fileName << " for writing.\n";
    return;
  }

  file << "description,quantity,wholesale,retail,month,day,year\n";
  file << std::fixed << std::setprecision(2);

  for (const Item &item : inventory) {
    file << quoted(item.ItemDescription) << ',' << item.quantityCost << ','
         << item.wholesale << ',' << item.retailCost << ','
         << item.dateAdded.month << ',' << item.dateAdded.day << ','
         << item.dateAdded.year << '\n';
  }
}

std::optional<int> readBoundedInteger(const std::string &prompt, int minimum,
                                      int maximum) {
  while (true) {
    std::cout << prompt;

    std::string text;

    if (!std::getline(std::cin, text)) {
      return std::nullopt;
    }

    std::istringstream input(text);
    int value{};

    if (input >> value) {
      input >> std::ws;

      if (input.eof() && value >= minimum && value <= maximum) {
        return value;
      }
    }
    std::cout << "invalid input. Enter a whole number from " << minimum
              << " to " << maximum << ".\n";
  }
}

std::optional<std::size_t> selectRecordIndex(const std::vector<Item> &inventory,
                                             const std::string &prompt) {
  if (inventory.empty()) {
    std::cout << "No inventory records are available.\n";
    return std::nullopt;
  }
  displayRecords(inventory);

  while (true) {
    std::cout << prompt << " (or c to cancel): ";

    std::string text;

    if (!std::getline(std::cin, text)) {
      return std::nullopt;
    }

    if (text == "c" || text == "C") {
      return std::nullopt;
    }
    std::istringstream input(text);
    std::size_t index{};

    if (input >> index) {
      input >> std::ws;

      if (input.eof() && index < inventory.size()) {
        return index;
      }
    }
    std::cout << "Invalid input. Enter a record number from 0 to "
              << inventory.size() - 1 << ", or c to cancel.\n";
  }
}

int displayMenu() {
  std::cout << "--------------------------------" << std::endl;
  std::cout << "1: Add new records to the file" << std::endl;
  std::cout << "2: List all records" << std::endl;
  std::cout << "3: Display any record in the file" << std::endl;
  std::cout << "4: Change any record in the file " << std::endl;
  std::cout << "5: Delete a record" << std::endl;
  std::cout << "6: Save & Exit" << std::endl;

  const std::optional<int> choice =
      readBoundedInteger("--------------------------------\n", 1, 6);

  if (!choice.has_value()) {
    std::cout << "\nInput ended. Saving and exiting.\n";
    return 6;
  }
  return choice.value();
}

void displayRecords(std::vector<Item> inventory) {
  std::cout << "Records: " << std::endl;

  for (std::size_t x = 0; x < inventory.size(); x++)
    std::cout << x << ": " << inventory[x].ItemDescription << std::endl;
}
