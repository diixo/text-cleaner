#include <iostream>
#include <time.h>
#include <string>
#include <wchar.h>
#include <wctype.h>

#include <fcntl.h>
#include <io.h>
#include <ostream>
#include <assert.h>

typedef std::wstring wstring_t;
typedef unsigned int UInt32;

// https://secure.n-able.com/webhelp/NC_9-1-0_SO_en/Content/SA_docs/API_Level_Integration/API_Integration_URLEncoding.html
//////////////////////////////////////////////////////////////////////////

const wchar_t eu_upper[48] = {
	L'\x00c0', L'\x00c1', L'\x00c2', L'\x00c3', L'\x00c4', L'\x00c5', L'\x0102', L'\x00c6', L'\x00c7',  L'\x0106', L'\x010c',L'\x010e',
	L'\x00d0', L'\x00c9', L'\x00c8', L'\x00ca', L'\x00cb', L'\x011e', L'\x00cc', L'\x00cd', L'\x00ce', L'\x00cf', L'\x0141', L'\x0147',
	L'\x00d1', L'\x00d2', L'\x00d3', L'\x00d4', L'\x00d5', L'\x00d6', L'\x00d8', L'\x0158', L'\x015a', L'\x0218', L'\x1e9e', L'\x0164',
	L'\x021a', L'\x00da', L'\x00d9', L'\x00db', L'\x016e', L'\x00dc', L'\x00dd', L'\x0178', L'\x0179', L'\x017b', L'\x017d', L'\x00de'
};

const wchar_t eu_lower[48] = {
	L'\x00e0', L'\x00e1', L'\x00e2', L'\x00e3', L'\x00e4', L'\x00e5', L'\x0103', L'\x00e6', L'\x00e7', L'\x0107', L'\x010d', L'\x010f',
	L'\x00f0', L'\x00e9', L'\x00e8', L'\x00ea', L'\x00eb', L'\x011f', L'\x00ec', L'\x00ed', L'\x00ee', L'\x00ef', L'\x0142', L'\x0148',
	L'\x00f1', L'\x00f2', L'\x00f3', L'\x00f4', L'\x00f5', L'\x00f6', L'\x00f8', L'\x0159', L'\x015b', L'\x0219', L'\x00df', L'\x0165',
	L'\x021b', L'\x00fa', L'\x00f9', L'\x00fb', L'\x016f', L'\x00fc', L'\x00fd', L'\x00ff', L'\x017a', L'\x017e', L'\x017c', L'\x00fe'
};

bool isModificatorGroup(const wchar_t ch)
{
   // general group: 02B0—02FF
   return ((ch >= 0x02b0) && (ch <= 0x02ff));
}

bool isDiacriticGroup(const wchar_t ch)
{
   // general group: 0300—036F
   const wchar_t diacr[] = // [22]
   {
      0x0301,
      0x0300,
      0x0308,
      0x0302,
      0x0311,
      0x030c,
      0x030b,
      0x030f,
      0x030a,
      0x0307,
      0x0303,
      0x0342,
      0x0304,
      0x0306,

      0x0326,

      0x032f,
      0x0331,
      0x032c,

      0x0327,
      0x0328,
      0x0337,
      0x0338
   };
   return ((ch >= 0x0300) && (ch <= 0x036F));
}

bool isOutdatedGroup(const wchar_t ch)
{
   return ((ch >= 0x0370) && (ch <= 0x03FF));
}

bool isApostrophe(const wchar_t ch)
{
   if (
      (ch == 0x0027) ||  // 39
      (ch == 0x0060) ||  // 96
      (ch == 0x0091) ||  // 145
      (ch == 0x00b4) ||  // 180 
      (ch == 0x2019)     // 8217
      )
   {
      return true;
   }
   return false;
}

