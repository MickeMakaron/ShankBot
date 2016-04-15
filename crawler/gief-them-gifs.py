#!/usr/bin/env python
from mechanize import Browser
from BeautifulSoup import BeautifulSoup
import urllib, os

# Downloads images starting from the Images category on Tibia Wiki.

# Edit this to include more top level categories present in the image page root: http://tibia.wikia.com/wiki/Category:Images
top_level_image_categories = ["Object Images"];

mech = Browser()
base_url = "http://tibia.wikia.com"
page = mech.open(base_url + "/wiki/Category:Images")
html = page.read()

soup = BeautifulSoup(html)

# Downloads all giefs on the current category page
def download_giefs(cat_media, save_dir):
    for row in cat_media.findAll('div', 'gallerytext')[0:]:
        print "Following link to image page " + row.a['href']
        image_page = mech.open(base_url + row.a['href'])
        image_html = image_page.read()
        image_soup = BeautifulSoup(image_html)
        content = image_soup.find('div', id='mw-content-text')
        gif_a = content.find('a')
        gif_url = gif_a['href']
        gif_name = gif_a.img['data-image-name']

        if not os.path.exists(save_dir):
            os.makedirs(save_dir)
        save_as = os.path.join(save_dir, gif_name)
        urllib.urlretrieve(gif_url, save_as)
        print "Downloaded %s to %s" % (gif_name, save_dir)

# Recursively download all images from the subcategories listed in sub_cats_to_crawl, starting from the root page current_cat_soup.
def subcrawl(current_cat_soup, sub_cats_to_crawl, save_dir_base):
    sub_cat_div = current_cat_soup.find('div', id='mw-subcategories')
    if not sub_cat_div:
        return
    sub_cats  = sub_cat_div.findAll('a', 'CategoryTreeLabel')
    for sub_cat in sub_cats[0:]:
        if sub_cats_to_crawl:
            if not sub_cat.string in sub_cats_to_crawl:
                print "Will not traverse " + sub_cat.string
                continue
        print "Traversing " + sub_cat.string
        next_page = mech.open(base_url + sub_cat['href'])
        next_html = next_page.read()
        next_soup = BeautifulSoup(next_html)
        cat_media = next_soup.find('div', id='mw-category-media')
        # Download the gifs
        current_save_dir_base = save_dir_base + "/" + sub_cat.string
        download_giefs(cat_media, current_save_dir_base)
        # Go to next subdirectory
        subcrawl(next_soup, [], current_save_dir_base)

subcrawl(soup, top_level_image_categories, "./downloads")