import urllib2

req = urllib2.Request('https://www.baileyofsheffield.com/shop/cable-bracelets/stainless-steel')
response = urllib2.urlopen(req)
the_page = response.read()
with open('D:\\Downloads\\assignments\\Principles of Data Management\\project\\output.txt', 'w') as f:
    f.write(the_page)