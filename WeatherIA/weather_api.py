import json
import os
import time

from datetime import datetime
import pytz

from WeatherIA.api import Requester
from WeatherIA.constants import CITIES_COORDINATES, MAX_HISTORIC


class WeatherApi:

    def __init__(self):
        self.__url = f'https://api.weatherapi.com/'
        self.__requester = Requester()

    def now(self):
        return datetime.now(pytz.timezone('America/Fortaleza'))

    def update(self):
        arrayToSend = []
        for city in CITIES_COORDINATES.items():
            print(f'Requesting data for: {city[0]}', end=' ')
            retries = 0
            responseCode = 0
            while responseCode != 200:
                retries += 1
                if retries == 10:
                    #raise Exception(f'Fail request data city {city[0]}')
                    print(f'Error while trying request data from {city[0]}')
                    return
                url = f'{self.__url}v1/current.json?key=a8856d705d3b4b17b25151225240605&q={city[1]}&aqi=yes'
                content = self.__requester.requestGet(url)
                responseCode = self.__requester.getResponseCode()
                if responseCode == 200:
                    arrayToSend.append(self.retrieveContent(content, city))
                    print()
                    time.sleep(1)
                elif retries < 10:
                    time.sleep(1)
        if not os.path.exists('Release'):
            os.mkdir('Release')
        file = open('Release/WeatherData.json', 'w', encoding="utf-8")
        json.dump(arrayToSend, file, ensure_ascii=False, indent=4)
        file.close()

    def removeRest(self, jsonArray):
        while len(jsonArray) > MAX_HISTORIC:
            jsonArray.pop(0)

    def retrieveContent(self, content, city):
        jsonData = json.loads(content)
        print(f'from {jsonData['location']['region']}. Successful.', end=' ')

        if not os.path.exists('Data'):
            os.mkdir('Data')

        filePath = f'Data/{city[0]}.json'
        current = jsonData['current']
        jsonCity = {}
        if os.path.exists(filePath):
            file = open(filePath, 'r', encoding="utf-8")
            jsonCity = json.loads(file.read())
            file.close()

        daysWithoutRain = 0
        hadPrecipitation = None
        if current['precip_mm'] > 0:
            daysWithoutRain = 0
            hadPrecipitation = True
        else:
            if len(jsonCity) > 0:
                daysWithoutRain = jsonCity['days_without_rain']
                hadPrecipitation = jsonCity['had_precipitation']
            else:
                daysWithoutRain = 0
                hadPrecipitation = False

        jsonItem = {
            'city': city[0], 'timestamp': current['last_updated_epoch'], 'date_time': current['last_updated'], 'temperature': current['temp_c'], 'humidity': current['humidity'],
            'precipitation': current['precip_mm'], 'cloud': current['cloud'], 'carbon_monoxide': current['air_quality']['co'], 'days_without_rain': daysWithoutRain
        }

        if 'data' not in jsonCity:
            jsonCity['data'] = []
        jsonArray = jsonCity['data']
        jsonArray.append(jsonItem)
        self.removeRest(jsonArray)
        jsonCity['city'] = city[0]
        jsonCity['days_without_rain'] = daysWithoutRain
        jsonCity['had_precipitation'] = hadPrecipitation
        file = open(filePath, 'w', encoding="utf-8")
        json.dump(jsonCity, file, ensure_ascii=False, indent=4)
        file.close()

        return jsonItem

    def verifyDaysWithoutRain(self):
        for city in CITIES_COORDINATES.items():
            filePath = f'Data/{city[0]}.json'
            jsonCity = {}
            if os.path.exists(filePath):
                file = open(filePath, 'r', encoding="utf-8")
                jsonCity = json.loads(file.read())
                file.close()

            if not jsonCity['had_precipitation']:
                jsonCity['days_without_rain'] += 1
            file = open(filePath, 'w', encoding="utf-8")
            json.dump(jsonCity, file, ensure_ascii=False, indent=4)
            file.close()
        time.sleep(1)
