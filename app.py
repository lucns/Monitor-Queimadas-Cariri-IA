import time

from WeatherIA.weather_api import WeatherApi


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

def main():
    UPDATE_TIME = 15 # time between verifications (minutes)
    weather = WeatherApi()

    while True:
        weather.update()
        count = int(UPDATE_TIME * 60)
        while count > 0:
            count -= 1
            time.sleep(1)
            print(f"Updating in {longToString(count)}...")

print("start")
main()
print("finish")