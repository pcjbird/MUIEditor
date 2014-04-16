#include "StdAfx.h"
#include "DragDropImpl.h"
namespace DuiLib 
{

	#pragma region CEnumFormatEtc
	//
	//	"Drop-in" replacement for SHCreateStdEnumFmtEtc. Called by CDataObject::EnumFormatEtc
	//
	HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC *pFormatEtc, IEnumFORMATETC **ppEnumFormatEtc)
	{
		if(nNumFormats == 0 || pFormatEtc == 0 || ppEnumFormatEtc == 0)
			return E_INVALIDARG;

		*ppEnumFormatEtc = new CEnumFormatEtc(pFormatEtc, nNumFormats);

		return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
	}

	//
	//	Helper function to perform a "deep" copy of a FORMATETC
	//
	static void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source)
	{
		// copy the source FORMATETC into dest
		*dest = *source;

		if(source->ptd)
		{
			// allocate memory for the DVTARGETDEVICE if necessary
			dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

			// copy the contents of the source DVTARGETDEVICE into dest->ptd
			*(dest->ptd) = *(source->ptd);
		}
	}

	//
	//	Constructor 
	//
	CEnumFormatEtc::CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats)
	{
		m_lRefCount   = 1;
		m_nIndex      = 0;
		m_nNumFormats = nNumFormats;
		m_pFormatEtc  = new FORMATETC[nNumFormats];

		// copy the FORMATETC structures
		for(int i = 0; i < nNumFormats; i++)
		{	
			DeepCopyFormatEtc(&m_pFormatEtc[i], &pFormatEtc[i]);
		}
	}

	//
	//	Destructor
	//
	CEnumFormatEtc::~CEnumFormatEtc()
	{
		if(m_pFormatEtc)
		{
			for(ULONG i = 0; i < m_nNumFormats; i++)
			{
				if(m_pFormatEtc[i].ptd)
					CoTaskMemFree(m_pFormatEtc[i].ptd);
			}

			delete[] m_pFormatEtc;
		}
	}

	//
	//	IUnknown::AddRef
	//
	ULONG __stdcall CEnumFormatEtc::AddRef(void)
	{
		// increment object reference count
		return InterlockedIncrement(&m_lRefCount);
	}

	//
	//	IUnknown::Release
	//
	ULONG __stdcall CEnumFormatEtc::Release(void)
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&m_lRefCount);

		if(count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}

	//
	//	IUnknown::QueryInterface
	//
	HRESULT __stdcall CEnumFormatEtc::QueryInterface(REFIID iid, void **ppvObject)
	{
		// check to see what interface has been requested
		if(iid == IID_IEnumFORMATETC || iid == IID_IUnknown)
		{
			AddRef();
			*ppvObject = this;
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	//
	//	IEnumFORMATETC::Next
	//
	//	If the returned FORMATETC structure contains a non-null "ptd" member, then
	//  the caller must free this using CoTaskMemFree (stated in the COM documentation)
	//
	HRESULT __stdcall CEnumFormatEtc::Next(ULONG celt, FORMATETC *pFormatEtc, ULONG * pceltFetched)
	{
		ULONG copied  = 0;

		// validate arguments
		if(celt == 0 || pFormatEtc == 0)
			return E_INVALIDARG;

		// copy FORMATETC structures into caller's buffer
		while(m_nIndex < m_nNumFormats && copied < celt)
		{
			DeepCopyFormatEtc(&pFormatEtc[copied], &m_pFormatEtc[m_nIndex]);
			copied++;
			m_nIndex++;
		}

		// store result
		if(pceltFetched != 0) 
			*pceltFetched = copied;

		// did we copy all that was requested?
		return (copied == celt) ? S_OK : S_FALSE;
	}

	//
	//	IEnumFORMATETC::Skip
	//
	HRESULT __stdcall CEnumFormatEtc::Skip(ULONG celt)
	{
		m_nIndex += celt;
		return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
	}

	//
	//	IEnumFORMATETC::Reset
	//
	HRESULT __stdcall CEnumFormatEtc::Reset(void)
	{
		m_nIndex = 0;
		return S_OK;
	}

	//
	//	IEnumFORMATETC::Clone
	//
	HRESULT __stdcall CEnumFormatEtc::Clone(IEnumFORMATETC ** ppEnumFormatEtc)
	{
		HRESULT hResult;

		// make a duplicate enumerator
		hResult = CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc);

		if(hResult == S_OK)
		{
			// manually set the index state
			((CEnumFormatEtc *) *ppEnumFormatEtc)->m_nIndex = m_nIndex;
		}

		return hResult;
	}
	#pragma endregion


	#pragma region CDataObject

	//
	//	Constructor
	//
	CDataObject::CDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count) 
	{
		m_lRefCount  = 1;
		m_nNumFormats = count;

		m_pFormatEtc  = new FORMATETC[count];
		m_pStgMedium  = new STGMEDIUM[count];

		for(int i = 0; i < count; i++)
		{
			m_pFormatEtc[i] = fmtetc[i];
			m_pStgMedium[i] = stgmed[i];
		}
	}

	//
	//	Destructor
	//
	CDataObject::~CDataObject()
	{
		// cleanup
		if(m_pFormatEtc) delete[] m_pFormatEtc;
		if(m_pStgMedium) delete[] m_pStgMedium;
	}

	BOOL CDataObject::DataRealloc(int count)
	{
		if(count < m_nNumFormats || NULL == m_pFormatEtc || NULL == m_pStgMedium)
		{
			return FALSE;
		}
		FORMATETC* pFormatEtc = new FORMATETC[count];
		STGMEDIUM* pStgMedium = new STGMEDIUM[count];
		for(int i = 0; i < m_nNumFormats; i++)
		{
			pFormatEtc[i] = m_pFormatEtc[i];
			pStgMedium[i] = m_pStgMedium[i];
		}
		delete[] m_pFormatEtc;
		delete[] m_pStgMedium;
		m_pFormatEtc = pFormatEtc;
		m_pStgMedium = pStgMedium;
		m_nNumFormats ++;
		return TRUE;
	}
	//
	//	IUnknown::AddRef
	//
	ULONG __stdcall CDataObject::AddRef(void)
	{
		// increment object reference count
		return InterlockedIncrement(&m_lRefCount);
	}

	//
	//	IUnknown::Release
	//
	ULONG __stdcall CDataObject::Release(void)
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&m_lRefCount);

		if(count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}

	//
	//	IUnknown::QueryInterface
	//
	HRESULT __stdcall CDataObject::QueryInterface(REFIID iid, void **ppvObject)
	{
		// check to see what interface has been requested
		if(iid == IID_IDataObject || iid == IID_IUnknown)
		{
			AddRef();
			*ppvObject = this;
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	HGLOBAL DupMem(HGLOBAL hMem)
	{
		// lock the source memory object
		DWORD   len    = GlobalSize(hMem);
		PVOID   source = GlobalLock(hMem);

		// create a fixed "global" block - i.e. just
		// a regular lump of our process heap
		PVOID   dest   = GlobalAlloc(GMEM_FIXED, len);

		memcpy(dest, source, len);

		GlobalUnlock(hMem);

		return dest;
	}

	int CDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
	{
		for(int i = 0; i < m_nNumFormats; i++)
		{
			if((pFormatEtc->tymed    &  m_pFormatEtc[i].tymed)   &&
				pFormatEtc->cfFormat == m_pFormatEtc[i].cfFormat && 
				pFormatEtc->dwAspect == m_pFormatEtc[i].dwAspect)
			{
				return i;
			}
		}

		return -1;

	}

	int CDataObject::LookupFormatEtc(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, BOOL bAddOrUpdate)
	{
		/* Comparing two DVTARGETDEVICE structures is hard, so we don't even try */
		if (pFormatEtc->ptd != NULL) return DV_E_DVTARGETDEVICE;

		/* See if it's in our list */
		for (int i = 0; i < m_nNumFormats; i++) 
		{
			if (m_pFormatEtc[i].cfFormat == pFormatEtc->cfFormat &&
				m_pFormatEtc[i].dwAspect == pFormatEtc->dwAspect &&
				m_pFormatEtc[i].lindex == pFormatEtc->lindex) 
			{
					if (bAddOrUpdate || (m_pFormatEtc[i].tymed & pFormatEtc->tymed)) 
					{
						return i;
					} 
					else
					{
						return -1;
					}
			}
		}

		if (!bAddOrUpdate) return -1;

		FORMATETC *_pFormatEtc = (FORMATETC *)CoTaskMemAlloc(sizeof(FORMATETC));
		STGMEDIUM *_pStgMedium = (STGMEDIUM *)CoTaskMemAlloc(sizeof(STGMEDIUM));
		
		if (_pFormatEtc && _pStgMedium &&DataRealloc(m_nNumFormats + 1)) 
		{
			m_pFormatEtc[m_nNumFormats-1] = *pFormatEtc;
			ZeroMemory(&m_pStgMedium[m_nNumFormats-1], sizeof(STGMEDIUM));
			return m_nNumFormats-1;
		}
	    else
	   {
			return -1;
		}
	}

	//
	//	IDataObject::GetData
	//
	HRESULT __stdcall CDataObject::GetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
	{
		int idx;

		//
		// try to match the requested FORMATETC with one of our supported formats
		//
		if((idx = LookupFormatEtc(pFormatEtc)) == -1)
		{
			return DV_E_FORMATETC;
		}

		//
		// found a match! transfer the data into the supplied storage-medium
		//
		pMedium->tymed			 = m_pFormatEtc[idx].tymed;
		pMedium->pUnkForRelease  = 0;

		switch(m_pFormatEtc[idx].tymed)
		{
		case TYMED_HGLOBAL:

			pMedium->hGlobal = DupMem(m_pStgMedium[idx].hGlobal);
			//return S_OK;
			break;
		case TYMED_ISTREAM:
			if(NULL!=pMedium->pstm)
				pMedium->pstm->AddRef();
			break;
		case TYMED_ISTORAGE:
			if(NULL != pMedium->pstg)
				pMedium->pstg->AddRef();
			break;
		default:
			return DV_E_FORMATETC;
		}

		return S_OK;
	}

	//
	//	IDataObject::GetDataHere
	//
	HRESULT __stdcall CDataObject::GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
	{
		// GetDataHere is only required for IStream and IStorage mediums
		// It is an error to call GetDataHere for things like HGLOBAL and other clipboard formats
		//
		//	OleFlushClipboard 
		//
		return DATA_E_FORMATETC;
	}

	//
	//	IDataObject::QueryGetData
	//
	//	Called to see if the IDataObject supports the specified format of data
	//
	HRESULT __stdcall CDataObject::QueryGetData (FORMATETC *pFormatEtc)
	{
		return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
	}

	//
	//	IDataObject::GetCanonicalFormatEtc
	//
	HRESULT __stdcall CDataObject::GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
	{
		// Apparently we have to set this field to NULL even though we don't do anything else
		pFormatEtcOut->ptd = NULL;
		return E_NOTIMPL;
	}

	IUnknown *GetCanonicalIUnknown(IUnknown *punk)
	{
		IUnknown *punkCanonical;
		if (punk && SUCCEEDED(punk->QueryInterface(IID_IUnknown,
			(LPVOID*)&punkCanonical))) {
				punkCanonical->Release();
		} else {
			punkCanonical = punk;
		}
		return punkCanonical;
	}

	

	//
	//	IDataObject::SetData
	//
	HRESULT __stdcall CDataObject::SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease)
	{
		int res=LookupFormatEtc(pFormatEtc, pMedium, TRUE);
		if(-1 == res)
		{
			return S_FALSE;
		}
		if (m_pStgMedium[res].tymed)
		{	
			ReleaseStgMedium ( &m_pStgMedium[res] );
			ZeroMemory (&m_pStgMedium[res], sizeof(STGMEDIUM) );		
		} 
		if(fRelease)
		{
			m_pStgMedium[res]=*pMedium;
		}
		else
		{
			CopyStgMedium(pMedium, &m_pStgMedium[res]);
		}
		
		m_pFormatEtc[res].tymed=m_pStgMedium[res].tymed;
		if(m_pStgMedium[res].tymed==TYMED_HGLOBAL)
		{
			m_pStgMedium[res].hGlobal=DupMem(pMedium->hGlobal);//必须分配足够的内存空间和拷贝数据。
		}
		if(GetCanonicalIUnknown(m_pStgMedium[res].pUnkForRelease)==
			GetCanonicalIUnknown(static_cast<IDataObject*>(this)))
		{
			m_pStgMedium[res].pUnkForRelease->Release();
			m_pStgMedium[res].pUnkForRelease = NULL;
		}
		return  S_OK;
	}

	//
	//	IDataObject::EnumFormatEtc
	//
	HRESULT __stdcall CDataObject::EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
	{
		if(dwDirection == DATADIR_GET)
		{
			// for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
			// to support all Windows platforms we need to implement IEnumFormatEtc ourselves.
			return CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc);
		}
		else
		{
			// the direction specified is not support for drag+drop
			return E_NOTIMPL;
		}
	}

	//
	//	IDataObject::DAdvise
	//
	HRESULT __stdcall CDataObject::DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}

	//
	//	IDataObject::DUnadvise
	//
	HRESULT __stdcall CDataObject::DUnadvise (DWORD dwConnection)
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}

	//
	//	IDataObject::EnumDAdvise
	//
	HRESULT __stdcall CDataObject::EnumDAdvise (IEnumSTATDATA **ppEnumAdvise)
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}

	//
	//	Helper function
	//
	HRESULT CreateDataObject (FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, IDataObject **ppDataObject)
	{
		if(ppDataObject == 0)
			return E_INVALIDARG;

		*ppDataObject = new CDataObject(fmtetc, stgmeds, count);

		return (*ppDataObject) ? S_OK : E_OUTOFMEMORY;
	}

	#pragma endregion
}
