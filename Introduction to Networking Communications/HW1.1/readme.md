# EE5516 HW1.1 A RESTfull Website

The source codes are adopted from this tutorial <https://closebrace.com/tutorials/2017-03-02/creating-a-simple-restful-web-app-with-nodejs-express-and-mongodb>.

Since the source codes are already well documented, I am going to write my understanding of the codes. I will explain some important folders and fils in the directory.

* package.json

    This files specifies what packages this website will use. Among the packages list in the file, we are most interested in Mongodb which is used as the backend databse to store user information.

* app.js

    We define some environment varibles, configure the backend database connection and error handler in this file.
* bin
    * www

        The listening port of the website (3000) is specified in this file.
* public
    * javascripts

        * global.js
            
            This file defines many functions that will run in the client side, and links UI elements such as buttons to their callback functions. It also uses ajax to post/load user data dynamiclly.

* router
    * index.js

        This file defines the pages of the website that are going to be exposed. It handles page request and renders the corresponding page for the request. In my example, there is only a default home page.
    
    * users.js

        This file has some functions that will run at the server side. In my example, functions querying the database are defined here.

* views
    * layout.jade

        Layout that are supposed to be shared by multiple pages.

    * index.jade

        The raw homepage is defined in this file.

    * error.jade

        The error page.

The following picture is a screen shot of the website:

[![FQJ98s.md.png](https://s1.ax1x.com/2018/12/04/FQJ98s.md.png)](https://imgchr.com/i/FQJ98s)








