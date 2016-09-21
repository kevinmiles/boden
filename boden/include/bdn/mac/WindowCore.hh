#ifndef BDN_MAC_WindowCore_HH_
#define BDN_MAC_WindowCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/NotImplementedError.h>

#import <bdn/mac/UiProvider.hh>

#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/util.hh>


namespace bdn
{
namespace mac
{


class WindowCore : public Base, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS IParentViewCore
{
public:
    WindowCore(View* pOuter);
    
    ~WindowCore();
   
    NSWindow* getNSWindow()
    {
        return _nsWindow;
    }
    
    
    P<Window> getOuterWindowIfStillAttached()
    {
        return _pOuterWindowWeak.toStrong();
    }
    
    P<const Window> getOuterWindowIfStillAttached() const
    {
        return _pOuterWindowWeak.toStrong();
    }

    
    void setTitle(const String& title) override
    {
        [_nsWindow setTitle: stringToMacString(title)];
    }
    
    
    Rect getContentArea() override
    {
        // the content parent is inside the inverted coordinate space of the window
        // origin is bottom left. So we need to pass the content height, so that
        // the coordinates can be flipped.
        return macRectToRect( _nsContentParent.frame, _nsContentParent.frame.size.height );
    }
    
    
    Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
    {
        // we can ignore the coordinate space being inverted here. We do not
        // care about the position, just the size.
        NSRect macContentRect = rectToMacRect( Rect(Point(0,0), contentSize), -1 );
    
        NSRect macWindowRect = [_nsWindow frameRectForContentRect:macContentRect];
        
        return macRectToRect(macWindowRect, -1).getSize();
    }
    
    
    Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
    {
        // we can ignore the coordinate space being inverted here. We do not
        // care about the position, just the size.
        NSRect macWindowRect = rectToMacRect( Rect(Point(0,0), windowSize), -1 );
        
        NSRect macContentRect = [_nsWindow contentRectForFrameRect:macWindowRect];
        
        Size resultSize = macRectToRect(macContentRect, -1).getSize();
        
        resultSize.width = std::max(resultSize.width, 0.0);
        resultSize.height = std::max(resultSize.height, 0.0);
        
        return resultSize;
    }
    

    Size calcMinimumSize() const override
    {
        return macSizeToSize( _nsWindow.minSize );
    }
    
    
    
    
    Rect getScreenWorkArea() const override
    {
        NSScreen* screen = _getNsScreen();
    
        NSRect workArea = screen.visibleFrame;
        NSRect fullArea = screen.frame;
        
        return macRectToRect(workArea, fullArea.size.height);
    }



    
    void	setVisible(const bool& visible) override
    {
        if(visible)
            [_nsWindow makeKeyAndOrderFront:NSApp];
        else
            [_nsWindow orderOut:NSApp];
    }
    
        
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        // the outer window handles padding during layout. So nothing to do here.
    }
    

    void setBounds(const Rect& bounds) override
    {
        if(bounds!=_currActualWindowBounds)
        {
            NSScreen* screen = _getNsScreen();
            
            // the screen's coordinate system is inverted. So we need to
            // flip the coordinates.
            NSRect nsBounds = rectToMacRect(bounds, screen.frame.size.height);
        
            [_nsWindow setFrame:nsBounds display: FALSE];
            _currActualWindowBounds = bounds;
        }
    }
    
    
    double uiLengthToDips(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToDips(uiLength);
    }
    
    
    Margin uiMarginToDipMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToDipMargin(margin);
    }
    
    
    
    
    Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
    {
        // the implementation for this must be provided by the outer Window object.
        throw NotImplementedError("WindowCore::calcPreferredSize");
    }
    
       
    
    
    bool tryChangeParentView(View* pNewParent) override
    {
        // we don't have a parent. Report that we cannot do this.
        return false;
    }
    
    
    void addChildNsView( NSView* childView ) override
    {
        [_nsContentParent addSubview:childView];
    }
    
    
    void _resized()
    {
        _currActualWindowBounds = macRectToRect( _nsWindow.frame, _getNsScreen().frame.size.height );

        P<Window> pOuter = getOuterWindowIfStillAttached();
        if(pOuter!=nullptr)
        {     
            pOuter->bounds() = _currActualWindowBounds;        
            pOuter->needLayout();
        }
    }
    
    void _moved()
    {
        _currActualWindowBounds = macRectToRect( _nsWindow.frame, _getNsScreen().frame.size.height );

        P<Window> pOuter = getOuterWindowIfStillAttached();
        if(pOuter!=nullptr)        
            pOuter->bounds() = _currActualWindowBounds;
    }
    
private:

    NSScreen* _getNsScreen() const
    {
        NSScreen* screen = _nsWindow.screen;
        
        if(screen==nil) // happens when window is not visible
            screen = [NSScreen mainScreen];
        
        return screen;
    }
    
    
    WeakP<Window>   _pOuterWindowWeak;
    NSWindow*       _nsWindow;
    NSView*         _nsContentParent;
    
    NSObject*       _ourDelegate;
    
    Rect            _currActualWindowBounds;
};


}
}


#endif