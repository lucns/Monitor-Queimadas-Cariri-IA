# Developed by @lucns

import os
import json
import urllib
import urllib.request
import urllib.parse
import time

MAX_HISTORIC = 96
DESKTOP_PATH = os.environ["HOMEPATH"] + '/Desktop'
DATA_PATH = "C:" + DESKTOP_PATH.replace("\\", "/") + "/Data"
#DATA_PATH = os. getcwd() + "/Data"
WEATHER_API_DATA = "WeatherApiData.json"

CITIES_COORDINATES = {
    'Salitre': '-7.285748,-40.457514', 'Campo Sales': '-7.075103,-40.372339', 'Araripe': '-7.211309,-40.138323',
    'Potengi': '-7.091934,-40.027603', 'Assaré': '-6.870889,-39.871030', 'Antonia do Norte': '-6.775348,-39.988188',
    'Tarrafas': '-6.684036,-39.758108', 'Altaneira': '-6.998939,-39.738878', 'Nova Olinda': '-7.092372,-39.678686',
    'Santana do Cariri': '-7.185914,-39.737159', 'Farias Brito': '-6.928198,-39.571438', 'Crato': '-7.231216,-39.410477',
    'Juazeiro do Norte': '-7.228166,-39.312093', 'Barbalha': '-7.288738,-39.299320', 'Jardim': '-7.586031,-39.279563',
    'Porteiras': '-7.534501,-39.116856', 'Penaforte': '-7.830278,-39.072340', 'Jati': '-7.688990,-39.005227',
    'Brejo Santo': '-7.488500,-38.987459', 'Abaiara': '-7.349389,-39.033383', 'Milagres': '-7.310940,-38.938627',
    'Mauriti': '-7.382958,-38.771900', 'Barro': '-7.174146,-38.779534', 'Caririaçu': '-7.042127,-39.285435',
    'Granjeiro': '-6.887292,-39.220469', 'Aurora': '-6.943031,-38.969761', 'Lavras da Mangabeira': '-6.752719,-38.965939',
    'Ipaumirim': '-6.789527,-38.718022', 'Baixio': '-6.730631,-38.716898', 'Umari': '-6.644247,-38.699599'
}

def verifyData():
    if not os.path.exists(WEATHER_API_DATA):
        raise Exception(f"{WEATHER_API_DATA} file not found!")

    apiDataFile = open(WEATHER_API_DATA, 'r', encoding='utf-8')
    jsonFile = json.load(apiDataFile)
    apiDataFile.close()
    
    for city in CITIES_COORDINATES.items():
        data = {'key': jsonFile['api_key'], 'q': CITIES_COORDINATES[city[0]], 'aqi': 'yes'}
        url = jsonFile['host'] + jsonFile['path'] + '?' + urllib.parse.urlencode(data)
        #print(f'Requesting in url: {url} City: {city} ', end='')
        print()
        print(f'Requesting data for: {city[0]}', end='')

        requestGet = urllib.request.Request(url, method='GET')
        try:
            response = urllib.request.urlopen(requestGet)
        except urllib.error.HTTPError as e:
            print(f'. HTTPError: {e.code}', end='')
        except urllib.error.URLError as e:
            print(f'. URLError: {e.reason}', end='')
        else: 
            result = response.read()
            jsonData = json.loads(result)
            
            print(f' from {jsonData['location']['region']}. Successful.', end='')

            if not os.path.exists(DATA_PATH):
                os.mkdir(DATA_PATH)

            current = jsonData['current']
            jsonItem = {
                'timestamp': current['last_updated_epoch'] * 1000, 'date_time': current['last_updated'], 'temperature': current['temp_c'], 'humidity': current['humidity'], 
                'precipitation': current['precip_mm'], 'cloud': current['cloud'], 'air_quality': current['air_quality']
            }

            filePath = DATA_PATH + "/" + city[0] + ".json"
            if os.path.exists(filePath):
                file = open(filePath, 'r', encoding="utf-8")
                jsonArray = json.loads(file.read())['data']
                jsonArray.append(jsonItem)
                removeRest(jsonArray)
                file.close()
            else:            
                jsonArray = [jsonItem]
            jsonObject = {'data': jsonArray}
            file = open(filePath, 'w', encoding="utf-8")
            json.dump(jsonObject, file, ensure_ascii=False, indent=4)
            file.close()

            # Enter AI here for odds <--- Insert AI code here 
            time.sleep(1)
    print() 
# End verifyData()

def removeRest(jsonArray):
    while len(jsonArray) > MAX_HISTORIC:
        jsonArray.pop(0)        
# End removeRest()

def longToString(timeCount):
    seconds = timeCount % 60
    if timeCount < 600:
        if seconds < 10:
            return '0' + str(int((timeCount - seconds) / 60)) + ':0' + str(seconds)
        else:
            return '0' + str(int((timeCount - seconds) / 60)) + ':' + str(seconds)
    else:
        if seconds < 10:
            return str(int((timeCount - seconds) / 60)) + ':0' + str(seconds)
        else:
            return str(int((timeCount - seconds) / 60)) + ':' + str(seconds)      
# End longToString()

def main():    
    UPDATE_TIME = 15 # time between verifications (minutes)
    print(DATA_PATH)

    while True:
        verifyData()
        count = int(UPDATE_TIME * 60)
        while count > 0:
            count -= 1
            time.sleep(1)
            print(f"Updating in {longToString(count)}...")
# End main()

print("start")
main()
print("finish")
