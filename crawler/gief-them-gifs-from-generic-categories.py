#!/usr/bin/env python
from mechanize import Browser
from BeautifulSoup import BeautifulSoup
import urllib, os
import re

# Downloads images on generic categories on Tibia wiki.
#
# Interesting categories
# http://tibia.wikia.com/wiki/Category:Objects
# http://tibia.wikia.com/wiki/Category:Creatures
# http://tibia.wikia.com/wiki/Category:Items

# Edit this to include more top level categories present in the image page root: http://tibia.wikia.com/wiki/Category:Images
top_level_categories = ["Items"];
save_dir = "./downloads"

mech = Browser()
base_url = "http://tibia.wikia.com"

# Downloads all giefs on the current category page
def download_giefs(soup):
    pages = soup.find('div', id='mw-pages')
    table = pages.find('table')
    for page in table.findAll('a'):
        if "Template:" in page['href']:
            print "Disallowed by robots!" + page['href']
            continue
        specific_page = mech.open(base_url + page['href'])
        specific_html = specific_page.read()
        specific_soup = BeautifulSoup(specific_html)
        image_box = specific_soup.find('div', id='twbox-image')

        if not image_box:
            print "Found a non-standard page (" + page['href'] + "). It does not contain an image."
        elif not image_box.find('a'):
            print "Did not find any image link. " + page['href']
        elif "Special:Upload" in image_box.a['href']:
            print "No image available for " + image_box.a.string.replace("File:","").replace(".gif","")
        else:
            gif_url = image_box.a['href']
            gif_name = image_box.a.img['data-image-name']
            if not os.path.exists(save_dir):
                os.makedirs(save_dir)
            save_as = os.path.join(save_dir, gif_name)
            if os.path.isfile(save_as):
                print gif_name + " is already downloaded"
            else:
                urllib.urlretrieve(gif_url, save_as)
                print "Downloaded %s to %s" % (gif_name, save_dir)

for top_level_category in top_level_categories:
    print "In category " + top_level_category
    base_page = mech.open(base_url + "/wiki/Category:" + top_level_category)
    base_html = base_page.read()
    base_soup = BeautifulSoup(base_html)

    current_page = base_page
    current_html = base_html
    current_soup = base_soup
    while True:
        download_giefs(current_soup)
        next_url = "nop"
        pages = current_soup.find('div', id='mw-pages')
        for link in pages.findAll(href=re.compile('/wiki/Category:' + top_level_category)):
            if "next 200" in link.string:
                next_url = base_url + link['href']
                print "This link must be forward! " + next_url
                break
        if next_url == "nop":
            break
        current_page = mech.open(next_url)
        current_html = current_page.read()
        current_soup = BeautifulSoup(current_html)
