#include "stdafx.h"
#include "BitmapLoad.h"

CBitmapLoad::CBitmapLoad()
{
	m_szFilePathName[0] = '\0';
	m_pBuffer = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nSize = 0;
}

CBitmapLoad::~CBitmapLoad()
{
	this->Destroy();
}

BOOL CBitmapLoad::Load(const char* lpszFilePathName)
{
	if (lpszFilePathName == NULL || *lpszFilePathName == '\0')
		return FALSE;

	FILE* stream = fopen(lpszFilePathName, "rb");
	if (stream == NULL)
		return FALSE;
	BITMAPFILEHEADER fileHeader;
	fread(&fileHeader, 1, sizeof(fileHeader), stream);

	BITMAPINFOHEADER infoHeader;
	fread(&infoHeader, 1, sizeof(infoHeader), stream);

	printf("infoHeader.biHeight=%d\n,infoHeader.biWidth=%d\n,infoHeader.biSizeImage=%d\n,infoHeader.biSize=%d\n",
			infoHeader.biHeight,infoHeader.biWidth,infoHeader.biSizeImage,infoHeader.biSize);

	m_nWidth = infoHeader.biWidth;
	m_nHeight = infoHeader.biHeight;

	int nStride = (m_nWidth*3+3)/4 * 4;
	m_nSize = m_nHeight*nStride;

	m_pBuffer = new unsigned char[m_nSize];
	printf("m_pBuffer = new unsigned char[m_nSize];");
	if (m_pBuffer == NULL)
	{
		fclose(stream);
		return FALSE;
	}

	fseek(stream, fileHeader.bfOffBits, SEEK_SET);
	fread(m_pBuffer, 1, m_nSize, stream);

	fclose(stream);

	strcpy(m_szFilePathName, lpszFilePathName);
	return TRUE;
}

BOOL CBitmapLoad::Destroy()
{
	if (m_pBuffer != NULL)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	return TRUE;
}

BOOL CBitmapLoad::Copy(void* _dst) const
{
	if (m_pBuffer == NULL)
		return FALSE;

	memcpy(_dst, m_pBuffer, m_nSize);
	return TRUE;
}

unsigned char* CBitmapLoad::GetData() const
{
	return this->m_pBuffer;
}

void CBitmapLoad::GetSize(int* pWidth, int* pHeight) const
{
	*pWidth = m_nWidth;
	*pHeight = m_nHeight;
}

unsigned long CBitmapLoad::GetDataSize() const
{
	return this->m_nSize;
}
