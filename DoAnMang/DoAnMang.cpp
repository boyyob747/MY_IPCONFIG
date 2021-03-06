#include "stdafx.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <conio.h>
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#include <iostream>
#include <string>
using namespace std;
/* Note: could also use malloc() and free() */
IP_ADAPTER_INFO  *pAdapterInfo;
ULONG            ulOutBufLen;
DWORD            dwRetVal;
void enter_to_continue() {
	cout << "Press Enter to Continue";
	cin.ignore();
}
void call_ip_config() {

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
	printf("GetAdaptersInfo call failed with %d\n", dwRetVal);
	}
	else {
	PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
	printf("Windows IP Configuration\n\n\n");
	while (pAdapter) {
	printf("%s\n\n", pAdapter->Description);
	printf("\tConnection-specific DNS Suffix  .: %s\n", pAdapter->DhcpServer.IpAddress.String);
	printf("\tIPv4 Address. . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpAddress.String);
	printf("\tSubnet Mask . . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpMask.String);
	printf("\tDefault Gateway . . . . . . . . . : %s\n\n\n", pAdapter->GatewayList.IpAddress.String);

	pAdapter = pAdapter->Next;
	}
	}
}
void call_ip_config_all() {

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
		printf("GetAdaptersInfo call failed with %d\n", dwRetVal);
	}
	else {
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		printf("Windows IP Configuration\n\n\n");
		while (pAdapter) {
			printf("%s\n\n", pAdapter->Description);
			printf("\tConnection-specific DNS Suffix  .: %s\n", pAdapter->DhcpServer.IpAddress.String);
			printf("\tDescription . . . . . . . . . . . : %s\n", pAdapter->Description);
			printf("\tPhysical Address. . . . . . . . . : ");
			for (UINT i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
					printf("%.2X\n", (int)pAdapter->Address[i]);
				else
					printf("%.2X-", (int)pAdapter->Address[i]);
			}
			printf("\tDHCP Enabled. . . . . . . . . . . : %s\n", pAdapter->DhcpEnabled ? "YES" : "NO");
			printf("\tAutoconfiguration Enabled . . . . : \n"); // chua duoc
			printf("\tLink-local IPv6 Address . . . . . : \n"); // chua duoc
			printf("\tIPv4 Address. . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpAddress.String);
			printf("\tSubnet Mask . . . . . . . . . . . : %s\n", pAdapter->IpAddressList.IpMask.String);
			printf("\tDefault Gateway . . . . . . . . . : %s\n", pAdapter->GatewayList.IpAddress.String);
			printf("\tDHCP Server . . . . . . . . . . . : %s\n\n", pAdapter->DhcpServer.IpAddress.String);

			pAdapter = pAdapter->Next;
		}
	}
}
void call_renew() {
	ULONG ulOutBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_INTERFACE_INFO pInfo;
	pInfo = (IP_INTERFACE_INFO *)MALLOC(sizeof(IP_INTERFACE_INFO));

	// Make an initial call to GetInterfaceInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetInterfaceInfo(pInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) {
		FREE(pInfo);
		pInfo = (IP_INTERFACE_INFO *)MALLOC(ulOutBufLen);
	}

	// Make a second call to GetInterfaceInfo to get the
	// actual data we want
	if ((dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen)) == NO_ERROR) {
		//printf("\tAdapter Name: %ws\n", pInfo->Adapter[0].Name);
		//printf("\tAdapter Index: %ld\n", pInfo->Adapter[0].Index);
		//printf("\tNum Adapters: %ld\n", pInfo->NumAdapters);
		printf("\nCall ipconfig /renew...\n");
	}
	else if (dwRetVal == ERROR_NO_DATA) {
		printf("There are no network adapters with IPv4 enabled on the local system\n");
		return;
	}
	else {
		LPVOID lpMsgBuf;
		printf("GetInterfaceInfo failed.\n");

		if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetVal,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL)) {
			printf("\tError: %s", lpMsgBuf);
		}
		LocalFree(lpMsgBuf);
		return;
	}

	// Call IpReleaseAddress and IpRenewAddress to release and renew
	// the IP address on the first network adapter returned 
	// by the call to GetInterfaceInfo.
	PIP_INTERFACE_INFO pAdapter = pInfo;
	int isSucces = 0;
	for (int i = 0; i < pAdapter->NumAdapters; i++) {
		/*if ((dwRetVal = IpReleaseAddress(&pInfo->Adapter[i])) == NO_ERROR) {
			printf("IP release succeeded.\n");
		}
		else {
			printf("IP release failed: %ld\n", dwRetVal);
		}*/

		if ((dwRetVal = IpRenewAddress(&pInfo->Adapter[i])) == NO_ERROR) {
			isSucces = 1;
		}
		else {
			//printf("IP renew failed: %ld\n", dwRetVal);
		}
	}
	call_ip_config();
	if (isSucces == 1) {
		printf("\n\nipconfig /renew succeeded.\n");
	}
	else {
		printf("\n\nipconfig /renew failed.\n");
	}
	if (pInfo != NULL) {
		FREE(pInfo);
	}
	return;
}
void call_release() {
	ULONG ulOutBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_INTERFACE_INFO pInfo;
	pInfo = (IP_INTERFACE_INFO *)MALLOC(sizeof(IP_INTERFACE_INFO));

	// Make an initial call to GetInterfaceInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetInterfaceInfo(pInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) {
		FREE(pInfo);
		pInfo = (IP_INTERFACE_INFO *)MALLOC(ulOutBufLen);
	}

	// Make a second call to GetInterfaceInfo to get the
	// actual data we want
	if ((dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen)) == NO_ERROR) {
		//printf("\tAdapter Name: %ws\n", pInfo->Adapter[0].Name);
		//printf("\tAdapter Index: %ld\n", pInfo->Adapter[0].Index);
		//printf("\tNum Adapters: %ld\n", pInfo->NumAdapters);
		printf("\nCall ipconfig /release...\n");
	}
	else if (dwRetVal == ERROR_NO_DATA) {
		printf("There are no network adapters with IPv4 enabled on the local system\n");
		return;
	}
	else {
		LPVOID lpMsgBuf;
		printf("GetInterfaceInfo failed.\n");

		if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetVal,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL)) {
			printf("\tError: %s", lpMsgBuf);
		}
		LocalFree(lpMsgBuf);
		return;
	}

	// Call IpReleaseAddress and IpRenewAddress to release and renew
	// the IP address on the first network adapter returned 
	// by the call to GetInterfaceInfo.
	PIP_INTERFACE_INFO pAdapter = pInfo;
	int isSucces = 0;
	for (int i = 0; i < pAdapter->NumAdapters; i++) {
		if ((dwRetVal = IpReleaseAddress(&pInfo->Adapter[i])) == NO_ERROR) {
		isSucces = 1;
		}
		else {
		}
	}
	call_ip_config();
	if (isSucces == 1) {
		printf("\n\nipconfig /release succeeded.\n");
	}
	else {
		printf("\n\nipconfig /release failed.\n");
	}
	if (pInfo != NULL) {
		FREE(pInfo);
	}
	return;
}
int main()
{
	string menu = "";
	int chon_menu = 0;
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}
	do {
		cout << "Menu: " << endl;
		cout << "\tOption 1: ipconfig" << endl;
		cout << "\tOption 2: ipconfig /all" << endl;
		cout << "\tOption 3: ipconfig /renew" << endl;
		cout << "\tOption 4: ipconfig /release" << endl;
		cout << "Option: ";
		cin >> chon_menu ;
		switch (chon_menu)
		{
		case 1:
			call_ip_config();
			break;
		case 2:
			call_ip_config_all();
			break;
		case 3:
			call_renew();
			break;
		case 4:
			call_release();
			break;
		default:
			break;
		}
	} while (chon_menu != 0);
	getchar();
}

