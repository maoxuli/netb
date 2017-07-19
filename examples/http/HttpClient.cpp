/*
 * Copyright (C) 2013, Maoxu Li. Email: maoxu@lebula.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "HttpClient.h"
#include "EventLoop.h"

NET_BASE_BEGIN

using namespace std::placeholders;

HttpClient::HttpClient(EventLoop* loop)
: mLoop(loop)
, mConnector(loop)
, mConnection(NULL)
{
    mConnector.SetConnectedCallback(std::bind(&HttpClient::OnConnected, this, _1));
}

HttpClient::~HttpClient()
{

}

bool HttpClient::Connect(const char* host, unsigned short port)
{
    assert(mConnection == NULL);
    return mConnector.Connect(host, port);
}

// Tcp connector established a outgoing connection
void HttpClient::OnConnected(TcpConnection* conn)
{
    assert(conn != NULL);
    assert(mConnection == NULL);
    mConnection = conn;
    mConnection->SetReceivedCallback(std::bind(&HttpClient::OnReceived, this, _1, _2));
    mConnection->SetClosedCallback(std::bind(&HttpClient::OnClosed, this, _1));
}  

void HttpClient::OnReceived(TcpConnection* conn, StreamBuffer* buf)
{
    assert(conn == mConnection);
    if(mResponse.FromBuffer(buf))
    {
        HandleResponse(&mResponse);
        mResponse.Reset();
    }
}
// Tcp connection is closed
void HttpClient::OnClosed(TcpConnection* conn)
{
    assert(conn == mConnection);
    mConnection = NULL;
}

void HttpClient::SendRequest(HttpRequest* request)
{
    assert(mConnection != NULL);
    StreamBuffer buf;
    request->ToBuffer(&buf);
    mConnection->Send(&buf);
    std::cout << request->Dump() << "\n";
}

void HttpClient::HandleResponse(HttpResponse* response)
{
    std::cout << response->Dump() << "\n";
}

NET_BASE_END

// httpclient 23.45.126.9
int main(const int argc, char* argv[])
{
    assert(argc >= 2);
    netbase::EventLoop loop;
    netbase::HttpClient client(&loop);
    if(!client.Connect(argv[1], 80))
    {
        std::cout << "Connect failed.\n";
    }
    
    netbase::HttpRequest request("GET", argv[1]);
    client.SendRequest(&request);
    loop.Run();

    return 0;
}