wchar_t translateChar(const wchar_t ch)
{
   const wchar_t space = 0x0020;
   const wchar_t apostrophe = 0x0027;
   
   if (ch < space)
   {
      return space;
   }
   
   if (isApostrophe(ch))
   {
      return apostrophe;
   }

   // replace hieroglyph symbols, also: (0x2028, 0x2029)
   if (ch >= 1280) // 0x0500
   {
      if ((ch >= 0x1e00) && (ch <= 0x1eff))
      {
		  return ch;
      }
      return space;
   }

   const wchar_t replaceTable[12] = 
   {
      0x0022,  // """
      0x0028,  // ("(")
      0x0029,  // (")") 29
      0x00a0,  // NBPS = 160
      0x0085,  // NEL

      0x007b,  // ("{")
      0x007c,  // ("|")
      0x007d,  // ("}")
      0x005c,  // ("\")

      0x005e,  // ("^")
      0x005b,  // ("[")
      0x005d   // ("]")
   };

   for (UInt32 i = 0; i < sizeof(replaceTable)/sizeof(replaceTable[0]); i++)
   {
      if (replaceTable[i] == ch)
      {
         return space;
      }
   }

   // check outside if return 0, to skip this symbol 
   if (ch == 0x00ad)  // soft NewLine-symbol
   {
      return 0;
   }
   
   if (isModificatorGroup(ch))
   {
      printf("isModificator=%d\n", int(ch));
      return ch;
   }
   else if (isDiacriticGroup(ch))
   {
      printf("isDiacritic=%d\n", int(ch));
      return ch;
   }
   else if (isOutdatedGroup(ch))
   {
      printf("isOutdated=%d\n", int(ch));
      return ch;
   }

   // special symbols
   if ((ch >= 0x0080) && (ch <= 0x00a0))
   {
      return space;
   }

   // separated symbols
   const wchar_t separated[21] = 
   {
      0x00a1,
      0x00a4,  // general currency sign (164)
      0x00a6,
      0x00a8,
      0x00aa,  // indicator
      0x00ab,  // l-double quotation mark
      0x00ac,  // Not sign
      //
      0x00af,  // LF (U+000A): line feed
      0x00b0,  // CR (U+000D): carriage return
      0x00b1,  // NEL (U+0085): next line
      0x00b2,  // LS (U+2028): line separator
      0x00b3,  // PS (U+2029): paragraph separator
      //
      0x00b6,  // paragraph
      0x00b7,  // middle dot 
      0x00b8,  // cedilla
      0x00ba,  // indicator
      0x00bb,  // r-double quotation mark
      0x00bc,  // fraction
      0x00bd,  // fraction
      0x00be,  // fraction
      0x00bf   // inverted question
   };

   for (UInt32 i = 0; i < sizeof(separated)/sizeof(separated[0]); i++)
   {
      if (separated[i] == ch)
      {
         return space;
      }
   }

   // return input symbol without modifications
   return ch;
}

void test_translateChar()
{
   _setmode(_fileno(stdout), _O_U16TEXT);

   for (size_t i = 0; i < 48; i++)
   {
      if (translateChar(eu_lower[i]) != eu_lower[i])
      {
         assert(false);
      }
      if (translateChar(eu_upper[i]) != eu_upper[i])
      {
         assert(false);
      }
   }

   for (size_t i = 0; i < 48; i++)
   {
      putwchar(eu_lower[i]);
      putwchar(L'-');
      putwchar(eu_upper[i]);
      putwchar(L'\n');
   }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void processLineString(const wchar_t* str)
{
   // stub
}

void readFile(const std::wstring& filename_in, const std::wstring& filename_out)
{
   setlocale(LC_ALL, "Russian");
   //////////////////////////////////////////////////////////////////////////

   FILE *pFile = _wfopen(filename_in.c_str(), L"rt, ccs=UTF-8");
   // MSDN: Allowed values of encoding are UNICODE, UTF-8, and UTF-16LE.

   if (pFile == NULL)
   {
      wprintf(L"can't load file: %s\n", filename_in.c_str());
      return;
   }
   
   FILE *pOutput = _wfopen(filename_out.c_str(), L"w, ccs=UTF-16LE");
   //////////////////////////////////////////////////////////////////////////

   wchar_t buff[2048];
   buff[0] = 0;
   wchar_t* pBuff = buff;
  
   wchar_t wch = 0;

   //////////////////////////////////////////////////////////////////////////
   while ((wch = fgetwc(pFile)) != WEOF)
   {
      if (wch == L'\n')
      {
         fputwc(wch, pOutput);

         if (pBuff != buff)
         {
            *pBuff = 0;
            pBuff = buff;

            processLineString(buff);
         }
         else
         {
            printf("!!! skip empty\n");
         }
      }
      else
      {
         const wchar_t tch = translateChar(wch);

         // check if need to skip symbol.
         if (tch > 0)
         {
            fputwc(tch, pOutput);

            *pBuff = tch;
            pBuff++;
         }
      }
   }

   fclose(pFile);
   fclose(pOutput);
}

int main()
{
   readFile(L"db-input.u16", L"db-out.u16");
   //test_translateChar();

   return 0;
}
