// RotoScopeDoc.cpp : implementation of the CRotoScopeDoc class
//

#include "pch.h"
#include "RotoScope.h"
#include <fstream>

#include "RotoScopeDoc.h"
#include  "xmlhelp..h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// Simple inline function to range bound a double and cast to a short.
inline short ShortRange(double a) {return a > 32767 ? 32767 : (a < -32768. ? -32768 : short(a));}


// CRotoScopeDoc

IMPLEMENT_DYNCREATE(CRotoScopeDoc, CDocument)

BEGIN_MESSAGE_MAP(CRotoScopeDoc, CDocument)
    ON_COMMAND(ID_MOVIES_OPENSOURCEMOVIE, &CRotoScopeDoc::OnMoviesOpensourcemovie)
    ON_COMMAND(ID_MOVIES_OPENOUTPUTMOVIE, &CRotoScopeDoc::OnMoviesOpenoutputmovie)
    ON_COMMAND(ID_FRAMES_CREATEONEFRAME, &CRotoScopeDoc::OnFramesCreateoneframe)
    ON_COMMAND(ID_FRAMES_WRITEONEFRAME, &CRotoScopeDoc::OnFramesWriteoneframe)
    ON_UPDATE_COMMAND_UI(ID_FRAMES_WRITEONEFRAME, &CRotoScopeDoc::OnUpdateFramesWriteoneframe)
    ON_COMMAND(ID_MOVIES_CLOSESOURCEMOVIE, &CRotoScopeDoc::OnMoviesClosesourcemovie)
    ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSESOURCEMOVIE, &CRotoScopeDoc::OnUpdateMoviesClosesourcemovie)
    ON_COMMAND(ID_MOVIES_CLOSEOUTPUTMOVIE, &CRotoScopeDoc::OnMoviesCloseoutputmovie)
    ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSEOUTPUTMOVIE, &CRotoScopeDoc::OnUpdateMoviesCloseoutputmovie)
    ON_COMMAND(ID_FRAMES_WRITETHENCREATEONEFRAME, &CRotoScopeDoc::OnFramesWritethencreateoneframe)
    ON_UPDATE_COMMAND_UI(ID_FRAMES_WRITETHENCREATEONEFRAME, &CRotoScopeDoc::OnUpdateFramesWritethencreateoneframe)
    ON_COMMAND(ID_FRAMES_WRITETHENCREATEONESECOND, &CRotoScopeDoc::OnFramesWritethencreateonesecond)
    ON_COMMAND(ID_FRAMES_WRITETHENCREATEREMAINING, &CRotoScopeDoc::OnFramesWritethencreateremaining)
    ON_COMMAND(ID_MOVIES_OPENBACKGROUNDAUDIO, &CRotoScopeDoc::OnMoviesOpenbackgroundaudio)
    ON_COMMAND(ID_MOVIES_CLOSEBACKGROUNDAUDIO, &CRotoScopeDoc::OnMoviesClosebackgroundaudio)
    ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSEBACKGROUNDAUDIO, &CRotoScopeDoc::OnUpdateMoviesClosebackgroundaudio)
END_MESSAGE_MAP()


//! Constructor for the document class.  
CRotoScopeDoc::CRotoScopeDoc()
{
    ::CoInitialize(NULL);

    // Set the image size to an initial default value and black.
	m_image.SetSize(640, 480);
    m_image.Fill(0, 0, 0);
    m_movieframe = 0;
    
}

//! Destructor
CRotoScopeDoc::~CRotoScopeDoc()
{
    ::CoUninitialize();
}


//! Function that is called when a new document is created
//! \returns true if successful
BOOL CRotoScopeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



// CRotoScopeDoc diagnostics

#ifdef _DEBUG
void CRotoScopeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRotoScopeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Source movie management
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :         CRotoScopeDoc::OnMoviesOpensourcemovie()
// Description :  Open a video input source.
//

void CRotoScopeDoc::OnMoviesOpensourcemovie()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Video Files (*.avi;*.wmv;*.asf)|*.avi; *.wmv; *.asf|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".avi"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
        return;

    if(!m_moviesource.Open(dlg.GetPathName()))
        return;
}

