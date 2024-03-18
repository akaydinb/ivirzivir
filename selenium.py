#!/usr/bin/python

import time
import smtplib

from email.mime.text import MIMEText
from selenium import webdriver
from selenium.webdriver.common.by import By
# from selenium.webdriver.common.keys import Keys
# key stroke gondermek icin gerekliydi. Gerek kalmadi
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.firefox_binary import FirefoxBinary


options = Options()
options.add_argument("--headless")
options.binary_location = "/home/bakaydin/Downloads/fLBAGM8dMtDyn10r/firefox";
# options.headless = True;
# binary = FirefoxBinary('/home/bakaydin/Downloads/fLBAGM8dMtDyn10r/firefox');

browser = webdriver.Firefox(options = options)

browser.get('https://www.auto-goetz.de/angebote/Skoda/Skoda-Karoq-Tour-DSG-AHK-Vorb-Navi/17641')

browser.find_element(By.ID, 'cookie-consent-deny-all').click()
time.sleep(2) # seconds

assert 'Skoda Karoq' in browser.title

# fiyat = browser.find_element(By.CLASS_NAME, 'leading-normal text-xl font-bold')
fiyat = browser.find_element(By.CSS_SELECTOR, "div[class='leading-normal text-xl font-bold']");

print(fiyat.text)

TEXT = MIMEText("Aracin bugunku fiyati  " + fiyat.text);
TEXT['Subject'] = "Fiyat bilgisi";
TEXT['From']  = 'script@allane.com'
TEXT['To']    = "bora.akaydin@allane.com"

server = smtplib.SMTP("localhost")
server.sendmail(TEXT['From'], TEXT['To'], TEXT.as_string());
server.quit();

browser.close()
browser.quit()
