/*
NAMED PLACES HASH TABLE IMPLEMENTATION
Currently deafult locations are set to: /home/subhajit/Desktop/named-places.txt, /home/subhajit/Desktop/states.txt
Different file location can be given in commandline
Description: Efficient hash table implementation for storing and retrieving geographic place data
Features: Dynamic resizing, closed addressing, polynomial hashing, and interactive query system
Author: Subhajit Halder
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>
#include <cstdlib>

using namespace std;

// Place class to represent each named place
class Place {
public:
    int code;
    string state;
    string name;
    int population;
    double area;
    double latitude;
    double longitude;
    int road_intersection;
    double distance;
    
    Place(int c, string s, string n, int p, double a, double lat, double lon, int ri, double d)
        : code(c), state(s), name(n), population(p), area(a), latitude(lat), longitude(lon), 
          road_intersection(ri), distance(d) {}
};

// Node for linked list in hash table
struct HashNode {
    Place* place;
    HashNode* next;
    
    HashNode(Place* p) : place(p), next(nullptr) {}
    ~HashNode() { delete place; }
};

// Hash Table class with dynamic resizing
class HashTable {
private:
    HashNode** table;
    int capacity;
    int size;
    const double LOAD_FACTOR = 0.75;
    
    // Polynomial hash function for strings
    int hashFunction(const string& key) const {
        const int prime = 31;
        long long hash = 0;
        for (char c : key) {
            hash = (hash * prime + c) % capacity;
        }
        return hash % capacity;
    }
    
    // Resize the hash table when load factor is exceeded
    void resize() {
        int oldCapacity = capacity;
        capacity *= 2;
        HashNode** newTable = new HashNode*[capacity]();
        
        // Rehash all elements
        for (int i = 0; i < oldCapacity; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* next = current->next;
                int newIndex = hashFunction(current->place->name);
                
                // Insert at front of new bucket
                current->next = newTable[newIndex];
                newTable[newIndex] = current;
                
                current = next;
            }
        }
        
        delete[] table;
        table = newTable;
    }
    
public:
    HashTable(int initialCapacity = 101) : capacity(initialCapacity), size(0) {
        table = new HashNode*[capacity]();
    }
    
    ~HashTable() {
        for (int i = 0; i < capacity; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] table;
    }
    
    // Insert a place into the hash table
    void insert(Place* place) {
        if ((double)size / capacity > LOAD_FACTOR) {
            resize();
        }
        
        int index = hashFunction(place->name);
        HashNode* newNode = new HashNode(place);
        
        // Insert at front of chain
        newNode->next = table[index];
        table[index] = newNode;
        size++;
    }
    
    // Find all places with given name
    vector<Place*> findByName(const string& name) const {
        vector<Place*> results;
        int index = hashFunction(name);
        HashNode* current = table[index];
        
        while (current != nullptr) {
            if (current->place->name == name) {
                results.push_back(current->place);
            }
            current = current->next;
        }
        
        return results;
    }
    
    // Find specific place by name and state
    Place* findByNameAndState(const string& name, const string& state) const {
        int index = hashFunction(name);
        HashNode* current = table[index];
        
        while (current != nullptr) {
            if (current->place->name == name && current->place->state == state) {
                return current->place;
            }
            current = current->next;
        }
        
        return nullptr;
    }
};

// Trim whitespace from both ends of string
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Parse a line from named-places.txt and create Place object
Place* parsePlaceLine(const string& line) {
    // Fixed-width format discovered from sample data
    // Positions: 0-7: code, 8-9: state, 10-59: name, 60-67: population, 
    // 68-77: area, 78-87: latitude, 88-97: longitude, 98-105: road intersection, 106-113: distance
    
    try {
        int code = stoi(line.substr(0, 8));
        string state = line.substr(8, 2);
        string name = trim(line.substr(10, 50));
        int population = stoi(trim(line.substr(60, 8)));
        double area = stod(trim(line.substr(68, 10)));
        double latitude = stod(trim(line.substr(78, 10)));
        double longitude = stod(trim(line.substr(88, 10)));
        int road_intersection = stoi(trim(line.substr(98, 8)));
        double distance = stod(trim(line.substr(106, 8)));
        
        return new Place(code, state, name, population, area, latitude, longitude, road_intersection, distance);
    } catch (const exception& e) {
        cerr << "Error parsing line: " << e.what() << endl;
        return nullptr;
    }
}

// Read state abbreviations and full names
vector<pair<string, string>> readStates(const string& filename) {
    vector<pair<string, string>> states;
    ifstream file(filename);
    string line;
    
    if (!file.is_open()) {
        cerr << "Warning: Could not open states file: " << filename << endl;
        return states;
    }
    
    while (getline(file, line)) {
        if (line.length() >= 4) { // Minimum: "AK Alaska"
            string abbr = line.substr(0, 2);
            string fullName = trim(line.substr(2));
            states.emplace_back(abbr, fullName);
        }
    }
    
    file.close();
    return states;
}

// Get full state name from abbreviation
string getStateFullName(const vector<pair<string, string>>& states, const string& abbr) {
    for (const auto& state : states) {
        if (state.first == abbr) {
            return state.second;
        }
    }
    return "Unknown State";
}

int main(int argc, char* argv[]) {
    // Set default file paths
    string placesFile = "/home/subhajit/Desktop/named-places.txt";
    string statesFile = "/home/subhajit/Desktop/states.txt";
    
    // Allow command line arguments to override defaults
    if (argc >= 3) {
        placesFile = argv[1];        
        statesFile = argv[2];
        cout<<"USING SPECIFIED FILES"<<endl;
    }
    else if(argc ==2)
    {
    	placesFile = argv[1]; 
        cout<<"USING SPECIFIED FILE for PLACES"<<endl;
    }
    else
    	cout<<"USING DEFAULT FILE(S)"<<endl;
    
    cout << "Using places file: " << placesFile << endl;
    cout << "Using states file: " << statesFile << endl;
    
    // Read state information
    vector<pair<string, string>> states = readStates(statesFile);
    cout << "Loaded " << states.size() << " states" << endl;
    
    // Create hash table and read places data
    HashTable hashTable;
    ifstream file(placesFile);
    string line;
    int count = 0;
    
    if (!file.is_open()) {
        cerr << "Error: Could not open places file: " << placesFile << endl;
        return 1;
    }
    
    cout << "Reading places data..." << endl;
    while (getline(file, line)) {
        if (!line.empty()) {
            Place* place = parsePlaceLine(line);
            if (place != nullptr) {
                hashTable.insert(place);
                count++;
            }
        }
    }
    file.close();
    
    cout << "Successfully loaded " << count << " places into hash table" << endl;
    
    // Interactive query loop
    string input;
    cout << "\nInteractive Query System (Enter Q to quit)" << endl;
    cout << "Commands:" << endl;
    cout << "  N placename - Find all states with this place name" << endl;
    cout << "  S placename state - Get detailed info for specific place" << endl;
    cout << "  Q - Quit" << endl;
    
    while (true) {
        cout << "\n> ";
        getline(cin, input);
        
        if (input.empty()) continue;
        
        // Quit command
        if (input[0] == 'Q' || input[0] == 'q') {
            break;
        }
        
        // Name search command
        else if (input[0] == 'N' || input[0] == 'n') {
            string placename = trim(input.substr(1));
            if (placename.empty()) {
                cout << "Error: Please provide a place name after N" << endl;
                continue;
            }
            
            vector<Place*> results = hashTable.findByName(placename);
            if (results.empty()) {
                cout << "No places found with name: " << placename << endl;
            } else {
                cout << "Found " << results.size() << " places with name '" << placename << "':" << endl;
                for (Place* place : results) {
                    string fullStateName = getStateFullName(states, place->state);
                    cout << "  " << place->state << " - " << fullStateName << endl;
                }
            }
        }
        
        // Specific place search command
        else if (input[0] == 'S' || input[0] == 's') {
            string rest = trim(input.substr(1));
            if (rest.empty()) {
                cout << "Error: Please provide place name and state after S" << endl;
                continue;
            }
            
            // Find the last space to separate placename and state
            size_t lastSpace = rest.find_last_of(' ');
            if (lastSpace == string::npos || lastSpace == 0 || lastSpace == rest.length() - 1) {
                cout << "Error: Format should be 'S placename state'" << endl;
                continue;
            }
            
            string placename = rest.substr(0, lastSpace);
            string state = rest.substr(lastSpace + 1);
            
            // Remove quotes if present
            if (placename.front() == '"' && placename.back() == '"') {
                placename = placename.substr(1, placename.length() - 2);
            }
            
            Place* place = hashTable.findByNameAndState(placename, state);
            if (place == nullptr) {
                cout << "Place not found: " << placename << ", " << state << endl;
            } else {
                string fullStateName = getStateFullName(states, place->state);
                cout << "Place Information:" << endl;
                cout << "  Name: " << place->name << endl;
                cout << "  State: " << place->state << " (" << fullStateName << ")" << endl;
                cout << "  Code: " << place->code << endl;
                cout << "  Population: " << place->population << endl;
                cout << "  Area: " << place->area << " sq units" << endl;
                cout << "  Latitude: " << place->latitude << endl;
                cout << "  Longitude: " << place->longitude << endl;
                cout << "  Road Intersection Code: " << place->road_intersection << endl;
                cout << "  Distance to Intersection: " << place->distance << " units" << endl;
            }
        }
        
        else {
            cout << "Unknown command. Use N, S, or Q." << endl;
        }
    }
    
    cout << "Goodbye!" << endl;
    return 0;
}
