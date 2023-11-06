/*! \file RotoScopeDoc.h
 *
 * Header file for the RotoScope document class
 * \author Charles B. Owen
 */

#pragma once

#include <vector>

#include "video/MovieSource.h"
#include "video/MovieMake.h"
#include "audio/DirSoundSource.h"
#include "graphics/GrImage.h"
#include <list>

/*! \mainpage RotoScope Starter Application
 *
 * The Rotoscope application is a simple program that allows you to 
 * load frames of video, modify them in interesting ways, and save 
 * them to a new video file. Rotoscoping is an animation technique where 
 * frames of a movie are advanced one by one and edited by a user. 
 */

//! Rotoscope document class 
/*! RotoScope document class */
class CRotoScopeDoc : public CDocument
{
public:
	CRotoScopeDoc();
	virtual ~CRotoScopeDoc();
	DECLARE_DYNCREATE(CRotoScopeDoc)

	virtual BOOL OnNewDocument();
    void Mouse(int p_x, int p_y);

    /*! Get the image we are editing 
     * \returns Constant pointer to image we are editing */
    const CGrImage &GetImage() const {return m_image;}

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
    int m_movieframe;       // Movie frame we are writing
    std::vector<std::list<CPoint> >     m_draw;
    afx_msg void OnMoviesOpensourcemovie();
    afx_msg void OnMoviesOpenoutputmovie();
    afx_msg void OnFramesCreateoneframe();

    void CreateOneFrame();

    CMovieSource        m_moviesource;      // Any source movie
    CMovieMake          m_moviemake;        // Any movie we are outputing to
    CDirSoundSource     m_backaudio;        // Any background audio?

    CGrImage            m_image;            // Current image being displayed
    std::vector<short>  m_audio;            // Current audio to be written to the output
    afx_msg void OnFramesWriteoneframe();
    afx_msg void OnUpdateFramesWriteoneframe(CCmdUI *pCmdUI);
    afx_msg void OnMoviesClosesourcemovie();
    afx_msg void OnUpdateMoviesClosesourcemovie(CCmdUI *pCmdUI);
    afx_msg void OnMoviesCloseoutputmovie();
    afx_msg void OnUpdateMoviesCloseoutputmovie(CCmdUI *pCmdUI);
    afx_msg void OnFramesWritethencreateoneframe();
    afx_msg void OnUpdateFramesWritethencreateoneframe(CCmdUI *pCmdUI);
    afx_msg void OnFramesWritethencreateonesecond();
    void MessagePump(void);
    afx_msg void OnFramesWritethencreateremaining();
    afx_msg void OnMoviesOpenbackgroundaudio();
    afx_msg void OnMoviesClosebackgroundaudio();
    afx_msg void OnUpdateMoviesClosebackgroundaudio(CCmdUI *pCmdUI);
public:
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    void SaveMovieData(IXMLDOMDocument* xmlDoc, IXMLDOMNode* inNode);
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


