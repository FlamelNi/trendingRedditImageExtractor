# trendingRedditImageExtractor
Webscrapper (with libcurl) that gathers currently trending wallpaper images from several subreddits and potentially store them or set them to the background. (Works for Windows only)

Unfortunately, this is Windows only program. When new image is downloaded, it automatically sets the image as background then it asks user if they want to save the image in separate folder. Since setting image as background was done with window specific functions, Mac machines cannot run this at all.

In future, Mac and other OS can be supported by using other OS specific commands.

This webscrapper use curl. So to compile, external library, "libcurl", must be included.

When user answers yes to "would you like to save this image?", it will copy the downloaded file to  a folder called "savedImage."


