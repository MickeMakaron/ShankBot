#!/usr/bin/env python
from mechanize import Browser
from BeautifulSoup import BeautifulSoup
import urllib, os

# Only downloads creature gif:s from the List of Creatures.

mech = Browser()
url = "http://tibia.wikia.com/wiki/List_of_Creatures"
page = mech.open(url)
html = page.read()

soup = BeautifulSoup(html)

table = soup.find("table")
for row in table.findAll('tr')[1:]:
    col = row.findAll('td')
    name = col[0].a.string
    gif = col[1].a['href']

#Debug prints
#    record = (name, gif)
#    print record

#Saving file
    if "/wiki/Special:Upload" in gif:
        print "Not a valid gif in " + gif
    else:
        save_dir = "./downloaded-gifs"
        if not os.path.exists(save_dir):
            os.makedirs(save_dir)
        save_as = os.path.join(save_dir, name + ".gif")
        urllib.urlretrieve(gif, save_as)
        print "Downloaded %s gief" % name