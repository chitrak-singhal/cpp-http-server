#include "client_handler.h"
using namespace std;

vector<string> split(string &request, string delim)
{
    stringstream ss (request);
    vector<string> comps;
    string temp;
    while(getline(ss, temp, delim[0]))
    {
      comps.push_back(temp);
      ss.ignore(delim.size()-1);
    }
    return comps;
}

void handleClient(int client, string directory)
{
  char buffer[4096];
  int bytes_received = recv(client, buffer, sizeof(buffer) - 1, 0);
  buffer[bytes_received] = '\0'; 
  string request(buffer);
  vector<string> comps = split(request, "\r\n");
  vector<string> comps2 = split(comps[0], " ");
  string method = comps2[0];
  string endpoint = comps2[1];
  string response="";
  if (endpoint=="/")
      response = "HTTP/1.1 200 OK\r\n\r\n";
  else if (endpoint.starts_with("/echo"))
  {
      string text= endpoint.substr(6);
      response ="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + to_string(text.size()) + "\r\n\r\n" + text;
      
  }
  else if (endpoint.starts_with("/user"))
  {
    string text;
      for (auto &elem:comps)
      {
        if (elem.starts_with("User"))
        {
          vector<string> temp = split(elem, " ");
          text = temp[1];
          break;
        }
      }
      response ="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + to_string(text.size()) + "\r\n\r\n" + text;
  }
  else if (endpoint.starts_with("/files"))
  {
    string filepath = directory+endpoint.substr(6);
    if (method=="GET")
    {
        ifstream file(filepath);
        if (file)
        {
            stringstream content;
            content << file.rdbuf();
            file.close();
            string text = content.str();
            response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " + to_string(text.size()) + "\r\n\r\n" + text;
        }
        else
        {
            response = "HTTP/1.1 404 Not Found\r\n\r\nFile not found";
        }
    }
    else if (method=="POST")
    {
        string text = comps[comps.size()-1];
        ofstream outfile(filepath);

        if (outfile.is_open()) 
        {
            outfile << text;
            outfile.close();
            response = "HTTP/1.1 201 Created\r\n\r\n";
            cout << "Content saved to 'output.txt' successfully.\n";
        } 
        else 
        {
            cerr << "Error opening the output file.\n";
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        }
    }
    
  }
  else
  {
      response = "HTTP/1.1 404 Not Found\r\n\r\n";
  }

  send(client, response.c_str(), response.length(), 0);
  cout << "Client connected\n";
  close(client);
}
