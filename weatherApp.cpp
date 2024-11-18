#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <curl/curl.h>

using namespace std;

vector<string> favoriteCities; // List of favorite cities

// Function to read the API key from a file
string loadApiKey() {
    ifstream file("apikey.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open apikey.txt. Make sure it exists in the program directory." << endl;
        exit(EXIT_FAILURE);
    }

    string apiKey;
    getline(file, apiKey); // Read the first line
    file.close();

    if (apiKey.empty()) {
        cerr << "Error: API key file is empty." << endl;
        exit(EXIT_FAILURE);
    }

    return apiKey;
}

// Menu display
void displayMenu() {
    cout << "\n1: Search weather by city\n"
         << "2: Add city to favorites\n"
         << "3: List favorite cities\n"
         << "4: Update favorites\n"
         << "5: Exit\n"
         << "Choose an option: ";
}

// cURL write callback
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userData) {
    userData->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Helper function to encode city names (e.g., spaces to %20)
string encodeCityName(const string& cityName) {
    string encoded;
    for (char c : cityName) {
        if (c == ' ') {
            encoded += "%20"; // Replace space with %20
        } else {
            encoded += c; // Keep other characters unchanged
        }
    }
    return encoded;
}

// Fetch data from API
string fetchWeatherData(const string& cityName, const string& apiKey) {
    CURL* curl = curl_easy_init();
    string response;

    if (curl) {
        string encodedCity = encodeCityName(cityName); // Encode city name for URL
        string url = "http://api.openweathermap.org/data/2.5/weather?q=" + encodedCity + "&appid=" + apiKey;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            return "Error fetching data. Check city name.";
        }
    }
    return response;
}

// Simplified weather extraction
string extractWeather(const string& data, const string& key) {
    size_t start = data.find("\"" + key + "\":");
    if (start == string::npos) return "N/A";
    start += key.length() + 3; // Skip to value
    size_t end = data.find(",", start);
    if (data[start] == '"') {
        start++;
        end = data.find("\"", start);
    }
    return data.substr(start, end - start);
}

// Display weather info
string getWeather(const string& cityName, const string& apiKey) {
    string data = fetchWeatherData(cityName, apiKey);
    if (data.empty()) return "No data available for " + cityName;

    string temp = extractWeather(data, "temp");
    string desc = extractWeather(data, "description");
    string humidity = extractWeather(data, "humidity");

    if (temp == "N/A") return "City not found.";
    double celsius = stod(temp) - 273.15;

    return "City: " + cityName + "\nTemp: " + to_string(celsius) + "C\nCondition: " + desc + "\nHumidity: " + humidity + "%";
}

// Add city to favorites
string addCity(const string& cityName) {
    if (favoriteCities.size() >= 3) return "Favorites are full!";
    favoriteCities.push_back(cityName);
    return cityName + " added!";
}

// List favorite cities
void listFavorites() {
    if (favoriteCities.empty()) {
        cout << "No favorites yet.\n";
        return;
    }
    for (size_t i = 0; i < favoriteCities.size(); i++) {
        cout << i + 1 << ": " << favoriteCities[i] << endl;
    }
}

// Update favorites
void updateFavorites() {
    if (favoriteCities.empty()) {
        cout << "No favorites to update.\n";
        return;
    }

    listFavorites();
    cout << "Enter number to update/remove: ";
    int choice;
    cin >> choice;
    if (choice < 1 || choice > (int)favoriteCities.size()) {
        cout << "Invalid choice.\n";
        return;
    }

    cout << "1: Update\n2: Remove\nChoose: ";
    int action;
    cin >> action;

    if (action == 1) {
        cout << "Enter new city name: ";
        string newCity;
        cin.ignore();
        getline(cin, newCity);
        favoriteCities[choice - 1] = newCity;
        cout << "Updated!\n";
    } else if (action == 2) {
        favoriteCities.erase(favoriteCities.begin() + choice - 1);
        cout << "Removed!\n";
    } else {
        cout << "Invalid action.\n";
    }
}

// Main program
int main() {
    string apiKey = loadApiKey(); // Load the API key from the file
    int choice;

    do {
        displayMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                cout << "Enter city: ";
                string city;
                getline(cin, city);
                cout << getWeather(city, apiKey) << endl;
                break;
            }
            case 2: {
                cout << "Enter city to add: ";
                string city;
                getline(cin, city);
                cout << addCity(city) << endl;
                break;
            }
            case 3:
                listFavorites();
                break;
            case 4:
                updateFavorites();
                break;
            case 5:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 5);

    return 0;
}
