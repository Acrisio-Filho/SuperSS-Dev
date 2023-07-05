// Arquivo MakeFont.cpp
// Criado em 30/05/2021 as 02:31 por Acrisio
// Cria arquivo de font de mono bit map para o PangYa com base no código do HL

#include <windows.h>

#include <tchar.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <memory>

#include <stdarg.h>

#include <gdiplus.h>
#include <objidl.h>

//using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// Timeapi
#pragma comment (lib,"Winmm.lib")

constexpr unsigned long ANTIALIASING_WORK_FONT_SIZE = 22ul; // 21 é o mínino, mas vou deixar número par
constexpr unsigned long MIN_FONT_SIZE = 4ul; // 11
constexpr unsigned long START_LETTER_CODE_POINT = 0x20ul;

constexpr auto DEFAULT_FONT = "Arial";

BOOL		bItalic = FALSE;
BOOL		bBold = FALSE;
BOOL		bUnderline = FALSE;
BOOL		bAntialiased = FALSE;

wchar_t		fontname[256];
int			pointsize = MIN_FONT_SIZE;
float		scale_size = 1.f;
int			paddingLeft = 0;

WCHAR		start_letter = START_LETTER_CODE_POINT;

// Teste, gera um bitmap do primeiro caracter
BOOL		bOneLetter = FALSE;

// GDI+
ULONG_PTR m_gdiplusToken = NULL;