void CRotoScopeDoc::OnMoviesClosesourcemovie() { m_moviesource.Close(); }
void CRotoScopeDoc::OnUpdateMoviesClosesourcemovie(CCmdUI *pCmdUI) { pCmdUI->Enable(m_moviesource.IsOpen()); }


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Background music management
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Name :        CRotoScopeDoc::OnMoviesOpenbackgroundaudio()
// Description : Opens an audio file we can use as a background music source.
//

void CRotoScopeDoc::OnMoviesOpenbackgroundaudio()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Audio Files (*.wav;*.mp3)|*.wav; *.mp3|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".wav"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
        return;

    if(!m_backaudio.Open(dlg.GetPathName()))
        return;
}

void CRotoScopeDoc::OnMoviesClosebackgroundaudio() { m_backaudio.Close(); }
void CRotoScopeDoc::OnUpdateMoviesClosebackgroundaudio(CCmdUI *pCmdUI) { pCmdUI->Enable(m_backaudio.IsOpen()); }


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Output movie management
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :        CRotoScopeDoc::OnMoviesOpenoutputmovie()
// Description : Open an output movie.
//

void CRotoScopeDoc::OnMoviesOpenoutputmovie()
{
    m_movieframe = 0;
	static TCHAR BASED_CODE szFilter[] = TEXT("ASF Files (*.asf)|*.asf|All Files (*.*)|*.*||");

	CFileDialog dlg(FALSE, TEXT(".asf"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
        return;

    if(!m_moviemake.Open(dlg.GetPathName()))
        return;

    // Make CImage match the size of the output movie.
    m_image.SetSize(m_moviemake.GetWidth(), m_moviemake.GetHeight());

    // Make the audio buffer match the storage requirement for one video frame
    m_audio.clear();
    m_audio.resize( size_t(m_moviemake.GetSampleRate() / m_moviemake.GetFPS()) * m_moviemake.GetNumChannels() );

    UpdateAllViews(NULL);
   
}


void CRotoScopeDoc::OnMoviesCloseoutputmovie() { m_moviemake.Close(); }
void CRotoScopeDoc::OnUpdateMoviesCloseoutputmovie(CCmdUI *pCmdUI) { pCmdUI->Enable(m_moviemake.IsOpen()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Creating and writing video frames.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :         CRotoScopeDoc::OnFramesCreateoneframe()
// Description :  Menu handler for Frame/Create One Frame menu option.
//                This will call the function that creates one frame.
//

void CRotoScopeDoc::OnFramesCreateoneframe()
{
    CreateOneFrame();
    UpdateAllViews(NULL);
}


//
// Name :        CRotoScopeDoc::CreateOneFrame()
// Description : This function creates a frame for display.
//               This is a demonstration of how we can read frames and audio 
//               and create an output frame.
//

void CRotoScopeDoc::CreateOneFrame()
{
    //
    // Clear our frame first
    //

    m_image.Fill(0, 0, 0);

    //
    // Read any image from source video?
    //

    if(m_moviesource.HasVideo())
    {
        // Important:  Don't read directly into m_image.  Our source may be a 
        // different size!  I'm reading into a temporary image, then copying
        // the data over.

        CGrImage image;
        if(m_moviesource.ReadImage(image))
        {
            // Write this into m_image
            for(int r=0;  r<m_image.GetHeight() && r<image.GetHeight();  r++)
            {
                for(int c=0;  c<m_image.GetWidth() && c<image.GetWidth();  c++)
                {
                    m_image[r][c*3] = image[r][c*3];
                    m_image[r][c*3+1] = image[r][c*3+1];
                    m_image[r][c*3+2] = image[r][c*3+2];
                }
            }
        }
    }

    //
    // Read any audio from the source video?  Note that we read and write the 
    // audio associated with one frame of video.
    //

    std::vector<short> audio;
    if(m_moviesource.HasAudio() && m_moviesource.ReadAudio(audio))
    {
        // The problem is that the input audio may not be in the same format
        // as the output audio.  For example, we may have a different number of 
        // audio frames for a given video frame.  Also, the channels may be
        // different.  I'll assume my output is stereo here, since I created a
        // stereo profile, but the input may be mono.

        if(m_moviesource.GetNumChannels() == 2)
        {
            // Easiest, both are the same.
            // What's the ratio of playback?
            double playrate = double(audio.size()) / double(m_audio.size());
            for(unsigned f=0;  f<m_audio.size() / 2;  f++)
            {
                int srcframe = int(playrate * f);
                m_audio[f*2] = audio[srcframe*2];
                m_audio[f*2+1] = audio[srcframe*2+1];
            }
        }
        else
        {
            // Mono into stereo
            double playrate = double(2. * audio.size()) / double(m_audio.size());
            for(unsigned f=0;  f<m_audio.size() / 2;  f++)
            {
                int srcframe = int(playrate * f);
                m_audio[f*2] = audio[srcframe];
                m_audio[f*2+1] = audio[srcframe];
            }
        }

    }
    else
    {
        // If there is no audio to read, set to silence.
        for(unsigned int i=0;  i<m_audio.size();  i++)
            m_audio[i] = 0;
    }

    //
    // Is there any background audio to mix in?
    //

    if(m_backaudio.IsOpen())
    {
        for(std::vector<short>::iterator i=m_audio.begin();  i!=m_audio.end();  )
        {
            short audio[2];
            m_backaudio.ReadStereoFrame(audio);

            *i = ShortRange(*i + audio[0] * 0.3);
            i++;
            *i = ShortRange(*i + audio[1] * 0.3);
            i++;
        }
    }
}

//
// Most of the following are various menu options for video processing designed to make
// the user interface easier.
//

void CRotoScopeDoc::OnFramesWriteoneframe()
{
    m_moviemake.WriteImage(m_image);
    m_moviemake.WriteAudio(m_audio);
    m_movieframe++;
}

void CRotoScopeDoc::OnUpdateFramesWriteoneframe(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_moviemake.IsOpen());
}


void CRotoScopeDoc::OnFramesWritethencreateoneframe()
{
    OnFramesWriteoneframe();
    OnFramesCreateoneframe();
}

void CRotoScopeDoc::OnUpdateFramesWritethencreateoneframe(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_moviemake.IsOpen());
}

void CRotoScopeDoc::OnFramesWritethencreateonesecond()
{
    if(m_moviesource.IsOpen() && !m_moviesource.HasAudio())
        return;

    // Do the creation operation for one entire second
    for(int i=0;  i<int(m_moviemake.GetFPS() + 0.5);  i++)
    {
        OnFramesWriteoneframe();
        OnFramesCreateoneframe();
        if(m_moviesource.IsOpen() && !m_moviesource.HasVideo())
            break;

        MessagePump();
    }
}

//
// Name :        CRotoScopeDoc::OnFramesWritethencreateremaining()
// Description : This is an example of a loop that will consume all remaining 
//               source video.
//

void CRotoScopeDoc::OnFramesWritethencreateremaining()
{
    // This only makes sense if there's an input video
    if(!m_moviesource.IsOpen() || !m_moviesource.HasAudio())
        return;

    // Do the creation operation for one entire second
    for(int i=0;  ;  i++)
    {
        OnFramesWriteoneframe();
        OnFramesCreateoneframe();
        if(!m_moviesource.HasVideo())
            break;

        MessagePump();
    }

    AfxMessageBox(TEXT("All Done"));
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// The mouse-based user interface.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//! This function is called by the view class when the mouse is
//! pressed or moved on the screen.
/*! It sets the associated pixel in the image to red right now. You'll 
 * likely make it do something different
 * \param p_x X location in image
 * \param p_y Y location in image */
void CRotoScopeDoc::Mouse(int p_x, int p_y)
{
    // We need to convert screen locations to image locations
    int x = p_x;                            // No problem there.
    int y = m_image.GetHeight() - p_y - 1;     // Just invert it.
    // Ensure there is an entry for every frame up till this one...
    std::list<CPoint> empty;
    while ((int)m_draw.size() < m_movieframe + 1)
        m_draw.push_back(empty);

    // Add the mouse point to the list for the frame
    m_draw[m_movieframe].push_back(CPoint(x, y));

    m_image.Set(x, y, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    m_image.Set(x+1, y, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    m_image.Set(x+1, y+1, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    m_image.Set(x, y+1, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    UpdateAllViews(NULL);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility functions...
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Name :        CRotoScopeDoc::MessagePump(void)
// Description : This function is a Windows message pump. It allows message processing to 
//               proceed while your program is in some loop.  This is so the menus and such
//               will continue to work.  Be very careful.  Usually you pop up a modal 
//               dialog box while this is going on to prevent accessing anything other than
//               a cancel button.  I'm not doing that, so be careful about what you hit.
//

void CRotoScopeDoc::MessagePump(void)
{
    // Allow any messages to be processed
    MSG msg;
    while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) 
    {
            AfxGetThread()->PumpMessage();
    }
}



BOOL CRotoScopeDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    //
   // Create an XML document
   //

   // CComPtr<> is a "smart" pointer. It takes care of reference counting on COM objects for
   // us so they know when to delete themselves.
    CComPtr<IXMLDOMDocument>  xmlDoc;
    HRESULT hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&xmlDoc);
    if (hr != S_OK)
    {
        AfxMessageBox(L"Unable to create XML document");
        return false;
    }

    // This is a pointer we'll use for nodes in our XML
    CComPtr<IXMLDOMNode> node;

    // Create a processing instruction targeted for xml.
    // This creates the line:  <?xml version='1.0'>
    CComPtr<IXMLDOMProcessingInstruction> pi;
    xmlDoc->createProcessingInstruction(L"xml", L"version='1.0'", &pi);
    xmlDoc->appendChild(pi, &node);
    pi.Release();
    node.Release();

    // Create the root element
    CComPtr<IXMLDOMElement> pe;
    xmlDoc->createElement(L"movie", &pe);
    xmlDoc->appendChild(pe, &node);
    pe.Release();
    SaveMovieData(xmlDoc, node);



    node.Release();

    hr = xmlDoc->save(CComVariant(lpszPathName));
    xmlDoc.Release();


    return TRUE;
}
void CRotoScopeDoc::SaveMovieData(IXMLDOMDocument* xmlDoc, IXMLDOMNode* inNode)
{
    for (int frame = 0; frame < (int)m_draw.size(); frame++)
    {
        // Create an XML node for the frame
        CComPtr<IXMLDOMNode> node;
        CComPtr<IXMLDOMElement> pe;
        xmlDoc->createElement(L"frame", &pe);

        // Add an attribute for the frame number
        pe->setAttribute(L"num", CComVariant(frame));

        // Append the node to the node we are nested inside.
        inNode->appendChild(pe, &node);
        pe.Release();

        //
        // Now save the point data for the frame
        //

        for (std::list<CPoint>::iterator p = m_draw[frame].begin(); p != m_draw[frame].end(); p++)
        {
            // Create an XML node for the point
            xmlDoc->createElement(L"point", &pe);

            // Add attributes for the point
            pe->setAttribute(L"x", CComVariant(p->x));
            pe->setAttribute(L"y", CComVariant(p->y));

            // Append the node to the node we are nested inside.
            CComPtr<IXMLDOMNode> nodePoint;
            node->appendChild(pe, &nodePoint);
            pe.Release();
            nodePoint.Release();
        }

        // When done, release <frame> the node
        node.Release();
    }
}


BOOL CRotoScopeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    // Clear any previous frame information
    m_draw.clear();

    //
    // Create an XML document
    //

    CComPtr<IXMLDOMDocument>  pXMLDoc;
    bool succeeded = SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
        IID_IXMLDOMDocument, (void**)&pXMLDoc));
    if (!succeeded)
    {
        AfxMessageBox(L"Failed to create an XML document to use");
        return FALSE;
    }

    // Open the XML document
    VARIANT_BOOL ok;
    succeeded = SUCCEEDED(pXMLDoc->load(CComVariant(lpszPathName), &ok));
    if (!succeeded || ok == VARIANT_FALSE)
    {
        AfxMessageBox(L"Failed to open XML score file");
        return FALSE;
    }

    //
    // Traverse the XML document in memory!!!!
    // Top level tag is <score>
    //

    CComPtr<IXMLDOMNode> node;
    pXMLDoc->get_firstChild(&node);
    for (; node != NULL; NextNode(node))
    {
        // Get the name of the node
        CComBSTR nodeName;
        node->get_nodeName(&nodeName);

        if (nodeName == L"movie")
        {
        }
    }


    return TRUE;
}
