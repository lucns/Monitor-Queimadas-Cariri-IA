import json
import os
from datetime import datetime
from Prediction.constants import VERIFY_YEARS_COUNT
from city import CityModel
import numpy as np
from sklearn.linear_model import LinearRegression

class DataAnalyzes:
    def __init__(self):
        self.totalChapadaAraripe = 0        # Total de focos da chapada do araripe nos ultimos VERIFY_YEARS_COUNT anos
        self.totalCurrentYear = 0           # Total do ano atual
        self.predictCurrentYear = 0         # Media anual final
        self.predictChapadaAraripe = []     # Previsao de queimadas em cada mes, para a chapada do araripe
        self.occurredChapadaAraripe = []    # Total de focos ocorridos no ano atual, por mes
        self.annualTotalOccurred = {}       # Total de focos ocorridos por ano
        self.cityModels = {}                # Dicionario de cidades

    def analyze(self):
        self.occurredChapadaAraripe.clear()
        for i in range(0, 12): self.occurredChapadaAraripe.append(0)

        self.totalCurrentYear = 0
        currentYear = datetime.now().year
        for year in range(currentYear - VERIFY_YEARS_COUNT, currentYear + 1):
            if year < currentYear:
                self.annualTotalOccurred[year] = 0
            for fileName in os.listdir(f'Filtered/{year}'): # fileName Eg:= Salitre.json
                filePath = f'Filtered/{year}/{fileName}'

                filtered = open(filePath, 'r', encoding="utf-8")
                array = json.loads(filtered.read())#['data']
                filtered.close()

                cityName = fileName[0:fileName.rfind('.')]
                cityModel = CityModel(cityName)
                if cityName in self.cityModels:
                    cityModel = self.cityModels[cityName]
                self.cityModels[cityName] = cityModel

                for arrayFire in array:
                    date = arrayFire[0].split('T')[0]
                    segments = date.split('-')
                    cityModel.putFiresData(segments[1], segments[0])
                    if currentYear == int(segments[0]):
                        self.totalCurrentYear += 1
                    else:
                        self.totalChapadaAraripe += 1

        for cityName in self.cityModels:
            cityModel = self.cityModels[cityName]
            cityModel.calculateMonthlyAverage()
            print(cityModel.name)

            for y in self.annualTotalOccurred:
                if y < currentYear:
                    self.annualTotalOccurred[y] += self.cityModels[cityName].totalPerYears[str(y)]

            count = 0
            for items in cityModel.years.items():
                months = items[1] # 0 = 2022, 1 = [23, 45, 45,...]
                totalPerYears = cityModel.totalPerYears[items[0]]
                if currentYear != int(items[0]):
                    count += totalPerYears
                else:
                    for i in range(0, 12):
                        self.occurredChapadaAraripe[i] += months[i]
                print(f'{items[0]} -> {months} total: {totalPerYears}')
            print(f'Media -> {cityModel.monthlyAverage}')
            print(f'Media anual da cidade sem contar o atual ano -> {count / VERIFY_YEARS_COUNT}')
            print()

        years = {}
        for year in range(currentYear - VERIFY_YEARS_COUNT, currentYear + 1):
            if year == currentYear:
                break
            years[year] = []
            for month in range(0, 12):
                years[year].append(0)
            for cityName in self.cityModels:
                for month in range(0, 12):
                    years[year][month] += self.cityModels[cityName].years[str(year)][month]
        base = []
        for month in range(0, 12):
            base.clear()
            for year in years:
                base.append(years[year][month])
            self.predictChapadaAraripe.append(self.predictNextNumber(base))

        print(f'________________________________________________________________________________________')
        print(f'|\tAno\t\t|\tTotal\t|\tMeses')
        base.clear()
        count = 0
        for y in self.annualTotalOccurred:
            count += self.annualTotalOccurred[y]
            if y != str(currentYear):
                base.append(self.annualTotalOccurred[y])
            print(f'|\t{y}\t|\t{self.annualTotalOccurred[y]}\t|\t{years[y]}')

        self.predictCurrentYear = self.predictNextNumber(base)
        print(f'|\t{currentYear}\t|\t{self.totalCurrentYear}\t\t|\t', end='')
        print('[', end='')
        jsonMonths = []
        for i in range(0, 12):
            jsonMonth = {}
            jsonMonth['number'] = i + 1
            if i + 1 > datetime.now().month:
                jsonMonth['prediction'] = True
                jsonMonth['fires'] = self.predictChapadaAraripe[i]
                print(f'~{self.predictChapadaAraripe[i]}', end='')
            else:
                jsonMonth['prediction'] = False
                jsonMonth['fires'] = self.occurredChapadaAraripe[i]
                print(f'{self.occurredChapadaAraripe[i]}', end='')
            if i < 11:
                print(', ', end='')
            jsonMonths.append(jsonMonth)
        print(']')
        print(f'_________________________________________________________________________________________')
        print()

        jsonObject = {}
        jsonObject['prediction_total'] = self.predictCurrentYear
        jsonObject['occurred_total'] = self.totalCurrentYear
        jsonObject['months'] = jsonMonths

        if not os.path.exists('Release'):
            os.mkdir('Release')
        file = open('Release/FiresPrediction.json', 'w', encoding="utf-8")
        json.dump(jsonObject, file, ensure_ascii=False, indent=4)
        file.close()

        #print(f'Total ocorrido nos ultimos {VERIFY_YEARS_COUNT} anos -> {self.totalChapadaAraripe}')
        print('----------------- PREVISTOS ------------------')
        print(f'PREVISTO MENSAL PARA ESSE ANO -> {self.predictChapadaAraripe}')
        print(f'PREVISTO TOTAL PARA ESSE ANO -> {self.predictCurrentYear}')

    def predictNextNumber(self, sequence):
        sequence = np.array(sequence)
        X = np.arange(len(sequence)).reshape(-1, 1)
        y = sequence

        model = LinearRegression()
        model.fit(X, y)

        next_index = len(sequence)
        next_value = model.predict([[next_index]])
        return int(next_value[0])