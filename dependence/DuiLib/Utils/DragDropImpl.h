#ifndef __DRAGDROPIMPL_H__ 
#define __DRAGDROPIMPL_H__ 
#include <ObjIdl.h>
namespace DuiLib 
{
	UILIB_API HRESULT CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, IDataObject **ppDataObject);

	/////////////////////////////////////////////////////////////////////////////////////////////// 
	class CEnumFormatEtc : public IEnumFORMATETC
	{
	public:

		//
		// IUnknown members
		//
		HRESULT __stdcall  QueryInterface (REFIID iid, void ** ppvObject);
		ULONG	__stdcall  AddRef (void);
		ULONG	__stdcall  Release (void);

		//
		// IEnumFormatEtc members
		//
		HRESULT __stdcall  Next  (ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched);
		HRESULT __stdcall  Skip  (ULONG celt); 
		HRESULT __stdcall  Reset (void);
		HRESULT __stdcall  Clone (IEnumFORMATETC ** ppEnumFormatEtc);

		//
		// Construction / Destruction
		//
		CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats);
		~CEnumFormatEtc();

	private:

		LONG		m_lRefCount;		// Reference count for this COM interface
		ULONG		m_nIndex;			// current enumerator index
		ULONG		m_nNumFormats;		// number of FORMATETC members
		FORMATETC * m_pFormatEtc;		// array of FORMATETC objects
	};

	///////////////////////////////////////////////////////////////////////////////////////////////


	HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC *pFormatEtc, IEnumFORMATETC **ppEnumFormatEtc);

	class CDataObject : public IDataObject
	{
	public:
		//
		// IUnknown members
		//
		HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
		ULONG   __stdcall AddRef (void);
		ULONG   __stdcall Release (void);

		//
		// IDataObject members
		//
		HRESULT __stdcall GetData				(FORMATETC *pFormatEtc,  STGMEDIUM *pMedium);
		HRESULT __stdcall GetDataHere			(FORMATETC *pFormatEtc,  STGMEDIUM *pMedium);
		HRESULT __stdcall QueryGetData			(FORMATETC *pFormatEtc);
		HRESULT __stdcall GetCanonicalFormatEtc (FORMATETC *pFormatEct,  FORMATETC *pFormatEtcOut);
		HRESULT __stdcall SetData				(FORMATETC *pFormatEtc,  STGMEDIUM *pMedium,  BOOL fRelease);
		HRESULT __stdcall EnumFormatEtc			(DWORD      dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
		HRESULT __stdcall DAdvise				(FORMATETC *pFormatEtc,  DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
		HRESULT __stdcall DUnadvise				(DWORD      dwConnection);
		HRESULT __stdcall EnumDAdvise			(IEnumSTATDATA **ppEnumAdvise);

		//
		// Constructor / Destructor
		//
		CDataObject(FORMATETC *fmt, STGMEDIUM *stgmed, int count);
		~CDataObject();

		
	private:
		BOOL DataRealloc(int count);
		int LookupFormatEtc(FORMATETC *pFormatEtc);
		int LookupFormatEtc(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, BOOL bAddOrUpdate);
		//
		// any private members and functions
		//
		LONG	   m_lRefCount;

		FORMATETC *m_pFormatEtc;
		STGMEDIUM *m_pStgMedium;
		LONG	   m_nNumFormats;

	};
 }
#endif