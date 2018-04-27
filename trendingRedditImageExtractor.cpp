

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <curl/curl.h>

#include <windows.h>
#include <direct.h>

using namespace std;

int writer(char *data, size_t size, size_t nmemb, string *buffer);
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

char showMenu();

int getLink(string&);
string getFileExtension(string);

void downloadFile(string);
void setBackground(string);

bool copyFile(const char *, const char*, bool);

void singleLink(string);
void runThrough();

string links[] = 
{
  "https://www.reddit.com/r/wallpaper/",
  "https://www.reddit.com/r/wallpapers/",
  "https://www.reddit.com/r/naturepics/",
  "https://www.reddit.com/r/EarthPorn/",
  "https://www.reddit.com/r/spaceporn/"
};

int main()
{
  
  string userURL = "";
  
  //TODO:universal coding
  
  char answer = showMenu();
  
  if(answer == '1')
  {
    cout << "Please write only URL of subreddits dedicated to images posts." << endl;
    cout << "Currently, this program only supports images that are directly linked images." << endl;
    cout << "What is the name of website?" << endl;
    cout << "[URL]:";
    cin >> userURL;
    cin.ignore();
    cout << endl;
  }
  else if(answer == '2')
  {
    runThrough();
  }
  else
  {
    switch(answer)
    {
      case('3'): userURL = "https://www.reddit.com/r/wallpaper/"; break;
      case('4'): userURL = "https://www.reddit.com/r/wallpapers/"; break;
      case('5'): userURL = "https://www.reddit.com/r/naturepics/"; break;
      case('6'): userURL = "https://www.reddit.com/r/EarthPorn/"; break;
      case('7'): userURL = "https://www.reddit.com/r/spaceporn/"; break;
      default: cout << "Exiting program..." << endl; system("pause"); return 0;
    }
    
    singleLink(userURL);
  }
  
  
  
}

int writer(char *data, size_t size, size_t nmemb, string *buffer)
{
  int result = 0;
  if(buffer != NULL)
  {
    buffer -> append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
  
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t written;
  written = fwrite(ptr, size, nmemb, stream);
  return written;
}

char showMenu()
{
  
  cout << "WARNING: If you are running this program in non-Windows operating system, exit this program by entering 'q'. "
       << "Currently this program only works for Windows." << endl;
       
  cout << "This program will use HTTP Request from Reddit to get its HTML code. "
       << "And extract/download currently most trending image and potentially set it to wallpaper." << endl << endl;
       
  cout << "Please place this program to its own separate files if possible. "
       << "If you have not, please enter 'q' to exit right now. "
       << "Otherwise, please enter 'a'."<< endl;
       
  cout << "[q to exit]:";
  char answer;
  cin >> answer;
  cin.ignore();
  cout << endl;
  
  if(answer == 'q')
    return 0;
  
  cout << "From which website would you like to extract? Please enter the number." << endl << endl;
  
  cout << "1. I will choose my own" << endl;
  cout << "2. Run through all of these" << endl;
  cout << "3. https://www.reddit.com/r/wallpaper/" << endl;
  cout << "4. https://www.reddit.com/r/wallpapers/" << endl;
  cout << "5. https://www.reddit.com/r/naturepics/" << endl;
  cout << "6. https://www.reddit.com/r/EarthPorn/" << endl;
  cout << "7. https://www.reddit.com/r/spaceporn/" << endl;
  cout << "8. Exit program" << endl;
  cout << endl;
  
  cout << "[number]:";
  cin >> answer;
  cin.ignore();
  cout << endl;
  return answer;
}

int getLink(string& userURL)
{
  
  CURL *curl;
  CURLcode res;
  
  string buffer = "";
  string imageFile = "";
  
  curl = curl_easy_init();
  if(curl)
  {
    
    curl_easy_setopt(curl, CURLOPT_URL, userURL.c_str());
    // /* example.com is redirected, so we tell libcurl to follow redirection */ 
    // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &buffer);
    
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    
    /* always cleanup */ 
    curl_easy_cleanup(curl);
    
    ofstream fout;
    fout.open("log.txt");
    fout << buffer << endl;
    fout.close();
    
    int highestScore = 0;
    int highestIndex = 0;
    int next = 0;
    for(int i = 0; i < 6; i++)
    {
      int beg = buffer.find("data-score=\"", (size_t)next);
      int end = buffer.find("\"", beg+strlen("data-score=\""));
      int tempScore = atoi(buffer.substr(beg+strlen("data-score=\""),end-(beg+strlen("data-score=\""))).c_str());
      if(tempScore > highestScore)
      {
        highestScore = tempScore;
        highestIndex = beg;
      }
      next = end;
    }
    
    int index     = buffer.rfind("data-url=\"", (size_t)highestIndex);
    int indexEnd  = buffer.find("\"", (size_t)index+strlen("data-url=\""));
    
    imageFile = buffer.substr(index+strlen("data-url=\""), indexEnd - (index+strlen("data-url=\"")));
    userURL = imageFile;
    return highestScore;
  }
  return 0;
}

string getFileExtension(string link)
{
  string fileExtension = "";
  fileExtension = link.substr(link.rfind(".")+1, 3);
  return fileExtension;
}

void downloadFile(string imageFile)
{
  CURL *curl;
  CURLcode res;
  string userURL = imageFile;
  
  string outputFileName = "image." + getFileExtension(imageFile);
  FILE *fp;
  curl = curl_easy_init();
  if(curl)
  {
    
    fp = fopen(outputFileName.c_str(), "wb");
    
    curl_easy_setopt(curl, CURLOPT_URL, userURL.c_str());
    // /* example.com is redirected, so we tell libcurl to follow redirection */ 
    // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     fp);
    
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    
    /* always cleanup */ 
    curl_easy_cleanup(curl);
    
    fclose(fp);
    
  }
  
}

void setBackground(string outputFileName)
{
  char cCurrentPath[FILENAME_MAX];

  if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
  {
    return;
    // return errno;
  }

  cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
  
  string imageFileWithPath = cCurrentPath;
  imageFileWithPath = cCurrentPath;
  imageFileWithPath = imageFileWithPath + "/" + outputFileName;
  
  SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, (PVOID)imageFileWithPath.c_str(), SPIF_UPDATEINIFILE );
  
}

