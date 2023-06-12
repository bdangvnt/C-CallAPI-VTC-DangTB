// C++CallAPI-VTC-DangTB.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "windows.h"
#include <winhttp.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#pragma comment(lib, "winhttp.lib")

using namespace rapidjson;

int main()
{
	std::cout << "Hello World!\n";

	// Initialize WinHTTP
	HINTERNET hSession = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) {
		std::cout << "Error: WinHttpOpen failed." << std::endl;
		return 1;
	}

	// Open a connection
	HINTERNET hConnect = WinHttpConnect(hSession, L"apivtcplus1.vtcgame.vn", INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (!hConnect) {
		std::cout << "Error: WinHttpConnect failed." << std::endl;
		WinHttpCloseHandle(hSession);
		return 1;
	}

	// Create a request
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/account/login_vtclauncher", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (!hRequest) {
		std::cout << "Error: WinHttpOpenRequest failed." << std::endl;
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 1;
	}

	// Set the request headers
	LPCWSTR headers = L"Content-Type: application/json";
	if (!WinHttpAddRequestHeaders(hRequest, headers, wcslen(headers), WINHTTP_ADDREQ_FLAG_ADD)) {
		std::cout << "Error: WinHttpAddRequestHeaders failed." << std::endl;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 1;
	}

	// Enable secure communication
	DWORD dwFlags = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
	WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURE_PROTOCOLS, &dwFlags, sizeof(dwFlags));

	// Write request data
	LPCWSTR data = L"{\"accountName\":\"dangtb\", \"password\":\"3d9188577cc9bfe9291ac66b5cc872b7\",\"deviceType\":1}";
	WinHttpWriteData(hRequest, data, wcslen(data), NULL);

	// Send the request
	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)data, wcslen(data) * sizeof(WCHAR),
		wcslen(data) * sizeof(WCHAR), 0)) {
		std::cout << "Error: WinHttpSendRequest failed." << std::endl;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return 1;
	}


	// End the request
	WinHttpReceiveResponse(hRequest, NULL);

	// Get the response headers
	DWORD dwhSize = 0;
	if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwhSize, WINHTTP_NO_HEADER_INDEX)) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			LPWSTR pszOutBuffer = new WCHAR[dwhSize / sizeof(WCHAR)];
			if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, pszOutBuffer, &dwhSize, WINHTTP_NO_HEADER_INDEX)) {
				wprintf(L"Response headers:\n%s\n", pszOutBuffer);
			}
			else {
				std::cout << "Error: WinHttpQueryHeaders failed." << std::endl;
			}
			delete[] pszOutBuffer;
		}
		else {
			std::cout << "Error: WinHttpQueryHeaders failed." << std::endl;
		}
	}

	// Read the response
	DWORD dwSize = 0;
	do
	{
		// Check for available data
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
		{
			printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
			break;
		}

		// Allocate memory for the buffer
		LPSTR pszOutBuffer = new char[dwSize + 1];
		if (!pszOutBuffer)
		{
			printf("Error allocating memory for the buffer.\n");
			break;
		}

		// Read the data
		ZeroMemory(pszOutBuffer, dwSize + 1);
		if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, NULL))
		{
			printf("Error %u in WinHttpReadData.\n", GetLastError());
		}
		else
		{
			// Print the response
			printf("OutBuffer: %s.\n", pszOutBuffer);
			//printf("pszOutBuffer[0]: %s.\n", pszOutBuffer[0]);
			if (pszOutBuffer && pszOutBuffer[0])
			{
				printf("\n\nRapidJson.Parse\n");
				// Parse the JSON string
				Document doc;
				doc.Parse(pszOutBuffer);

				// Access the values in the JSON object
				int responsecode = doc["responsecode"].GetInt();
				std::string description = doc["description"].GetString();
				std::string accountname = doc["info"]["accountname"].GetString();
				long accountid = doc["info"]["accountid"].GetInt64();
				std::string nickname = doc["info"]["nickname"].GetString();
				std::string avatar = doc["info"]["avatar"].GetString();
				int level = doc["info"]["level"].GetInt();
				int unreadmail = doc["info"]["unreadmail"].GetInt();
				std::string accesstoken = doc["info"]["accesstoken"].GetString();
				std::string billing_accesstoken = doc["info"]["billing_accesstoken"].GetString();
				std::string balance = doc["info"]["balance"].GetString();

				// Print the values
				std::cout << "responsecode: " << responsecode << std::endl;
				std::cout << "description: " << description << std::endl;
				std::cout << "accountid: " << accountid << std::endl;
				std::cout << "nickname: " << nickname << std::endl;
				std::cout << "avatar: " << avatar << std::endl;
				std::cout << "level: " << level << std::endl;
				std::cout << "unreadmail: " << unreadmail << std::endl;
				std::cout << "accesstoken: " << accesstoken << std::endl;
				std::cout << "billing_accesstoken: " << billing_accesstoken << std::endl;
				std::cout << "balance: " << balance << std::endl;
			}
		}

		// Free the memory
		delete[] pszOutBuffer;

	} while (dwSize > 0);

	// Close the request
	WinHttpCloseHandle(hRequest);

	// Close the connection
	WinHttpCloseHandle(hConnect);

	// Close the session
	WinHttpCloseHandle(hSession);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
