#!/usr/bin/env python
import urllib2
from BeautifulSoup import BeautifulSoup
import re

# max file size 23MB
maxFileSize = 1024*1024*23

def http_download(url):
    try:
        request = urllib2.urlopen(url)
        data = request.read(maxFileSize)
        # if file too large
        if request.read(1) != '':
            return None
        else:
            return data
    except urllib2.HTTPError, e:
        print("urllib2.HttpError")
        exit()
    except urllib2.URLError, e:
        print("urllib2.URLError")
        exit()

def extract_urls(baseurl):
    response = http_download(baseurl)
    soup = BeautifulSoup(response)
    links = soup.findAll('a')[5:]
    return links

def download_all(baseurl):
    links = extract_urls(baseurl)
    filenames = list()
    for link in links:
        url = baseurl + "/" + link.get("href")
        print("Downloading " + url)
        data = http_download(url)
        if data == None:
            print("Skipping (file too large)")
            continue
        data_file_name = url[url.rfind('/')+1:]
        with open("./Datasets/" + data_file_name, "wb") as data_file:
            data_file.write(data)
        filenames.append(data_file_name)
    return filenames

def main():
    baseurl = "http://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/"
    download_all(baseurl)

if __name__ == "__main__":
    main()
