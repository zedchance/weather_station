# Pulls data from Darksky API

import requests
from keys import darksky_key

# Fetch data
print("Fetching data...")
session = requests.session()
req = session.get("https://api.darksky.net/forecast/" +
                  darksky_key +
                  "/38.7521,-121.288")
if req.status_code == 404:
    print("404 code")
data = req.json()
current_temp = data['currently']['temperature']
current_humidity = data['currently']['humidity']
current_summary = data['currently']['summary']
print("Current temp:", current_temp)
print("Current humidity:", current_humidity * 100, "%")
print("Summary:", current_summary)

# Write to file
print("Writing to file current_conditions...")
file = open("current_conditions", "w")
file.write(f'{current_temp:.1f}F\n')
file.write(f'{current_humidity * 100}%\n')
file.write(f'{current_summary}\n')
file.close()
print("Done")
