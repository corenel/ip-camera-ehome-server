#pragma once

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "config.h"
#if (SDK_TYPE == SDK_ISUP)
#include "HCISUPCMS.h"
#include "HCISUPStream.h"
#else
#include "HCEHomeCMS.h"
#include "HCEHomeStream.h"
#endif
#include "PlayM4.h"

LONG lLoginID = -1;
LONG lLinkHandle = -1;
LONG lRealHandle = -1;
FILE *Videofile = nullptr;
LONG m_lPort = SMS_PREVIEW_STREAM_SERVER_PORT;

BOOL CALLBACK RegisterCallBack(LONG lUserID, DWORD dwDataType, void *pOutBuffer,
                               DWORD dwOutLen, void *pInBuffer, DWORD dwInLen,
                               void *pUser);
BOOL InputStreamData(BYTE byDataType, char *pBuffer, int iDataLen);
void CALLBACK fnPREVIEW_DATA_CB(LONG lPreviewHandle,
                                NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg,
                                void *pUserData);
BOOL CALLBACK fnPREVIEW_NEWLINK_CB(LONG lPreviewHandle,
                                   NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
                                   void *pUserData);
