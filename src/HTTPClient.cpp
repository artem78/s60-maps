/*
 ============================================================================
 Name		: HTTPClient.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : CHTTPClient implementation
 ============================================================================
 */

#include "HTTPClient.h"

CHTTPClient::CHTTPClient(MHTTPClientObserver* aObserver) :
	iObserver(aObserver)
	{
	// No implementation required
	}

CHTTPClient::~CHTTPClient()
	{
	iSession.Close();
	}

CHTTPClient* CHTTPClient::NewLC(MHTTPClientObserver* aObserver)
	{
	CHTTPClient* self = new (ELeave) CHTTPClient(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHTTPClient* CHTTPClient::NewL(MHTTPClientObserver* aObserver)
	{
	CHTTPClient* self = CHTTPClient::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CHTTPClient::ConstructL()
	{
	// Open http session with default protocol HTTP/TCP
	iSession.OpenL();
	}

void CHTTPClient::GetL(const TDesC8 &aUrl)
	{
	SendRequestL(/*THTTPMethod::*/EGet, aUrl);
	}

void CHTTPClient::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,
		const TDesC8 &aHdrValue)
	{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	//valStr.Close();
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,
			RHTTPSession::GetTable()), val);
	CleanupStack::PopAndDestroy(&valStr);
	}

void CHTTPClient::SetUserAgentL(const TDesC8 &aDes)
	{
	RHTTPHeaders headers = iSession.RequestSessionHeadersL();
	SetHeaderL(headers, HTTP::EUserAgent, aDes);
	}

void CHTTPClient::SendRequestL(THTTPMethod aMethod, const TDesC8 &aUrl)
	{
	// Method
	TInt method;
	switch (aMethod)
		{
		case /*THTTPMethod::*/EGet:
			{
			method = HTTP::EGET;
			break;
			}
			
		default:
			// Other methods not yet implemented
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}
	RStringF methodStr = iSession.StringPool().StringF(
			method, RHTTPSession::GetTable());
	// It`s not needed to call methodStr.Close() after use
	
	// Parse URL
	TUriParser8 uri;
	TInt r = uri.Parse(aUrl);
	User::LeaveIfError(r);

	// Create transaction
	RHTTPTransaction trans = iSession.OpenTransactionL(uri, *iObserver, methodStr);
	CleanupClosePushL(trans);	// Todo: Is it needed?
	
	trans.SubmitL();
	CleanupStack::Pop(&trans); // Not nedeed to destroy (only pop from stack)
		// beacause Close() will be called in MHFRunL on failed or success event
	}

void MHTTPClientObserver::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent &aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			OnHTTPHeadersRecieved(aTransaction);
			} 
			break;
			
		case THTTPEvent::EGotResponseBodyData:
			{			
			MHTTPDataSupplier* body = aTransaction.Response().Body();
			
			TPtrC8 dataChunk;
			TBool isLast = body->GetNextDataPart(dataChunk);

			OnHTTPResponseDataChunkRecieved(aTransaction, dataChunk,
					body->OverallDataSize(), isLast);
			
			// Done with that bit of body data
			body->ReleaseData();
			} 
			break;
			
		case THTTPEvent::EResponseComplete:
			{
			//aTransaction.Close();
			//OnHTTPResponse(aTransaction);
			} 
			break;
			
		case THTTPEvent::ESucceeded:
			{
			OnHTTPResponse(aTransaction);
			aTransaction.Close();
			} 
			break;
			
		case THTTPEvent::EFailed:
			{
			OnHTTPError(iLastError, aTransaction);
			iLastError = 0; // Reset last error code
			aTransaction.Close();
			} 
			break;
			
		case THTTPEvent::ERedirectedPermanently:
			break;
			
		case THTTPEvent::ERedirectedTemporarily:
			break;
			
		default:
			{
			iLastError = aEvent.iStatus;
			
			if (aEvent.iStatus < 0) // Any error
				{
				//OnHTTPError(aEvent.iStatus, aTransaction);
				// aTransaction.Close() not needed because THTTPEvent::EFailed will
				// be sent at final
				//aTransaction.Close();
				}
			else // Any warning
				{
			
				}
			}
			break;
		}
	}

TInt MHTTPClientObserver::MHFRunError(TInt /*aError*/, RHTTPTransaction aTransaction,
		const THTTPEvent& /*aEvent*/)
	{
	// Cleanup any resources in case MHFRunL() leaves
	aTransaction.Close();
	
	return KErrNone;
	}


// MHTTPClientObserver

void MHTTPClientObserver::OnHTTPError(TInt /*aError*/, RHTTPTransaction /*aTransaction*/)
	{
	// No implementation by default
	}
