
#include <string>
#include <time.h>

typedef unsigned int UInt32;

UInt32 i = 0;

bool isApostrophe(int c)
{
   bool result = false;

   if (
      (c == 0x0027) ||  // 39
      (c == 0x0060) ||  // 96
      (c == 0x0091) ||  // 145
      (c == 0x00b4) ||  // 180 
      (c == 0x2019)     // 8217
      )
   {
      result = true;
   }
   return result;
}

// https://secure.n-able.com/webhelp/NC_9-1-0_SO_en/Content/SA_docs/API_Level_Integration/API_Integration_URLEncoding.html
wchar_t translateChar(const wchar_t ch)
{
   const wchar_t space = 0x0020;
   const wchar_t apostrophe = 0x0027;

   const wchar_t replaceTable[11] = 
   {
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

   if (ch == 0x00ad)  // soft NL, should be skip
   {
      return 0;
   }

   if ((ch == 0x2028) || (ch == 0x2029))
   {
      return space;
   }

   if (ch < space)
   {
      return space;
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

   if (isApostrophe(ch))
   {
      return apostrophe;
   }

   // skip hieroglyph symbols
   if (ch >= 1280) // 0x0500
   {
      return space;
   }

   return ch;
}

bool processString(wchar_t* str)
{
   // stub
   return true;
}

void readFile(const std::wstring& filename)
{
   setlocale(LC_ALL, "Russian");
   //////////////////////////////////////////////////////////////////////////

   FILE *pFile = _wfopen(filename.c_str(), L"rt, ccs=UTF-8");
   // MSDN: Allowed values of encoding are UNICODE, UTF-8, and UTF-16LE.

   if (pFile == NULL)
   {
      wprintf(L"can't load file: %s\n", filename.c_str());
      return;
   }
   
   FILE *pOutput = _wfopen(L"db-out.u16", L"w, ccs=UTF-16LE");
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
            *pBuff = wch;
            pBuff++;
            //
            *pBuff = 0;
            pBuff = buff;

            processString(buff);
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
   readFile(L"db-input.u16");
   return 0;
}
