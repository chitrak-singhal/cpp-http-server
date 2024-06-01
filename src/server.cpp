#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<bits/stdc++.h>
#include <thread>
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
  else
  {
      response = "HTTP/1.1 404 Not Found\r\n\r\n";
  }

  send(client, response.c_str(), response.length(), 0);
  cout << "Client connected\n";
  close(client);
}

int main(int argc, char **argv)
{
  cout << unitbuf;
  cerr << unitbuf;

  string directory;
  for (int i=1;i<argc;i++)
  {
    if (strcmp(argv[i],"--directory")==0&&i+1<argc)
    {
      directory  = argv[i+1];
      break;
    }
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    cerr << "Failed to create server socket\n";
    return 1;
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
  {
    cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    cerr << "listen failed\n";
    return 1;
  }

  while(true)
  {
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    cout << "Waiting for a client to connect...\n";

    int client = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
     thread(handleClient, client,directory).detach();
  }
  

  close(server_fd);

  return 0;
}
