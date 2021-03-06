import gspread
from oauth2client.service_account import ServiceAccountCredentials
import time

scope = ['https://spreadsheets.google.com/feeds','https://www.googleapis.com/auth/drive']
creds = ServiceAccountCredentials.from_json_keyfile_name('Hackathon-dedfc974b240.json', scope)
client = gspread.authorize(creds)

sheet = client.open("ResourceHacking").worksheets()

for ii in sheet:
  print(ii.title)
  curr_time = time.localtime()
  time_str = time.strftime("%m/%d/%Y %H:%M:%S",curr_time)
  ii.append_row([time_str,22,50])
