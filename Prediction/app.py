from Prediction.data_analyzes import DataAnalyzes
from terra_brasilis import TerraBrasilis
from datetime import datetime

#    [
#        "2018-12-21T16:06:00.000Z",    Data e Hora
#       "NPP-375",                      Satelite
#       "Brasil",                       Pais
#       "CEARÁ",                        Estado do pais
#       "ARARIPE",                      Municipio
#       "Caatinga",                     Bioma
#       2,                              Dias sem chuva
#       1.7,                            Precipitação
#       0.2,                            Risco de Fogo
#       -7.18889,                       Latitude
#       -39.94255,                      Longitude
#       null,                           Area Industrial
#       2.8                             FRP
#   ]

def main():
    tbApi = TerraBrasilis()
    tbApi.initialize()
    #cookie = tbApi.getCookie()
    #print(f'cookie = {cookie}')
    #csrfCookie = tbApi.getCsrf()
    #print(f'csrf = {csrfCookie}')

    #tbApi.retrieveCities()     # Request and retrieve DB Queimadas data from last five years
    #tbApi.updateCurrentData()  # Request and retrieve DB Queimadas data from current year
    #tbApi.removeDuplicities()
    #tbApi.generateCustomData()

    dataAnalyzes = DataAnalyzes()
    dataAnalyzes.analyze()
    #dataAnalyzes.cityModels...
    #dataAnalyzes.totalCurrentYear...


print('start')
main()
print('finish')
