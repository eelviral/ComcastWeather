import requests
import json


def display_menu():
    """Simple function that displays the main menu options to the user."""
    
    print('Welcome to the Weather App!')
    print('Please choose one of the following options:')
    print('1: Search for the weather details of a city')
    print('2: Add a City to Favorites')
    print('3: List Favorite Cities')
    print('4: Update Favorite Cities')
    print('5: Exit')


favorite_cities = []


# Load API key from a secrets.json file
def load_api_key() -> str:
    """Loads the API key from a JSON file for secure API calls."""
    
    with open('secrets.json', 'r') as file:
        secrets = json.load(file)
    return secrets['api_key']


def search_city_weather(city_name: str) -> str:
    """Fetches and returns weather details for a specified city using the OpenWeather API."""
    
    # Grab the API key
    api_key = load_api_key()
    
    # Make the OpenWeather API call with the specified city
    url = f'http://api.openweathermap.org/data/2.5/weather?q={city_name}&appid={api_key}'
    response = requests.get(url)
    weather_data = response.json()

    # Weather details default to "not found"
    weather_details = f'Weather data not found for City name: {city_name}.'
    
    # Check if the response contains 'main' data
    if 'main' in weather_data:
        # Get temperature and convert from Kelvin to Celsius
        temp_kelvin = weather_data['main']['temp']
        temp_celsius = temp_kelvin - 273.15
        
        # Get other details
        weather_description = weather_data['weather'][0]['description']
        humidity = weather_data['main']['humidity']
        pressure = weather_data['main']['pressure']

        # Wind details if available
        wind_speed = weather_data['wind']['speed'] if 'wind' in weather_data else 'N/A'

        weather_details = (
            f'Weather for {city_name}:\n'
            f'Temperature: {temp_celsius:.2f}°C\n'
            f'Weather condition: {weather_description}\n'
            f'Humidity: {humidity}%\n'
            f'Pressure: {pressure} hPa\n'
            f'Wind Speed: {wind_speed} m/s'
        )
    return weather_details


def add_favorite_city(city_name: str) -> str:
    """Adds a new city to the favorite cities list if the limit has not been exceeded."""
    
    if len(favorite_cities) == 3:
        return 'Exceeded 3 Cities! Please update or remove a City to change your favorite\'s list.'
    else:
        favorite_cities.append(city_name)
        return f'City name added to favorites! {len(favorite_cities)} out of 3 cities now in favorite\'s list.'


def list_favorite_cities() -> str:
    """Returns a formatted string of all favorite cities."""
    
    # Check if there are no favorite cities
    if len(favorite_cities) == 0:
        return 'No favorite cities yet! Please add a city to the list.'
    else:
        # Return a string representation of favorite cities
        cities_list = '\n'.join([f"{index + 1}: {item}" for index, item in enumerate(favorite_cities)])
        return 'List of Favorite Cities:\n' + cities_list

def update_favorite_cities():
    """Allows the user to change or remove a city from the favorite cities list."""
    
    # Check if there are no favorite cities
    if len(favorite_cities) == 0:
        return 'No favorite cities yet! Please add a city to the list.'
    else:
        # Display current favorite cities
        print('Favorite Cities:')
        for index, city in enumerate(favorite_cities):
            print(f"{index + 1}: {city}")
        
        # Prompt user for update action.
        update_choice = input('Would you like to change a city (c) or remove a city (r)? ')
        
        # Handle change city request
        if update_choice.lower() == 'c':
            city_number = int(input('Enter the number of the city you want to change: '))
            if 1 <= city_number <= len(favorite_cities):  # Validate the city number
                new_city_name = input('Enter the new city name: ')
                favorite_cities[city_number - 1] = new_city_name  # Update the city name.
                return f'City number {city_number} has been updated to {new_city_name}.'
            else:
                return 'Invalid city number.'  # Error for invalid city number
        
        # Handle remove city request
        elif update_choice.lower() == 'r':
            city_number = int(input('Enter the number of the city you want to remove: '))
            if 1 <= city_number <= len(favorite_cities):  # Validate city number.
                removed_city = favorite_cities.pop(city_number - 1)  # Remove the city.
                return f'{removed_city} has been removed from favorites.'
            else:
                return 'Invalid city number.'  # Error for invalid city number
        
        # Handle invalid choice
        else:
            return 'Invalid choice. Please select change (c) or remove (r).'


if __name__ == '__main__':
    while True:
        display_menu()
        choice = input('Enter your choice (1-5): ')
        if choice == '5':
            print('\nExiting the interface.')
            break
        elif choice in ['1', '2']:
            city_name = input('Enter the city name: ')
            if choice == '1':
                print(f'\n{search_city_weather(city_name)}\n')
            elif choice == '2':
                print(f'\n{add_favorite_city(city_name)}\n')
        elif choice in ['3', '4']:
            if choice == '3':
                print(f'\n{list_favorite_cities()}\n')
            elif choice == '4':
                print(f'\n{update_favorite_cities()}\n')
        else:
            print('\nInvalid choice. Please select a valid option.\n')
            