void initGDIplus() {

	CoInitialize(NULL);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

void destroyGDIplus() {

	// Clear
	if (m_gdiplusToken != NULL)
		Gdiplus::GdiplusShutdown(m_gdiplusToken);

	CoUninitialize();
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {

	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

bool save_png_memory(Gdiplus::Bitmap *bmp, std::vector<BYTE> &data, const wchar_t* _format) {

	//write to IStream
	IStream* istream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &istream);

	CLSID clsid_png;
	GetEncoderClsid(_format, &clsid_png);
	Gdiplus::Status status = bmp->Save(istream, &clsid_png);
	if (status != Gdiplus::Status::Ok)
		return false;

	//get memory handle associated with istream
	HGLOBAL hg = NULL;
	GetHGlobalFromStream(istream, &hg);

	//copy IStream to buffer
	int bufsize = GlobalSize(hg);
	data.resize(bufsize);

	//lock & unlock memory
	LPVOID pimage = GlobalLock(hg);
	memcpy(data.data(), pimage, bufsize);
	GlobalUnlock(hg);

	istream->Release();
	return true;
}

bool save_png_memory(HBITMAP hbitmap, std::vector<BYTE> &data, const wchar_t* _format) {

	Gdiplus::Bitmap bmp(hbitmap, nullptr);

	return save_png_memory(&bmp, data, _format);
}

/*
=================
Error

For abnormal program terminations
=================
*/
void Error(const char *error, ...)
{
	va_list argptr;

	printf("\n************ ERROR ************\n");

	va_start(argptr, error);
	vprintf(error, argptr);
	va_end(argptr);
	printf("\n");

	// Clear GDI+
	destroyGDIplus();

	exit(1);
}

unsigned char* makeMyFont(const wchar_t* _font_family, unsigned long _size, float _scale, int _paddingLeft, bool _italic, bool _underline, bool _bold, bool _antialiased, bool _one_letter, size_t* _outsize) {

	Gdiplus::Status status;

	Gdiplus::RectF rc(0, 0, (float)_size * _scale, (float)_size * _scale);
	Gdiplus::RectF pwf;

	Gdiplus::StringFormat sf;

	Gdiplus::BitmapData btd;

	Gdiplus::Image *img = new Gdiplus::Bitmap((INT)rc.Width, (INT)rc.Height, _antialiased ? PixelFormat24bppRGB : PixelFormat1bppIndexed);
	Gdiplus::Bitmap *mono = nullptr;

	Gdiplus::Graphics g(img);
	float font_size = (float)_size;

	if (_antialiased) {

		g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);

		if (_size < ANTIALIASING_WORK_FONT_SIZE) {

			_scale *= _size / (float)ANTIALIASING_WORK_FONT_SIZE;

			font_size = (float)ANTIALIASING_WORK_FONT_SIZE;
		}

	}else {

		g.SetTextRenderingHint(Gdiplus::TextRenderingHintSingleBitPerPixelGridFit);
		g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
		g.SetInterpolationMode(Gdiplus::InterpolationModeDefault);
	}

	const Gdiplus::PointF pointF(_paddingLeft / _scale, 0.f);
	Gdiplus::RectF layoutF(0.f, 0.f, font_size, font_size);

	Gdiplus::FontFamily fm(_font_family);
	
	// !@ MAKE LANG ID
	//fm.GetFamilyName((LPWSTR)_font_family, MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN));
	
	Gdiplus::Font f(&fm, font_size, Gdiplus::FontStyleRegular, Gdiplus::Unit::UnitPixel);

	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

	Gdiplus::Region region;
	Gdiplus::CharacterRange charRange;

	charRange.First = 0;
	charRange.Length = 1;

	status = sf.SetMeasurableCharacterRanges(1, &charRange);

	status = sf.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
	status = sf.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	status = sf.SetTrimming(Gdiplus::StringTrimming::StringTrimmingNone);
	status = sf.SetFormatFlags(
		Gdiplus::StringFormatFlags::StringFormatFlagsNoClip | Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox
	);

#define FontUnitToPixel(_f_, _fm_, _value_) (float)((_f_).GetSize() * (_value_) / (_fm_).GetEmHeight(Gdiplus::FontStyleRegular))
#define MyFontUnitToPixel(_value_) FontUnitToPixel(f, fm, _value_)

	float ascentF = MyFontUnitToPixel(fm.GetCellAscent(Gdiplus::FontStyleRegular));
	float descentF = MyFontUnitToPixel(fm.GetCellDescent(Gdiplus::FontStyleRegular));
	float lineSpaceingF = MyFontUnitToPixel(fm.GetLineSpacing(Gdiplus::FontStyleRegular));

	printf("Font info, Asc: %f, Desc: %f, LineSpacing: %f, Size: %f\n",
		ascentF,
		descentF,
		lineSpaceingF,
		font_size
	);

	constexpr unsigned long limit_char = (1 << (sizeof(WCHAR) * 8)) - 1;

	unsigned long bytesPerRow = (unsigned long)((_antialiased ? 4 : 1) * (unsigned long)rc.Width + 7) >> 3;
	unsigned long bytesPerChar = bytesPerRow * (unsigned long)rc.Height + 2;

	WCHAR letter = start_letter; //start_char; //'0'; //0xBCC; //0x9ED1;

	size_t buff_size = (limit_char - start_letter) * bytesPerChar + 16/*Header*/;

	// Size buff Font
	*_outsize = buff_size;

	unsigned char* dst = new unsigned char[buff_size];
	unsigned char *pqdata = nullptr;
	unsigned char *src = nullptr;
	unsigned long offset = 0, bits = 0, bestWidth = 0;

	*(uint32_t*)dst = 0x544e4657; // WFNT
	*(uint32_t*)(dst + 4) = (uint32_t)rc.Width; // _size
	*(uint32_t*)(dst + 8) = _antialiased ? 1 : 0;
	*(uint32_t*)(dst + 12) = 0; // Reserved

	do {

		bestWidth = 0;
		offset = (letter - start_letter) * bytesPerChar + 16/*Header*/;

		// Reset
		status = g.ResetTransform();

		if (_scale != 1.f)
			status = g.ScaleTransform(_scale, _scale);

		layoutF.X = pointF.X;
		layoutF.Y = pointF.Y;

		if ((status = g.MeasureCharacterRanges(&letter, 1, &f, layoutF, &sf, 1, &region)) == Gdiplus::Status::Ok) {

			if ((status = region.GetBounds(&pwf, &g)) != Gdiplus::Status::Ok)
				printf("fail region get bounds(%d)", (long)status);

			layoutF.X -= pwf.X - layoutF.X;
			layoutF.Y += (lineSpaceingF - pwf.Height) + descentF;
		}

		// Clear
		status = g.Clear(Gdiplus::Color(0, 0, 0));

		// Draw
		status = g.DrawString(&letter, 1, &f, layoutF, &sf, &brush);

		mono = (Gdiplus::Bitmap*)img;

		if (_antialiased) {

			mono = mono->Clone(rc, PixelFormat4bppIndexed);

			// Não conseguiu clonar
			if (mono == nullptr)
				break;
		}

		// Save only bits
		status = mono->LockBits(NULL, Gdiplus::ImageLockModeRead, _antialiased ? PixelFormat4bppIndexed : PixelFormat1bppIndexed, &btd);

		src = (unsigned char*)btd.Scan0;
		pqdata = dst + offset;

		for (auto i = 0ul; i < btd.Height; i++) {

			bits = 0;

			if (_antialiased) {

				while (bits < (btd.Width * 4)) {

					pqdata[bits >> 3] |= src[bits >> 3] & (1 << (7 - bits & 7));

					if (src[bits >> 3] & (1 << (7 - bits & 7)) && (bits + 1) > bestWidth)
						bestWidth = bits + 1;

					bits++;
				}

				// Clear
				pqdata[bits >> 3] &= ~((1 << (7 - bits & 7)) - 1);

			}
			else {

				while (bits < btd.Width) {

					pqdata[bits >> 3] |= src[bits >> 3] & (1 << (7 - bits & 7));

					if (src[bits >> 3] & (1 << (7 - bits & 7)) && (bits + 1) > bestWidth)
						bestWidth = bits + 1;

					bits++;
				}

				// Clear
				pqdata[bits >> 3] &= ~((1 << (7 - bits & 7)) - 1);
			}

			src += btd.Stride;
			pqdata += bytesPerRow;
		}

		*(unsigned short*)pqdata = (unsigned short)(bestWidth == 0
			? btd.Width / 2 + 2
			: (_antialiased ? bestWidth / 4 : bestWidth));

		status = mono->UnlockBits(&btd);

		// Uma letra teste
		if (_one_letter) {

			// Teste
			if (mono != nullptr) {
				CLSID clsid_bmp;
				GetEncoderClsid(L"image/bmp", &clsid_bmp);
				status = mono->Save(L"letter3.bmp", &clsid_bmp);

				if (_antialiased)
					delete mono;

				mono = (Gdiplus::Bitmap*)img;
			}

			// Only a letter
			break;
		}

		// Clear Antialiasing
		if (_antialiased && mono != nullptr)
			delete mono;

	} while (++letter < limit_char);

	// last letter measure text
	std::ofstream ofe("log.txt", std::ofstream::binary);

	if (ofe.is_open()) {

		ofe << "X: " << pwf.X << "\nY: " << pwf.Y << "\nWidth: " << pwf.Width << "\nHeight: " << pwf.Height << std::endl;
	}

	ofe.close();

	// Clear
	if (img != nullptr)
		delete img;

	img = nullptr;

	return dst;
}

void StripExtension(wchar_t *path)
{
	int             length;

	length = _tcslen(path) - 1;
	while (length > 0 && path[length] != '.')
	{
		length--;
		if (path[length] == '/')
			return;		// no extension
	}
	if (length)
		path[length] = 0;
}

#ifdef	WIN32
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#else	//WIN32
#define PATHSEPARATOR(c) ((c) == '/')
#endif	//WIN32

void DefaultExtension(wchar_t *path, const wchar_t *extension)
{
	wchar_t    *src;
	//
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	//
	src = path + _tcslen(path) - 1;

	while (!PATHSEPARATOR(*src) && src != path)
	{
		if (*src == '.')
			return;                 // it has an extension
		src--;
	}

	_tcscat(path, extension);
}

int _tmain(int argc, wchar_t* argv[]) {

	int		i;
	DWORD	start, end;
	wchar_t	destfile[1024];
	unsigned char *font = nullptr;
	size_t outsize = 0;
    
	// Init GDI+
	initGDIplus();

	printf("makefont.exe Version 1.0 by Acrisio SuperSS Dev (%s)\n", __DATE__ );
	
	printf ("----- Creating Console Font ----\n");

	for (i=1 ; i<argc ; i++)
	{
		if (!_tcscmp(argv[i],L"-font"))
		{
			_tcscpy( fontname, argv[i+1] );
			i++;
		}
		else if (!_tcscmp(argv[i], L"-pointsize"))
		{
			if (i + 1 >= argc)
			{
				Error("Makefont:  Insufficient point size specified\n");
			}
			pointsize = _ttoi(argv[i + 1]);
			i += 1;
		}
		else if (!_tcscmp(argv[i],L"-italic"))
		{
			bItalic = TRUE;
			printf ( "italic set\n");
		}
		else if (!_tcscmp(argv[i],L"-bold"))
		{
			bBold = TRUE;
			printf ( "bold set\n");
		}
		else if (!_tcscmp(argv[i],L"-underline"))
		{
			bUnderline = TRUE;
			printf ( "underline set\n");
		}
		else if (!_tcscmp(argv[i], L"-antialiasing")) {
			bAntialiased = TRUE;
			printf("antialiasing set\n");
		}
		else if (!_tcscmp(argv[i], L"-one")) {
			bOneLetter = TRUE;
			printf("One Letter Print\n");
		}
		else if (!_tcscmp(argv[i], L"-startletter")) {

			if (i + 1 >= argc) {

				Error("Makefont: Insufficient start letter specified\n");
			}
			
			start_letter = _ttoi(argv[i + 1]);
			
			if (start_letter < START_LETTER_CODE_POINT)
				start_letter = START_LETTER_CODE_POINT;
			
			i += 1;
		}
		else if (!_tcscmp(argv[i], L"-scale")) {

			if (i + 1 >= argc) {

				Error("Makefont: Insuficient scale specified\n");
			}

			scale_size = (float)_ttoi(argv[i + 1]);

			i += 1;
		}
		else if (!_tcscmp(argv[i], L"-paddingleft")) {

			if (i + 1 >= argc) {

				Error("Makefont: Insuficient paddingleft specified\n");
			}

			paddingLeft = _ttoi(argv[i + 1]);

			i += 1;
		}
		else if ( argv[i][0] == '-' )
		{
			Error ("Unknown option \"%s\"", argv[i]);
		}
		else
			break;
	}

	if ( i != argc - 1 )
	{
		Error ("usage: makefont [-font \"fontname\"] [-italic] [-underline] [-bold] [-antialiasing] [-startletter letter] [-pointsize size] [-scale size] [paddingleft size] [-one] outfile");
	}

	if (scale_size == -1.f || scale_size == 0.f)
		scale_size = 1.f;
	else if (scale_size > 1.f)
		scale_size = scale_size / (float)pointsize;

	wprintf( L"Creating %i point %.02f scale %s fonts\n", pointsize, scale_size, fontname );

	start = timeGetTime();

	// Create the fonts
	font = makeMyFont(fontname, pointsize, scale_size, paddingLeft, bItalic, bUnderline, bBold, bAntialiased, bOneLetter, &outsize);

	if (font != nullptr) {

		_tcscpy(destfile, argv[argc - 1]);

		StripExtension(destfile);
		DefaultExtension(destfile, L".wft");

		std::ofstream o_f(destfile, std::ofstream::binary);

		if (o_f.is_open())
			o_f.write((char*)font, outsize);

		o_f.close();
	}

	// Clean up memory
	if (font != nullptr)
		delete font;

	end = timeGetTime();

	printf("%5.5f seconds elapsed\n", (float)(end - start) / 1000.0);

	// Display for a second since it might not be running from command prompt
	Sleep(1000);

	// Clear GDI+
	destroyGDIplus();

	return 0;
}
