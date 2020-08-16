import io
import os
import telepot
from time import *
from picamera import Picamera
import random
import datetime
cam = Picamera() 

os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = 'API KEY'
# Imports the Google Cloud client library
from google.cloud import vision
from google.cloud.vision import types
# Instantiates a client
client = vision.ImageAnnotatorClient()

​def image_content():
    content_list = []
    # The name of the image file to annotate
    file_name = os.path.join(
        os.path.dirname(__file__),
        'image1.jpg')
   # Loads the image into memory
    with io.open(file_name, 'rb') as image_file:
        content = image_file.read()
   image = types.Image(content=content)
   # Performs label detection on the image file
    response = client.label_detection(image=image)
    labels = response.label_annotations
   print('Labels:')
    for label in labels:
        print(label.description)
       # append to the list
        content_list.append(label.description)

def take_a_photo():
    #camera = PiCamera()
    camera.resolution = (1024, 768)
    #maybe resize?
    #camera.start_preview()
    #sleep(2)
    os.system("mpg321 shutter.mp3")
    sleep(0.5)
    camera.capture('image1.jpg')
    camera.stop_preview()
    #return ('image1.jpg')

def on(pin):
	return
def off(pin):
	return

def handle(msg):
    chat_id = msg['chat']['id']
    command = msg['text']

    print ('Got command: %s' % command)

    if command == 'Hey':
       bot.sendMessage(chat_id, "Hello")
    elif command =='Capture':
       take_a_photo()
       bot.sendMessage(chat_id, "Captured")
       image_content()
    elif command == 'Fill water':
       #on(17)
       bot.sendMessage(chat_id, "Filling")

bot = telepot.Bot('API Key')
bot.message_loop(handle)
print ('I am listening...')

while 1:
     time.sleep(10)

