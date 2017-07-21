#ifndef BDN_IOS_UiProvider_HH_
#define BDN_IOS_UiProvider_HH_

#include <bdn/IUiProvider.h>
#include <bdn/LayoutCoordinator.h>

namespace bdn
{
namespace ios
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
    UiProvider();
    

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


    static UiProvider& get();
    
    /** Returns the size of 1 sem in DIPs.*/
    double getSemSizeDips() const
    {
        return _semDips;    
    }
    
    /** Returns the layout coordinator that is used by view cores created by this UI provider.*/
    P<LayoutCoordinator> getLayoutCoordinator()
    {
        return _pLayoutCoordinator;
    }
    

private:
    double _semDips;
    
    P<LayoutCoordinator> _pLayoutCoordinator;
};

}
}


#endif