bool copyFile(const char *SRC, const char* DEST, bool overridePermission = false)
{
  if(!overridePermission)
  {
    ifstream fin;
    fin.open(DEST, std::ios::binary);
    if(fin.good())
    {
      fin.close();
      return false;
    }
    fin.close();
  }
  std::ifstream src(SRC, std::ios::binary);
  std::ofstream dest(DEST, std::ios::binary);
  dest << src.rdbuf();
  bool returnValue = src && dest;
  src.close();
  dest.close();
  return src && dest;
}

void singleLink(string userURL)
{
  int highestScore = getLink(userURL);
  
  string imageFile = userURL;
  
  if(highestScore == 0 || imageFile == "" || getFileExtension(imageFile) == "com")
  {
    cout << "No image found" << endl;
    return;
  }
  else
  {
    cout << "Downloading currently trending image from " << userURL << ":" << endl;
    cout << imageFile << endl;
    cout << "This image received " << highestScore << " scores. Wow!" << endl << endl;
  }
  
  downloadFile(imageFile);
  
  char answer;
  cout << "Would you like to set this as wallpaper?" << endl;
  cout << "CATIOUS: ONLY Windows machine may operate this function. "
       << "The creator of this program is not responsible for result from this feature ran from non-windows machine.^^" << endl;
  cout << "[y/n]:";
  cin >> answer;
  cin.ignore();
  
  if(answer != 'y')
  {
    return;
  }
  string imageFileName = "image." + getFileExtension(imageFile);
  setBackground(imageFileName);
  
  
}

void runThrough()
{
  
  char answer;
  cout << "Would you like to change wallpaper?" << endl;
  cout << "CATIOUS: ONLY Windows machine may operate this function. "
       << "The creator of this program is not responsible for result from this feature ran from non-windows machine.^^" << endl;
  cout << "[y/n]:";
  cin >> answer;
  cin.ignore();
  
  if(answer != 'y')
  {
    return;
  }
  
  string mainPic = "";
  
  for(string userURL:links)
  {
    int highestScore = getLink(userURL);
    
    string imageFile = userURL;
    
    if(highestScore == 0 || imageFile == "" || getFileExtension(imageFile) == "com")
    {
      cout << "No image found" << endl;
      continue;
    }
    else
    {
      cout << "Downloading currently trending image from " << userURL << ":" << endl;
      cout << imageFile << endl;
      cout << "This image received " << highestScore << " scores. Wow!" << endl << endl;
    }
    
    downloadFile(imageFile);
    string imageFileName = "image." + getFileExtension(imageFile);
    setBackground(imageFileName);
    
    cout << "Would you like to keep this image?" << endl;
    cout << "[y/n]:";
    cin >> answer;
    cin.ignore();
    cout << endl;
    
    if(answer == 'y')
    {
      string name = "";
      cout << "Enter the name of this image(without file extension)" << endl;
      cout << "[Name]:";
      getline(cin, name);
      cout << endl;
      
      while(true)
      {
        
        
        if( copyFile( ("image."+getFileExtension(imageFile)).c_str(), ("savedImage/"+name+"."+getFileExtension(imageFile)).c_str()) )
        {
          mainPic = ("savedImage/"+name+"."+getFileExtension(imageFile));
          break;
        }
        
        cout << "That name already exists" << endl;
        cout << "Enter the name of this image(without file extension)" << endl;
        cout << "[Name]:";
        getline(cin, name);
        cout << endl;
      }
      
    }
    
  }//big for loop
  
  //after all done, switch to mainPic
  if(mainPic != "")
  {
    copyFile( mainPic.c_str(), ("image."+getFileExtension(mainPic)).c_str(), true);
    setBackground("image."+getFileExtension(mainPic));
  }
  
  
}



