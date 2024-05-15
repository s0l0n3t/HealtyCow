import socket
import pymysql
import time
import adafruit_dht
import board
import telegram
import asyncio


TOKEN = "YOUR TOKEN"
chat_id = '338442260'
bot = telegram.Bot(token=TOKEN)
async def send_message(text, chat_id):
    async with bot:
        await bot.send_message(text=text, chat_id=chat_id)
connection = pymysql.connect(host="localhost", user="admin", passwd="root", database="healthcow")
cursor = connection.cursor()
cur = connection.cursor()
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = "192.168.1.103"
port = 8000
print (host)
print (port)
serversocket.bind((host, port))
dht_device = adafruit_dht.DHT11(board.D4)


serversocket.listen(5)
print ('server started and listening')
while 1:
    try:
        (clientsocket, address) = serversocket.accept()
        data = clientsocket.recv(1024).decode()
        splitdata = data.split(" ")
        temperature_c=dht_device.temperature
        humidity=dht_device.humidity
        thi_calc = ((0.8*temperature_c)+((humidity/100)*(temperature_c-14.4))+46.4)
        cur.execute(f"INSERT INTO data (device,status,temp,pulse,THI) VALUES ('{splitdata[0]}','{splitdata[1]}','{splitdata[2]}','{splitdata[3]}','{thi_calc}')")
        connection.commit()
        if thi_calc >= 72 and thi_calc < 79:
            async def main():
                await send_message(text=f"thi = {thi_calc}, orta düzey stres", chat_id=chat_id)
            asyncio.run(main())
        elif thi_calc >= 79 and thi_calc <= 89:
            async def main():
                await send_message(text=f"thi = {thi_calc}, şiddetli stres", chat_id=chat_id)
            asyncio.run(main())
        elif thi_calc >= 90 and thi_calc <= 98:
            async def main():
                await send_message(text=f"thi = {thi_calc}, çok şiddetli stres", chat_id=chat_id)
            asyncio.run(main())
        elif thi_calc > 98:
            async def main():
                await send_message(text=f"thi = {thi_calc}, ineğin ölümü.", chat_id=chat_id)
            asyncio.run(main())
    except Exception as err:
        print(err.args[0])
