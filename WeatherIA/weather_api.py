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

        current = jsonData['current']
        jsonObject = {}
        daysWithoutRain = 0
        filePath = f'Data/{city[0]}.json'
        if os.path.exists(filePath):
            file = open(filePath, 'r', encoding="utf-8")
            jsonObject = json.loads(file.read())
            file.close()
            daysWithoutRain = jsonObject['days_without_rain']

        now = self.now()
        if current['precip_mm'] > 0:
            daysWithoutRain = 0
        elif now.hour == 0 and now.minute < 15:
            if 'maximum_precipitation' in jsonObject:
                if jsonObject['maximum_precipitation'] == 0:
                    daysWithoutRain = jsonObject['days_without_rain'] + 1  # increment days without rain
        jsonItem = {
            'city': city[0], 'timestamp': current['last_updated_epoch'], 'date_time': current['last_updated'], 'temperature': current['temp_c'], 'humidity': current['humidity'],
            'precipitation': current['precip_mm'], 'cloud': current['cloud'], 'carbon_monoxide': current['air_quality']['co'], 'days_without_rain': daysWithoutRain
        }
        if os.path.exists(filePath):
            jsonArray = jsonObject['data']
            jsonArray.append(jsonItem)
            self.removeRest(jsonArray)
        else:
            jsonObject['data'] = [jsonItem]
            jsonObject['city'] = city[0]
        value = current['precip_mm']
        if 'maximum_precipitation' in jsonObject:
            value = jsonObject['maximum_precipitation']
        jsonObject['days_without_rain'] = daysWithoutRain
        jsonObject['maximum_precipitation'] = value
        file = open(filePath, 'w', encoding="utf-8")
        json.dump(jsonObject, file, ensure_ascii=False, indent=4)
        file.close()

        return jsonItem
