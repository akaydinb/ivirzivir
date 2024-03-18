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

# Geckodriver: 
# https://www.browserstack.com/guide/geckodriver-selenium-python
# https://www.browserstack.com/guide/run-selenium-tests-using-firefox-driver
# https://stackoverflow.com/questions/75677505/selenium-with-firefox-gecko-driver-on-aws-lambda-container-failed-to-read-mar

# RSS
# https://a-h.io/blog/website-to-rss/

# AWS Lambda
# https://stackoverflow.com/questions/66956447/firefox-selenium-on-aws-lambda-binary-is-not-a-firefox-executable
# https://stackoverflow.com/questions/52197632/what-is-the-difference-between-webdriver-firefox-and-webdriver-firefoxpath-t
# https://ftp.mozilla.org/pub/firefox/releases/

# TODO: Make it AWS lambda compatible
